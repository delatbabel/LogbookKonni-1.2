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


#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/generic/gridctrl.h>
#include <wx/mimetype.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "CrewList.h"
#include "Export.h"
#include "Logbook.h"
#include "LogbookDialog.h"
#include "Maintenance.h"
#include "Options.h"
#include "logbook_pi.h"


Maintenance::Maintenance(LogbookDialog* d, wxString data, wxString layout,
                         wxString layoutODT)
    : Export(d) {
  dialog = d;
  opt = dialog->logbookPlugIn->opt;
  this->layout_locn = layout;
  this->ODTLayout = layoutODT;
  grid = d->m_gridMaintanence;
  repairs = d->m_gridMaintanenceRepairs;
  buyparts = d->m_gridMaintenanceBuyParts;
  selectedCol = 0;
  selectedRow = 0;
  modified = false;
  modifiedR = false;
  modifiedB = false;

  green = wxColour(0, 255, 0);
  red = wxColour(255, 0, 0);
  redlight = wxColour(255, 128, 128);
  yellow = wxColour(255, 255, 0);
  yellowlight = wxColour(255, 255, 155);
  white = wxColour(255, 255, 255);

  wxString serviceData = data;
  wxTextFile* serviceFile;
  serviceData.Append("service.txt");
  wxFileName wxHomeFiledir = serviceData;
  if (!wxHomeFiledir.FileExists()) {
    serviceFile = new wxTextFile(serviceData);
    serviceFile->Create();
  } else
    serviceFile = new wxTextFile(serviceData);

  serviceFile->Close();
  data_locn = serviceData;

  wxString buyPartsData = data;
  wxTextFile* buyPartsFile;
  buyPartsData.Append("buyparts.txt");
  wxHomeFiledir = buyPartsData;
  if (!wxHomeFiledir.FileExists()) {
    buyPartsFile = new wxTextFile(buyPartsData);
    buyPartsFile->Create();
  } else
    buyPartsFile = new wxTextFile(buyPartsData);

  buyPartsFile->Close();
  data_locnBuyParts = buyPartsData;

  wxString repairsData = data;
  wxTextFile* repairsFile;
  repairsData.Append("repairs.txt");
  wxHomeFiledir = repairsData;
  if (!wxHomeFiledir.FileExists()) {
    repairsFile = new wxTextFile(repairsData);
    repairsFile->Create();
  } else
    repairsFile = new wxTextFile(repairsData);

  repairsFile->Close();
  data_locnRepairs = repairsData;

  m_choices[0] =
      dialog->m_gridGlobal->GetColLabelValue(6) + " +";  // Distance/T
  m_choices[1] = wxString(_("Engine ")) +
                 dialog->m_gridMotorSails->GetColLabelValue(1) +
                 " +";  // Motor1/h
  m_choices[2] = wxString(_("Engine ")) +
                 dialog->m_gridMotorSails->GetColLabelValue(4) +
                 " +";  // Motor2/h
  m_choices[3] =
      dialog->m_gridMotorSails->GetColLabelValue(11) + " +";  // Generator/h
  m_choices[4] =
      dialog->m_gridMotorSails->GetColLabelValue(13) + " <=";  // Bank1/AH
  m_choices[5] =
      dialog->m_gridMotorSails->GetColLabelValue(15) + " <=";  // Bank2/AH
  m_choices[6] =
      dialog->m_gridMotorSails->GetColLabelValue(17) + " +";  // Watermaker/h
  m_choices[7] = dialog->m_gridGlobal->GetColLabelValue(3);   // Status
  m_choices[8] = _("Fix Date");
  m_choices[9] = _("Date + Days");
  m_choices[10] = _("Date + Weeks");
  m_choices[11] = _("Date + Months");
  m_choicesCount = 12;

  m_YesNo[0] = _("Yes");
  m_YesNo[1] = _("No");

  m_Priority[0] = "0";
  m_Priority[1] = "1";
  m_Priority[2] = "2";
  m_Priority[3] = "3";
  m_Priority[4] = "4";
  m_Priority[5] = "5";
}

Maintenance::~Maintenance(void) {
  update();
  updateRepairs();
  updateBuyParts();
}

void Maintenance::setLayoutLocation() {
  wxString layout_locn;
  bool radio = dialog->m_radioBtnHTMLBuyParts->GetValue();

  if (radio)
    layout_locnBuyParts = this->layout_locn;
  else
    layout_locnBuyParts = ODTLayout;

  wxString buypartsLay = layout_locnBuyParts;

  buypartsLay.Append("buyparts");
  dialog->appendOSDirSlash(&buypartsLay);
  layout_locnBuyParts = buypartsLay;
  dialog->loadLayoutChoice(LogbookDialog::GBUYPARTS, buypartsLay,
                           dialog->m_choiceSelectLayoutBuyParts,
                           opt->layoutPrefix[LogbookDialog::GBUYPARTS]);
  if (radio)
    dialog->m_choiceSelectLayoutBuyParts->SetSelection(
        dialog->logbookPlugIn->opt->buypartsGridLayoutChoice);
  else
    dialog->m_choiceSelectLayoutBuyParts->SetSelection(
        dialog->logbookPlugIn->opt->buypartsGridLayoutChoiceODT);

  radio = dialog->m_radioBtnHTMLService->GetValue();
  if (radio)
    layout_locnService = this->layout_locn;
  else
    layout_locnService = ODTLayout;

  wxString serviceLay = layout_locnService;

  serviceLay.Append("service");
  dialog->appendOSDirSlash(&serviceLay);
  layout_locnService = serviceLay;
  dialog->loadLayoutChoice(LogbookDialog::GSERVICE, serviceLay,
                           dialog->m_choiceSelectLayoutService,
                           opt->layoutPrefix[LogbookDialog::GSERVICE]);
  if (radio)
    dialog->m_choiceSelectLayoutService->SetSelection(
        dialog->logbookPlugIn->opt->serviceGridLayoutChoice);
  else
    dialog->m_choiceSelectLayoutService->SetSelection(
        dialog->logbookPlugIn->opt->serviceGridLayoutChoiceODT);

  radio = dialog->m_radioBtnHTMLRepairs->GetValue();
  if (radio)
    layout_locnRepairs = this->layout_locn;
  else
    layout_locnRepairs = ODTLayout;

  wxString repairsLay = layout_locnRepairs;

  repairsLay.Append("repairs");
  dialog->appendOSDirSlash(&repairsLay);
  layout_locnRepairs = repairsLay;
  dialog->loadLayoutChoice(LogbookDialog::GREPAIRS, repairsLay,
                           dialog->m_choiceSelectLayoutRepairs,
                           opt->layoutPrefix[LogbookDialog::GREPAIRS]);
  if (radio)
    dialog->m_choiceSelectLayoutRepairs->SetSelection(
        dialog->logbookPlugIn->opt->repairsGridLayoutChoice);
  else
    dialog->m_choiceSelectLayoutRepairs->SetSelection(
        dialog->logbookPlugIn->opt->repairsGridLayoutChoiceODT);
}

void Maintenance::setAlignmentService() {
  grid->SetReadOnly(lastRow, START);
  grid->SetReadOnly(lastRow, PRIORITY);
  grid->SetCellAlignment(lastRow, PRIORITY, wxALIGN_CENTER, wxALIGN_TOP);
  grid->SetCellAlignment(lastRow, TEXT, wxALIGN_LEFT, wxALIGN_TOP);
  grid->SetCellAlignment(lastRow, IF, wxALIGN_CENTER, wxALIGN_TOP);
  grid->SetCellAlignment(lastRow, ACTIVE, wxALIGN_CENTER, wxALIGN_TOP);
}

void Maintenance::addLine() {
  modified = true;
  grid->AppendRows();

  lastRow = grid->GetNumberRows() - 1;
  selectedRow = lastRow;
  setAlignmentService();

  grid->SetCellEditor(lastRow, IF,
                      (new myGridCellChoiceEditor(12, m_choices, false)));

  myGridCellBoolEditor* boolEditor = new myGridCellBoolEditor();
  boolEditor->UseStringValues(_("Yes"), _("No"));
  grid->SetCellEditor(lastRow, ACTIVE, boolEditor);

  grid->SetCellValue(lastRow, PRIORITY, "5");
  grid->SetCellValue(lastRow, IF, m_choices[0]);
  grid->SetCellValue(lastRow, WARN, "1");
  grid->SetCellValue(lastRow, URGENT, "2");
  cellCollChanged(IF, lastRow);
  cellCollChanged(WARN, lastRow);
  checkService(dialog->m_gridGlobal->GetNumberRows() - 1);
  grid->SetCellBackgroundColour(lastRow, START, wxColour(240, 240, 240));

  grid->SetCellValue(lastRow, ACTIVE, _("Yes"));
}

void Maintenance::addLineRepairs() {
  modified = true;
  repairs->AppendRows();

  lastRowRepairs = repairs->GetNumberRows() - 1;
  selectedRowRepairs = lastRowRepairs;
  setAlignmentRepairs();

  repairs->SetCellValue(lastRowRepairs, RPRIORITY, "0");
  checkRepairs();
}

void Maintenance::addLineBuyParts() {
  modified = true;
  buyparts->AppendRows();

  lastRowBuyParts = buyparts->GetNumberRows() - 1;
  selectedRowBuyParts = lastRowBuyParts;
  setAlignmentBuyParts();

  buyparts->SetCellValue(lastRowBuyParts, PPRIORITY, "0");
  checkBuyParts();
}

void Maintenance::setAlignmentRepairs() {
  repairs->SetCellAlignment(lastRowRepairs, RPRIORITY, wxALIGN_CENTER,
                            wxALIGN_TOP);
  repairs->SetCellAlignment(lastRowRepairs, RTEXT, wxALIGN_LEFT, wxALIGN_TOP);
  repairs->SetCellEditor(lastRowRepairs, RTEXT,
                         new wxGridCellAutoWrapStringEditor);
  repairs->SetCellEditor(lastRowRepairs, RPRIORITY,
                         new myGridCellChoiceEditor(6, m_Priority, false));
}

void Maintenance::setAlignmentBuyParts() {
  buyparts->SetCellEditor(lastRowBuyParts, PPRIORITY,
                          new myGridCellChoiceEditor(6, m_Priority, false));

  buyparts->SetCellEditor(lastRowBuyParts, PARTS,
                          new wxGridCellAutoWrapStringEditor);
  buyparts->SetCellAlignment(lastRowBuyParts, PPRIORITY, wxALIGN_CENTER,
                             wxALIGN_TOP);
  buyparts->SetCellAlignment(lastRowBuyParts, PCATEGORY, wxALIGN_CENTER,
                             wxALIGN_TOP);
  buyparts->SetCellAlignment(lastRowBuyParts, TITLE, wxALIGN_LEFT, wxALIGN_TOP);
  buyparts->SetCellAlignment(lastRowBuyParts, PARTS, wxALIGN_LEFT, wxALIGN_TOP);
  buyparts->SetCellAlignment(lastRowBuyParts, DATE, wxALIGN_CENTER,
                             wxALIGN_TOP);
  buyparts->SetCellAlignment(lastRowBuyParts, AT, wxALIGN_LEFT, wxALIGN_TOP);
}

void Maintenance::loadData() {
  wxString t, s;

  wxFileInputStream input(data_locn);
  wxTextInputStream* stream = new wxTextInputStream(input, "\n", wxConvUTF8);

  int row = 0;
  int sel = -1;

  dialog->m_gridMaintanence->BeginBatch();
  while (!input.Eof()) {
    t = stream->ReadLine();
    if (input.Eof()) break;
    if (t.Contains("#1.2#")) {
      continue;
    }
    addLine();

    wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;
    while (tkz.HasMoreTokens()) {
      s = dialog->restoreDangerChar(tkz.GetNextToken());
      s.RemoveLast();

      switch (c) {
        case TEXT:
          grid->SetCellValue(row, TEXT, s);
          break;
        case IF:
          sel = getSelection(s);
          grid->SetCellValue(row, IF, s);
          break;
        case WARN:
          if (sel == 8) s = getDateString(s);
          grid->SetCellValue(row, WARN, s);
          break;
        case URGENT:
          if (sel == 8) s = getDateString(s);
          grid->SetCellValue(row, URGENT, s);
          break;
        case START:
          if (sel > 8) s = getDateString(s);
          grid->SetCellValue(row, START, s);
          break;
        case ACTIVE:
          grid->SetCellValue(row, ACTIVE, s);
          break;
      }
      c++;
    }
    row++;
  }
  dialog->m_gridMaintanence->EndBatch();
  wxFileInputStream input1(data_locnBuyParts);
  wxTextInputStream* stream1 = new wxTextInputStream(input1, "\n", wxConvUTF8);

  row = 0;

  while (true) {
    t = stream1->ReadLine();
    if (t.Contains("#1.2#")) {
      continue;
    }
    if (input1.Eof()) break;
    addLineBuyParts();

    wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;
    while (tkz.HasMoreTokens()) {
      s = dialog->restoreDangerChar(tkz.GetNextToken());
      s.RemoveLast();

      switch (c) {
        case PPRIORITY:
          buyparts->SetCellValue(row, PPRIORITY, s);
          break;
        case PCATEGORY:
          buyparts->SetCellValue(row, PCATEGORY, s);
          break;
        case TITLE:
#ifdef __WXOSX__
          s = wxString(s.To8BitData(), wxConvUTF8);
#endif
          buyparts->SetCellValue(row, TITLE, s);
          break;
        case PARTS:
#ifdef __WXOSX__
          s = wxString(s.To8BitData(), wxConvUTF8);
#endif
          buyparts->SetCellValue(row, PARTS, s);
          break;
        case DATE:
          buyparts->SetCellValue(row, DATE, getDateString(s));
          break;
        case AT:
          buyparts->SetCellValue(row, AT, s);
          break;
      }
      c++;
    }
    buyparts->AutoSizeRow(row, false);
    row++;
  }

  wxFileInputStream input2(data_locnRepairs);
  wxTextInputStream* stream2 = new wxTextInputStream(input2, "\n", wxConvUTF8);

  row = 0;
  while (true) {
    t = stream2->ReadLine();
    if (input2.Eof()) break;
    if (t.Contains("#1.2#")) {
      continue;
    }

    addLineRepairs();

    wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;
    while (tkz.HasMoreTokens()) {
      s = dialog->restoreDangerChar(tkz.GetNextToken());
      s.RemoveLast();

      switch (c) {
        case RPRIORITY:
          repairs->SetCellValue(row, RPRIORITY, s);
          break;
        case RTEXT:
#ifdef __WXOSX__
          s = wxString(s.To8BitData(), wxConvUTF8);
#endif
          repairs->SetCellValue(row, RTEXT, s);
          break;
      }
      c++;
    }
    repairs->AutoSizeRow(row, false);
    row++;
  }

  checkService(dialog->m_gridGlobal->GetNumberRows() - 1);
  checkRepairs();
  //	checkBuyParts();
  modified = false;
}

int Maintenance::getSelection(wxString s) {
  for (int i = 0; i < dialog->maintenance->m_choicesCount; i++) {
    if (s == dialog->maintenance->m_choices[i]) return i;
  }
  return -1;
}

wxString Maintenance::getDateString(wxString s) {
  if (s.IsEmpty() || s.GetChar(0) == ' ') return "";

  wxStringTokenizer tkz(s, "/");
  int month = wxAtoi(tkz.GetNextToken());
  int day = wxAtoi(tkz.GetNextToken());
  int year = wxAtoi(tkz.GetNextToken());
  wxDateTime dt;
  dt.Set(day, (wxDateTime::Month)month, year);

  return wxString::Format("%s", dt.Format(opt->sdateformat).c_str());
}

void Maintenance::buyParts(int i) {
  wxString s;
  wxGrid* grid;
  int text, selectedRow;

  if (i == 0) {
    s = _("Service");
    grid = this->grid;
    text = TEXT;
    selectedRow = this->selectedRow;
  } else {
    s = _("Repairs");
    grid = repairs;
    text = RTEXT;
    selectedRow = selectedRowRepairs;
  }

  addLineBuyParts();

  dialog->m_gridMaintenanceBuyParts->SetCellValue(
      lastRowBuyParts, PPRIORITY, grid->GetCellValue(selectedRow, PRIORITY));
  dialog->m_gridMaintenanceBuyParts->SetCellValue(lastRowBuyParts, PCATEGORY,
                                                  s);
  dialog->m_gridMaintenanceBuyParts->SetCellValue(
      lastRowBuyParts, TITLE, grid->GetCellValue(selectedRow, text).Trim());
  checkBuyParts();

  dialog->m_notebook6->SetSelection(2);
}

void Maintenance::setRowBackground(int row, wxColour& c) {
  for (int i = 0; i < grid->GetNumberCols(); i++)
    grid->SetCellBackgroundColour(row, i, c);

  if (c == wxColour(255, 0, 0))
    grid->SetCellValue(row, PRIORITY, "1");
  else if (c == wxColour(255, 255, 0))
    grid->SetCellValue(row, PRIORITY, "3");
  else if (c == wxColour(0, 255, 0))
    grid->SetCellValue(row, PRIORITY, "5");
  else if (c == wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW))
    grid->SetCellValue(row, PRIORITY, "0");
}

void Maintenance::checkService(int row) {
  if (dialog->m_gridGlobal->GetNumberRows() == 0) return;

  wxString date;
  wxDateTime dtstart, dturgent, dtwarn;
  wxDateSpan spanu, spanw;
  wxString g, yesno;
  int choice = -1;
  int col = 0;
  double startValue, warnValue, urgentValue;
  double distanceTotal, motorTotal, motorTotal2, generator, watermaker, bank1,
      bank2;
  wxString sign, cell;
  int border = 0;
  wxColour rowBack;
  //	int sailscol = dialog->logbook->sailsCol;

  for (int r = 0; r < grid->GetNumberRows(); r++) {
    g = grid->GetCellValue(r, IF);

    yesno = grid->GetCellValue(r, ACTIVE);
    if (g.IsEmpty()) continue;

    cell = grid->GetCellValue(r, START);
    cell.ToDouble(&startValue);
    cell = grid->GetCellValue(r, WARN);
    cell.ToDouble(&warnValue);
    cell = grid->GetCellValue(r, URGENT);
    cell.ToDouble(&urgentValue);

    cell = dialog->m_gridGlobal->GetCellValue(row, 6);
    cell.ToDouble(&distanceTotal);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::MOTORT);
    cell.ToDouble(&motorTotal);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::MOTOR1T);
    cell.ToDouble(&motorTotal2);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::GENET);
    cell.ToDouble(&generator);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::WATERMT);
    cell.ToDouble(&watermaker);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::BANK1T);
    cell.ToDouble(&bank1);
    cell = dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::BANK2T);
    cell.ToDouble(&bank2);

    if (g == m_choices[0]) {
      choice = 0;
      col = LogbookHTML::DTOTAL;
    } else if (g == m_choices[1]) {
      choice = 1;
      col = LogbookHTML::MOTORT;
    } else if (g == m_choices[2]) {
      choice = 2;
      col = LogbookHTML::MOTOR1T;
    } else if (g == m_choices[3]) {
      choice = 3;
      col = LogbookHTML::GENET;
    } else if (g == m_choices[4]) {
      choice = 4;
      col = LogbookHTML::BANK1T;
    } else if (g == m_choices[5]) {
      choice = 5;
      col = LogbookHTML::BANK2T;
    } else if (g == m_choices[6]) {
      choice = 6;
      col = LogbookHTML::WATERMT;
    } else if (g == m_choices[7]) {
      choice = 7;
      col = LogbookHTML::STATUS;
    } else if (g == m_choices[8])
      choice = 8;
    else if (g == m_choices[9])
      choice = 9;
    else if (g == m_choices[10])
      choice = 10;
    else if (g == m_choices[11])
      choice = 11;

    if (yesno == _("Yes"))  // Active set to 'yes'
    {
      switch (choice) {
        case 0:  // Distance
          if (distanceTotal >= startValue + urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (distanceTotal >= startValue + warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridGlobal->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 1:  // Engine #1
          if (motorTotal >= startValue + urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (motorTotal >= startValue + warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 2:  // Engine #2
          if (motorTotal2 >= startValue + urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (motorTotal2 >= startValue + warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 3:  // Generator
          if (generator >= startValue + urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (generator >= startValue + warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 4:  // Bank #1
          if (bank1 <= urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (bank1 <= warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 5:  // Bank #2
          if (bank2 <= urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (bank2 <= warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          } else {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 6:  // WaterMaker
          if (watermaker >= startValue + urgentValue) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          } else if (watermaker >= startValue + warnValue) {
            if (border != 2) border = 1;
            rowBack = yellow;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, yellow);
            break;
          }
          {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 7:  // Status
          if (grid->GetCellValue(r, URGENT) ==
              dialog->m_gridGlobal->GetCellValue(row, LogbookHTML::STATUS)) {
            border = 2;
            rowBack = red;
            dialog->m_gridMotorSails->SetCellBackgroundColour(row, col, red);
            break;
          }
          {
            rowBack = green;
            for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
              dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
          }
          break;
        case 8: {
          date = grid->GetCellValue(r, URGENT);
          dialog->myParseDate(date, dturgent);
          date = grid->GetCellValue(r, WARN);
          dialog->myParseDate(date, dtwarn);
          dtstart = wxDateTime::Now();

          if (dtstart >= dturgent) {
            border = 2;
            rowBack = red;
            break;
          } else if (dtstart >= dtwarn) {
            if (border != 2) border = 1;
            rowBack = yellow;
            break;
          } else
            rowBack = green;
        } break;
        case 9: {
          long days;
          date = dialog->m_gridMaintanence->GetCellValue(r, START);
          dialog->myParseDate(date, dtstart);

          grid->GetCellValue(r, WARN).ToLong(&days);
          spanw.SetDays((int)days);
          dtwarn = dtstart;
          dtwarn += spanw;
          grid->GetCellValue(r, URGENT).ToLong(&days);
          spanu.SetDays((int)days);
          dturgent = dtstart;
          dturgent += spanu;
          dtstart = wxDateTime::Now();

          if (dtstart >= dturgent) {
            border = 2;
            rowBack = red;
            break;
          } else if (dtstart >= dtwarn) {
            if (border != 2) border = 1;
            rowBack = yellow;
            break;
          } else
            rowBack = green;
        } break;
        case 10: {
          long weeks;
          date = dialog->m_gridMaintanence->GetCellValue(r, START);
          dialog->myParseDate(date, dtstart);

          grid->GetCellValue(r, WARN).ToLong(&weeks);
          spanw.SetWeeks((int)weeks);
          dtwarn = dtstart;
          dtwarn += spanw;
          grid->GetCellValue(r, URGENT).ToLong(&weeks);
          spanu.SetWeeks((int)weeks);
          dturgent = dtstart;
          dturgent += spanu;
          dtstart = wxDateTime::Now();

          if (dtstart >= dturgent) {
            border = 2;
            rowBack = red;
            break;
          } else if (dtstart >= dtwarn) {
            if (border != 2) border = 1;
            rowBack = yellow;
            break;
          } else
            rowBack = green;
        } break;
        case 11: {
          long month;
          date = dialog->m_gridMaintanence->GetCellValue(r, START);
          dialog->myParseDate(date, dtstart);

          grid->GetCellValue(r, WARN).ToLong(&month);
          spanw.SetMonths((int)month);
          dtwarn = dtstart;
          dtwarn += spanw;
          grid->GetCellValue(r, URGENT).ToLong(&month);
          spanu.SetMonths((int)month);
          dturgent = dtstart;
          dturgent += spanu;
          dtstart = wxDateTime::Now();

          if (dtstart >= dturgent) {
            border = 2;
            rowBack = red;
            break;
          } else if (dtstart >= dtwarn) {
            if (border != 2) border = 1;
            rowBack = yellow;
            break;
          } else
            rowBack = green;
        } break;
      }
      setRowBackground(r, rowBack);
    } else {
      setRowBackground(r, white);
      for (int i = 0; i < dialog->m_gridMotorSails->GetNumberRows(); i++)
        dialog->m_gridMotorSails->SetCellBackgroundColour(i, col, white);
    }
    setBuyPartsPriority(grid, r, PRIORITY, TEXT);
  }

  checkBuyParts();

  switch (border) {
    case 0:
      dialog->SetBackgroundColour(dialog->defaultBackground);
      break;
    case 1:
      dialog->SetBackgroundColour(yellow);
      break;
    case 2:
      dialog->SetBackgroundColour(red);
      break;
  }
  dialog->Refresh();
}

void Maintenance::checkRepairs() {
  for (int row = 0; row < repairs->GetNumberRows(); row++) {
    long i;
    repairs->GetCellValue(row, RPRIORITY).ToLong(&i);
    switch (i) {
      case 0:
        setRowBackgroundRepairs(row, white);
        break;
      case 1:
        setRowBackgroundRepairs(row, red);
        break;
      case 2:
        setRowBackgroundRepairs(row, redlight);
        break;
      case 3:
        setRowBackgroundRepairs(row, yellow);
        break;
      case 4:
        setRowBackgroundRepairs(row, yellowlight);
        break;
      case 5:
        setRowBackgroundRepairs(row, green);
        break;
    }
    setBuyPartsPriority(repairs, row, RPRIORITY, RTEXT);
  }
  repairs->Refresh();
  checkBuyParts();
}

void Maintenance::checkBuyParts() {
  for (int row = 0; row < buyparts->GetNumberRows(); row++) {
    long i;
    buyparts->GetCellValue(row, PPRIORITY).ToLong(&i);
    switch (i) {
      case 0:
        setRowBackgroundBuyParts(row, white);
        break;
      case 1:
        setRowBackgroundBuyParts(row, red);
        break;
      case 2:
        setRowBackgroundBuyParts(row, redlight);
        break;
      case 3:
        setRowBackgroundBuyParts(row, yellow);
        break;
      case 4:
        setRowBackgroundBuyParts(row, yellowlight);
        break;
      case 5:
        setRowBackgroundBuyParts(row, green);
        break;
    }
  }
  buyparts->Refresh();
}

void Maintenance::setBuyPartsPriority(wxGrid* grid, int row, int p, int t) {
  wxString priority = grid->GetCellValue(row, p);
  for (int i = 0; i < buyparts->GetNumberRows(); i++) {
    if (buyparts->GetCellValue(i, TITLE).Trim() ==
        grid->GetCellValue(row, t).Trim())
      buyparts->SetCellValue(i, PRIORITY, grid->GetCellValue(row, p));
  }
}

void Maintenance::setRowDone(int row) {
  //	int sailscol = dialog->logbook->sailsCol;
  wxString g = grid->GetCellValue(selectedRow, IF);
  int choice = -1;

  if (g == m_choices[0])
    choice = 0;
  else if (g == m_choices[1])
    choice = 1;
  else if (g == m_choices[2])
    choice = 2;
  else if (g == m_choices[3])
    choice = 3;
  else if (g == m_choices[4])
    choice = 4;
  else if (g == m_choices[5])
    choice = 5;
  else if (g == m_choices[6])
    choice = 6;
  else if (g == m_choices[7])
    choice = 7;
  else if (g == m_choices[8])
    choice = 8;
  else if (g == m_choices[9])
    choice = 9;
  else if (g == m_choices[10])
    choice = 10;
  else if (g == m_choices[11])
    choice = 11;

  switch (choice) {
    case 0:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridGlobal->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::DTOTAL));
      break;
    case 1:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::MOTORT));
      break;
    case 2:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::MOTOR1T));
      break;
    case 3:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::GENET));
      break;
    case 4:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::BANK1T));
      break;
    case 5:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::BANK2T));
      break;
    case 6:
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridMotorSails->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::WATERMT));
      break;
    case 7:
      grid->SetCellValue(selectedRow, ACTIVE, _("No"));
      checkService(dialog->m_gridGlobal->GetNumberRows() - 1);
      break;
    case 8:
      grid->SetCellValue(selectedRow, ACTIVE, _("No"));
      grid->SetCellValue(selectedRow, WARN,
                         wxDateTime::Now().Format(opt->sdateformat));
      grid->SetCellValue(selectedRow, URGENT,
                         wxDateTime::Now().Format(opt->sdateformat));
    case 9:
    case 10:
    case 11:
      grid->SetCellValue(selectedRow, START,
                         wxDateTime::Now().Format(opt->sdateformat));
      break;
  }

  if (grid->GetCellValue(row, ACTIVE) == m_YesNo[0])
    setRowBackground(row, green);
  else
    setRowBackground(selectedRow, white);
  grid->Refresh();
}

void Maintenance::setRepairDone(int row) {
  repairs->SetCellValue(row, RPRIORITY, "0");
  setRowBackgroundRepairs(row, white);
  checkBuyParts();

  modified = true;
}

void Maintenance::setRowBackgroundRepairs(int row, wxColour& c) {
  for (int i = 0; i < repairs->GetNumberCols(); i++)
    repairs->SetCellBackgroundColour(row, i, c);
}

void Maintenance::setRowBackgroundBuyParts(int row, wxColour& c) {
  for (int i = 0; i < buyparts->GetNumberCols(); i++)
    buyparts->SetCellBackgroundColour(row, i, c);
}

void Maintenance::cellCollChanged(int col, int row) {
  //	int sailscol = dialog->logbook->sailsCol;

  if (dialog->m_gridGlobal->GetNumberRows() == 0) return;

  if (col == IF) {
    wxString g = grid->GetCellValue(selectedRow, IF);

    if (g == m_choices[0])  // Distance
    {
      grid->SetCellValue(
          selectedRow, START,
          dialog->m_gridGlobal->GetCellValue(
              dialog->m_gridGlobal->GetNumberRows() - 1, LogbookHTML::DTOTAL));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    } else if (g == m_choices[1])  // Engine #1
    {
      grid->SetCellValue(selectedRow, START,
                         dialog->m_gridMotorSails->GetCellValue(
                             dialog->m_gridMotorSails->GetNumberRows() - 1,
                             LogbookHTML::MOTORT));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    } else if (g == m_choices[2])  // Engine #2
    {
      grid->SetCellValue(selectedRow, START,
                         dialog->m_gridMotorSails->GetCellValue(
                             dialog->m_gridMotorSails->GetNumberRows() - 1,
                             LogbookHTML::MOTOR1T));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    } else if (g == m_choices[3])  // Generator
    {
      grid->SetCellValue(selectedRow, START,
                         dialog->m_gridMotorSails->GetCellValue(
                             dialog->m_gridMotorSails->GetNumberRows() - 1,
                             LogbookHTML::GENET));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    } else if (g == m_choices[4])  // Bank #1
    {
      grid->SetCellValue(
          selectedRow, START,
          wxString::Format("%s %s", opt->bank1.c_str(), opt->ampereh.c_str()));
      grid->SetCellValue(
          selectedRow, WARN,
          wxString::Format("%i", (int)(wxAtoi(opt->bank1) / 100 * 25)));
      grid->SetCellValue(
          selectedRow, URGENT,
          wxString::Format("%i", (int)(wxAtoi(opt->bank1) / 100 * 15)));
    } else if (g == m_choices[5])  // Bank #2
    {
      grid->SetCellValue(
          selectedRow, START,
          wxString::Format("%s %s", opt->bank2.c_str(), opt->ampereh.c_str()));
      grid->SetCellValue(
          selectedRow, WARN,
          wxString::Format("%i", (int)(wxAtoi(opt->bank2) / 100 * 25)));
      grid->SetCellValue(
          selectedRow, URGENT,
          wxString::Format("%i", (int)(wxAtoi(opt->bank2) / 100 * 15)));
    } else if (g == m_choices[6])  // Watermaker
    {
      grid->SetCellValue(selectedRow, START,
                         dialog->m_gridMotorSails->GetCellValue(
                             dialog->m_gridMotorSails->GetNumberRows() - 1,
                             LogbookHTML::WATERMT));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    } else if (g == m_choices[7])  // Status
    {
      grid->BeginBatch();
      grid->SetCellValue(selectedRow, WARN, "");
      grid->EndBatch();
    } else if (g == m_choices[8]) {
      grid->SetCellValue(selectedRow, START, "");
      grid->SetCellValue(selectedRow, WARN,
                         (wxDateTime::Now().Add(wxDateSpan(0, 0, 0, 1)))
                             .Format(opt->sdateformat));
      grid->SetCellValue(selectedRow, URGENT,
                         (wxDateTime::Now().Add(wxDateSpan(0, 0, 0, 1)))
                             .Format(opt->sdateformat));
    } else if (g == m_choices[9] || g == m_choices[10] || g == m_choices[11]) {
      grid->SetCellValue(selectedRow, START,
                         wxDateTime::Now().Format(opt->sdateformat));
      grid->SetCellValue(selectedRow, WARN, "1");
      grid->SetCellValue(selectedRow, URGENT, "2");
    }
    col = WARN;
  }

  if (col == WARN || col == URGENT) {
    wxString ss;

    wxString g = grid->GetCellValue(row, IF);

    if (g == m_choices[0] || g == m_choices[1] ||  // Distance || Engine #1
        g == m_choices[2] || g == m_choices[3] ||  // Engine #2 || Generator
        g == m_choices[4] || g == m_choices[5] ||  // Bank 1 || Bank2
        g == m_choices[6])                         // Watermaker
    {
      wxString s = grid->GetCellValue(row, START);
      s = s.substr(s.find_last_of(' '));
      wxDouble d;
      grid->GetCellValue(row, col).ToDouble(&d);
      ss = wxString::Format("%5.0f %s", d, s.c_str());
    } else if (g == m_choices[9] || g == m_choices[10] || g == m_choices[11]) {
      wxString s;
      if (g == m_choices[4])
        s = dialog->logbookPlugIn->opt->days;
      else if (g == m_choices[5])
        s = dialog->logbookPlugIn->opt->weeks;
      else if (g == m_choices[6])
        s = dialog->logbookPlugIn->opt->month;
      wxDouble d;
      grid->GetCellValue(row, col).ToDouble(&d);
      ss = wxString::Format("%5.0f %s", d, s.c_str());
    } else {
      ss = grid->GetCellValue(row, col).Upper();
    }

    if (col == WARN) {
      grid->SetCellValue(row, WARN, ss);
      grid->SetCellValue(row, URGENT, ss);
    } else
      grid->SetCellValue(row, URGENT, ss);
  }

  if (col == ACTIVE) {
    if (grid->GetCellValue(row, ACTIVE) == m_YesNo[0] &&
        grid->GetCellValue(row, PRIORITY) == "0")
      grid->SetCellValue(row, PRIORITY, "5");
    else if (grid->GetCellValue(row, ACTIVE) == m_YesNo[1])
      grid->SetCellValue(row, PRIORITY, "0");

    setBuyPartsPriority(grid, row, PRIORITY, TEXT);
  }
}

void Maintenance::cellSelected(int col, int row) {
  selectedCol = col;
  selectedRow = row;
}

void Maintenance::update() {
  if (!modified) return;

  wxString s = "#1.2#\n", temp;

  wxString newLocn = data_locn;
  newLocn.Replace("txt", "Bak");
  wxRename(data_locn, newLocn);

  wxFileOutputStream output(data_locn);
  wxTextOutputStream* stream =
      new wxTextOutputStream(output, wxEOL_NATIVE, wxConvUTF8);

  stream->WriteString(s);
  s = wxEmptyString;
  int count = grid->GetNumberRows();
  for (int r = 0; r < count; r++) {
    int sel = -1;
    for (int c = 0; c < grid->GetNumberCols(); c++) {
      if (c < 2 || c == 6)
        temp = grid->GetCellValue(r, c);
      else if (c == 2) {
        temp = grid->GetCellValue(r, c);
        sel = getSelection(grid->GetCellValue(r, c));
      } else if (c == 3) {
        if (sel == 8) {
          wxDateTime dt;
          temp = grid->GetCellValue(r, c);
          LogbookDialog::myParseDate(temp, dt);
          temp = wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                  dt.GetYear());
        } else
          temp = grid->GetCellValue(r, c);
      } else if (c == 4) {
        if (sel == 8) {
          wxDateTime dt;
          temp = grid->GetCellValue(r, c);
          LogbookDialog::myParseDate(temp, dt);
          temp = wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                  dt.GetYear());
        } else
          temp = grid->GetCellValue(r, c);
      } else if (c == 5) {
        if (sel > 8) {
          wxDateTime dt;
          temp = grid->GetCellValue(r, c);
          LogbookDialog::myParseDate(temp, dt);
          temp = wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                  dt.GetYear());
        } else
          temp = grid->GetCellValue(r, c);
      }
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }
    s.RemoveLast();
    stream->WriteString(s + "\n");
    s = "";
  }
  output.Close();
  //	modified = false;
}

void Maintenance::updateRepairs() {
  if (!modifiedR) return;
  wxString s = "", temp;

  wxString newLocn = data_locnRepairs;
  newLocn.Replace("txt", "Bak");
  wxRename(data_locnRepairs, newLocn);

  wxFileOutputStream output(data_locnRepairs);
  wxTextOutputStream* stream =
      new wxTextOutputStream(output, wxEOL_NATIVE, wxConvUTF8);

  int count = dialog->m_gridMaintanenceRepairs->GetNumberRows();
  for (int r = 0; r < count; r++) {
    for (int c = 0; c < dialog->m_gridMaintanenceRepairs->GetNumberCols();
         c++) {
      temp = dialog->m_gridMaintanenceRepairs->GetCellValue(r, c);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }
    s.RemoveLast();
    stream->WriteString(s + "\n");
    s = "";
  }
  output.Close();
  //	modified = false;
}

void Maintenance::updateBuyParts() {
  if (!modifiedB) return;
  wxString s = "", temp;

  wxString newLocn = data_locnBuyParts;
  newLocn.Replace("txt", "Bak");
  wxRename(data_locnBuyParts, newLocn);

  wxFileOutputStream output(data_locnBuyParts);
  wxTextOutputStream* stream =
      new wxTextOutputStream(output, wxEOL_NATIVE, wxConvUTF8);

  int count = dialog->m_gridMaintenanceBuyParts->GetNumberRows();
  for (int r = 0; r < count; r++) {
    for (int c = 0; c < dialog->m_gridMaintenanceBuyParts->GetNumberCols();
         c++) {
      temp = dialog->m_gridMaintenanceBuyParts->GetCellValue(r, c);
      s += dialog->replaceDangerChar(temp);
      if (c == DATE && (!temp.IsEmpty() && temp.GetChar(0) != ' ')) {
        wxDateTime dt;
        dialog->myParseDate(temp.RemoveLast(), dt);
        s.Replace(temp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                         dt.GetYear()));
      }

      s += " \t";
    }
    s.RemoveLast();
    stream->WriteString(s + "\n");
    s = "";
  }
  output.Close();
  modified = false;
}

void Maintenance::viewODT(int tab, wxString path, wxString layout, int mode) {
  wxString locn, fn;

  if (tab == dialog->SERVICE) {
    locn = layout_locnService;
    fn = data_locn;
    if (opt->filterLayout[LogbookDialog::GSERVICE])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GSERVICE]);
  } else if (tab == dialog->REPAIRS) {
    locn = layout_locnRepairs;
    fn = data_locnRepairs;
    if (opt->filterLayout[LogbookDialog::GREPAIRS])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GREPAIRS]);
  } else if (tab == dialog->BUYPARTS) {
    locn = this->layout_locnBuyParts;
    fn = data_locnBuyParts;
    if (opt->filterLayout[LogbookDialog::GBUYPARTS])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GBUYPARTS]);
  }

  toODT(tab, locn, layout, mode);

  if (layout != "") {
    fn.Replace("txt", "odt");
    dialog->startApplication(fn, ".odt");
  }
}

void Maintenance::viewHTML(int tab, wxString path, wxString layout, int mode) {
  wxString locn, fn;

  if (tab == dialog->SERVICE) {
    locn = layout_locnService;
    fn = data_locn;
    if (opt->filterLayout[LogbookDialog::GSERVICE])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GSERVICE]);
  } else if (tab == dialog->REPAIRS) {
    locn = layout_locnRepairs;
    fn = data_locnRepairs;
    if (opt->filterLayout[LogbookDialog::GREPAIRS])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GREPAIRS]);
  } else if (tab == dialog->BUYPARTS) {
    locn = this->layout_locnBuyParts;
    fn = data_locnBuyParts;
    if (opt->filterLayout[LogbookDialog::GBUYPARTS])
      layout.Prepend(opt->layoutPrefix[LogbookDialog::GBUYPARTS]);
  }

  toHTML(tab, locn, layout, mode);

  if (layout != "") {
    fn.Replace("txt", "html");
    dialog->startBrowser(fn);
  }
}

wxString Maintenance::toHTML(int tab, wxString path, wxString layout,
                             int mode) {
  wxString top;
  wxString header;
  wxString middle;
  wxString bottom;

  wxString layout_loc;
  wxGrid* grid = NULL;

  wxString savePath = path;

  if (tab == dialog->SERVICE) {
    path = data_locn;
    layout_loc = layout_locnService;
    grid = this->grid;
  } else if (tab == dialog->REPAIRS) {
    path = data_locnRepairs;
    layout_loc = layout_locnRepairs;
    grid = repairs;
  } else if (tab == dialog->BUYPARTS) {
    path = this->data_locnBuyParts;
    layout_loc = layout_locnBuyParts;
    grid = buyparts;
  }

  wxString tempPath = path;

  wxString html = readLayoutHTML(layout_loc, layout);
  html = replaceLabels(html, grid);

  if (!cutInPartsHTML(html, &top, &header, &middle, &bottom)) return "";

  wxTextFile* text = setFiles(savePath, &tempPath, mode);

  writeToHTML(text, grid, tempPath, layout_loc + layout + ".html", top, header,
              middle, bottom, mode);

  return tempPath;
}

wxString Maintenance::replaceLabels(wxString s, wxGrid* grid) {
  if (grid == this->grid) {
    s.Replace("#LSERVICE#", dialog->m_notebook6->GetPageText(0));
    s.Replace("#LPRIORITY#", grid->GetColLabelValue(0));
    s.Replace("#LTEXT#", grid->GetColLabelValue(1));
    s.Replace("#LIF#", grid->GetColLabelValue(2));
    s.Replace("#LWARN#", grid->GetColLabelValue(3));
    s.Replace("#LURGENT#", grid->GetColLabelValue(4));
    s.Replace("#LSTART#", grid->GetColLabelValue(5));
    s.Replace("#LACTIVE#", grid->GetColLabelValue(6));
  } else if (grid == repairs) {
    s.Replace("#LREPAIRS#", dialog->m_notebook6->GetPageText(1));
    s.Replace("#LPRIORITY#", grid->GetColLabelValue(0));
    s.Replace("#LTEXT#", grid->GetColLabelValue(1));
  } else if (grid == buyparts) {
    s.Replace("#LBUYPARTS#", dialog->m_notebook6->GetPageText(2));
    s.Replace("#LPRIORITY#", grid->GetColLabelValue(0));
    s.Replace("#LCATEGORY#", grid->GetColLabelValue(1));
    s.Replace("#LTITLE#", grid->GetColLabelValue(2));
    s.Replace("#LBUYPARTS#", grid->GetColLabelValue(3));
    s.Replace("#LDATE#", grid->GetColLabelValue(4));
    s.Replace("#LAT#", grid->GetColLabelValue(5));
  }

  return s;
}

wxString Maintenance::toODT(int tab, wxString path, wxString layout, int mode) {
  wxString top;
  wxString header;
  wxString middle;
  wxString bottom;

  wxString layout_loc;
  wxGrid* grid = NULL;

  wxString savePath = path;

  if (tab == dialog->SERVICE) {
    path = data_locn;
    layout_loc = layout_locnService;
    grid = this->grid;
  } else if (tab == dialog->REPAIRS) {
    path = data_locnRepairs;
    layout_loc = layout_locnRepairs;
    grid = repairs;
  } else if (tab == dialog->BUYPARTS) {
    path = this->data_locnBuyParts;
    layout_loc = layout_locnBuyParts;
    grid = buyparts;
  }

  wxString tempPath = path;

  wxString odt = readLayoutODT(layout_loc, layout);
  odt = replaceLabels(odt, grid);

  if (!cutInPartsODT(odt, &top, &header, &middle, &bottom)) return "";

  wxTextFile* text = setFiles(savePath, &tempPath, mode);
  writeToODT(text, grid, tempPath, layout_loc + layout + ".odt", top, header,
             middle, bottom, mode);

  return tempPath;
}

wxString Maintenance::setPlaceHolders(int mode, wxGrid* grid, int row,
                                      wxString middleODT) {
  wxString s;

  if (grid == this->grid)
    s = setPlaceHoldersService(mode, grid, row, middleODT);
  else if (grid == repairs)
    s = setPlaceHoldersRepairs(mode, grid, row, middleODT);
  else if (grid == buyparts)
    s = setPlaceHoldersBuyParts(mode, grid, row, middleODT);

  return s;
}

wxString Maintenance::setPlaceHoldersService(int mode, wxGrid* grid, int row,
                                             wxString middleODT) {
  wxString newMiddleODT;

  newMiddleODT = middleODT;
  newMiddleODT.Replace("#PRIORITY#",
                       replaceNewLine(mode, grid->GetCellValue(row, PRIORITY)));
  newMiddleODT.Replace("#TEXT#",
                       replaceNewLine(mode, grid->GetCellValue(row, TEXT)));
  newMiddleODT.Replace("#IF#",
                       replaceNewLine(mode, grid->GetCellValue(row, IF)));
  newMiddleODT.Replace("#WARN#",
                       replaceNewLine(mode, grid->GetCellValue(row, WARN)));
  newMiddleODT.Replace("#URGENT#",
                       replaceNewLine(mode, grid->GetCellValue(row, URGENT)));
  newMiddleODT.Replace("#START#",
                       replaceNewLine(mode, grid->GetCellValue(row, START)));
  newMiddleODT.Replace("#ACTIVE#",
                       replaceNewLine(mode, grid->GetCellValue(row, ACTIVE)));

  return newMiddleODT;
}

wxString Maintenance::setPlaceHoldersRepairs(int mode, wxGrid* grid, int row,
                                             wxString middleODT) {
  wxString newMiddleODT;

  newMiddleODT = middleODT;
  newMiddleODT.Replace(
      "#PRIORITY#", replaceNewLine(mode, grid->GetCellValue(row, RPRIORITY)));
  newMiddleODT.Replace("#REPAIRSTEXT#",
                       replaceNewLine(mode, grid->GetCellValue(row, RTEXT)));

  return newMiddleODT;
}

wxString Maintenance::setPlaceHoldersBuyParts(int mode, wxGrid* grid, int row,
                                              wxString middleODT) {
  wxString newMiddleODT;
  newMiddleODT = middleODT;

  newMiddleODT.Replace(
      "#PRIORITY#", replaceNewLine(mode, grid->GetCellValue(row, PPRIORITY)));
  newMiddleODT.Replace(
      "#CATEGORY#", replaceNewLine(mode, grid->GetCellValue(row, PCATEGORY)));
  newMiddleODT.Replace("#TITLE#",
                       replaceNewLine(mode, grid->GetCellValue(row, TITLE)));
  newMiddleODT.Replace("#BUYPARTS#",
                       replaceNewLine(mode, grid->GetCellValue(row, PARTS)));
  newMiddleODT.Replace("#DATE#",
                       replaceNewLine(mode, grid->GetCellValue(row, DATE)));
  newMiddleODT.Replace("#AT#",
                       replaceNewLine(mode, grid->GetCellValue(row, AT)));

  return newMiddleODT;
}

wxString Maintenance::replaceNewLine(int mode, wxString str) {
  switch (mode) {
    case 0:
      // HTML
      str.Replace("\n", "<br>");
      break;
    case 1:  // ODT
      str.Replace("\n", "<text:line-break/>");
      break;
  }

  return str;
}

wxString Maintenance::readLayoutHTML(wxString path1, wxString layoutFileName) {
  wxString html, path;

  path = path1 + layoutFileName + ".html";
  ;
  wxTextFile layout(path);

  layout.Open();

  for (unsigned int i = 0; i < layout.GetLineCount(); i++) {
    html += layout.GetLine(i) + "\n";
  }

  layout.Close();

  return html;
}

void Maintenance::deleteFindItRow(wxString category, wxString plugin) {
  int row = 0;
  while (row < buyparts->GetNumberRows()) {
    if (buyparts->GetCellValue(row, PCATEGORY).Contains(category) &&
        buyparts->GetCellValue(row, TITLE).Contains(plugin))
      buyparts->DeleteRows(row);
    else
      row++;
  }
}

void Maintenance::showDateDialog(int row, int col, wxGrid* grid) {
  DateDialog* d = new DateDialog(grid);
  if (d->ShowModal() == wxID_OK) {
    wxDateTime date = d->m_calendar2->GetDate();
    grid->SetCellValue(row, col, date.Format(opt->sdateformat));
  }
}
/////////////////////////////////////
// DateDialog
/////////////////////////////////////
DateDialog::DateDialog(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer21;
  bSizer21 = new wxBoxSizer(wxVERTICAL);

  m_calendar2 =
      new wxCalendarCtrl(this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition,
                         wxDefaultSize, wxCAL_SHOW_HOLIDAYS);
  bSizer21->Add(m_calendar2, 1, wxALL | wxEXPAND, 5);

  m_sdbSizer6 = new wxStdDialogButtonSizer();
  m_sdbSizer6OK = new wxButton(this, wxID_OK);
  m_sdbSizer6->AddButton(m_sdbSizer6OK);
  m_sdbSizer6Cancel = new wxButton(this, wxID_CANCEL);
  m_sdbSizer6->AddButton(m_sdbSizer6Cancel);
  m_sdbSizer6->Realize();
  bSizer21->Add(m_sdbSizer6, 0, wxALIGN_CENTER, 5);

  this->SetSizer(bSizer21);
  this->Layout();

  this->Centre(wxBOTH);

  // Connect Events
  m_calendar2->Connect(wxEVT_CALENDAR_SEL_CHANGED,
                       wxCalendarEventHandler(DateDialog::OnCalenderSelChanged),
                       NULL, this);
}

DateDialog::~DateDialog() {
  // Disconnect Events
  m_calendar2->Disconnect(
      wxEVT_CALENDAR_SEL_CHANGED,
      wxCalendarEventHandler(DateDialog::OnCalenderSelChanged), NULL, this);
}
