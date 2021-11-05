/**
 * Copyright (c) 2011-2013 Konnibe
 * Copyright (c) 2013-2015 Del Edson
 * Copyright (c) 2015-2021 Peter Tulp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/filename.h>
#include <wx/generic/gridctrl.h>
#include <wx/mimetype.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>


#include "boat.h"
#include "Export.h"
#include "LogbookDialog.h"

using namespace std;

Export::Export(LogbookDialog* dialog) { this->dialog = dialog; }

Export::~Export(void) {}

wxString Export::readLayoutODT(wxString path, wxString layout) {
  wxString odt = "";

  wxString filename = path + layout + ".odt";

  if (wxFileExists(filename)) {
    //#ifdef __WXOSX__
    unique_ptr<wxZipEntry> entry;
    static const wxString fn = "content.xml";
    wxString name = wxZipEntry::GetInternalName(fn);
    wxFFileInputStream in(filename);
    wxZipInputStream zip(in);
    do {
      entry.reset(zip.GetNextEntry());
    } while (entry.get() != NULL && entry->GetInternalName() != name);
    if (entry.get() != NULL) {
      wxTextInputStream txt(zip, "\n", wxConvUTF8);
      while (!zip.Eof()) odt += txt.ReadLine();
    }
    //#else
    /*	static const wxString fn = "content.xml";
            wxFileInputStream in(filename);
            wxZipInputStream zip(in);
            wxTextInputStream txt(zip);
            while(!zip.Eof())
                    odt += txt.ReadLine();*/
    //#endif
  }
  return odt;
}

bool Export::cutInPartsODT(wxString odt, wxString* top, wxString* header,
                           wxString* middle, wxString* bottom) {
  int indexTop;
  int indexBottom;
  wxString seperatorTop = "[[";
  wxString seperatorBottom = "]]";

  if (odt.Contains(seperatorTop)) {
    indexTop = odt.Find(seperatorTop);
    indexBottom = odt.Find(seperatorBottom);
    *top = odt.substr(0, indexTop);
    *top = (*top).substr(0, (*top).find_last_of('<'));
    *bottom = odt.substr(indexBottom + 1);
    *bottom = (*bottom).substr((*bottom).find_first_of('>') + 1);
    *middle = odt.substr(indexTop + 11);
    *middle = (*middle).substr(0, (*middle).Find(seperatorBottom));
    *middle = (*middle).substr(0, (*middle).find_last_of('<'));
    return "OK";
  } else
    return "";
}

bool Export::writeToHTML(wxTextFile* logFile, wxGrid* grid,
                         wxString filenameOut, wxString filenameIn,
                         wxString top, wxString header, wxString middle,
                         wxString bottom, int mode) {
  wxFileInputStream input(filenameIn);

  wxFileOutputStream output(filenameOut);
  wxTextOutputStream htmlFile(output);

  top.Replace("#TYPE#", dialog->boatType->GetValue());
  top.Replace("#BOATNAME#", dialog->boatName->GetValue());
  top.Replace("#HOMEPORT#", dialog->homeport->GetValue());
  top.Replace("#CALLSIGN#", dialog->callsign->GetValue());
  top.Replace("#REGISTRATION#", dialog->registration->GetValue());

  htmlFile << top;

  wxString newMiddle = middle;
  for (int row = 0; row < grid->GetNumberRows(); row++) {
    newMiddle = setPlaceHolders(mode, grid, row, middle);
    htmlFile << newMiddle;
  }

  htmlFile << bottom;

  output.Close();
  return true;
}

wxString Export::readLayoutHTML(wxString path, wxString layoutFileName) {
  wxString html, path1;

  path1 = path + layoutFileName + ".html";
  ;
  wxTextFile layout(path1);

  layout.Open();

  for (unsigned int i = 0; i < layout.GetLineCount(); i++) {
    html += layout.GetLine(i) + "\n";
  }

  layout.Close();

  return html;
}

bool Export::cutInPartsHTML(wxString html, wxString* top, wxString* header,
                            wxString* middle, wxString* bottom) {
  wxString seperatorTop = "<!-- Repeat -->";
  wxString seperatorBottom = "<!-- Repeat End -->";

  int indexTop = html.Find(seperatorTop);
  indexTop += seperatorTop.Len();
  int indexBottom = html.Find(seperatorBottom);
  indexBottom += seperatorBottom.Len();

  *top = html.substr(0, indexTop);
  *bottom = html.substr(indexBottom);
  *middle =
      html.substr(indexTop, (indexBottom - indexTop) - seperatorBottom.Len());

  return true;
}

wxTextFile* Export::setFiles(wxString savePath, wxString* path, int mode) {
  if (mode == 0) {
    (*path).Replace("txt", "html");
  } else if (mode == 1) {
    (*path).Replace("txt", "odt");
  } else {
    (*path) = savePath;
  }

  if (::wxFileExists(*path)) ::wxRemoveFile(*path);

  wxTextFile* logFile = new wxTextFile(*path);
  return logFile;
}

bool Export::writeToODT(wxTextFile* logFile, wxGrid* grid, wxString filenameOut,
                        wxString filenameIn, wxString top, wxString header,
                        wxString middle, wxString bottom, int mode) {
  unique_ptr<wxFFileInputStream> in(new wxFFileInputStream(filenameIn));
  wxTempFileOutputStream out(filenameOut);

  wxZipInputStream inzip(*in);
  wxZipOutputStream outzip(out);
  wxTextOutputStream odtFile(outzip);
  unique_ptr<wxZipEntry> entry;

  outzip.CopyArchiveMetaData(inzip);

  while (entry.reset(inzip.GetNextEntry()), entry.get() != NULL)
    if (!entry->GetName().Matches("content.xml"))
      if (!outzip.CopyEntry(entry.release(), inzip)) break;

  in.reset();

  outzip.PutNextEntry("content.xml");

  odtFile << top;

  wxString newMiddle;
  for (int row = 0; row < grid->GetNumberRows(); row++) {
    newMiddle = setPlaceHolders(mode, grid, row, middle);
    odtFile << newMiddle;
  }

  odtFile << bottom;

  inzip.Eof() && outzip.Close() && out.Commit();
  logFile->Close();

  return true;
}

wxString Export::replaceNewLine(int mode, wxString str, bool label) {
  str.Replace("&", "&amp;");
  str.Replace("<", "&lt;");
  str.Replace(">", "&gt;");
  str.Replace("'", "&apos;");
  str.Replace("\"", "&quot;");
  //	str.Replace("°","&deg;");

  if (mode == 0) {
    // HTML
    str.Replace("\n", "<br>");
  } else {
    // ODT
    if (!label) {
      str.Replace("\n", "<text:line-break/>");
      str.Replace("\xD\xA", "<text:line-break/>");
    } else {
      str.Replace("\n", " ");
      str.Replace("\xD\xA", " ");
    }
  }

  return str;
}
