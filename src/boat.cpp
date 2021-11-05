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

//#pragma once
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/dir.h>
#include <wx/event.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/object.h>
#include <wx/textctrl.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "boat.h"
#include "Export.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Options.h"

using namespace std;

Boat::Boat(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT) {
  parent = d;
  modified = false;
  this->layout = lay;
  this->ODTLayout = layoutODT;

  createFiles(data, lay);
  createTextCtrlConnections();
  createStaticTextList();
}

Boat::~Boat(void) {
  saveData();
  for (unsigned int i = 0; i < ctrl.GetCount(); i++) {
    if (ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl))) {
      ctrl[i]->Disconnect(
          wxEVT_COMMAND_TEXT_UPDATED,
          wxCommandEventHandler(LogbookDialog::boatNameOnTextEnter), NULL,
          parent);
    }
  }
}

void Boat::setLayoutLocation(wxString loc) {
  bool radio = parent->m_radioBtnHTMLBoat->GetValue();
  if (radio)
    layout_locn = layout;
  else
    layout_locn = ODTLayout;
  wxString boatLay = layout_locn;

  layout_locn.Append("boat");
  parent->appendOSDirSlash(&layout_locn);

  parent->loadLayoutChoice(
      LogbookDialog::BOAT, layout_locn, parent->boatChoice,
      parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);
  if (radio)
    parent->boatChoice->SetSelection(
        parent->logbookPlugIn->opt->boatGridLayoutChoice);
  else
    parent->boatChoice->SetSelection(
        parent->logbookPlugIn->opt->boatGridLayoutChoiceODT);
}

void Boat::createFiles(wxString data, wxString lay) {
  data_locn = data;
  data_locn.Append("boat.txt");
  boatFile = new wxTextFile(data_locn);
  wxFileName wxHomeFiledir = data_locn;

  if (true != wxHomeFiledir.FileExists()) {
    boatFile->Create();
  }

  equip_locn = data;
  equip_locn.Append("equipment.txt");
  equipFile = new wxTextFile(equip_locn);
  wxHomeFiledir = equip_locn;

  if (true != wxHomeFiledir.FileExists()) {
    equipFile->Create();
  }

  setLayoutLocation(lay);
}

void Boat::createTextCtrlConnections() {
  wxWindowList l = parent->m_panel72->GetWindowChildren();

  for (unsigned int i = 0; i < l.GetCount(); i++) {
    if (l[i]->IsKindOf(CLASSINFO(wxTextCtrl))) {
      l[i]->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                    wxCommandEventHandler(LogbookDialog::boatNameOnTextEnter),
                    NULL, parent);
      ctrl.Append(l[i]);
    }
  }
}

void Boat::createStaticTextList() {
  wxWindowList l = parent->m_panel72->GetWindowChildren();

  for (unsigned int i = 0; i < l.GetCount(); i++) {
    if (l[i]->IsKindOf(CLASSINFO(wxStaticText))) ctrlStaticText.Append(l[i]);
  }
}

void Boat::loadData() {
  wxString line;

  boatFile->Open();

  if (boatFile->GetLineCount() > 0) {
    line = boatFile->GetLine(0);
    if (line.Contains("#1.2#")) line = boatFile->GetLine(1);

    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);

    int c = 0;

    while (tkz.HasMoreTokens()) {
      wxString s;
      s += tkz.GetNextToken().RemoveLast();
      s = parent->restoreDangerChar(s);
      if (c == 18) s = parent->maintenance->getDateString(s);
      wxTextCtrl* t = wxDynamicCast(ctrl[c++], wxTextCtrl);
      t->ChangeValue(s);
    }
  }
  boatFile->Close();

  equipFile->Open();
  if (equipFile->GetLineCount() <= 0) {
    equipFile->Close();
    return;
  }
  if (parent->m_gridEquipment->GetNumberRows() > 0)
    parent->m_gridEquipment->DeleteRows(
        0, parent->m_gridEquipment->GetNumberRows() - 1);

  for (unsigned int i = 0; i < equipFile->GetLineCount(); i++) {
    line = equipFile->GetLine(i);

    parent->m_gridEquipment->AppendRows();

    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;

    while (tkz.HasMoreTokens()) {
      wxString s;
      s = tkz.GetNextToken().RemoveLast();
      s = parent->restoreDangerChar(s);
      parent->m_gridEquipment->SetCellValue(i, c++, s);
    }
  }
  equipFile->Close();
}

void Boat::saveData() {
  if (!modified) return;
  modified = false;

  wxString t, s;

  boatFile->Open();
  boatFile->Clear();

  for (unsigned int i = 0; i < ctrl.GetCount(); i++) {
    if (ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl))) {
      wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);
      wxString temp = te->GetValue();
      temp = parent->replaceDangerChar(temp);
      if (i == 18 && (!temp.IsEmpty() && temp.GetChar(0) != ' ')) {
        wxDateTime dt;
        parent->myParseDate(temp, dt);
        t += wxString::Format("%i/%i/%i \t", dt.GetMonth(), dt.GetDay(),
                              dt.GetYear());
      } else
        t += temp + " \t";
    }
  }
  t.RemoveLast();

  boatFile->AddLine(t);
  boatFile->Write();
  boatFile->Close();

  equipFile->Open();
  equipFile->Clear();

  int count = parent->m_gridEquipment->GetNumberRows();
  for (int r = 0; r < count; r++) {
    s = "";
    for (int i = 0; i < parent->m_gridEquipment->GetNumberCols(); i++) {
      s += parent->replaceDangerChar(
               parent->m_gridEquipment->GetCellValue(r, i)) +
           " \t";
    }
    equipFile->AddLine(s);
  }

  equipFile->Write();
  equipFile->Close();
}

wxString Boat::readLayoutFileODT(wxString layout) {
  wxString odt = "";

  wxString filename = layout_locn + layout + ".odt";

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

void Boat::viewODT(wxString path, wxString layout, bool mode) {
  if (parent->logbookPlugIn->opt->filterLayout[LogbookDialog::BOAT])
    layout.Prepend(
        parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);

  toODT(path, layout, mode);
  if (layout != "") {
    wxString fn = data_locn;
    fn.Replace("txt", "odt");
    parent->startApplication(fn, ".odt");
  }
}

wxString Boat::toODT(wxString path, wxString layout, bool mode) {
  wxString s, odt;

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return "";
  }

  saveData();

  odt = readLayoutFileODT(layout);

  for (unsigned int i = 0; i < ctrl.GetCount(); i++) {
    if (ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl))) {
      wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

      switch (i) {
        case 0:
          odt.Replace("#BOATNAME#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace(
              "#LBOATNAME#",
              Export::replaceNewLine(true, parent->bname->GetLabel(), true));
          break;
        case 1:
          odt.Replace("#HOMEPORT#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LHOMEPORT#",
                      Export::replaceNewLine(
                          true, parent->m_staticText114->GetLabel(), true));
          break;
        case 2:
          odt.Replace("#CALLSIGN#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LCALLSIGN#",
                      Export::replaceNewLine(
                          true, parent->m_staticText115->GetLabel(), true));
          break;
        case 3:
          odt.Replace("#HIN#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LHIN#",
                      Export::replaceNewLine(
                          true, parent->m_staticText116->GetLabel(), true));
          break;
        case 4:
          odt.Replace("#SAILNO#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LSAILNO#",
                      Export::replaceNewLine(
                          true, parent->m_staticText117->GetLabel(), true));
          break;
        case 6:
          odt.Replace("#REGISTRATION#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LREGISTRATION#",
                      Export::replaceNewLine(
                          true, parent->m_staticText118->GetLabel(), true));
          break;
        case 5:
          odt.Replace("#INSURANCE#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LINSURANCE#",
                      Export::replaceNewLine(
                          true, parent->m_staticText119->GetLabel(), true));
          break;
        case 7:
          odt.Replace("#POLICY#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LPOLICY#",
                      Export::replaceNewLine(
                          true, parent->m_staticText120->GetLabel(), true));
          break;
        case 8:
          odt.Replace("#MMSI#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LMMSI#",
                      Export::replaceNewLine(
                          true, parent->m_staticText53->GetLabel(), true));
          break;
        case 9:
          odt.Replace("#ONAME#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LONAME#",
                      Export::replaceNewLine(
                          true, parent->m_staticText90->GetLabel(), true));
          break;
        case 10:
          odt.Replace("#OFIRST#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LOFIRST#",
                      Export::replaceNewLine(
                          true, parent->m_staticText91->GetLabel(), true));
          break;
        case 11:
          odt.Replace("#TELEPHONE#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LTELEPHONE#",
                      Export::replaceNewLine(
                          true, parent->m_staticText95->GetLabel(), true));
          break;
        case 12:
          odt.Replace("#STREET#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LSTREET#",
                      Export::replaceNewLine(
                          true, parent->m_staticText92->GetLabel(), true));
          break;
        case 13:
          odt.Replace("#ZIP#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LZIP#",
                      Export::replaceNewLine(
                          true, parent->m_staticText93->GetLabel(), true));
          break;
        case 14:
          odt.Replace("#TOWN#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LTOWN#",
                      Export::replaceNewLine(
                          true, parent->m_staticText94->GetLabel(), true));
          break;
        case 15:
          odt.Replace("#TYPE#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LTYPE#",
                      Export::replaceNewLine(
                          true, parent->m_staticText128->GetLabel(), true));
          break;
        case 16:
          odt.Replace("#BUILDER#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LBUILDER#",
                      Export::replaceNewLine(
                          true, parent->m_staticText125->GetLabel(), true));
          break;
        case 17:
          odt.Replace("#HULL#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LHULL#",
                      Export::replaceNewLine(
                          true, parent->m_staticText124->GetLabel(), true));
          break;
        case 18:
          odt.Replace("#LAUNCHED#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LLAUNCHED#",
                      Export::replaceNewLine(
                          true, parent->m_staticText126->GetLabel(), true));
          break;
        case 19:
          odt.Replace("#YARDNO#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LYARDNO#",
                      Export::replaceNewLine(
                          true, parent->m_staticText127->GetLabel(), true));
          break;
        case 20:
          odt.Replace("#DESIGNER#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LDESIGNER#",
                      Export::replaceNewLine(
                          true, parent->m_staticText123->GetLabel(), true));
          break;
        case 21:
          odt.Replace("#CONSTRUCT#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LCONSTRUCT#",
                      Export::replaceNewLine(
                          true, parent->m_staticText129->GetLabel(), true));
          break;
        case 22:
          odt.Replace("#LOA#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LLOA#",
                      Export::replaceNewLine(
                          true, parent->m_staticText106->GetLabel(), true));
          break;
        case 23:
          odt.Replace("#LOD#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LLOD#",
                      Export::replaceNewLine(
                          true, parent->m_staticText107->GetLabel(), true));
          break;
        case 24:
          odt.Replace("#LWL#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LLWL#",
                      Export::replaceNewLine(
                          true, parent->m_staticText108->GetLabel(), true));
          break;
        case 25:
          odt.Replace("#BEAM#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LBEAM#",
                      Export::replaceNewLine(
                          true, parent->m_staticText109->GetLabel(), true));
          break;
        case 26:
          odt.Replace("#DRAFT#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LDRAFT#",
                      Export::replaceNewLine(
                          true, parent->m_staticText110->GetLabel(), true));
          break;
        case 27:
          odt.Replace("#LUSER1#",
                      Export::replaceNewLine(
                          true, parent->UserLabel1->GetValue(), true));
          break;
        case 28:
          odt.Replace("#USER1#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          break;
        case 29:
          odt.Replace("#LUSER2#",
                      Export::replaceNewLine(
                          true, parent->UserLabel2->GetValue(), true));
          break;
        case 30:
          odt.Replace("#USER2#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          break;
        case 31:
          odt.Replace("#LUSER3#",
                      Export::replaceNewLine(
                          true, parent->UserLabel3->GetValue(), true));
          break;
        case 32:
          odt.Replace("#USER3#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          break;
        case 33:
          odt.Replace("#DISPL#",
                      Export::replaceNewLine(true, te->GetValue(), false));
          odt.Replace("#LDISPL#",
                      Export::replaceNewLine(
                          true, parent->m_staticText122->GetLabel(), true));
          break;
      }
    }
  }

  odt.Replace("#LOWNER#",
              Export::replaceNewLine(
                  true, parent->sbSizer6->GetStaticBox()->GetLabel(), true));
  if (odt.Contains("{{")) odt = repeatAreaODT(odt);

  wxString fn = data_locn;
  fn.Replace("txt", "odt");
  if (mode == false) {
    fn.Replace("txt", "odt");
  } else
    fn = path;

  if (::wxFileExists(fn)) ::wxRemoveFile(fn);

  unique_ptr<wxFFileInputStream> in(
      new wxFFileInputStream(layout_locn + layout + ".odt"));
  wxTempFileOutputStream out(fn);

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

  odtFile << odt;

  inzip.Eof() && outzip.Close() && out.Commit();
  return fn;
}

wxString Boat::repeatAreaODT(wxString odt) {
  wxString topODT;
  wxString bottomODT;
  wxString middleODT;

  wxString seperatorTop = "{{";
  wxString seperatorBottom = "}}";

  int indexTop = odt.First(seperatorTop) + seperatorTop.Len();
  int indexBottom = odt.First(seperatorBottom) + seperatorBottom.Len();

  topODT = odt.substr(0, indexTop);
  middleODT = odt.substr(indexTop + 9);
  indexTop = topODT.find_last_of('<');
  topODT = odt.substr(0, indexTop);
  bottomODT = odt.substr(indexBottom, odt.Len() - 1);
  indexBottom = bottomODT.find_first_of('>') + 1;
  bottomODT = bottomODT.substr(indexBottom, odt.Len() - 1);

  middleODT.Replace(bottomODT, "");
  indexBottom = middleODT.First(seperatorBottom) + seperatorBottom.Len();
  middleODT = middleODT.substr(0, indexBottom - 1);
  indexBottom = middleODT.find_last_of('<');
  middleODT = middleODT.substr(0, indexBottom);

  topODT.Replace("#LEQUIP#", parent->sbSizer12->GetStaticBox()->GetLabel());
  topODT.Replace("#LETYPE#", parent->m_gridEquipment->GetColLabelValue(0));
  topODT.Replace("#LDISCRIPTION#",
                 parent->m_gridEquipment->GetColLabelValue(1));
  topODT.Replace("#LSERIAL#", parent->m_gridEquipment->GetColLabelValue(2));
  topODT.Replace("#LREMARKS#", parent->m_gridEquipment->GetColLabelValue(3));

  wxString newMiddleODT = "";
  for (int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++) {
    wxString temp = middleODT;
    for (int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++) {
      wxString text = parent->m_gridEquipment->GetCellValue(i, c);

      switch (c) {
        case 0:
          temp.Replace("#ETYPE#", text);
          break;
        case 1:
          temp.Replace("#DISCRIPTION#", text);
          break;
        case 2:
          temp.Replace("#SERIAL#", text);
          break;
        case 3:
          temp.Replace("#REMARKS#", text);
          break;
      }
    }
    newMiddleODT += temp;
  }

  odt = topODT + newMiddleODT + bottomODT;
  return odt;
}

void Boat::toHTML(wxString path, wxString layout, bool mode) {
  wxString s, html;

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return;
  }

  saveData();

  boatLayoutFile = new wxTextFile(layout_locn + layout + ".html");
  boatLayoutFile->Open();

  wxString fn = data_locn;
  fn.Replace("txt", "html");
  if (mode == false)
    boatHTMLFile = new wxFile(fn, wxFile::write);
  else
    boatHTMLFile = new wxFile(path, wxFile::write);

  for (unsigned int i = 0; i < boatLayoutFile->GetLineCount(); i++)
    html += boatLayoutFile->GetLine(i);
  boatLayoutFile->Close();

  for (unsigned int i = 0; i < ctrl.GetCount(); i++) {
    if (ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl))) {
      wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

      switch (i) {
        case 0:
          html.Replace("#BOATNAME#", te->GetValue());
          html.Replace("#LBOATNAME#", parent->bname->GetLabel());
          break;
        case 1:
          html.Replace("#HOMEPORT#", te->GetValue());
          html.Replace("#LHOMEPORT#", parent->m_staticText114->GetLabel());
          break;
        case 2:
          html.Replace("#CALLSIGN#", te->GetValue());
          html.Replace("#LCALLSIGN#", parent->m_staticText115->GetLabel());
          break;
        case 3:
          html.Replace("#HIN#", te->GetValue());
          html.Replace("#LHIN#", parent->m_staticText116->GetLabel());
          break;
        case 4:
          html.Replace("#SAILNO#", te->GetValue());
          html.Replace("#LSAILNO#", parent->m_staticText117->GetLabel());
          break;
        case 6:
          html.Replace("#REGISTRATION#", te->GetValue());
          html.Replace("#LREGISTRATION#", parent->m_staticText118->GetLabel());
          break;
        case 5:
          html.Replace("#INSURANCE#", te->GetValue());
          html.Replace("#LINSURANCE#", parent->m_staticText119->GetLabel());
          break;
        case 7:
          html.Replace("#POLICY#", te->GetValue());
          html.Replace("#LPOLICY#", parent->m_staticText120->GetLabel());
          break;
        case 8:
          html.Replace("#MMSI#", te->GetValue());
          html.Replace("#LMMSI#", parent->m_staticText53->GetLabel());
          break;
        case 9:
          html.Replace("#ONAME#", te->GetValue());
          html.Replace("#LONAME#", parent->m_staticText90->GetLabel());
          break;
        case 10:
          html.Replace("#OFIRST#", te->GetValue());
          html.Replace("#LOFIRST#", parent->m_staticText91->GetLabel());
          break;
        case 11:
          html.Replace("#TELEPHONE#", te->GetValue());
          html.Replace("#LTELEPHONE#", parent->m_staticText95->GetLabel());
          break;
        case 12:
          html.Replace("#STREET#", te->GetValue());
          html.Replace("#LSTREET#", parent->m_staticText92->GetLabel());
          break;
        case 13:
          html.Replace("#ZIP#", te->GetValue());
          html.Replace("#LZIP#", parent->m_staticText93->GetLabel());
          break;
        case 14:
          html.Replace("#TOWN#", te->GetValue());
          html.Replace("#LTOWN#", parent->m_staticText94->GetLabel());
          break;
        case 15:
          html.Replace("#TYPE#", te->GetValue());
          html.Replace("#LTYPE#", parent->m_staticText128->GetLabel());
          break;
        case 16:
          html.Replace("#BUILDER#", te->GetValue());
          html.Replace("#LBUILDER#", parent->m_staticText125->GetLabel());
          break;
        case 17:
          html.Replace("#HULL#", te->GetValue());
          html.Replace("#LHULL#", parent->m_staticText124->GetLabel());
          break;
        case 18:
          html.Replace("#LAUNCHED#", te->GetValue());
          html.Replace("#LLAUNCHED#", parent->m_staticText126->GetLabel());
          break;
        case 19:
          html.Replace("#YARDNO#", te->GetValue());
          html.Replace("#LYARDNO#", parent->m_staticText127->GetLabel());
          break;
        case 20:
          html.Replace("#DESIGNER#", te->GetValue());
          html.Replace("#LDESIGNER#", parent->m_staticText123->GetLabel());
          break;
        case 21:
          html.Replace("#CONSTRUCT#", te->GetValue());
          html.Replace("#LCONSTRUCT#", parent->m_staticText129->GetLabel());
          break;
        case 22:
          html.Replace("#LOA#", te->GetValue());
          html.Replace("#LLOA#", parent->m_staticText106->GetLabel());
          break;
        case 23:
          html.Replace("#LOD#", te->GetValue());
          html.Replace("#LLOD#", parent->m_staticText107->GetLabel());
          break;
        case 24:
          html.Replace("#LWL#", te->GetValue());
          html.Replace("#LLWL#", parent->m_staticText108->GetLabel());
          break;
        case 25:
          html.Replace("#BEAM#", te->GetValue());
          html.Replace("#LBEAM#", parent->m_staticText109->GetLabel());
          break;
        case 26:
          html.Replace("#DRAFT#", te->GetValue());
          html.Replace("#LDRAFT#", parent->m_staticText110->GetLabel());
          break;
        case 27:
          html.Replace("#LUSER1#", parent->UserLabel1->GetValue());
          break;
        case 28:
          html.Replace("#USER1#", te->GetValue());
          break;
        case 29:
          html.Replace("#LUSER2#", parent->UserLabel2->GetValue());
          break;
        case 30:
          html.Replace("#USER2#", te->GetValue());
          break;
        case 31:
          html.Replace("#LUSER3#", parent->UserLabel3->GetValue());
          break;
        case 32:
          html.Replace("#USER3#", te->GetValue());
          break;
        case 33:
          html.Replace("#DISPL#", te->GetValue());
          html.Replace("#LDISPL#", parent->m_staticText122->GetLabel());
          break;
      }
    }
  }

  html.Replace("#LOWNER#", parent->sbSizer6->GetStaticBox()->GetLabel());
  html.Replace("#LDATE#", parent->m_gridGlobal->GetColLabelValue(1));
  html.Replace("#LEQUIP#", parent->sbSizer12->GetStaticBox()->GetLabel());

  if (html.Contains("<!--Repeat -->")) html = repeatArea(html);
#ifdef __WXOSX__
  wxString str(html.wx_str(), wxConvUTF8);
#else
  wxString str(html);
#endif
  boatHTMLFile->Write(str);
  boatHTMLFile->Close();
}

wxString Boat::repeatArea(wxString html) {
  wxString topHTML;
  wxString bottomHTML;
  wxString middleHTML;

  wxString seperatorTop = "<!--Repeat -->";
  wxString seperatorBottom = "<!--Repeat End -->";

  int indexTop = html.First(seperatorTop) + seperatorTop.Len();
  int indexBottom = html.First(seperatorBottom) + seperatorBottom.Len();

  topHTML = html.substr(0, indexTop);
  bottomHTML = html.substr(indexBottom, html.Len() - 1);
  middleHTML = html.substr(indexTop, indexBottom - indexTop);

  topHTML.Replace("#LETYPE#", parent->m_gridEquipment->GetColLabelValue(0));
  topHTML.Replace("#LDISCRIPTION#",
                  parent->m_gridEquipment->GetColLabelValue(1));
  topHTML.Replace("#LSERIAL#", parent->m_gridEquipment->GetColLabelValue(2));
  topHTML.Replace("#LREMARKS#", parent->m_gridEquipment->GetColLabelValue(3));

  wxString newMiddleHTML = "";
  for (int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++) {
    wxString temp = middleHTML;
    for (int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++) {
      wxString text = parent->m_gridEquipment->GetCellValue(i, c);

      switch (c) {
        case 0:
          temp.Replace("#ETYPE#", text);
          break;
        case 1:
          temp.Replace("#DISCRIPTION#", text);
          break;
        case 2:
          temp.Replace("#SERIAL#", text);
          break;
        case 3:
          temp.Replace("#REMARKS#", text);
          break;
      }
    }
    newMiddleHTML += temp;
  }

  html = topHTML + newMiddleHTML + bottomHTML;
  return html;
}

void Boat::viewHTML(wxString path, wxString layout, bool mode) {
  if (parent->logbookPlugIn->opt->filterLayout[LogbookDialog::BOAT])
    layout.Prepend(
        parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);

  toHTML(path, layout, mode);
  if (layout != "") {
    wxString fn = data_locn;
    fn.Replace("txt", "html");
    parent->startBrowser(fn);
  }
}

void Boat::addEquip() {
  wxString s;

  modified = true;

  parent->m_gridEquipment->AppendRows();
  parent->m_gridEquipment->SetCellValue(
      parent->m_gridEquipment->GetNumberRows() - 1,
      parent->m_gridEquipment->GetNumberCols() - 1, " ");
  parent->m_gridEquipment->MakeCellVisible(
      parent->m_gridEquipment->GetNumberRows() - 1, 0);

  equipFile->Open();

  for (int i = 0; i < parent->m_gridEquipment->GetNumberCols(); i++) {
    s += ",";
  }

  equipFile->AddLine(s);
  equipFile->Write();
  equipFile->Close();
}
void Boat::cellChanged(int row, int col) {
  modified = true;

  if (parent->m_gridEquipment->GetCellValue(
          row, parent->m_gridEquipment->GetNumberCols() - 1) == "")
    parent->m_gridEquipment->SetCellValue(
        row, parent->m_gridEquipment->GetNumberCols() - 1, " ");
}

void Boat::deleteRow(int row) {
  int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"), row + 1),
                            _("Confirm"), wxYES_NO | wxCANCEL, parent);

  if (answer == wxYES) {
    parent->m_gridEquipment->DeleteRows(row);
  }
}

void Boat::toCSV(wxString savePath) {
  wxString line;
  wxString s;

  saveData();

  wxTextFile* csvFile = new wxTextFile(savePath);
  wxString path = savePath;
  path.Replace("Boat", "Equipment");
  wxTextFile* csvEquipFile = new wxTextFile(path);

  if (csvFile->Exists()) {
    ::wxRemoveFile(savePath);
    ::wxRemoveFile(path);
    csvFile->Create();
    equipFile->Create();
  }

  saveCSV(csvFile, true);
  saveCSV(csvEquipFile, false);
}

void Boat::saveCSV(wxTextFile* file, bool mode) {
  wxString line, s;
  int col = 0;

  saveData();

  ((mode) ? boatFile : equipFile)->Open();

  for (unsigned int i = 0; i < ((mode) ? boatFile : equipFile)->GetLineCount();
       i++) {
    line = ((mode) ? boatFile : equipFile)->GetLine(i);
    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);
    col = 0;

    while (tkz.HasMoreTokens()) {
      if (mode == true) {
        if (col != 27 && col != 29 && col != 31) {
          s += "\"" +
               parent->restoreDangerChar(tkz.GetNextToken().RemoveLast()) +
               "\",";
        } else
          tkz.GetNextToken();
      } else
        s += "\"" + parent->restoreDangerChar(tkz.GetNextToken().RemoveLast()) +
             "\",";
      col++;
    }
    s.RemoveLast();
    file->AddLine(s);
    s = "";
  }

  file->Write();
  file->Close();
  ((mode) ? boatFile : equipFile)->Close();
}

void Boat::toXML(wxString savePath) {
  wxString line;
  wxString s;

  wxTextFile* xmlFile = new wxTextFile(savePath);
  wxString path = savePath;
  path.Replace("Boat", "Equipment");
  wxTextFile* xmlEquipFile = new wxTextFile(path);

  if (xmlFile->Exists()) {
    ::wxRemoveFile(savePath);
    ::wxRemoveFile(path);
    xmlFile->Create();
    xmlEquipFile->Create();
  }

  saveData();
  saveXML(xmlFile, true);
  saveXML(xmlEquipFile, false);
}

void Boat::saveXML(wxTextFile* xmlFile, bool mode) {
  wxTextFile* file;
  wxString temp;

  wxString line, s;

  if (mode)
    file = boatFile;
  else
    file = equipFile;

  file->Open();

  xmlFile->AddLine(parent->xmlHead);

  for (unsigned int i = 0; i < file->GetLineCount(); i++) {
    int col = 0;
    line = file->GetLine(i);
    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);
    s = wxString::Format("<Row ss:Height=\"%u\">",
                         parent->m_gridGlobal->GetRowHeight(i));

    while (tkz.HasMoreTokens()) {
      if (col != 27 && col != 29 && col != 31) {
        s += "<Cell>";
        s += "<Data ss:Type=\"String\">#DATA#</Data>";
        temp = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
        temp.Replace("\n", "&#10;");
        temp.Replace("&", "&amp;");
        temp.Replace("\"", "&quot;");
        temp.Replace("<", "&lt;");
        temp.Replace(">", "&gt;");
        temp.Replace("'", "&apos;");
        s.Replace("#DATA#", temp);
        s += "</Cell>";
      } else
        tkz.GetNextToken();
      col++;
    }
    s += "</Row>";
    xmlFile->AddLine(s);
  }

  xmlFile->AddLine(parent->xmlEnd);
  xmlFile->Write();
  file->Close();
  xmlFile->Close();
}

void Boat::backup(wxString path) {
  wxFileName fn(path);
  wxString z = fn.GetName();

  saveData();

  wxCopyFile(data_locn, path);
  path.Replace(z, "equipment");
  wxCopyFile(equip_locn, path);
}

void Boat::toODS(wxString path) {
  wxString s = "";
  wxString line;
  wxString temp;

  saveData();

  saveODS(path, false);

  wxFileName fn(path);
  wxString sf = fn.GetName();
  path.Replace(sf, "equipment");

  saveODS(path, true);
}

void Boat::saveODS(wxString path, bool mode) {
  wxString temp, col, tableName;
  int x = 0;

  if (mode) {
    temp = equip_locn;
    col = "table:number-columns-repeated=\"4\"";
    tableName = "Equipment";
    x = parent->m_gridEquipment->GetNumberCols();
  } else {
    temp = data_locn;
    col = "table:number-columns-repeated=\"31\"";
    tableName = "Boat";
    x = ctrlStaticText.GetCount();
  }

  wxFileInputStream input(temp);
  wxTextInputStream* stream = new wxTextInputStream(input);

  wxFFileOutputStream out(path);
  wxZipOutputStream zip(out);
  wxTextOutputStream txt(zip);
  wxString sep(wxFileName::GetPathSeparator());

  temp = parent->content;
  temp.Replace("table:number-columns-repeated=\"33\"", col);
  temp.Replace("Logbook", tableName);
  zip.PutNextEntry("content.xml");
  txt << temp;

  txt << "<table:table-row table:style-name=\"ro2\">";
  for (int i = 0; i < x; i++) {
    txt << "<table:table-cell office:value-type=\"string\">";
    txt << "<text:p>";
    if (!mode) {
      if (i == 27) {
        txt << parent->UserLabel1->GetValue();
        txt << "</text:p>";
        txt << "</table:table-cell>";

        txt << "<table:table-cell office:value-type=\"string\">";
        txt << "<text:p>";
        txt << parent->UserLabel2->GetValue();
        txt << "</text:p>";
        txt << "</table:table-cell>";

        txt << "<table:table-cell office:value-type=\"string\">";
        txt << "<text:p>";
        txt << parent->UserLabel3->GetValue();
        txt << "</text:p>";
        txt << "</table:table-cell>";

        txt << "<table:table-cell office:value-type=\"string\">";
        txt << "<text:p>";
        wxStaticText* t = wxDynamicCast(ctrlStaticText[i], wxStaticText);
        txt << t->GetLabel();
      } else {
        wxStaticText* t = wxDynamicCast(ctrlStaticText[i], wxStaticText);
        txt << t->GetLabel();
      }
    } else
      txt << parent->m_gridEquipment->GetColLabelValue(i);
    txt << "</text:p>";
    txt << "</table:table-cell>";
  }
  txt << "</table:table-row>";

  //	bool empty = false;
  long emptyCol = 0;

  wxString line;
  while (!input.Eof()) {
    line = stream->ReadLine();  // for #1.2#
    int col = 0;
    if (input.Eof()) break;
    txt << "<table:table-row table:style-name=\"ro2\">";
    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);

    while (tkz.HasMoreTokens()) {
      wxString s = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
      if (s == "") {
        txt << "<table:table-cell />";
        // empty = true;
        emptyCol++;
        col++;
        continue;
      }

      if (col != 27 && col != 29 && col != 31)
        txt << "<table:table-cell office:value-type=\"string\">";
      else {
        col++;
        continue;
      }

      wxStringTokenizer str(s, "\n");
      while (str.HasMoreTokens()) {
        wxString e = str.GetNextToken();
        e.Replace("&", "&amp;");
        e.Replace("\"", "&quot;");
        e.Replace("<", "&lt;");
        e.Replace(">", "&gt;");
        e.Replace("'", "&apos;");
        txt << "<text:p>";
        txt << e;
        txt << "</text:p>";
      }
      txt << "</table:table-cell>";
      col++;
    }
    txt << "</table:table-row>";
    ;
  }
  txt << parent->contentEnd;

  zip.PutNextEntry("mimetype");
  txt << "application/vnd.oasis.opendocument.spreadsheet";

  zip.PutNextEntry("styles.xml");
  txt << parent->styles;

  zip.PutNextEntry("meta.xml");
  txt << parent->meta;

  zip.PutNextEntry("META-INF" + sep + "manifest.xml");
  txt << parent->manifest;

  zip.PutNextEntry("Thumbnails" + sep);

  zip.PutNextEntry("Configurations2" + sep + "floater");
  zip.PutNextEntry("Configurations2" + sep + "menubar");
  zip.PutNextEntry("Configurations2" + sep + "popupmenu");
  zip.PutNextEntry("Configurations2" + sep + "progressbar");
  zip.PutNextEntry("Configurations2" + sep + "statusbar");
  zip.PutNextEntry("Configurations2" + sep + "toolbar");
  zip.PutNextEntry("Configurations2" + sep + "images" + sep + "Bitmaps");

  zip.Close();
  out.Close();
}
