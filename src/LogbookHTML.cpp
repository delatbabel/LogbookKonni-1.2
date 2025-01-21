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

#include <map>
#include <memory>

#include "Logbook.h"
#include "LogbookHTML.h"
#include "LogbookDialog.h"
#include "Options.h"
#include "logbook_pi.h"


#include <wx/grid.h>
#include <wx/mimetype.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>


using namespace std;

LogbookHTML::LogbookHTML(Logbook *l, LogbookDialog *d, wxString data,
                         wxString layout) : kmlFile(nullptr) {
  parent = d;
  logbook = l;
  data_locn = data;
  layout_locn = layout;
}

LogbookHTML::~LogbookHTML(void) {}

void LogbookHTML::setPlaceholders() {
  gridc gridcols = {0, LogbookHTML::ROUTE};
  placeholders["ROUTE"] = gridcols;
  placeholders["LROUTE"] = gridcols;
  gridc gridcols1 = {0, LogbookHTML::RDATE};
  placeholders["DATE"] = gridcols1;
  placeholders["LDATE"] = gridcols1;
  gridc gridcols2 = {0, LogbookHTML::RTIME};
  placeholders["TIME"] = gridcols2;
  placeholders["LTIME"] = gridcols2;
  gridc gridcols3 = {0, LogbookHTML::STATUS};
  placeholders["STATUS"] = gridcols3;
  placeholders["LSTATUS"] = gridcols3;
  gridc gridcols4 = {0, LogbookHTML::WAKE};
  placeholders["WAKE"] = gridcols4;
  placeholders["LWAKE"] = gridcols4;
  gridc gridcols5 = {0, LogbookHTML::DISTANCE};
  placeholders["DISTANCE"] = gridcols5;
  placeholders["LDISTANCE"] = gridcols5;
  gridc gridcols6 = {0, LogbookHTML::DTOTAL};
  placeholders["DTOTAL"] = gridcols6;
  placeholders["LDTOTAL"] = gridcols6;
  gridc gridcols7 = {0, LogbookHTML::POSITION};
  placeholders["POSITION"] = gridcols7;
  placeholders["LPOSITION"] = gridcols7;
  gridc gridcols8 = {0, LogbookHTML::COG};
  placeholders["COG"] = gridcols8;
  placeholders["LCOG"] = gridcols8;
  gridc gridcols9 = {0, LogbookHTML::RTIME};
  placeholders["COW"] = gridcols9;
  placeholders["LCOW"] = gridcols9;
  gridc gridcols10 = {0, LogbookHTML::COW};
  placeholders["SOG"] = gridcols10;
  placeholders["LSOG"] = gridcols10;
  gridc gridcols11 = {0, LogbookHTML::SOW};
  placeholders["SOW"] = gridcols11;
  placeholders["LSOW"] = gridcols11;
  gridc gridcols12 = {0, LogbookHTML::DEPTH};
  placeholders["DEPTH"] = gridcols12;
  placeholders["LDEPTH"] = gridcols12;
  gridc gridcols13 = {0, LogbookHTML::REMARKS};
  placeholders["REMARKS"] = gridcols13;
  placeholders["LREMARKS"] = gridcols13;

  gridc gridcols14 = {1, LogbookHTML::BARO};
  placeholders["BARO"] = gridcols14;
  placeholders["LBARO"] = gridcols14;
  gridc gridcols15 = {1, LogbookHTML::HYDRO};
  placeholders["HYDRO"] = gridcols15;
  placeholders["LHYDRO"] = gridcols15;
  gridc gridcols16 = {1, LogbookHTML::AIRTE};
  placeholders["AIRTE"] = gridcols16;
  placeholders["LAIRTE"] = gridcols16;
  gridc gridcols17 = {1, LogbookHTML::WATERTE};
  placeholders["WTE"] = gridcols17;
  placeholders["LWTE"] = gridcols17;
  gridc gridcols18 = {1, LogbookHTML::WIND};
  placeholders["WIND"] = gridcols18;
  placeholders["LWIND"] = gridcols18;
  gridc gridcols19 = {1, LogbookHTML::WSPD};
  placeholders["WSPD"] = gridcols19;
  placeholders["LWSPD"] = gridcols19;
  gridc gridcols20 = {1, LogbookHTML::WINDR};
  placeholders["WIND"] = gridcols20;
  placeholders["LWIND"] = gridcols20;
  gridc gridcols21 = {1, LogbookHTML::WSPDR};
  placeholders["WSPD"] = gridcols21;
  placeholders["LWSPD"] = gridcols21;
  gridc gridcols22 = {1, LogbookHTML::CURRENT};
  placeholders["CUR"] = gridcols22;
  placeholders["LCUR"] = gridcols22;
  gridc gridcols23 = {1, LogbookHTML::CSPD};
  placeholders["CSPD"] = gridcols23;
  placeholders["LCSPD"] = gridcols23;
  gridc gridcols24 = {1, LogbookHTML::WAVE};
  placeholders["WAVE"] = gridcols24;
  placeholders["LWAVE"] = gridcols24;
  gridc gridcols25 = {1, LogbookHTML::SWELL};
  placeholders["SWELL"] = gridcols25;
  placeholders["LSWELL"] = gridcols25;
  gridc gridcols26 = {1, LogbookHTML::WEATHER};
  placeholders["WEATHER"] = gridcols26;
  placeholders["LWEATHER"] = gridcols26;
  gridc gridcols27 = {1, LogbookHTML::CLOUDS};
  placeholders["CLOUDS"] = gridcols27;
  placeholders["LCLOUDS"] = gridcols27;
  gridc gridcols28 = {1, LogbookHTML::VISIBILITY};
  placeholders["VISIBILITY"] = gridcols28;
  placeholders["LVISIBILITY"] = gridcols28;

  gridc gridcols29 = {2, LogbookHTML::MOTOR};
  placeholders["MOTOR"] = gridcols29;
  placeholders["LMOTOR"] = gridcols29;
  gridc gridcols30 = {2, LogbookHTML::MOTORT};
  placeholders["MOTORT"] = gridcols30;
  placeholders["LMOTORT"] = gridcols30;
  gridc gridcols31 = {2, LogbookHTML::MOTOR1};
  placeholders["MOTOR1"] = gridcols31;
  placeholders["LMOTOR1"] = gridcols31;
  gridc gridcols32 = {2, LogbookHTML::MOTOR1T};
  placeholders["MOTOR1T"] = gridcols32;
  placeholders["LMOTOR1T"] = gridcols32;
  gridc gridcols33 = {2, LogbookHTML::FUEL};
  placeholders["FUEL"] = gridcols33;
  placeholders["LFUEL"] = gridcols33;
  gridc gridcols34 = {2, LogbookHTML::FUELT};
  placeholders["FUELT"] = gridcols34;
  placeholders["LFUELT"] = gridcols34;
  gridc gridcols35 = {2, LogbookHTML::GENE};
  placeholders["GENE"] = gridcols35;
  placeholders["LGENE"] = gridcols35;
  gridc gridcols36 = {2, LogbookHTML::GENET};
  placeholders["GENET"] = gridcols36;
  placeholders["LGENET"] = gridcols36;
  gridc gridcols37 = {2, LogbookHTML::BANK1};
  placeholders["BANK1"] = gridcols37;
  placeholders["LBANK1"] = gridcols37;
  gridc gridcols38 = {2, LogbookHTML::BANK1T};
  placeholders["BANK1T"] = gridcols38;
  placeholders["LBANK1T"] = gridcols38;
  gridc gridcols39 = {2, LogbookHTML::BANK2};
  placeholders["BANK2"] = gridcols39;
  placeholders["LBANK2"] = gridcols39;
  gridc gridcols40 = {2, LogbookHTML::BANK2T};
  placeholders["BANK2T"] = gridcols40;
  placeholders["LBANK2T"] = gridcols40;
  gridc gridcols41 = {2, LogbookHTML::SAILS};
  placeholders["SAILS"] = gridcols41;
  placeholders["LSAILS"] = gridcols41;
  gridc gridcols42 = {2, LogbookHTML::REEF};
  placeholders["REEF"] = gridcols42;
  placeholders["LREEF"] = gridcols42;
  gridc gridcols43 = {2, LogbookHTML::WATERM};
  placeholders["WATERM"] = gridcols43;
  placeholders["LWATERM"] = gridcols43;
  gridc gridcols44 = {2, LogbookHTML::WATERMT};
  placeholders["WATERMT"] = gridcols44;
  placeholders["LWATERMT"] = gridcols44;
  gridc gridcols45 = {2, LogbookHTML::WATERMO};
  placeholders["WATERMO"] = gridcols45;
  placeholders["LWATERMO"] = gridcols45;
  gridc gridcols46 = {2, LogbookHTML::WATER};
  placeholders["WATER"] = gridcols46;
  placeholders["LWATER"] = gridcols46;
  gridc gridcols47 = {2, LogbookHTML::WATERT};
  placeholders["WATERT"] = gridcols47;
  placeholders["LWATERT"] = gridcols47;
  gridc gridcols48 = {2, LogbookHTML::MREMARKS};
  placeholders["MREMARKS"] = gridcols48;
  placeholders["LMREMARKS"] = gridcols48;

  placeholdersboat["LLOGBOOK"] = parent->m_logbook->GetPageText(0);
  placeholdersboat["LFROM"] = _("from");
  placeholdersboat["LTO"] = _("to");
  if (parent->logGrids[0]->GetNumberRows() > 0) {
    placeholdersboat["SDATE"] = parent->m_gridGlobal->GetCellValue(0, 1);
    placeholdersboat["EDATE"] = parent->m_gridGlobal->GetCellValue(
        parent->m_gridGlobal->GetNumberRows() - 1, 1);
  }
  placeholdersboat["TYPE"] = parent->boatType->GetValue();
  placeholdersboat["BOATNAME"] = parent->boatName->GetValue();
  placeholdersboat["HOMEPORT"] = parent->homeport->GetValue();
  placeholdersboat["CALLSIGN"] = parent->callsign->GetValue();
  placeholdersboat["REGISTRATION"] = parent->registration->GetValue();
  //	placeholdersboat["LOCATION"]     = layout_locn + layout + ".html";
}

void LogbookHTML::viewHTML(wxString path, wxString layout, bool mode) {
  if (layout.Contains("Help")) {
    path = layout_locn + layout + ".html";
    parent->startBrowser(path);
    return;
  }

  setSelection();

  wxString prefix = logbook->opt->engineStr[logbook->opt->engines] +
                    logbook->opt->layoutPrefix[LogbookDialog::LOGBOOK];
  if (logbook->opt->filterLayout[LogbookDialog::LOGBOOK])
    layout.Prepend(prefix);
  wxString file = toHTML(path, layout, mode);
  if (file != "") parent->startBrowser(file);
}

void LogbookHTML::setSelection() {
  int row = parent->selGridRow;
  int z = 0, r = 1, topRow, bottomRow, first, last;

  first = last = 0;

  if (parent->logGrids[0]->GetNumberRows() == 0) return;

  if (parent->logGrids[parent->m_notebook8->GetSelection()]->IsSelection()) {
    wxGridCellCoordsArray top =
        parent->logGrids[parent->m_notebook8->GetSelection()]
            ->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray bottom =
        parent->logGrids[parent->m_notebook8->GetSelection()]
            ->GetSelectionBlockBottomRight();
    if (top.Count() != 0) {
      topRow = top[0].GetRow();
      bottomRow = bottom[0].GetRow();
      r = bottomRow - topRow + 1;
    }
  }

  z = parent->m_gridGlobal->GetNumberRows();
  if (r == z) {
    parent->m_gridGlobal->SelectAll();
    parent->m_gridWeather->SelectAll();
    parent->m_gridMotorSails->SelectAll();
  } else {
    parent->m_gridGlobal->ClearSelection();
    parent->m_gridWeather->ClearSelection();
    parent->m_gridMotorSails->ClearSelection();
  }

  wxString selRoute = parent->m_gridGlobal->GetCellValue(row, 0);

  while (row > -1 && parent->m_gridGlobal->GetCellValue(row, 0) == selRoute)
    first = row--;
  row = parent->selGridRow;
  while (row < parent->m_gridGlobal->GetNumberRows() &&
         parent->m_gridGlobal->GetCellValue(row, 0) == selRoute)
    last = row++;

  for (row = first; row <= last; row++) {
    parent->m_gridGlobal->SelectRow(row, true);
    parent->m_gridWeather->SelectRow(row, true);
    parent->m_gridMotorSails->SelectRow(row, true);
  }
}

wxString LogbookHTML::toHTML(wxString path, wxString layout, bool mode) {
  wxArrayInt arrayRows;
  int count = 0, selCount = 0;
  bool selection = false;
  route = wxEmptyString;

  selCount = parent->m_gridGlobal->GetSelectedRows().Count();

  if (selCount > 0) {
    selection = true;
    arrayRows = parent->m_gridGlobal->GetSelectedRows();
  }

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return "";
  }

  wxString html = readLayoutFile(layout);

  wxString topHTML;
  wxString bottomHTML;
  wxString headerHTML;
  wxString middleHTML;

  wxString seperatorTop = "<!--Repeat -->";
  wxString seperatorBottom = "<!--Repeat End -->";
  wxString seperatorHeaderTop = "<!--Header -->";
  wxString seperatorHeaderBottom = "<!--Header end -->";

  int indexTop = html.Find(seperatorTop);
  if (!checkLayoutError(indexTop, seperatorTop, layout)) return "";
  indexTop += seperatorTop.Len();
  int indexBottom = html.Find(seperatorBottom);
  if (!checkLayoutError(indexBottom, seperatorBottom, layout)) return "";
  indexBottom += seperatorBottom.Len();
  int indexHeaderTop = html.Find(seperatorHeaderTop);
  if (!checkLayoutError(indexHeaderTop, seperatorHeaderTop, layout)) return "";
  indexHeaderTop += seperatorHeaderTop.Len();
  int indexHeaderBottom = html.Find(seperatorHeaderBottom);
  if (!checkLayoutError(indexHeaderBottom, seperatorHeaderBottom, layout))
    return "";
  indexHeaderBottom += seperatorHeaderBottom.Len();

  topHTML = html.substr(0, indexHeaderTop);
  bottomHTML = html.substr(indexBottom, html.Len() - 1);
  middleHTML = html.substr(indexTop, indexBottom - indexTop);
  headerHTML = html.substr(indexHeaderTop, indexHeaderBottom - indexHeaderTop);

  wxString filename = this->fileName;

  if (mode == false) {
    filename.Replace("txt", "html");
  } else
    filename = path;

  if (::wxFileExists(filename)) ::wxRemoveFile(filename);

  wxFileOutputStream output(filename);
  wxTextOutputStream htmlFile(output);

  wxString newMiddleHTML;

  topHTML.Replace("#LLOGBOOK#", parent->m_logbook->GetPageText(0), false);
  topHTML.Replace("#LFROM#", _("from"), false);
  topHTML.Replace("#LTO#", _("to"), false);
  topHTML.Replace("#SDATE#", parent->m_gridGlobal->GetCellValue(0, 1), false);
  topHTML.Replace("#EDATE#",
                  parent->m_gridGlobal->GetCellValue(
                      parent->m_gridGlobal->GetNumberRows() - 1, 1),
                  false);
  topHTML.Replace("#TYPE#", parent->boatType->GetValue(), false);
  topHTML.Replace("#BOATNAME#", parent->boatName->GetValue(), false);
  topHTML.Replace("#HOMEPORT#", parent->homeport->GetValue(), false);
  topHTML.Replace("#CALLSIGN#", parent->callsign->GetValue(), false);
  topHTML.Replace("#REGISTRATION#", parent->registration->GetValue(), false);
  topHTML.Replace("#LOCATION#", layout_locn + layout + ".html", false);
  htmlFile << topHTML;

  int rowsMax = parent->m_gridGlobal->GetNumberRows();
  for (int row = 0; row < rowsMax; row++) {
    count++;
    if (selection && arrayRows[0] + 1 > count) continue;
    if (selection && arrayRows[selCount - 1] + 1 < count) break;

    newMiddleHTML = middleHTML;

#ifdef __WXMSW__
    unsigned int first = 0, ofirst = 0;
#endif
    for (int grid = 0; grid < 3; grid++) {
      wxGrid *g = parent->logGrids[grid];
      for (int col = 0; col < g->GetNumberCols(); col++) {
        if (grid == 0 && col == 0) {
          replacePlaceholder(newMiddleHTML, headerHTML, grid, row, 0, 0,
                             htmlFile);
          continue;
        }
#ifdef __WXMSW__
        first = newMiddleHTML.find_first_of('#') - 1;
        if (first != ofirst) {
          htmlFile << newMiddleHTML.SubString(0, first);
          newMiddleHTML.Remove(0, first + 1);
          ofirst = first;
        }
#endif
        newMiddleHTML = replacePlaceholder(newMiddleHTML, headerHTML, grid, row,
                                           col, 0, htmlFile);
      }
    }
    htmlFile << newMiddleHTML;
  }
  htmlFile << bottomHTML;

  if (count <= 0) {
    wxMessageBox(_("Sorry, Logbook has no lines"), _("Information"), wxOK);
    return "";
  }

  output.Close();

  return filename;
}

wxString LogbookHTML::replacePlaceholder(wxString html, wxString htmlHeader,
                                         int grid, int row, int col, bool mode,
                                         wxTextOutputStream &htmlFile) {
  wxString s;
  wxGrid *g = parent->logGrids[grid];

  switch (grid) {
    case 0:
      switch (col) {
        case ROUTE:
          if (route !=
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false)) {
            htmlHeader.Replace(
                "#ROUTE#",
                Export::replaceNewLine(mode, g->GetCellValue(row, col), false));
            htmlHeader.Replace(
                "#LROUTE#",
                Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
                false);
            htmlFile << htmlHeader;
          }
          route =
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false);

          break;
        case RDATE:
          html.Replace(
              "#DATE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LDATE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          html.Replace("#NO.#", wxString::Format("%i", row + 1));
          break;
        case RTIME:
          html.Replace(
              "#TIME#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LTIME#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case STATUS:
          html.Replace(
              "#STATUS#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LSTATUS#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WAKE:
          html.Replace(
              "#WAKE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWAKE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case DISTANCE:
          html.Replace(
              "#DISTANCE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LDISTANCE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case DTOTAL:
          html.Replace(
              "#DTOTAL#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LDTOTAL#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case POSITION:
          html.Replace(
              "#POSITION#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LPOSITION#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case COG:
          html.Replace(
              "#COG#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LCOG#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case COW:
          html.Replace(
              "#COW#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LCOW#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case SOG:
          html.Replace(
              "#SOG#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LSOG#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case SOW:
          html.Replace(
              "#SOW#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LSOW#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case DEPTH:
          html.Replace(
              "#DEPTH#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LDEPTH#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case REMARKS:
          html.Replace(
              "#REMARKS#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LREMARKS#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
      }
      break;
    case 1:
      switch (col) {
        case BARO:
          html.Replace(
              "#BARO#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LBARO#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case HYDRO:
          html.Replace(
              "#HYDRO#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LHYDRO#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case AIRTE:
          html.Replace(
              "#AIRTE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LAIRTE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATERTE:
          html.Replace(
              "#WTE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWTE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WIND:
          html.Replace(
              "#WIND#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWIND#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WSPD:
          html.Replace(
              "#WSPD#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWSPD#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WINDR:
          html.Replace(
              "#WINDR#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWINDR#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WSPDR:
          html.Replace(
              "#WSPDR#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWSPDR#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case CURRENT:
          html.Replace(
              "#CUR#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LCUR#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case CSPD:
          html.Replace(
              "#CSPD#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LCSPD#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WAVE:
          html.Replace(
              "#WAVE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWAVE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case SWELL:
          html.Replace(
              "#SWELL#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LSWELL#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WEATHER:
          html.Replace(
              "#WEATHER#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWEATHER#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case CLOUDS:
          html.Replace("#CLOUDS#", Export::replaceNewLine(
                                       mode, g->GetCellValue(row, col), false));
          html.Replace("#LCLOUDS#", Export::replaceNewLine(
                                        mode, g->GetColLabelValue(col), true));
          {
            wxString cl =
                Export::replaceNewLine(mode, g->GetCellValue(row, col), false)
                    .Lower();
            if (cl.IsEmpty()) cl = "nocloud";
            if (cl.Contains(" "))
              cl = cl.SubString(0, cl.find_first_of(' ') - 1);
            if (html.Contains("#PCLOUDS%23")) {
              wxString s = parent->data;
              s.Replace("\\", "/");
              html.Replace("Clouds/#PCLOUDS%23", s + "Clouds/" + cl, false);
            } else
              html.Replace("#PCLOUDS#", cl, false);
          }
          break;
        case VISIBILITY:
          html.Replace(
              "#VISIBILITY#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LVISIBILITY#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
      }
      break;
    case 2:
      switch (col) {
        case MOTOR:
          html.Replace(
              "#MOTOR#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LMOTOR#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case MOTORT:
          html.Replace(
              "#MOTORT#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LMOTORT#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case RPM1:
          html.Replace(
              "#RPM1#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LRPM1#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case MOTOR1:
          html.Replace(
              "#MOTOR1#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LMOTOR1#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case MOTOR1T:
          html.Replace(
              "#MOTOR1T#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LMOTOR1T#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case RPM2:
          html.Replace(
              "#RPM2#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LRPM2#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case FUEL:
          html.Replace(
              "#FUEL#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LFUEL#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case FUELT:
          html.Replace(
              "#FUELT#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LFUELT#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case GENE:
          html.Replace(
              "#GENE#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LGENE#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case GENET:
          html.Replace(
              "#GENET#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LGENET#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case BANK1:
          html.Replace(
              "#BANK1#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LBANK1#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case BANK1T:
          html.Replace(
              "#BANK1T#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LBANK1T#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case BANK2:
          html.Replace(
              "#BANK2#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LBANK2#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case BANK2T:
          html.Replace(
              "#BANK2T#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LBANK2T#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case SAILS:
          html.Replace(
              "#SAILS#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LSAILS#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case REEF:
          html.Replace(
              "#REEF#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LREEF#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATERM:
          html.Replace(
              "#WATERM#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWATERM#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATERMT:
          html.Replace(
              "#WATERMT#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWATERMT#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATERMO:
          html.Replace(
              "#WATERMO#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWATERMO#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATER:
          html.Replace(
              "#WATER#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWATER#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case WATERT:
          html.Replace(
              "#WATERT#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LWATERT#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
        case MREMARKS:
          html.Replace(
              "#MREMARKS#",
              Export::replaceNewLine(mode, g->GetCellValue(row, col), false),
              false);
          html.Replace(
              "#LMREMARKS#",
              Export::replaceNewLine(mode, g->GetColLabelValue(col), true),
              false);
          break;
      }
      break;
  }

  if (mode == 0)
    return html;
  else {
#ifdef __WXOSX__
    wxString str(html.wx_str(), wxConvUTF8);
#else
    wxString str(html.wx_str(), wxConvUTF8);
#endif
    return str;
  }
}

bool LogbookHTML::checkLayoutError(int result, wxString html, wxString layout) {
  if (result == wxNOT_FOUND) {
    wxMessageBox(html + _("\nnot found in layoutfile ") + layout +
                     _("!\n\nDid you forget to add this line in your layout ?"),
                 _("Information"));
    return false;
  }
  return true;
}

void LogbookHTML::setFileName(wxString s, wxString l) {
  fileName = s;
  layout_locn = l;
}

wxString LogbookHTML::readLayoutFile(wxString layout) {
  wxString html;

  wxString filename = layout_locn + layout + ".html";
  wxTextFile *layoutfile = new wxTextFile(filename);

  if (wxFileExists(filename)) {
    layoutfile->Open();

    for (unsigned row = 0; row < layoutfile->GetLineCount(); row++) {
      html += layoutfile->GetLine(row) + "\n";
    }
  }
  layoutfile->Close();
  return html;
}

wxString LogbookHTML::readLayoutFileODT(wxString layout) {
  unique_ptr<wxZipEntry> entry;
  wxString odt = "";
  wxString filename = layout_locn + layout + ".odt";
  if (wxFileExists(filename)) {
    //#ifdef __WXOSX__

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
    /*        static const wxString fn = "content.xml";
                    wxFileInputStream in(filename);
            wxZipInputStream zip(in);
                    wxTextInputStream txt(zip);
                    while(!zip.Eof())
                            odt += txt.ReadLine();
    //#endif*/
  }
  return odt;
}

void LogbookHTML::viewODT(wxString path, wxString layout, bool mode) {
  wxString prefix = logbook->opt->engineStr[logbook->opt->engines] +
                    logbook->opt->layoutPrefix[LogbookDialog::LOGBOOK];
  if (logbook->opt->filterLayout[LogbookDialog::LOGBOOK])
    layout.Prepend(prefix);

  setSelection();

  wxString file = toODT(path, layout, mode);
  if (file != "") parent->startApplication(file, ".odt");
}

wxString LogbookHTML::toODT(wxString path, wxString layout, bool mode) {
  wxArrayInt arrayRows;
  int count = 0, selCount = 0;
  bool selection = false;
  route = wxEmptyString;

  selCount = parent->m_gridGlobal->GetSelectedRows().Count();

  if (selCount > 0) {
    selection = true;
    arrayRows = parent->m_gridGlobal->GetSelectedRows();
  }

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return "";
  }

  wxString odt = readLayoutFileODT(layout);

  if (!odt.Contains("[[") && !odt.Contains("{{")) {
    wxMessageBox(
        _("Have You forgotten to enclose the Header with [[ and ]]\n or Data "
          "with {{ and }} ?"));
    return "";
  }

  wxString topODT;
  wxString bottomODT;
  wxString headerODT;
  wxString middleODT;

  wxString seperatorTop = "{{";
  wxString seperatorBottom = "}}";
  wxString seperatorHeaderTop = "[[";
  wxString seperatorHeaderBottom = "]]";
  wxString textbegin = "<text:p";
  wxString textend = "</text:p";

  odt.Replace("#LLOGBOOK#", parent->m_logbook->GetPageText(0), false);
  odt.Replace("#LFROM#", _("from"), false);
  odt.Replace("#LTO#", _("to"), false);
  odt.Replace("#SDATE#", parent->m_gridGlobal->GetCellValue(0, 1), false);
  odt.Replace("#EDATE#",
              parent->m_gridGlobal->GetCellValue(
                  parent->m_gridGlobal->GetNumberRows() - 1, 1),
              false);
  odt.Replace("#TYPE#", parent->boatType->GetValue(), false);
  odt.Replace("#BOATNAME#", parent->boatName->GetValue(), false);
  odt.Replace("#HOMEPORT#", parent->homeport->GetValue(), false);
  odt.Replace("#CALLSIGN#", parent->callsign->GetValue(), false);
  odt.Replace("#REGISTRATION#", parent->registration->GetValue(), false);

  int indexTopODT = odt.Find(seperatorHeaderTop);
  int top = indexTopODT;
  topODT = odt.substr(0, indexTopODT);
  indexTopODT = topODT.find_last_of('<');
  topODT = topODT.substr(0, indexTopODT);
  odt = odt.substr(top);

  int headerStart = odt.find_first_of('>') + 1;
  odt = odt.substr(headerStart);

  int indexBottomODT = odt.Find(seperatorHeaderBottom);
  headerODT = odt.substr(0, indexBottomODT);
  odt = odt.substr(indexBottomODT);
  int headerEnd = odt.find_first_of('>') + 1;
  odt = odt.substr(headerEnd);
  indexBottomODT = headerODT.find_first_of('<');
  headerODT = headerODT.substr(indexBottomODT);
  headerODT = headerODT.substr(0, headerODT.find_last_of('<'));

  indexBottomODT = odt.Find(seperatorTop);
  middleODT = odt.substr(indexBottomODT);
  odt = odt.substr(indexBottomODT);

  int middleStart = middleODT.find_first_of('>') + 1;
  middleODT = middleODT.substr(middleStart);
  odt = odt.substr(odt.find_first_of('>') + 1);

  indexBottomODT = odt.Find(seperatorBottom);
  middleODT = odt.substr(0, indexBottomODT);
  odt = odt.substr(indexBottomODT);

  int middleEnd = odt.find_first_of('>') + 1;
  odt = odt.substr(middleEnd);
  middleODT = middleODT.substr(0, middleODT.find_last_of('<'));

  wxString filename = this->fileName;

  if (mode == false) {
    filename.Replace("txt", "odt");
  } else
    filename = path;

  if (::wxFileExists(filename)) ::wxRemoveFile(filename);

  unique_ptr<wxFFileInputStream> in(
      new wxFFileInputStream(layout_locn + layout + ".odt"));
  wxTempFileOutputStream out(filename);

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

  odtFile << topODT;

  wxString newMiddleODT;
  int rowsMax = parent->m_gridGlobal->GetNumberRows();
  for (int row = 0; row < rowsMax; row++) {
    count++;
    if (selection && arrayRows[0] + 1 > count) continue;
    if (selection && arrayRows[selCount - 1] + 1 < count) break;

    newMiddleODT = middleODT;
    for (int grid = 0; grid < 3; grid++) {
      wxGrid *g = parent->logGrids[grid];
      for (int col = 0; col < g->GetNumberCols(); col++)
        newMiddleODT = replacePlaceholder(newMiddleODT, headerODT, grid, row,
                                          col, 1, odtFile);
    }
    odtFile << newMiddleODT;
  }

  odtFile << odt;

  inzip.Eof() && outzip.Close() && out.Commit();
  return filename;
}

void LogbookHTML::toCSV(wxString path) {
  wxString s, temp;

  if (::wxFileExists(path)) ::wxRemoveFile(path);

  wxFileOutputStream output(path);
  wxTextOutputStream csvFile(output);

  for (int n = 0; n < parent->numPages; n++) {
    for (int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++) {
      wxString str = "\"" + parent->logGrids[n]->GetColLabelValue(i) + "\",";
      if (i == LogbookHTML::POSITION && n == 0) {
        csvFile << str;
        csvFile << str;
      } else
        csvFile << str;
    }
  }
  csvFile << "\n";
  for (int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++) {
    for (int grid = 0; grid < parent->numPages; grid++) {
      for (int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++) {
        temp = parent->logGrids[grid]->GetCellValue(row, col);
        if (col == LogbookHTML::POSITION && grid == 0) {
          wxStringTokenizer p(temp, "\n");
          wxString lat = p.GetNextToken();
          wxString lon = p.GetNextToken();
          temp = lat + "\",\"" + lon;
        }
        s += "\"" + temp + "\",";
      }
    }
    s.RemoveLast();
    csvFile << s + "\n";
    s = wxEmptyString;
  }
  output.Close();
}

void LogbookHTML::toXML(wxString path) {
  wxString s = "";
  wxString temp;

  if (::wxFileExists(path)) ::wxRemoveFile(path);

  wxFileOutputStream output(path);
  wxTextOutputStream xmlFile(output);

  xmlFile << parent->xmlHead;

  s = wxString::Format("<Row>");
  for (int n = 0; n < parent->numPages; n++) {
    for (int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++) {
      s += "<Cell>\n";
      s += "<Data ss:Type=\"String\">#DATA#</Data>\n";
      s.Replace("#DATA#", parent->logGrids[n]->GetColLabelValue(i));
      s += "</Cell>";
    }
  }
  s += "</Row>>";
  xmlFile << s;
  for (int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++) {
    xmlFile << wxString::Format("<Row ss:Height=\"%u\">",
                                parent->m_gridGlobal->GetRowHeight(row));
    for (int grid = 0; grid < parent->numPages; grid++) {
      for (int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++) {
        s = "<Cell>\n";
        s += "<Data ss:Type=\"String\">#DATA#</Data>\n";
        temp = parent->logGrids[grid]->GetCellValue(row, col);
        temp.Replace("&", "&amp;");
        temp.Replace("\\n", "&#xA;");
        temp.Replace("\"", "&quot;");
        temp.Replace("<", "&lt;");
        temp.Replace(">", "&gt;");
        temp.Replace("'", "&apos;");
        s.Replace("#DATA#", temp);
        s += "</Cell>";
        xmlFile << s;
      }
    }
    xmlFile << "</Row>>";
    ;
  }
  xmlFile << parent->xmlEnd;
  output.Close();
}

void LogbookHTML::toODS(wxString path) {
  wxString s = "";
  wxString line;
  wxString temp;

  wxFFileOutputStream out(path);
  wxZipOutputStream zip(out);
  wxTextOutputStream txt(zip);
  wxString sep(wxFileName::GetPathSeparator());

  zip.PutNextEntry("content.xml");
  txt << parent->content;

  txt << "<table:table-row table:style-name=\"ro2\">";

  for (int n = 0; n < parent->numPages; n++) {
    for (int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++) {
      txt << "<table:table-cell office:value-type=\"string\">";
      txt << "<text:p>";
      txt << parent->logGrids[n]->GetColLabelValue(i);
      txt << "</text:p>";
      txt << "</table:table-cell>";
    }
  }
  txt << "</table:table-row>";
  for (int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++) {
    txt << "<table:table-row table:style-name=\"ro2\">";
    for (int grid = 0; grid < parent->numPages; grid++) {
      for (int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++) {
        wxString s = parent->logGrids[grid]->GetCellValue(row, col);
        s.Replace("&", "&amp;");
        s.Replace("\"", "&quot;");
        s.Replace("<", "&lt;");
        s.Replace(">", "&gt;");
        s.Replace("'", "&apos;");

        txt << "<table:table-cell office:value-type=\"string\">";
        txt << "<text:p>";
        txt << s;
        txt << "</text:p>";
        txt << "</table:table-cell>";
      }
    }
    txt << "</table:table-row>";
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

void LogbookHTML::backup(wxString path) {
  logbook->update();
  wxCopyFile(data_locn + parent->backupFile + ".txt", path);
}

void LogbookHTML::toKML(wxString path) {
  wxString datetime, position, description, temp, folder, t, header,
      logpointName, route = "nil", oldroute, remarks, fRemarks, label, pathXML;
  wxString snil = "---";
  wxString trackID = wxEmptyString, trackOldID = wxEmptyString;
  wxString routeID = wxEmptyString, routeOldID = wxEmptyString;

  bool error = false, first = true, rfirst = true;
  wxDateTime dt;
  int maxRow = parent->m_gridGlobal->GetNumberRows(), row = 0;

  if (::wxFileExists(path)) ::wxRemoveFile(path);

  wxFileOutputStream output(path);
  wxTextOutputStream kmlFile1(output);

  kmlFile = &kmlFile1;

  wxString h = parent->kmlHead;
  h.Replace("#TITLE#", logbook->title);
  *kmlFile << h;
  h = parent->kmlLine;
  h.Replace("#LWIDTH#", logbook->opt->kmlLineWidth);
  h.Replace("#LTRANS#",
            logbook->opt->kmlTrans.Item(logbook->opt->kmlLineTransparancy));
  h.Replace("#LCOLORR#",
            logbook->opt->kmlColor.Item(logbook->opt->kmlRouteColor));
  h.Replace("#LCOLORT#",
            logbook->opt->kmlColor.Item(logbook->opt->kmlTrackColor));
  *kmlFile << h;

  for (; row < parent->m_gridGlobal->GetNumberRows(); row++) {
    temp = parent->kmlBody;
    folder = parent->kmlFolder;
    error = false;
    remarks = wxEmptyString;
    for (int grid = 0; grid < parent->numPages; grid++) {
      for (int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++) {
        wxString e = parent->logGrids[grid]->GetCellValue(row, col);
        if (grid == 0) {
          switch (col) {
            case LogbookHTML::ROUTE:
              // temp.Replace("#ROW#",wxString::Format(_("Row: %i"),row));
              if (e != route) {
                if (!first) (*kmlFile) << parent->kmlEndFolder;
                first = false;

                e = replaceKMLCharacters(e);
                folder.Replace("#NAME#", e);
                (*kmlFile) << folder;

                route = e;
                rfirst = true;
                routeID = wxEmptyString;

              } else {
                first = false;
                rfirst = false;
              }
              break;
            case LogbookHTML::RDATE:
              logpointName = e + " ";
              break;
            case LogbookHTML::RTIME:
              logpointName += e;
              break;
            case LogbookHTML::POSITION:
              if (e.IsEmpty()) {
                error = true;
                break;
              }
              position = e;
              e = replaceKMLCharacters(e);
              description += position + "\n";
              position = convertPositionToDecimalDegrees(position);
              temp.Replace("#POSITION#", position, false);
              break;
            case LogbookHTML::COG:
              label = parent->m_gridGlobal->GetColLabelValue(LogbookHTML::COG);
              label.Replace("\n", " ");
              if (e.IsEmpty()) e = snil;
              e = replaceKMLCharacters(e);
              description += label + " " + e + "  ";
              break;
            case LogbookHTML::SOG:
              label = parent->m_gridGlobal->GetColLabelValue(LogbookHTML::SOG);
              label.Replace("\n", " ");
              if (e.IsEmpty()) e = snil;
              e = replaceKMLCharacters(e);
              description += label + " " + e + "\n";
              break;
            case LogbookHTML::DEPTH:
              label =
                  parent->m_gridGlobal->GetColLabelValue(LogbookHTML::DEPTH);
              label.Replace("\n", " ");
              if (e.IsEmpty()) e = snil;
              e = replaceKMLCharacters(e);
              description += label + " " + e + "\n";
              break;
            case LogbookHTML::REMARKS:
              e = replaceKMLCharacters(e);
              fRemarks = e;
              if (rfirst) {
                remarks = wxEmptyString;  // e.SubString(0,50)+"...";
                folder.Replace("#CREATED#", remarks, false);
                rfirst = false;
              }
              break;
          }
        } else if (grid == 1) {
          switch (col) {
            case WIND:
              label = parent->m_gridWeather->GetColLabelValue(WIND);
              label.Replace("\n", " ");
              if (e.IsEmpty()) e = snil;
              e = replaceKMLCharacters(e);
              description += label + " " + e + "  ";
              break;
            case WSPD:
              label = parent->m_gridWeather->GetColLabelValue(WSPD);
              label.Replace("\n", " ");
              if (e.IsEmpty()) e = snil;
              e = replaceKMLCharacters(e);
              description += label + " " + e + "\n";
              break;
          }
        } else if (grid == 2) {
          switch (col) {
            case MREMARKS:
              e = replaceKMLCharacters(e);
              temp.Replace("#NAME#", logpointName);
              temp.Replace("#DESCRIPTION#",
                           description +
                               ((fRemarks.IsEmpty()) ? "" : "\n" + fRemarks) +
                               "\n" + e,
                           false);
              break;
            case ROUTEID:
              routeOldID = routeID;
              routeID = e;
              if ((logbook->opt->kmlRoute && !routeID.IsEmpty()) &&
                  (routeID != routeOldID)) {
                wxJSONWriter w;
                wxString out;
                wxJSONValue v;
                v["Route_ID"] = parent->logGrids[2]->GetCellValue(row, ROUTEID);
                w.Write(v, out);
                SendPluginMessage(wxString("OCPN_ROUTE_REQUEST"), out);

                ::wxSafeYield();
              }
              break;
            case TRACKID:
              trackOldID = trackID;
              trackID = e;
              if ((logbook->opt->kmlTrack && !trackID.IsEmpty()) &&
                  (trackID != trackOldID)) {
                wxJSONWriter w;
                wxString out;
                wxJSONValue v;
                v["Track_ID"] = parent->logGrids[2]->GetCellValue(row, TRACKID);
                w.Write(v, out);
                SendPluginMessage(wxString("OCPN_TRACK_REQUEST"), out);

                ::wxSafeYield();
              }
              break;
          }
        }
      }
    }
    if (!error) {
      temp.Replace(
          "#icon#",
          "http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png");
      (*kmlFile) << temp;
    }
    description = wxEmptyString;
  }
  if (row == maxRow - 1) *kmlFile << parent->kmlEndFolder;
  (*kmlFile) << parent->kmlEnd;
  output.Close();

  wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromExtension("kml");
  wxString cmd = filetype->GetOpenCommand(path);
  if (!cmd.IsEmpty()) wxExecute(cmd);
}

void LogbookHTML::writeTrackToKML(wxJSONValue data) {
  wxString trkLine = parent->kmlPathHeader;
  trkLine.Replace("#NAME#", "Trackline");
  if (kmlFile == nullptr) {
    return;
  }
  *kmlFile << trkLine;
  for (int i = 0; i < data.Size(); i++)
    (*kmlFile) << wxString::Format("%.13f,%.13f\n", data[i][1].AsDouble(),
                                   data[i][0].AsDouble());

  (*kmlFile) << parent->kmlPathFooter;
}

void LogbookHTML::writeRouteToKML(wxJSONValue data) {
  wxString routeLine = parent->kmlPathHeader;
  routeLine.Replace("#NAME#", "Routeline");
  routeLine.Replace("#LINE#", "#LineRoute");
  if (kmlFile == nullptr) {
    return;
  }
  *kmlFile << routeLine;

  for (int i = 0; i < data.Size(); i++)
    (*kmlFile) << wxString::Format("%f,%f\n", data[i]["lon"].AsDouble(),
                                   data[i]["lat"].AsDouble());

  (*kmlFile) << parent->kmlPathFooter;

  wxString n = parent->kmlFolder;
  n.Replace("#NAME#", _("Routepoints"));
  *kmlFile << n;

  for (int i = 0; i < data.Size(); i++) {
    wxString routeWP = parent->kmlBody;
    routeWP.Replace("#icon#",
                    "http://maps.google.com/mapfiles/kml/pal4/icon48.png");

    routeWP.Replace("#NAME#", data[i]["WPName"].AsString());
    wxString description = data[i]["WPDescription"].AsString() + "<br>";

    int li = 1;
    wxString links = wxEmptyString;
    wxString desc = wxEmptyString;

    while (true) {
      wxString count = wxString::Format("%d", li);

      if (data[i].HasMember("WPLink" + count))
        links = data[i]["WPLink" + count].AsString();
      else
        break;

      if (data[i].HasMember("WPLinkDesciption" + count)) {
        desc = data[i]["WPLinkDesciption" + count].AsString();
        description += "<a href=\"" + links + "\">" + desc + "</a><br>";
      } else

        break;
      li++;
    }
    routeWP.Replace("#DESCRIPTION#", " <![CDATA[\n" + description + "\n]]>");
    routeWP.Replace("#POSITION#",
                    wxString::Format("%f,%f\n", data[i]["lon"].AsDouble(),
                                     data[i]["lat"].AsDouble()));
    (*kmlFile) << routeWP;
  }
  *kmlFile << parent->kmlEndFolder;
}
/*
          +void LogbookHTML::createJumpTable()
          +{
          +	wxString path  = parent->basePath+"navobj.xml";
          +	wxString patho = parent->basePath+"navobj.xml.changes";
          +
          +	offsetChanges.clear();
          +	offsetNavobj.clear();
          +
          +	if(wxFile::Exists(patho))
          +		insertTracks(patho,&offsetChanges,&offsetChangesGuid);
          +	else
          +		insertTracks(path,&offsetNavobj,&offsetNavobjGuid);
          +
          +}
          +
          +void LogbookHTML::insertTracks(wxString file, std::map<wxString,long>
   *navobj, std::map<wxString,long> *navobjgui )
          +{
          +	wxString temp;
          +
          +	wxFileInputStream in(file);
          +	wxTextInputStream xml(in);
          +
          +	long i = -1, n = 0;
          +	while(!in.Eof())
          +	{
          +		temp = xml.ReadLine();
          +		i++;
          +		if(temp.Contains("<trk>"))
          +		{
          +			n = i;
          +			temp = xml.ReadLine();
          +			i++;
          +			if(temp.Contains("<name>"))
          +			{
          +				temp.Trim(false);
          +				temp.Replace("<name>","");
          +				temp.Replace("</name>","");
          +				navobj->insert(pair(temp,n));
          +			}
          +
          +			do{
          +				temp = xml.ReadLine();
          +				i++;
          +			}while(!temp.Contains("<opencpn:guid>"));
          +
          +			temp.Trim(false);
          +			temp = temp.AfterFirst('>');
          +			temp = temp.BeforeFirst('<');
          +			navobjgui->insert(pair(temp,n));
          +		}
          +	}
          +}
          +*/
wxString LogbookHTML::replaceKMLCharacters(wxString e) {
  e.Replace("\"", "&quot;");
  e.Replace("<", "&lt;");
  e.Replace(">", "&gt;");
  e.Replace("'", "&apos;");
  e.Replace("&", "&amp;");

  return e;
}
/*
          +wxString LogbookHTML::findTrackInXML(wxDateTime dt, wxString file,
   wxString *name, wxString route, wxString trackguid, long offset, bool f, bool
   mode)
          +{
          +	wxString temp,lat,lon,track = wxEmptyString, trkguid =
   wxEmptyString; +	bool first = true;
          +
          +	wxString path = parent->basePath + file;
          +	if(f ==	0)
          +			if(!wxFile::Exists(path)) return wxEmptyString;
          +
          +		wxTextFile in(path);
          +		in.Open();
          +		in.GoToLine(offset);
          +			while(!in.Eof())
          +			{
          +				temp = in.GetLine(offset);
          +				if(temp.Contains("<trk>"))
          +				{
          +					temp = in.GetNextLine();
          +					if(mode)
          +					{
          +					if(temp.Contains("<name>"))
          +						{
          + temp.Trim(false);
          +							temp =
   temp.AfterFirst('>');
          +							temp =
   temp.BeforeFirst('<');
          +							*name = temp;
          +						}
          +					}
          +					else
          +					{
          +						do{
          +							temp =
   in.GetNextLine(); +
   }while(!temp.Contains("<opencpn:guid>")); +
   temp.Trim(false); +						temp =
   temp.AfterFirst('>');
          +						trkguid =
   temp.BeforeFirst('<'); +					}
          +
          +					do
          +					{
          +						temp = in.GetNextLine();
          +						if(temp.Contains("</trk>"))
   return track;
          +
          + if(temp.Contains("<trkpt")) +
   { +							temp.Trim(false); +
   temp.Replace("<trkpt lat=",""); +
   temp.Replace("lon=",""); +
   temp.Replace("\"",""); +
   wxStringTokenizer tkz(temp," ");
          +							lon =
   tkz.GetNextToken();
          +							lat =
   tkz.GetNextToken().RemoveLast(); + }
          +
          + if(temp.Contains("<time>")) +
   { +							temp.Trim(false); +
   temp = temp.AfterFirst('>'); +
   temp = temp.BeforeFirst('<'); +
   temp.Replace("T"," ");
          +
          +							wxDateTime dtt;
          +
   dtt.ParseDateTime(temp.RemoveLast()); +
   //wxMessageBox(dtt.FormatDate()+" "+dtt.FormatTime()); +
   }
          +
          +							if(*name == route ||
   trkguid == trackguid) +
   track += lat+","+lon+"\n"; + }while(!in.Eof()); + } + } +	in.Close(); +
   return track;
          +}
          +
          +wxString LogbookHTML::getPathFromTrack(wxDateTime dt, wxString route,
   wxString trackguid, long offset, bool ind, bool mode)
          +{
          +	wxString name = wxEmptyString;
          +	wxString kmlData = wxEmptyString;
          +	wxString header = parent->kmlPathHeader;
          +
          +	if(!ind)
          +		kmlData = findTrackInXML(dt, "navobj.xml.changes", &name,
   route, trackguid, offset, ind, mode); +	else +		kmlData =
   findTrackInXML(dt, "navobj.xml", &name, route, trackguid, offset, ind, mode);
          +
          +	name = route;
          +	if(!kmlData.IsEmpty())
          +	{
          +		header.Replace("#NAME#",name);
          +		return header+kmlData+parent->kmlPathFooter;
          +	}
          +	else
          +		return wxEmptyString;
          +}
          +*/
wxString LogbookHTML::convertPositionToDecimalDegrees(wxString str) {
  wxString pos;

  wxStringTokenizer tkz(str, "\n");
  pos = positionToDecimalDegrees(tkz.GetNextToken());
  pos.Prepend(positionToDecimalDegrees(tkz.GetNextToken()) + ",");

  return pos;
}
wxString LogbookHTML::positionToDecimalDegrees(wxString str) {
  double deg, min, sec = 0;
  wxString ind;

  wxStringTokenizer tkz(str, " ");

  if (tkz.CountTokens() == 4) {
    deg = wxAtof(tkz.GetNextToken());
    min = wxAtof(tkz.GetNextToken());
    wxString t = tkz.GetNextToken();
    t.Replace(",", ".");
    sec = wxAtof(t);

    sec = min * 60 + sec;
    deg = deg + (sec / 3600);
  } else {
    deg = wxAtof(tkz.GetNextToken());
    wxString t = tkz.GetNextToken();
    t.Replace(",", ".");
    min = wxAtof(t);
    min = min / 60;
    deg += min;
  }

  ind = tkz.GetNextToken();
  if (ind == 'W' || ind == 'S') deg = -deg;

  return wxString::Format("%f", deg);
}
