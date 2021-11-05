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

#include "Logbook.h"
#include "CrewList.h"
#include "Export.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Options.h"

#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/mimetype.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>


using namespace std;

bool ActualWatch::active = false;
unsigned int ActualWatch::day = 0;
int ActualWatch::col = -1;
wxTimeSpan ActualWatch::time = wxTimeSpan();
wxDateTime ActualWatch::start = wxDateTime();
wxDateTime ActualWatch::end = wxDateTime();
wxString ActualWatch::member = wxEmptyString;
wxArrayString ActualWatch::menuMembers = wxArrayString();

CrewList::CrewList(LogbookDialog* d, wxString data, wxString layout,
                   wxString layoutODT) {
  dialog = d;
  opt = d->logbookPlugIn->opt;
  gridCrew = d->m_gridCrew;
  gridWake = d->m_gridCrewWake;
  rowHeight = gridCrew->GetRowHeight(0);
  selRow = 0;
  selCol = 0;
  day = 0;
  ActualWatch::end = wxDateTime::Now() + wxDateSpan(1);

  this->layout = layout;
  this->ODTLayout = layoutODT;
  modified = false;

  if (dialog->m_radioBtnHTMLCrew->GetValue())
    layout_locn = layout;
  else
    layout_locn = layoutODT;

  wxString watchData = data;
  watchData.Append("watchlist.txt");
  wxFileName wxHomeFiledir(watchData);
  if (true != wxHomeFiledir.FileExists()) {
    watchListFile = new wxTextFile(watchData);
    watchListFile->Create();
    //		watchListFile->AddLine("#1.2#");
  } else
    watchListFile = new wxTextFile(watchData);

  wxString crewData = data;
  crewData.Append("crewlist.txt");
  wxFileName wxHomeFiledir1(crewData);
  if (true != wxHomeFiledir1.FileExists()) {
    crewListFile = new wxTextFile(crewData);
    crewListFile->Create();
  } else
    crewListFile = new wxTextFile(crewData);

  wxString crewLay = layout_locn;
  crewLay.Append("crew");
  dialog->appendOSDirSlash(&crewLay);

  data_locn = crewData;
  layout_locn = crewLay;
  html_locn = data_locn;
  html_locn.Replace("txt", "html");

  setLayoutLocation(layout_locn);

  gridWakeInit();
}

CrewList::~CrewList(void) { saveData(); }

void CrewList::gridWakeInit() {
  gridCrew->EnableDragCell();
  gridCrew->GetGridWindow()->SetDropTarget(new DnDCrew(gridCrew, this));

  gridWake->EnableDragCell();
  gridWake->GetGridWindow()->SetDropTarget(new DnDWatch(gridWake, this));

  gridWake->AutoSizeColumns();
  gridWake->AutoSizeRows();

  firstColumn();

#ifdef __WXGTK__
  dialog->m_splitterWatch->SetSashPosition(1);
#else
  dialog->m_splitterWatch->SetSashPosition(160);
#endif

  statustext[0] =
      _("Enter default watchtime e.g. 3.30 / available formats are 3.30, 3,30, "
        "3:30, 0330 for 3 hours 30 minutes");
  statustext[1] =
      _("Alter watchtimes as desired / Drag 'n Drop members from the Crewlist");
  statustext[2] =
      _("*Optional* Prepend a * to a member to make this member static to a "
        "watch / Click Calculate");
  statustext[3] = _("All changes depending to this day only.");
}

void CrewList::firstColumn() {
  gridWake->SetCellEditor(3, 0, new wxGridCellAutoWrapStringEditor);
  dialog->m_textCtrlWatchStartTime->SetValue("08:00");
  dialog->m_textCtrlWatchStartDate->SetValue(
      wxDateTime::Now().Format(dialog->logbookPlugIn->opt->sdateformat));

  gridWake->SetCellValue(
      0, 0,
      wxString::Format("00:00%s", dialog->logbookPlugIn->opt->motorh.c_str()));
  wxDateTime dt, e;
  dt = wxDateTime::Now();
  e = dt;
  dt.Set(8, 0);
  e.Set(7, 59);
  gridWake->SetCellValue(
      1, 0, wxDateTime::Now().Format(dialog->logbookPlugIn->opt->sdateformat));
  gridWake->SetCellValue(
      2, 0,
      wxString::Format(
          "%s-%s", dt.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
          e.Format(dialog->logbookPlugIn->opt->stimeformatw)
              .c_str()));  //"08:00-07:59");
  gridWake->SetCellValue(3, 0, wxEmptyString);

  gridWake->SetReadOnly(1, 0);
  gridWake->SetReadOnly(2, 0);
  gridWake->AutoSizeColumns();
}

void CrewList::setLayoutLocation(wxString loc) {
  bool radio = dialog->m_radioBtnHTMLCrew->GetValue();
  if (radio)
    layout_locn = layout;
  else
    layout_locn = ODTLayout;
  wxString crewLay = layout_locn;

  crewLay.Append("crew");
  dialog->appendOSDirSlash(&crewLay);
  layout_locn = crewLay;
  dialog->loadLayoutChoice(LogbookDialog::CREW, crewLay, dialog->crewChoice,
                           opt->layoutPrefix[LogbookDialog::CREW]);
  if (radio)
    dialog->crewChoice->SetSelection(
        dialog->logbookPlugIn->opt->crewGridLayoutChoice);
  else
    dialog->crewChoice->SetSelection(
        dialog->logbookPlugIn->opt->crewGridLayoutChoiceODT);
}

void CrewList::loadData() {
  wxString s, line;
  wxGrid* grid;
  int lineCount, numRows;
  bool newCol;

  if (gridCrew->GetNumberRows() > 0)
    gridCrew->DeleteRows(0, gridCrew->GetNumberRows());

  crewListFile->Open();
  lineCount = crewListFile->GetLineCount();

  if (lineCount <= 0) {
    crewListFile->Close();
    return;
  }

  for (int i = 0; i < lineCount; i++) {
    line = crewListFile->GetLine(i);
    if (line.Contains("#1.2#")) continue;
    gridCrew->AppendRows();
    numRows = gridCrew->GetNumberRows() - 1;
    myGridCellBoolEditor* boolEditor = new myGridCellBoolEditor();
    boolEditor->UseStringValues(_("Yes"));
    gridCrew->SetCellEditor(numRows, 0, boolEditor);
    gridCrew->SetCellAlignment(numRows, 0, wxALIGN_CENTRE, wxALIGN_CENTER);

    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);
    int c;
    int colCount = gridCrew->GetNumberCols();
    if (tkz.CountTokens() != (unsigned int)colCount) {
      c = 1;
      newCol = true;
    } else {
      c = 0;
      newCol = false;
    }

    grid = gridCrew;

    int cell = 0;
    while (tkz.HasMoreTokens()) {
      s = tkz.GetNextToken().RemoveLast();
      s = dialog->restoreDangerChar(s);

      if (cell == BIRTHDATE && (!s.IsEmpty() && s.GetChar(0) != ' '))
        s = dialog->maintenance->getDateString(s);
      if (cell == EST_ON && (!s.IsEmpty() && s.GetChar(0) != ' '))
        s = dialog->maintenance->getDateString(s);
      grid->SetCellValue(numRows, c++, s);
      cell++;
      if (c == colCount) break;
    }

    if (newCol) {
      modified = true;
      grid->SetCellValue(numRows, 0, _("Yes"));
    }
  }

  crewListFile->Close();
  gridCrew->ForceRefresh();

  watchListFile->Open();
  lineCount = watchListFile->GetLineCount();

  if (lineCount <= 6) {
    watchListFile->Close();
    statusText(DEFAULTWATCH);
    return;
  }

  line = watchListFile->GetFirstLine();
  wxStringTokenizer tkz(line, "\t");
  s = tkz.GetNextToken();
  if (s != "#1.2#") return;
  s = tkz.GetNextToken();
  dialog->m_textCtrlWatchStartDate->SetValue(s);
  s = tkz.GetNextToken();
  dialog->m_textCtrlWatchStartTime->SetValue(s);
  s = tkz.GetNextToken();
  if (s == "1") setDayButtons(true);
  dialog->m_buttonReset->Enable();
  s = tkz.GetNextToken();
  dialog->m_textCtrlWakeTrip->SetValue(s);

  setMembersInMenu();

  dayNow(false);
  if (day != 0) {
    dialog->m_textCtrlWatchStartDate->Enable(false);
    dialog->m_textCtrlWatchStartTime->Enable(false);
    dialog->m_textCtrlWakeTrip->Enable(false);
  }
}

void CrewList::filterCrewMembers() {
  int i = 0;

  for (int row = 0; row < gridCrew->GetNumberRows(); row++) {
    if (gridCrew->GetCellValue(row, ONBOARD) == "") {
      gridCrew->SetRowSize(row, 0);
      i++;
    }
  }

  if (i == gridCrew->GetNumberRows()) {
    showAllCrewMembers();
    dialog->m_menu2->Check(MENUCREWONBOARD, false);
    dialog->m_menu2->Check(MENUCREWALL, true);
  } else
    gridCrew->ForceRefresh();
}

void CrewList::showAllCrewMembers() {
  for (int row = 0; row < gridCrew->GetNumberRows(); row++) {
    if (gridCrew->GetCellValue(row, ONBOARD) == "")
      gridCrew->SetRowSize(row, rowHeight);
  }

  gridCrew->ForceRefresh();
}

void CrewList::saveData() {
  if (!modified) return;
  modified = false;

  wxString s = "";
  crewListFile->Open();
  crewListFile->Clear();

  int count = gridCrew->GetNumberRows();
  crewListFile->AddLine("#1.2#");
  for (int r = 0; r < count; r++) {
    for (int c = 0; c < gridCrew->GetNumberCols(); c++) {
      wxString temp = gridCrew->GetCellValue(r, c);
      if (c == BIRTHDATE && (!temp.IsEmpty() && temp.GetChar(0) != ' ')) {
        wxDateTime dt;
        dialog->myParseDate(temp, dt);
        s += wxString::Format("%i/%i/%i \t", dt.GetMonth(), dt.GetDay(),
                              dt.GetYear());
      } else if (c == EST_ON && (!temp.IsEmpty() && temp.GetChar(0) != ' ')) {
        wxDateTime dt;
        dialog->myParseDate(temp, dt);
        s += wxString::Format("%i/%i/%i \t", dt.GetMonth(), dt.GetDay(),
                              dt.GetYear());
      } else
        s += temp + " \t";
    }
    s.RemoveLast();
    crewListFile->AddLine(s);
    s = "";
  }
  crewListFile->Write();
  crewListFile->Close();

  watchListFile->Close();
}

void CrewList::addCrew(wxGrid* grid, wxGrid* wake) {
  wxString s;

  modified = true;

  gridCrew->AppendRows();

  int numRows = gridCrew->GetNumberRows() - 1;
  myGridCellBoolEditor* boolEditor = new myGridCellBoolEditor();
  boolEditor->UseStringValues(_("Yes"));
  gridCrew->SetCellEditor(numRows, 0, boolEditor);

  gridCrew->SetCellAlignment(numRows, 0, wxALIGN_CENTRE, wxALIGN_CENTER);
  gridCrew->MakeCellVisible(numRows, NAME);

  if (dialog->m_menu2->IsChecked(MENUCREWALL))
    grid->SetCellValue(numRows, ONBOARD, "");
  else
    grid->SetCellValue(numRows, ONBOARD, _("Yes"));

  gridCrew->SetFocus();
  gridCrew->SetGridCursor(numRows, NAME);
}

void CrewList::changeCrew(wxGrid* grid, int row, int col, int offset) {
  wxString result;

  modified = true;
  wxString search;

  if (col == ONBOARD && dialog->m_menu2->IsChecked(MENUCREWONBOARD)) {
    if (grid->GetCellValue(row, col) == "") {
      filterCrewMembers();
      grid->ForceRefresh();
    }
  }
  /*	if(col == NAME && offset == 0)
      {
                      int rowWake = searchInWatch();
                      if(rowWake >= 0)
                              gridWake->SetCellValue(rowWake,0,gridCrew->GetCellValue(dialog->selGridRow,NAME));
      }
      if(col == FIRSTNAME && offset == 0)
      {
                      int rowWake = searchInWatch();
                      if(rowWake >= 0)
                              gridWake->SetCellValue(rowWake,1,gridCrew->GetCellValue(row,FIRSTNAME));
      }
      */
}
/*
int CrewList::searchInWatch()
{
        for(int row = 0; row < gridWake->GetNumberRows(); row++)
        {
                wxString name  = gridWake->GetCellValue(row,NAME);
                wxString first = gridWake->GetCellValue(row,FIRSTNAME);

                if(lastSelectedName == name && lastSelectedFirstName == first )
                        return row;
        }
        return -1;
}
*/
void CrewList::changeCrewWake(wxGrid* grid, int row, int col, bool* toggle) {
  wxDateTime dt, time;
  wxString s;

  if (row == 0) {
    s = gridWake->GetCellValue(0, col);

    if (s.Contains(" ")) {
      s = s.RemoveLast();
      s = s.RemoveLast();
    }

    if (!checkHourFormat(s, 0, col, &dt)) return;

    wxString t = wxString::Format("%s %s", dt.Format("%H:%M").c_str(),
                                  dialog->logbookPlugIn->opt->motorh.c_str());

    if (t == s || (dt.GetHour() == 0 && dt.GetMinute() == 0)) return;
    gridWake->SetCellValue(0, col, t);
  }

  if (row == 3) {
    s = gridWake->GetCellValue(row, col);
    if (s.IsEmpty()) {
      gridWake->SetCellValue(row, col, " ");
      return;
    }
    if (s.GetChar(0) == '\n') {
      s = s.substr(1);
      gridWake->SetCellValue(row, col, s);
    }
  }
}

void CrewList::insertDefaultCols(bool* insertCols) {
  wxDateTime dt, dtend, time;
  wxTimeSpan hr(24, 0, 0, 0);
  wxTimeSpan ed(0, 1), df;
  int i = 1;
  wxString t;

  df = createDefaultDateTime(dt, dtend, time);
  if (df.GetHours() == 0 && df.GetMinutes() == 0) return;

  while (dt < dtend) {
    wxDateTime e = dt;
    e.Add(df);
    e.Subtract(ed);
    //		wxMessageBox(wxString::Format("%s %s %s
    //\n%s\n%i",dt.FormatDate(),dtend.FormatDate(),time.FormatISOTime(),e.FormatDate(),df.GetHours()));
    if (e < dtend) {
      insertWatchColumn(i, t, time, dt, e, insertCols);
    } else {
      e = dtend;
      wxDateTime tt = e;
      e.Subtract(ed);
      wxTimeSpan x = tt.Subtract(dt);
      int hh = x.GetHours();
      int mm = x.GetMinutes() - hh * 60;
      wxDateTime y(hh, mm);

      insertWatchColumn(i, t, y, dt, e, insertCols);
    }
    i++;
    dt.Add(df);
  }
  day = 0;
  statusText(ALTERWATCH);
}

wxTimeSpan CrewList::createDefaultDateTime(wxDateTime& dt, wxDateTime& dtend,
                                           wxDateTime& time) {
  wxTimeSpan hr(24, 0, 0, 0);
  wxTimeSpan ed(0, 1);
  wxString t;

  dialog->myParseTime(gridWake->GetCellValue(0, 0), time);
  dt = stringToDateTime(dialog->m_textCtrlWatchStartDate->GetValue(),
                        dialog->m_textCtrlWatchStartTime->GetValue(), true);
  dtend = dt;
  dtend.Add(hr);

  t = gridWake->GetCellValue(0, 0);
  wxStringTokenizer tkz(t, ":");
  long h, m;
  tkz.GetNextToken().ToLong(&h);
  tkz.GetNextToken().ToLong(&m);
  wxTimeSpan df(h, m);

  wxDateTime e = dt;
  e.Add(df);
  e.Subtract(ed);
  gridWake->SetCellValue(
      2, 0,
      wxString::Format(
          "%s-%s", dt.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
          e.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str()));
  dt.Add(df);

  return df;
}

void CrewList::insertWatchColumn(int col, wxString time, wxDateTime wtime,
                                 wxDateTime dt, wxDateTime dtend,
                                 bool* insertCols) {
  gridWake->BeginBatch();
  gridWake->AppendCols();
  gridWake->SetCellEditor(3, col, new wxGridCellAutoWrapStringEditor);

  gridWake->SetColLabelValue(
      col,
      wxString::Format(
          "%d. %s", col + 1,
          dialog->m_gridGlobal->GetColLabelValue(LogbookDialog::WAKE).c_str()));
  gridWake->SetCellValue(
      0, col,
      wxString::Format("%s %s", wtime.Format("%H:%M").c_str(),
                       dialog->logbookPlugIn->opt->motorh.c_str()));
  if (dt.GetDateOnly() != dtend.GetDateOnly())
    gridWake->SetCellValue(
        1, col,
        wxString::Format(
            "%s\n%s",
            dt.Format(dialog->logbookPlugIn->opt->sdateformat).c_str(),
            dtend.Format(dialog->logbookPlugIn->opt->sdateformat).c_str()));
  else
    gridWake->SetCellValue(1, col,
                           dt.Format(dialog->logbookPlugIn->opt->sdateformat));
  gridWake->SetCellValue(
      2, col,
      wxString::Format(
          "%s-%s", dt.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
          dtend.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str()));
  gridWake->SetCellValue(3, col, " ");
  gridWake->SetReadOnly(1, col);
  gridWake->SetReadOnly(2, col);
  gridWake->EndBatch();
  gridWake->AutoSizeColumns();
}

wxString CrewList::updateWatchTime(unsigned int day, int col,
                                   bool* insertCols) {
  wxDateTime dt, dtst, dtend, time, e, tmp;
  wxTimeSpan h((long)24, (long)0);
  wxTimeSpan ed((long)0, (long)1);
  wxString s, str, str1, str2, ret;
  int delcol = 0;
  time = wxDateTime::Now();

  ret = gridWake->GetCellValue(0, col);

  for (int i = 0; i < gridWake->GetNumberCols(); i++) {
    gridWake->SetColLabelValue(
        i, wxString::Format(
               "%i. %s", i + 1,
               dialog->m_gridGlobal->GetColLabelValue(LogbookDialog::WAKE)
                   .c_str()));
    dialog->myParseTime(gridWake->GetCellValue(0, i), time);
    wxTimeSpan df((long)time.GetHour(), (long)time.GetMinute());

    if (i == 0) {
      str1 = dialog->m_textCtrlWatchStartTime->GetValue();
      dialog->myParseTime(str1, dtst);
      getStartEndDate(gridWake->GetCellValue(1, i), dtst, dtend);
      //			wxTimeSpan ts(dtst.GetHour(),dtst.GetMinute());
      //			dtst.Add(ts);
    } else {
      str = gridWake->GetCellValue(2, i - 1);
      wxStringTokenizer tkz(str, "-");
      str1 = tkz.GetNextToken();
      str2 = tkz.GetNextToken();
      // dialog->myParseTime(str1,dtst);
      dialog->myParseTime(str2, dtst);
      getStartEndDate(gridWake->GetCellValue(1, i - 1), dtst, dtend);
      //			wxTimeSpan ts(dtst.GetHour(),dtst.GetMinute());
      //			dtst.Add(ts);
      //			wxTimeSpan
      //tse(dtend.GetHour(),dtend.GetMinute()); 			dtend.Add(tse);
    }

    // wxMessageBox(wxString::Format("ohne %i %s %s\n%s
    // %s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
    if (i != 0) {
      // dtst.Add(df);
      //	wxMessageBox(wxString::Format("%i %s %s\n%s
      //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
      dtst = dtend;
      //	wxMessageBox(wxString::Format("%i %s %s\n%s
      //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
      dtst.Add(ed);
      //	wxMessageBox(wxString::Format("%i %s %s\n%s
      //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
      dtend.Add(df);
      //	wxMessageBox(wxString::Format("%i %s %s\n%s
      //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
      // dtend.Subtract(ed);

    } else {
      dtend = dtst;
      dtend.Add(df);
      dtend.Subtract(ed);
      //			wxMessageBox(wxString::Format("%i %s %s\n%s
      //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
    }
    //		wxMessageBox(wxString::Format("%i %s %s\n%s
    //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));

    //		dtend.Add(df);

    if (gridWake->GetNumberCols() == 1) return ret;
    //	wxMessageBox(wxString::Format("%i %s  %s\n%s
    //%s",i,dtst.FormatDate(),dtst.FormatTime(),dtend.FormatDate(),dtend.FormatTime()));
    if (dtend.GetDateOnly() == dtst.GetDateOnly())
      gridWake->SetCellValue(
          1, i, dtst.Format(dialog->logbookPlugIn->opt->sdateformat));
    else
      gridWake->SetCellValue(
          1, i,
          dtst.Format(dialog->logbookPlugIn->opt->sdateformat) + "\n" +
              dtend.Format(dialog->logbookPlugIn->opt->sdateformat));

    gridWake->SetCellValue(
        2, i,
        wxString::Format(
            "%s-%s",
            dtst.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
            dtend.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str()));

    h.Subtract(df);
    if (h.GetMinutes() < 0) {
      delcol = i;
      break;
    }
  }

  if (h.GetMinutes() > 1) {
    wxDateTime t, dt;
    dt = dtend;
    t.Set(h.GetHours(), (h.GetMinutes() - (h.GetHours() * 60)));
    dt.Add(ed);
    e = dt;
    e.Add(h);
    e.Subtract(ed);
    insertWatchColumn(gridWake->GetNumberCols(), "", t, dt, e, insertCols);
  } else if (h.GetMinutes() < 0)  // end > startvalue in textctrl
  {
    // gridWake->DeleteCols(delcol);
    wxStringTokenizer date(gridWake->GetCellValue(1, delcol),
                           "\n");  // start and end can be different date

    wxStringTokenizer tkz(gridWake->GetCellValue(2, delcol), "-");
    wxString tmp = tkz.GetNextToken();
    dialog->myParseTime(tmp, dt);  // now i have the actuall starting date/time

    dialog->myParseTime(dialog->m_textCtrlWatchStartTime->GetValue(), dtend);
    //		getStartEndDate(gridWake->GetCellValue(1,delcol-1),dt,dtend);
    if (date.CountTokens() == 1)  // start and end are same date
    {
      dialog->myParseDate(date.GetNextToken(), dt);
      dialog->myParseDate(gridWake->GetCellValue(1, delcol), dtend);
      // dtend = dt;
    } else  // start and end are not same date
    {
      dialog->myParseDate(date.GetNextToken(), dt);
      dialog->myParseDate(date.GetNextToken(), dtend);
    }

    wxTimeSpan t = dtend - dt;
    dtend.Subtract(ed);  // now i have the actuall end date/time

    if (dtend.GetDateOnly() == dt.GetDateOnly())  // decide if display two data
      gridWake->SetCellValue(
          1, delcol, dt.Format(dialog->logbookPlugIn->opt->sdateformat));
    else {
      gridWake->SetCellValue(
          1, delcol,
          dt.Format(dialog->logbookPlugIn->opt->sdateformat) + "\n" +
              dtend.Format(dialog->logbookPlugIn->opt->sdateformat));
    }

    gridWake->SetCellValue(
        2, delcol,
        wxString::Format(
            "%s-%s",
            dt.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
            dtend.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str()));
    gridWake->BeginBatch();
    gridWake->SetCellValue(0, delcol, t.Format("%H:%M"));
    gridWake->EndBatch();
    gridWake->ForceRefresh();
    int z = gridWake->GetNumberCols();
    wxDateTime absEnd;
    absEnd = getAbsEndTimeWatch(absEnd);
    for (int x = 0; x < z; x++) {
      wxDateTime e;
      e = getEndTimeWatch(gridWake->GetCellValue(2, x));
      e.Add(ed);
      if (e.GetHour() == absEnd.GetHour() &&
          e.GetMinute() == absEnd.GetMinute()) {
        int d = x + 1;
        if (d < z) {
          gridWake->DeleteCols(d, z - d);
          break;
        }
      }
    }
  }
  return ret;
}

void CrewList::getStartEndDate(wxString date, wxDateTime& dtstart,
                               wxDateTime& dtend) {
  wxStringTokenizer d(date, "\n");  // start and end can be different date
  if (d.CountTokens() == 1)         // start and end are same date
  {
    dialog->myParseDate(d.GetNextToken(), dtstart);
    dtend = dtstart;
  } else  // start and end are not same date
  {
    // dialog->myParseDate(date,dtstart);
    d.GetNextToken();
    // dialog->myParseDate(d.GetNextToken(),dtstart);
    dialog->myParseDate(d.GetNextToken(), dtstart);
  }
}

wxDateTime CrewList::getStartTimeWatch(wxString s) {
  wxDateTime dtstart;

  wxStringTokenizer tkz(s, "-");
  s = tkz.GetNextToken();
  dialog->myParseTime(s, dtstart);
  return dtstart;
}

wxDateTime CrewList::getEndTimeWatch(wxString s) {
  wxDateTime dtend;

  wxStringTokenizer tkz(s, "-");
  tkz.GetNextToken();
  s = tkz.GetNextToken();
  dialog->myParseTime(s, dtend);
  return dtend;
}

wxDateTime CrewList::getAbsEndTimeWatch(wxDateTime dtend) {
  dialog->myParseTime(dialog->m_textCtrlWatchStartTime->GetValue(), dtend);

  return dtend;
}

void CrewList::Calculate() {
  dialog->m_buttonCalculate->Enable(false);
  dialog->m_buttonReset->Enable(true);
  dialog->m_textCtrlWakeDay->Enable(true);

  wxDateTime dtstart, dtend, time, dtNoShift = wxDateTime::Now();
  wxTimeSpan ed(0, 1);
  wxString s, w;
  wxArrayString ar, *noshift;
  wxArrayInt singleNoShift;
  bool shift = false, ins;

  map<wxDateTime, wxArrayString*> noShiftMap;
  map<wxDateTime, wxArrayString*>::iterator it;

  watchListFile->Clear();
  dialog->m_textCtrlWatchStartDate->Enable(false);
  dialog->m_textCtrlWatchStartTime->Enable(false);
  dialog->m_textCtrlWakeTrip->Enable(false);

  for (int c = 0; c < gridWake->GetNumberCols(); c++)  // write the base = day 0
  {
    s = wxString::Format("%i\t", 0);

    for (int r = 0; r < gridWake->GetNumberRows(); r++) {
      if (r == 0) {
        s += wxString::Format("%i\t", gridWake->GetColSize(c));
        s += dialog->replaceDangerChar(gridWake->GetCellValue(r, c)) + "\t";
      }

      if (r == 1) {
        wxDateTime dtstart1, dtend1;
        wxString t = gridWake->GetCellValue(r, c);
        if (t.Contains("\n")) {
          wxStringTokenizer tkz(t, "\n");
          dialog->myParseDate(tkz.GetNextToken(), dtstart1);
          dialog->myParseDate(tkz.GetNextToken(), dtend1);
          s += dialog->replaceDangerChar(wxString::Format(
              "%i/%i/%i\n%i/%i/%i\t", dtstart1.GetMonth(), dtstart1.GetDay(),
              dtstart1.GetYear(), dtend1.GetMonth(), dtend1.GetDay(),
              dtend1.GetYear()));
        } else {
          dialog->myParseDate(gridWake->GetCellValue(r, c), dtstart1);
          dtend1 = dtstart1;
          s += wxString::Format("%i/%i/%i\t", dtstart1.GetMonth(),
                                dtstart1.GetDay(), dtstart1.GetYear());
        }
      }

      if (r == 2) {
        wxDateTime from, to;
        dtNoShift = getStartTimeWatch(gridWake->GetCellValue(r, c));
        wxString g = gridWake->GetCellValue(r, c);
        wxStringTokenizer sep(g, "-");
        dialog->myParseTime(sep.GetNextToken(), from);
        dialog->myParseTime(sep.GetNextToken(), to);
        s += wxString::Format("%02i,%02i,%02i,%02i\t", from.GetHour(),
                              from.GetMinute(), to.GetHour(), to.GetMinute());
      }

      if (r == 3) {
        w = gridWake->GetCellValue(r, c);
        // if(w.IsEmpty() || w.GetChar(0) == ' ') continue;
        wxStringTokenizer l(w, "\n");
        int z = l.CountTokens();
        int cc = 0;
        while (l.HasMoreTokens())
          if (l.GetNextToken().GetChar(0) == '*') cc++;

        if (z != 0 && cc == z)
          singleNoShift.Add(1);
        else if (!w.Contains("\n") && w.Contains("*"))
          singleNoShift.Add(1);
        else
          singleNoShift.Add(0);

        {
          noshift = new wxArrayString();
          noshift->clear();
          wxString g = wxEmptyString;
          wxStringTokenizer tkz(w, "\n");

          while (tkz.HasMoreTokens()) {
            wxString tmp = tkz.GetNextToken();

            if (tmp.GetChar(0) != '*')
              g += tmp + "\n";
            else {
              if (!tmp.IsEmpty())
                noshift->Add(dialog->replaceDangerChar(tmp));
              else
                noshift->Add(wxEmptyString);
            }
          }
          g.RemoveLast();

          if (!g.IsEmpty() && g.GetChar(0) != ' ')
            ar.Add(dialog->replaceDangerChar(g));

          noShiftMap[dtNoShift] = noshift;

          s += dialog->replaceDangerChar(gridWake->GetCellValue(r, c)) + "\t";
        }
      }
    }
    s.RemoveLast();
    s = dialog->replaceDangerChar(s);
    watchListFile->AddLine(s);
    s = "";
  }

  dialog->myParseTime(dialog->m_textCtrlWatchStartTime->GetValue(), dtstart);
  dialog->myParseDate(dialog->m_textCtrlWatchStartDate->GetValue(), dtstart);
  dtend = dtstart;

  int dend = wxAtoi(dialog->m_textCtrlWakeTrip->GetValue()) + 1;
  unsigned int i = 0;
  for (int d = 1; d < dend; d++) {
    for (int c = 0; c < gridWake->GetNumberCols(); c++) {
      s = wxString::Format("%i\t", d);
      ins = false;

      for (int r = 0; r < gridWake->GetNumberRows(); r++) {
        if (r == 0) {
          s += wxString::Format("%i\t", gridWake->GetColSize(c));
          s += gridWake->GetCellValue(r, c) + "\t";
          dialog->myParseTime(gridWake->GetCellValue(r, c), time);
        }
        if (r == 2) {
          wxTimeSpan df(time.GetHour(), time.GetMinute());

          dtend = dtstart;
          dtNoShift.Set(dtstart.GetHour(), dtstart.GetMinute());
          dtend.Add(df);
          dtend.Subtract(ed);

          if (dtend.GetDateOnly() != dtstart.GetDateOnly())
            s += wxString::Format("%i/%i/%i\n%i/%i/%i\t", dtstart.GetMonth(),
                                  dtstart.GetDay(), dtstart.GetYear(),
                                  dtend.GetMonth(), dtend.GetDay(),
                                  dtend.GetYear());
          else
            s += wxString::Format("%i/%i/%i\t", dtstart.GetMonth(),
                                  dtstart.GetDay(), dtstart.GetYear());

          s += wxString::Format("%02i,%02i,%02i,%02i\t", dtstart.GetHour(),
                                dtstart.GetMinute(), dtend.GetHour(),
                                dtend.GetMinute());
          dtstart = dtend;
          dtstart.Add(ed);
        }
        if (r == 3) {
          if (!ar.IsEmpty() && singleNoShift[c] == 0) {
            s += ar[i++];
            ins = true;
          }
          if (i == ar.size()) {
            i = 0;
            shift = true;
          }

          it = noShiftMap.find(dtNoShift);
          wxString g = wxEmptyString;
          if (it != noShiftMap.end()) {
            wxArrayString* t = (*it).second;
            if (t->GetCount() > 0) {
              for (unsigned int ii = 0; ii < t->GetCount(); ii++)
                if (!(*t)[ii].IsEmpty()) g += (*t)[ii] + "\n";
              g.RemoveLast();
              g = dialog->restoreDangerChar(g);
              if (ins)
                s += "\n" + g;
              else
                s += g;
            }
          }
          s += +"\t";
          ;
        }
      }
      s.RemoveLast();
      s = dialog->replaceDangerChar(s);
      watchListFile->AddLine(s);
      s = wxEmptyString;
    }
  }
  watchListFile->InsertLine(
      wxString::Format("#1.2#\t%s\t%s\t%i\t%s",
                       dialog->m_textCtrlWatchStartDate->GetValue().c_str(),
                       dialog->m_textCtrlWatchStartTime->GetValue().c_str(),
                       shift, dialog->m_textCtrlWakeTrip->GetValue().c_str()),
      0);
  watchListFile->Write();
  day = 1;
  dayNow(false);
  setDayButtons(shift);
  setMembersInMenu();
}

void CrewList::rightClickMenu(int row, int col) {
  selRowWake = row;
  selColWake = col;

  wxArrayInt cols = gridWake->GetSelectedCols();
  int count = cols.GetCount();

  if (count > 1) {
    dialog->m_menu21->Enable(MENUWAKESPLIT, false);
    dialog->m_menu21->Enable(MENUWAKEMERGE, true);
    dialog->m_menu21->Enable(MENUWAKECHANGE, true);
  } else {
    dialog->m_menu21->Enable(MENUWAKESPLIT, true);
    dialog->m_menu21->Enable(MENUWAKEMERGE, false);
    dialog->m_menu21->Enable(MENUWAKEDELETE, false);
    dialog->m_menu21->Enable(MENUWAKECHANGE, false);
  }

  if (gridWake->IsSelection())
    dialog->m_menu21->Enable(MENUWAKEDELETE, true);
  else
    dialog->m_menu21->Enable(MENUWAKEDELETE, false);

  gridWake->SetGridCursor(row, col);
  gridWake->PopupMenu(dialog->m_menu21);
}

void CrewList::mergeWatches() {
  bool dummy;
  int leftCol = 10000;
  wxDateTime dt;
  wxTimeSpan sp(0);

  wxArrayInt col = gridWake->GetSelectedCols();
  unsigned int cc = col.GetCount();
  if (cc <= 1) return;

  bool sort = false;
  unsigned int i = 0;
  while (true) {
    int tmp = col[i + 1];
    if (col[i] > col[i + 1]) {
      col[i + 1] = col[i];
      col[i] = tmp;
      sort = true;
    }
    i++;
    if (i == cc - 1) {
      i = 0;
      if (!sort) break;
      sort = false;
    }
  }

  leftCol = col[0];

  for (unsigned int i = 0; i < cc; i++) {
    dialog->myParseTime(gridWake->GetCellValue(0, col[i]), dt);
    wxTimeSpan df(dt.GetHour(), dt.GetMinute());
    sp += df;
  }

  gridWake->BeginBatch();
  for (int i = cc - 1; i >= 0; i--)
    if (leftCol != col[i]) gridWake->DeleteCols(col[i]);
  gridWake->SetCellValue(
      0, leftCol,
      wxString::Format("%s %s", sp.Format("%H:%M").c_str(),
                       dialog->logbookPlugIn->opt->motorh.c_str()));
  gridWake->EndBatch();
  updateWatchTime(day, 0, &dummy);

  updateLine();
}

void CrewList::flipWatches() {
  wxArrayInt col = gridWake->GetSelectedCols();
  if (col.Count() != 2) return;

  wxString tmp = gridWake->GetCellValue(3, col[0]);
  gridWake->BeginBatch();
  gridWake->SetCellValue(3, col[0], gridWake->GetCellValue(3, col[1]));
  gridWake->SetCellValue(3, col[1], tmp);
  gridWake->EndBatch();
}

void CrewList::splitWatch() {
  wxDateTime dt;
  bool dummy;

  wxString s = gridWake->GetCellValue(0, selColWake);
  dialog->myParseTime(s, dt);
  int hr = dt.GetHour() * 60;
  int min1 = dt.GetMinute() + hr;
  int min = min1 / 2;
  hr = min / 60;
  min = min - hr * 60;
  wxTimeSpan sp(hr, min);
  wxTimeSpan sp1 = sp;
  if (min1 % 2 != 0) sp1.Add(wxTimeSpan(0, 1));

  gridWake->BeginBatch();
  gridWake->InsertCols(selColWake + 1);
  gridWake->SetCellValue(
      0, selColWake,
      wxString::Format("%s %s", sp.Format("%H:%M").c_str(),
                       dialog->logbookPlugIn->opt->motorh.c_str()));
  gridWake->SetCellValue(
      0, selColWake + 1,
      wxString::Format("%s %s", sp1.Format("%H:%M").c_str(),
                       dialog->logbookPlugIn->opt->motorh.c_str()));
  gridWake->EndBatch();
  updateWatchTime(day, 0, &dummy);

  updateLine();
}

void CrewList::setAsNewWatchbase() {
  wxDateTime start, end;
  wxString t = gridWake->GetCellValue(1, 0);
  getStartEndDate(gridWake->GetCellValue(1, 0), start, end);
  dialog->m_textCtrlWatchStartDate->SetValue(
      start.Format(dialog->logbookPlugIn->opt->sdateformat));
  watchListFile->Clear();
  day = 0;
  dialog->m_textCtrlWakeDay->SetValue("0");
  dialog->m_buttonCalculate->Enable(true);
  dialog->m_buttonReset->Enable(false);
  dialog->m_textCtrlWatchStartDate->Enable(true);
  dialog->m_textCtrlWatchStartTime->Enable(true);
  dialog->m_textCtrlWakeTrip->Enable(true);
  dialog->m_textCtrlWakeDay->Enable(false);
  setDayButtons(false);
  this->statusText(HITCALCULATE);
}

void CrewList::enterDay() {
  int d = wxAtoi(dialog->m_textCtrlWakeDay->GetValue());
  if (d < 1) d = 1;
  int trip = wxAtoi(dialog->m_textCtrlWakeTrip->GetValue());
  if (d > trip) d = trip;
  day = d;
  readRecord(day);
}

void CrewList::deleteMembers() {
  wxArrayInt col = gridWake->GetSelectedCols();

  if (col.Count() != 0)
    for (unsigned int i = 0; i < col.Count(); i++)
      gridWake->SetCellValue(3, col[i], wxEmptyString);
  else
    for (int i = 0; i < gridWake->GetNumberCols(); i++)
      gridWake->SetCellValue(3, i, wxEmptyString);

  updateLine();
  gridWake->AutoSizeColumns();
}

void CrewList::setMembersInMenu() {
  wxString col, m, member;

  ActualWatch::menuMembers.clear();
  if (watchListFile->GetLineCount() < 1) return;

  watchListFile->GoToLine(1);
  col = watchListFile->GetLine(1);

  while (!watchListFile->Eof()) {
    wxStringTokenizer tkz(col, "\t");
    for (int i = 0; i < 5; i++) tkz.GetNextToken();
    if (tkz.HasMoreTokens())
      m = tkz.GetNextToken();
    else {
      col = watchListFile->GetNextLine();
      continue;
    }

    m = dialog->restoreDangerChar(m);
    wxStringTokenizer mtkz(m, "\n");
    while (mtkz.HasMoreTokens()) {
      member = mtkz.GetNextToken();
      member.Replace("*", "");

      if (ActualWatch::menuMembers.IsEmpty() &&
          (member.length() == 1 && member.GetChar(0) != ' '))
        ActualWatch::menuMembers.Add(member);
      else {
        bool found = false;
        for (unsigned int i = 0; i < ActualWatch::menuMembers.Count(); i++)
          if (ActualWatch::menuMembers[i] == member ||
              (member.length() == 1 && member.GetChar(0) == ' ')) {
            found = true;
            break;
          }

        if (!found) ActualWatch::menuMembers.Add(member);
      }
    }
    col = watchListFile->GetNextLine();
  }
}

void CrewList::checkMemberIsInMenu(wxString member) {
  bool insert = true;

  for (unsigned int i = 0; i < ActualWatch::menuMembers.Count(); i++) {
    if (member == ActualWatch::menuMembers[i]) {
      insert = false;
      break;
    }
  }

  if (insert) ActualWatch::menuMembers.Add(member);
}

void CrewList::wakeMemberDrag(int row, int col) {
  //	gridWake->SetRowSize(row,gridWake->GetRowHeight(row)+30);
  gridWake->SetColSize(col, gridWake->GetColSize(col) + 20);
  gridWake->Refresh();
}

void CrewList::watchEditorShown(int row, int col) {
  wxGridCellEditor* ed = gridWake->GetCellEditor(row, col);
  wxTextCtrl* gridTextCtrl = wxDynamicCast(ed->GetControl(), wxTextCtrl);

  if (gridTextCtrl) {
    gridTextCtrl->Connect(wxEVT_MOTION,
                          wxMouseEventHandler(LogbookDialog::OnMotion), NULL,
                          dialog);
    gridTextCtrl->SetDropTarget(new DnDWatch(gridWake, this));
    ((DnDWatch*)gridWake->GetGridWindow()->GetDropTarget())->source = gridWake;
    ((DnDCrew*)gridCrew->GetGridWindow()->GetDropTarget())->source = gridWake;
    ((DnDWatch*)gridWake->GetGridWindow()->GetDropTarget())->col = col;

    //		gridTextCtrl->ShowPosition(0);
  }
}

void CrewList::watchEditorHidden(int row, int col) {
  wxGridCellEditor* ed = gridWake->GetCellEditor(row, col);
  wxTextCtrl* gridTextCtrl = wxDynamicCast(ed->GetControl(), wxTextCtrl);

  if (gridTextCtrl && row == 3) {
    if (gridTextCtrl->IsModified()) {
      updateLine();
      if (ActualWatch::day == day && ActualWatch::col == col)
        ActualWatch::member = gridTextCtrl->GetValue();
    }
    gridTextCtrl->Disconnect(wxEVT_MOTION,
                             wxMouseEventHandler(LogbookDialog::OnMotion), NULL,
                             dialog);
  }
  gridWake->AutoSizeRow(3);
  gridWake->AutoSizeColumn(col);
  gridWake->SetRowSize(3, gridWake->GetRowHeight(3) + 10);
}

void CrewList::watchEditorHighlight(wxMouseEvent& event) {
#ifndef __WXOSX__
  wxPoint pt;
  wxTextCoord col, row;

  event.GetPosition(&pt.x, &pt.y);
  wxObject* o = event.GetEventObject();
  wxTextCtrl* gridTextCtrl = wxDynamicCast(o, wxTextCtrl);
  gridTextCtrl->HitTest(pt, &col, &row);
  int len = gridTextCtrl->GetLineLength(row);
  int pos = gridTextCtrl->XYToPosition(0, row);
  gridTextCtrl->SetSelection(pos, pos + len);
#endif
}

void CrewList::setDayButtons(bool shift) {
  if (shift) {
    dialog->m_buttonDayPlus->Enable();
    dialog->m_buttonDayMinus->Enable();
    dialog->m_buttonNow->Enable();
  } else {
    dialog->m_buttonDayPlus->Enable(false);
    dialog->m_buttonDayMinus->Enable(false);
    dialog->m_buttonNow->Enable(false);
  }
}

void CrewList::clearWake() {
  watchListFile->Clear();
  watchListFile->Write();

  gridWake->BeginBatch();
  gridWake->DeleteCols(0, gridWake->GetNumberCols());
  gridWake->AppendCols();
  firstColumn();
  gridWake->EndBatch();
  setDayButtons(false);
  dialog->m_buttonCalculate->Enable(false);
  dialog->m_buttonReset->Enable(false);
  dialog->m_textCtrlWakeDay->Enable(false);
  dialog->m_textCtrlWatchStartDate->Enable(true);
  dialog->m_textCtrlWatchStartTime->Enable(true);
  dialog->m_textCtrlWakeTrip->Enable(true);
  dialog->m_textCtrlWakeDay->SetValue("0");
  gridWake->SetColLabelValue(
      0,
      wxString::Format(
          "1. %s",
          dialog->m_gridGlobal->GetColLabelValue(LogbookDialog::WAKE).c_str()));
  day = 0;
  gridWake->AutoSizeColumns();
  gridWake->AutoSizeRows();
  ActualWatch::menuMembers.clear();
  statusText(DEFAULTWATCH);
}

void CrewList::dayPlus() {
  if (day == (unsigned int)wxAtoi(dialog->m_textCtrlWakeTrip->GetValue()))
    return;
  day++;
  readRecord(day);
  if (day == ActualWatch::day)
    gridWake->SetCellBackgroundColour(2, ActualWatch::col, wxColor(0, 255, 0));
}

void CrewList::dayMinus() {
  if (day <= 1) return;
  day--;
  readRecord(day);
  if (day == ActualWatch::day)
    gridWake->SetCellBackgroundColour(2, ActualWatch::col, wxColor(0, 255, 0));
}

void CrewList::dayNow(bool mode) {
  wxString s, sd, st, date, time, timedf, ttmp, str;
  wxDateTime dtstart, dtend, now;
  wxTimeSpan ed(0, 0, 0, 1);
  long d;
  int lineno;
  unsigned int col = 0, daylast = 1;

  if (dialog->logbook->sDate != wxEmptyString)
    now = dialog->logbook->mCorrectedDateTime;
  else
    now = wxDateTime::Now();

  if ((lineno = getDayOne(1)) == -1) {
    statusText(DEFAULTWATCH);
    return;
  }

  ActualWatch::active = false;
  while (lineno < (int)watchListFile->GetLineCount()) {
    s = watchListFile->GetLine(lineno);
    wxStringTokenizer tkz(s, "\t");
    tkz.GetNextToken().ToLong(&d);
    if ((unsigned int)d != daylast) col = 0;

    tkz.GetNextToken();

    timedf = tkz.GetNextToken();
    wxStringTokenizer tkzdf(timedf, ":");
    long h, m;
    tkzdf.GetNextToken().ToLong(&h);
    tkzdf.GetNextToken().ToLong(&m);
    wxTimeSpan df(h, m);

    date = tkz.GetNextToken();
    getStartEndDate(date, dtstart, dtend);

    ttmp = tkz.GetNextToken();
    wxStringTokenizer timetkz(ttmp, ",");
    time = timetkz.GetNextToken();
    time += ":" + timetkz.GetNextToken();

    dtstart = stringToDateTime(date, time, mode);
    dtend = dtstart;

    dtend.Add(df);
    dtend.Subtract(ed);
    //		wxMessageBox(dtstart.FormatDate()+"
    //"+dtstart.FormatTime()+"\n"+now.FormatDate()+"
    //"+now.FormatTime()+"\n"+dtend.FormatDate()+" "+dtend.FormatTime());
    str = tkz.GetNextToken();
    if (now.IsBetween(dtstart, dtend)) {
      readRecord(d);
      gridWake->SetCellBackgroundColour(2, col, wxColor(0, 255, 0));
      gridWake->MakeCellVisible(0, col);

      ActualWatch::active = true;
      ActualWatch::day = d;
      ActualWatch::col = col;
      ActualWatch::time = df;
      ActualWatch::start = dtstart;
      ActualWatch::end = dtend;
      ActualWatch::member = dialog->restoreDangerChar(str);

      statusText(ALTERDAY);
      return;
    }
    col++;
    daylast = d;
    lineno++;
  }

  if (watchListFile->GetLineCount() > 0) {
    readRecord(1);
    statusText(ALTERDAY);
  } else
    statusText(DEFAULTWATCH);
}

void CrewList::statusText(int i) {
  dialog->m_staticTextStatusWatch->SetLabel(statustext[i]);
}

void CrewList::Reset() {
  setDayButtons(false);
  dialog->m_buttonCalculate->Enable(true);
  dialog->m_buttonReset->Enable(false);
  dialog->m_textCtrlWatchStartDate->Enable(true);
  dialog->m_textCtrlWatchStartTime->Enable(true);
  dialog->m_textCtrlWakeTrip->Enable(true);
  dialog->m_textCtrlWakeDay->Enable(false);
  day = 0;
  readRecord(day);
  statusText(ALTERWATCH);
}

void CrewList::dateTextCtrlClicked() {
  bool dummy;
  wxDateTime dt, dtend, time;

  dialog->m_textCtrlWatchStartTime->SetFocus();
  DateDialog* dlg = new DateDialog(gridWake);

  if (dlg->ShowModal() == wxID_OK) {
    wxDateTime dt = dlg->m_calendar2->GetDate();
    //	wxMessageBox(dt.FormatDate());
    dialog->m_textCtrlWatchStartDate->ChangeValue(
        dt.Format(dialog->logbookPlugIn->opt->sdateformat));
    gridWake->SetCellValue(1, 0,
                           dt.Format(dialog->logbookPlugIn->opt->sdateformat));
    createDefaultDateTime(dt, dtend, time);
    updateWatchTime(0, 0, &dummy);
  }

  delete dlg;
}

void CrewList::timeTextCtrlTextEntered(wxCommandEvent& event) {
  bool dummy;
  wxDateTime dt, dtend, time;

  if (checkHourFormat(event.GetString(), -1, -1, &dt)) {
    dialog->m_textCtrlWatchStartTime->SetValue(dt.Format("%H:%M"));
    dialog->myParseDate(dialog->m_textCtrlWatchStartDate->GetValue(), dt);
    createDefaultDateTime(dt, dtend, time);
    updateWatchTime(0, 0, &dummy);
  }
  gridWake->SetFocus();
  gridWake->SetGridCursor(0, 0);
}

wxDateTime CrewList::stringToDateTime(wxString date, wxString time, bool mode) {
  wxDateTime dt;
  wxStringTokenizer tkz;

  if (!mode) {
    tkz.SetString(date, "/");
    int month = wxAtoi(tkz.GetNextToken());
    int day = wxAtoi(tkz.GetNextToken());
    int year = wxAtoi(tkz.GetNextToken());
    dialog->myParseTime(time, dt);
    dt.Set(day, (wxDateTime::Month)month, year, dt.GetHour(), dt.GetMinute(),
           dt.GetSecond());
  } else {
    dialog->myParseTime(time, dt);
    dialog->myParseDate(date, dt);
  }
  return dt;
}

void CrewList::readRecord(int nr) {
  int uu = watchListFile->GetLineCount();
  if (watchListFile->GetLineCount() <= 1) return;

  long d;
  int lineno;
  wxString s;

  if ((linenoStart = getDayOne(nr)) == -1) return;
  lineno = linenoStart;
  if (lineno == -1) return;

  gridWake->DeleteCols(0, gridWake->GetNumberCols());

  int c = 1;
  while (!watchListFile->Eof()) {
    s = watchListFile->GetLine(lineno);
    s = dialog->restoreDangerChar(s);

    wxStringTokenizer tkz(s, "\t");
    tkz.GetNextToken().ToLong(&d);
    if (d != nr) {
      linenoEnd--;
      break;
    }
    day = d;

    gridWake->BeginBatch();
    gridWake->AppendCols();

    int col = gridWake->GetNumberCols() - 1;
    gridWake->SetColSize(col, 160);
    gridWake->SetCellEditor(3, col, new wxGridCellAutoWrapStringEditor);

    gridWake->SetReadOnly(1, col);
    gridWake->SetReadOnly(2, col);
    gridWake->SetColLabelValue(col, wxString::Format("%i. Watch", c++));
    tkz.GetNextToken();

    for (int row = 0; row < 4; row++) {
      if (row == 0 || row == 3)
        gridWake->SetCellValue(row, col,
                               dialog->restoreDangerChar(tkz.GetNextToken()));
      else if (row == 1) {
        wxString s = tkz.GetNextToken();
        if (s.Contains("\n")) {
          wxStringTokenizer nl(s, "\n");
          wxString t = nl.GetNextToken();
          wxStringTokenizer tkz(t, "/");
          int month = wxAtoi(tkz.GetNextToken());
          int day = wxAtoi(tkz.GetNextToken());
          int year = wxAtoi(tkz.GetNextToken());
          wxDateTime dt(day, (wxDateTime::Month)month, year);

          t = nl.GetNextToken();
          wxStringTokenizer tkz1(t, "/");
          month = wxAtoi(tkz1.GetNextToken());
          day = wxAtoi(tkz1.GetNextToken());
          year = wxAtoi(tkz1.GetNextToken());
          wxDateTime dt1(day, (wxDateTime::Month)month, year);
          gridWake->SetCellValue(
              row, col,
              dt.Format(dialog->logbookPlugIn->opt->sdateformat) + "\n" +
                  dt1.Format(dialog->logbookPlugIn->opt->sdateformat));
        } else {
          wxStringTokenizer tkz(s, "/");
          int month = wxAtoi(tkz.GetNextToken());
          int day = wxAtoi(tkz.GetNextToken());
          int year = wxAtoi(tkz.GetNextToken());
          wxDateTime dt(day, (wxDateTime::Month)month, year);
          gridWake->SetCellValue(
              row, col, dt.Format(dialog->logbookPlugIn->opt->sdateformat));
        }
      } else if (row == 2) {
        wxDateTime from, to;
        wxString s = tkz.GetNextToken();
        wxStringTokenizer tkz(s, ",");
        int fh = wxAtoi(tkz.GetNextToken());
        int fm = wxAtoi(tkz.GetNextToken());
        int th = wxAtoi(tkz.GetNextToken());
        int tm = wxAtoi(tkz.GetNextToken());
        from.Set(fh, fm);
        to.Set(th, tm);
        gridWake->SetCellValue(
            row, col,
            wxString::Format(
                "%s-%s",
                from.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str(),
                to.Format(dialog->logbookPlugIn->opt->stimeformatw).c_str()));
      }
    }
    gridWake->EndBatch();

    lineno++;
    linenoEnd = lineno;
    if (lineno >= uu) break;
  }

  gridWake->AutoSizeColumns();
  gridWake->AutoSizeRows();
  gridWake->SetRowSize(3, gridWake->GetRowHeight(3) + 10);

  dialog->m_textCtrlWakeDay->SetValue(wxString::Format("%i", day));
}

void CrewList::updateLine() {
  int lineno = linenoStart;
  wxString s = wxEmptyString;
#ifdef __WXOSX__
  if (watchListFile->GetLineCount() < 1 || day < (long)0) return;
#else
  if (watchListFile->GetLineCount() < 1 || day < 0) return;
#endif
  int cc = gridWake->GetNumberCols();
  for (int c = 0; c < cc; c++) {
    s = wxString::Format("%i\t", day);
    s += wxString::Format("%i\t", gridWake->GetColSize(c));

    for (int r = 0; r < gridWake->GetNumberRows(); r++) {
      if (r == 0 || r == 3)
        s += gridWake->GetCellValue(r, c) + "\t";
      else if (r == 1) {
        wxDateTime dtstart, dtend;
        wxString tmp = gridWake->GetCellValue(r, c);
        wxString start, end;
        if (tmp.Contains("\n")) {
          wxStringTokenizer tkz(tmp, "\n");
          start = tkz.GetNextToken();
          end = tkz.GetNextToken();

          LogbookDialog::myParseDate(start, dtstart);
          LogbookDialog::myParseDate(end, dtend);

          s += wxString::Format("%i/%i/%i\n%i/%i/%i\t", dtstart.GetMonth(),
                                dtstart.GetDay(), dtstart.GetYear(),
                                dtend.GetMonth(), dtend.GetDay(),
                                dtend.GetYear());
        } else {
          LogbookDialog::myParseDate(tmp, dtstart);
          s += wxString::Format("%i/%i/%i\t", dtstart.GetMonth(),
                                dtstart.GetDay(), dtstart.GetYear());
        }

      } else if (r == 2) {
        wxDateTime from, to;
        wxString g = gridWake->GetCellValue(r, c);
        wxStringTokenizer sep(g, "-");
        dialog->myParseTime(sep.GetNextToken(), from);
        dialog->myParseTime(sep.GetNextToken(), to);
        s += wxString::Format("%i,%i,%i,%i\t", from.GetHour(), from.GetMinute(),
                              to.GetHour(), to.GetMinute());
      }
    }
    s.RemoveLast();
    s = dialog->replaceDangerChar(s);
    watchListFile->InsertLine(s, lineno++);
    s = "";
  }

  wxString l;
  while (true) {
    l = watchListFile->GetLine(lineno);
    wxStringTokenizer tkz(l, "\t");
    if ((unsigned int)wxAtoi(tkz.GetNextToken()) != day)
      break;
    else
      watchListFile->RemoveLine(lineno);
  }
  watchListFile->Write();
}

int CrewList::getDayOne(int dayone) {
  long d;
  wxString s;

  d = -1;
  if (watchListFile->GetLineCount() < 1) return -1;

  watchListFile->GoToLine(0);
  while (d != dayone) {
    s = watchListFile->GetNextLine();
    if (watchListFile->Eof()) return -1;
    wxStringTokenizer tkz(s, "\t");
    tkz.GetNextToken().ToLong(&d);
  }
  return watchListFile->GetCurrentLine();
}

bool CrewList::checkHourFormat(wxString s, int row, int col, wxDateTime* dt) {
  bool t = false;
  wxString sep;

  if (s.IsEmpty()) {
    gridWake->SetCellValue(row, col, "00:00");
    s = "0";
  }

  if (s.GetChar(0) == ',' ||
      s.GetChar(0) == '.')  // no leading zero when ,3 is entered
    s.Prepend("0");

  if (s.Length() == 4 &&
      (!s.Contains(".") &&
       !s.Contains(",")))  // NATO-Format entered 1230 => 12.30
    s.insert(2, ".");

  if (s.Contains(".")) {
    t = true;
    sep = ".";
  }

  if (s.Contains(",")) {
    t = true;
    sep = ",";
  }

  if (s.Contains(":")) {
    t = true;
    sep = ":";
  }

  if (true != t) {
    s.Append(":0");
    sep = ":";
  }

  wxStringTokenizer tkz(s, sep);
  wxString h = tkz.GetNextToken();
  wxString m;
  if (tkz.HasMoreTokens())
    m = tkz.GetNextToken();
  else
    m = "0";
  if (!h.IsNumber()) h = "24";
  if (!m.IsNumber()) m = "60";

  if (wxAtoi(h) > 23 || wxAtoi(m) > 59 || wxAtoi(h) < 0 || wxAtoi(m) < 0) {
    // wxMessageBox(_("Hours < 0 or > 23\nMinutes < 0 or > 59"),"");
    if (row != -1) gridWake->SetCellValue(row, col, "00:00");
    return false;
  } else
    s = wxString::Format("%s:%s", h.c_str(), m.c_str());

  dialog->myParseTime(s, *dt);
  return true;
}

void CrewList::saveCSV(wxString path) {
  wxString result;

  saveData();

  wxTextFile csvFile(path);

  if (csvFile.Exists()) {
    ::wxRemoveFile(path);
    csvFile.Create();
  }

  crewListFile->Open();

  for (unsigned int i = 0; i < crewListFile->GetLineCount(); i++) {
    wxString line = crewListFile->GetLine(i);
    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);

    while (tkz.HasMoreTokens()) {
      wxString s;
      s += tkz.GetNextToken().RemoveLast();
      s = dialog->restoreDangerChar(s);
      result += "\"" + s + "\",";
    }
    result.RemoveLast();
    csvFile.AddLine(result);
    result = "";
  }

  csvFile.Write();
  csvFile.Close();
  crewListFile->Close();
}

void CrewList::saveHTML(wxString savePath, wxString layout, bool mode) {
  wxString path;

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return;
  }

  wxArrayString watch;
  wxString html = readLayout(layout);

  int indexTop;
  int indexBottom;
  int indexWakeTop;
  int indexWakeBottom;
  int indexWake1Top;
  int indexWake1Bottom;
  int indexWakeColumn0Top;
  int indexWakeColumn0Bottom;
  int indexWakeColumn1Top;
  int indexWakeColumn1Bottom;
  int indexWakeColumn2Top;
  int indexWakeColumn2Bottom;
  int indexWakeColumn3Top;
  int indexWakeColumn3Bottom;
  int indexWakeColumn4Top;
  int indexWakeColumn4Bottom;

  wxString topHTML;
  wxString bottomHTML;
  wxString middleHTML;
  wxString columnWake0HTML;
  wxString columnWake1HTML;
  wxString columnWake2HTML;
  wxString columnWake3HTML;
  wxString columnWake4HTML;
  wxString topWakeHTML;
  wxString bottomWakeHTML;
  wxString middleWakeHTML;
  wxString middleWake1HTML;
  wxString headerHTML;

  wxString seperatorTop = "<!--Repeat -->";
  wxString seperatorBottom = "<!--Repeat End -->";
  wxString seperatorWakeTop = "<!--Repeat Wake -->";
  wxString seperatorWakeBottom = "<!--Repeat Wake End -->";
  wxString seperatorWake1Top = "<!--Repeat Wake1 -->";
  wxString seperatorWake1Bottom = "<!--Repeat Wake1 End -->";
  wxString seperatorWakeCol0Top = "<!--Repeat Col0 -->";
  wxString seperatorWakeCol0Bottom = "<!--Repeat Col0 End -->";
  wxString seperatorWakeCol1Top = "<!--Repeat Col1 -->";
  wxString seperatorWakeCol1Bottom = "<!--Repeat Col1 End -->";
  wxString seperatorWakeCol2Top = "<!--Repeat Col2 -->";
  wxString seperatorWakeCol2Bottom = "<!--Repeat Col2 End -->";
  wxString seperatorWakeCol3Top = "<!--Repeat Col3 -->";
  wxString seperatorWakeCol3Bottom = "<!--Repeat Col3 End -->";
  wxString seperatorWakeCol4Top = "<!--Repeat Col4 -->";
  wxString seperatorWakeCol4Bottom = "<!--Repeat Col4 End -->";

  if (!html.Contains("<!--Repeat Wake -->")) {
    indexTop = html.First(seperatorTop) + seperatorTop.Len();
    indexBottom = html.First(seperatorBottom) + seperatorBottom.Len();
    topHTML = html.substr(0, indexTop);
    bottomHTML = html.substr(indexBottom, html.Len() - indexBottom - 1);
    middleHTML = html.substr(indexTop, indexBottom - indexTop);
  } else if (!html.Contains("<!--Repeat -->")) {
    indexWakeTop = html.First(seperatorWakeTop) + seperatorWakeTop.Len();
    indexWakeBottom =
        html.First(seperatorWakeBottom) + seperatorWakeBottom.Len();
    indexWake1Top = html.First(seperatorWake1Top) + seperatorWake1Top.Len();
    indexWake1Bottom = html.First(seperatorWake1Bottom);
    indexWakeColumn0Top =
        html.First(seperatorWakeCol0Top) + seperatorWakeCol0Top.Len();
    indexWakeColumn0Bottom =
        html.First(seperatorWakeCol0Bottom) + seperatorWakeCol0Bottom.Len();
    indexWakeColumn1Top =
        html.First(seperatorWakeCol1Top) + seperatorWakeCol1Top.Len();
    indexWakeColumn1Bottom =
        html.First(seperatorWakeCol1Bottom) + seperatorWakeCol1Bottom.Len();
    indexWakeColumn2Top =
        html.First(seperatorWakeCol2Top) + seperatorWakeCol2Top.Len();
    indexWakeColumn2Bottom =
        html.First(seperatorWakeCol2Bottom) + seperatorWakeCol2Bottom.Len();
    indexWakeColumn3Top =
        html.First(seperatorWakeCol3Top) + seperatorWakeCol3Top.Len();
    indexWakeColumn3Bottom =
        html.First(seperatorWakeCol3Bottom) + seperatorWakeCol3Bottom.Len();
    indexWakeColumn4Top =
        html.First(seperatorWakeCol4Top) + seperatorWakeCol4Top.Len();
    indexWakeColumn4Bottom =
        html.First(seperatorWakeCol4Bottom) + seperatorWakeCol4Bottom.Len();
    topHTML = html.substr(0, indexWakeTop);
    bottomWakeHTML =
        html.substr(indexWake1Bottom, html.Len() - indexWake1Bottom - 1);
    middleWakeHTML = html.substr(indexWakeTop, indexWakeBottom - indexWakeTop -
                                                   seperatorWakeBottom.Len());
    middleWake1HTML =
        html.substr(indexWake1Top, indexWake1Bottom - indexWake1Top);
    columnWake0HTML = html.substr(indexWakeColumn0Top,
                                  indexWakeColumn0Bottom - indexWakeColumn0Top -
                                      seperatorWakeCol0Bottom.Len());
    columnWake1HTML = html.substr(indexWakeColumn1Top,
                                  indexWakeColumn1Bottom - indexWakeColumn1Top -
                                      seperatorWakeCol1Bottom.Len());
    columnWake2HTML = html.substr(indexWakeColumn2Top,
                                  indexWakeColumn2Bottom - indexWakeColumn2Top -
                                      seperatorWakeCol2Bottom.Len());
    columnWake3HTML = html.substr(indexWakeColumn3Top,
                                  indexWakeColumn3Bottom - indexWakeColumn3Top -
                                      seperatorWakeCol3Bottom.Len());
    columnWake4HTML = html.substr(indexWakeColumn4Top,
                                  indexWakeColumn4Bottom - indexWakeColumn4Top -
                                      seperatorWakeCol4Bottom.Len());

    topHTML.Replace("#LFROM#", _("from"));
    topHTML.Replace("#LTO#", _("to"));
    topHTML.Replace("#LWATCH#", dialog->m_gridGlobal->GetColLabelValue(4));
  } else {
    indexTop = html.First(seperatorTop) + seperatorTop.Len();
    indexBottom = html.First(seperatorBottom) + seperatorBottom.Len();
    indexWakeTop = html.First(seperatorWakeTop) + seperatorWakeTop.Len();
    indexWakeBottom =
        html.First(seperatorWakeBottom) + seperatorWakeBottom.Len();

    topHTML = html.substr(0, indexTop);
    bottomHTML = html.substr(indexBottom, indexWakeTop - indexBottom);
    middleHTML = html.substr(indexTop, indexBottom - indexTop);
    bottomWakeHTML = html.substr(indexWakeBottom, html.Len() - 1);
    middleWakeHTML = html.substr(indexWakeTop, indexWakeBottom - indexWakeTop);
    bottomHTML.Replace("#LFROM#", _("from"));
    bottomHTML.Replace("#LTO#", _("to"));
    bottomHTML.Replace("#LWATCH#", dialog->m_gridGlobal->GetColLabelValue(4));
  }

  path = data_locn;
  wxTextFile* logFile = new wxTextFile(path);
  if (mode != 0)
    path.Replace("txt", "html");
  else
    path = savePath;

  if (::wxFileExists(path)) ::wxRemoveFile(path);

  wxFileOutputStream output(path);
  wxTextOutputStream htmlFile(output);

  logFile->Open();

  wxString newMiddleHTML;
  wxString newWakeHTML;

  topHTML.Replace("#TYPE#", Export::replaceNewLine(
                                mode, dialog->boatType->GetValue(), false));
  topHTML.Replace("#BOATNAME#", Export::replaceNewLine(
                                    mode, dialog->boatName->GetValue(), false));
  topHTML.Replace("#HOMEPORT#", Export::replaceNewLine(
                                    mode, dialog->homeport->GetValue(), false));
  topHTML.Replace("#CALLSIGN#", Export::replaceNewLine(
                                    mode, dialog->callsign->GetValue(), false));
  topHTML.Replace(
      "#REGISTRATION#",
      Export::replaceNewLine(mode, dialog->registration->GetValue(), false));

  topHTML.Replace(
      "#LTYPE#",
      Export::replaceNewLine(mode, dialog->m_staticText128->GetLabel(), true));
  topHTML.Replace("#LBOATNAME#", Export::replaceNewLine(
                                     mode, dialog->bname->GetLabel(), true));
  topHTML.Replace(
      "#LHOMEPORT#",
      Export::replaceNewLine(mode, dialog->m_staticText114->GetLabel(), true));
  topHTML.Replace(
      "#LCALLSIGN#",
      Export::replaceNewLine(mode, dialog->m_staticText115->GetLabel(), true));
  topHTML.Replace(
      "#LREGISTRATION#",
      Export::replaceNewLine(mode, dialog->m_staticText118->GetLabel(), true));
  topHTML.Replace(
      "#LCREWLIST#",
      Export::replaceNewLine(mode, dialog->m_logbook->GetPageText(2), true));

  if (html.Contains("<!--Repeat -->")) {
    htmlFile << topHTML;

    int rowsMax = dialog->m_gridCrew->GetNumberRows();
    int colsMax = dialog->m_gridCrew->GetNumberCols();
    for (int row = 0; row < rowsMax; row++) {
      if (dialog->m_menu2->IsChecked(MENUCREWONBOARD) &&
          dialog->m_gridCrew->GetCellValue(row, ONBOARD) == "")
        continue;
      newMiddleHTML = middleHTML;
      for (int col = 0; col < colsMax; col++)
        newMiddleHTML = replacePlaceholder(newMiddleHTML, headerHTML, 0, row,
                                           col, 0, watch, 0);
      htmlFile << newMiddleHTML;
    }
    htmlFile << bottomHTML;
    topHTML = "";
  }

  if (html.Contains("<!--Repeat Wake -->")) {
    htmlFile << topHTML;

    int lineno;
    wxString s;
    int col = 1;
    unsigned int actDay = 0, lastday = 1;
    wxString tmp;
    int colTotal = 0;
    unsigned int maxDay = dialog->m_choiceWakeDisplay->GetSelection();
    unsigned int lineCount = watchListFile->GetLineCount();

    if (day == 0) day = 1;

    if (maxDay == 0)
      maxDay = -1;
    else
      maxDay = (day + maxDay);

    if ((lineno = getDayOne(day)) == -1) return;

    do {
      int offset = 0;
      colTotal = 0;
      col = 1;
      watch.clear();
      do {
        if (lineno >= (int)lineCount) break;
        s = watchListFile->GetLine(lineno);
        s = dialog->restoreDangerChar(s);
        wxStringTokenizer tkz(s, "\t");
        wxString t = tkz.GetNextToken();
        actDay = wxAtoi(t);
        if (actDay != lastday || actDay == maxDay) break;
        watch.Add(t);  // day
        tkz.GetNextToken();
        watch.Add(tkz.GetNextToken());  // watchlength

        wxString date = Logbook::makeDateFromFile(
            tkz.GetNextToken(), dialog->logbookPlugIn->opt->sdateformat);
        watch.Add(date);  // date

        wxString time = Logbook::makeWatchtimeFromFile(
            tkz.GetNextToken(), dialog->logbookPlugIn->opt->stimeformat);
        watch.Add(time);                // watch start/end
        watch.Add(tkz.GetNextToken());  // member

        lineno++;
        colTotal++;
        offset += 5;
      } while (!watchListFile->Eof());

      if (!watch.IsEmpty()) {
        tmp = middleWakeHTML;

        for (int row = 0; row < 5; row++) {
          tmp += "<tr>";
          for (int i = 0, offset = 0; i != colTotal; i++, offset += 5) {
            switch (row) {
              case 0:
                tmp += replacePlaceholder(columnWake0HTML, headerHTML, 1, row,
                                          col++, 0, watch, offset);
                break;
              case 1:
                tmp += replacePlaceholder(columnWake1HTML, headerHTML, 1, row,
                                          col++, 0, watch, offset);
                break;
              case 2:
                tmp += replacePlaceholder(columnWake2HTML, headerHTML, 1, row,
                                          col++, 0, watch, offset);
                break;
              case 3:
                tmp += replacePlaceholder(columnWake3HTML, headerHTML, 1, row,
                                          col++, 0, watch, offset);
                break;
              case 4:
                tmp += replacePlaceholder(columnWake4HTML, headerHTML, 1, row,
                                          col++, 0, watch, offset);
                break;
            }
          }
          tmp += "</tr>";
        }
        tmp += middleWake1HTML;
        htmlFile << tmp;
      }
      lastday++;

      if (actDay == maxDay) break;
    } while (lineno < (int)lineCount);
    htmlFile << bottomWakeHTML;
  }

  logFile->Close();
  output.Close();
}

wxString CrewList::replacePlaceholder(wxString html, wxString s, int nGrid,
                                      int row, int col, bool mode,
                                      wxArrayString watch, int offset) {
  wxGrid* grid = dialog->m_gridCrew;

  switch (nGrid) {
    case 0:
      switch (col) {
        case NAME:
          html.Replace("#NAME#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace("#LNAME#", Export::replaceNewLine(
                                      mode, grid->GetColLabelValue(col), true));
          break;
        case BIRTHNAME:
          html.Replace("#BIRTHNAME#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LBIRTHNAME#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case FIRSTNAME:
          html.Replace("#FIRSTNAME#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LFIRSTNAME#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case TITLE:
          html.Replace("#TITLE#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LTITLE#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case BIRTHPLACE:
          html.Replace("#BIRTHPLACE#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LBIRTHPLACE#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case BIRTHDATE:
          html.Replace("#BIRTHDATE#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LBIRTHDATE#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case NATIONALITY:
          html.Replace("#NATIONALITY#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LNATIONALITY#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case PASSPORT:
          html.Replace("#PASSPORT#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LPASSPORT#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case EST_IN:
          html.Replace("#EST_IN#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LEST_IN#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case EST_ON:
          html.Replace("#EST_ON#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LEST_ON#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case ZIP:
          html.Replace("#ZIP#", Export::replaceNewLine(
                                    mode, grid->GetCellValue(row, col), false));
          html.Replace("#LZIP#", Export::replaceNewLine(
                                     mode, grid->GetColLabelValue(col), true));
          break;
        case COUNTRY:
          html.Replace("#COUNTRY#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LCOUNTRY#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
        case TOWN:
          html.Replace("#TOWN#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace("#LTOWN#", Export::replaceNewLine(
                                      mode, grid->GetColLabelValue(col), true));
          break;
        case STREET:
          html.Replace("#STREET#",
                       Export::replaceNewLine(
                           mode, grid->GetCellValue(row, col), false));
          html.Replace(
              "#LSTREET#",
              Export::replaceNewLine(mode, grid->GetColLabelValue(col), true));
          break;
      }
      break;
    case 1: {
      html.Replace("#N#", wxString::Format("%i", col));
      html.Replace("#LWATCH#", dialog->m_gridGlobal->GetColLabelValue(4));
      html.Replace("#LENGTH#",
                   Export::replaceNewLine(mode, watch[1 + offset], false));
      html.Replace("#DATE#",
                   Export::replaceNewLine(mode, watch[2 + offset], false));
      html.Replace("#TIME#",
                   Export::replaceNewLine(mode, watch[3 + offset], false));
      html.Replace("#MEMBER#",
                   Export::replaceNewLine(mode, watch[4 + offset], false));
    }
    case 2: {
      wxString no = wxString::Format("#N%i#", col);
      wxString length = wxString::Format("#LENGTH%i#", col);
      wxString date = wxString::Format("#DATE%i#", col);
      wxString time = wxString::Format("#TIME%i#", col);
      wxString member = wxString::Format("#MEMBER%i#", col);

      html.Replace(no, wxString::Format("%i", col), false);
      html.Replace(length, Export::replaceNewLine(mode, watch[1], false),
                   false);
      html.Replace(date, Export::replaceNewLine(mode, watch[2], false), false);
      html.Replace(time, Export::replaceNewLine(mode, watch[3], false), false);
      html.Replace(member, Export::replaceNewLine(mode, watch[4], false),
                   false);
    } break;
  }
  html.Replace("#LADRESS#", _("Adress"));

  wxString str(html);
  return str;
}

wxString CrewList::readLayout(wxString layoutFileName) {
  wxString html, path;

  path = layout_locn + layoutFileName + ".html";
  ;
  wxTextFile layout(path);

  layout.Open();

  for (unsigned int i = 0; i < layout.GetLineCount(); i++) {
    html += layout.GetLine(i);
  }

  layout.Close();

  return html;
}

void CrewList::viewHTML(wxString path, wxString layout) {
  if (opt->filterLayout[LogbookDialog::CREW])
    layout.Prepend(opt->layoutPrefix[LogbookDialog::CREW]);

  saveHTML(path, layout, true);

  if (layout != "" && wxFile::Exists(html_locn))
    dialog->startBrowser(html_locn);
}

void CrewList::viewODT(wxString path, wxString layout) {
  if (opt->filterLayout[LogbookDialog::CREW])
    layout.Prepend(opt->layoutPrefix[LogbookDialog::CREW]);

  saveODT(path, layout, true);

  if (layout != "" && wxFile::Exists(ODT_locn))
    dialog->startApplication(ODT_locn, ".odt");
}

void CrewList::saveODT(wxString savePath, wxString layout, bool mode) {
  wxString path;

  if (layout == "") {
    wxMessageBox(_("Sorry, no Layout installed"), _("Information"), wxOK);
    return;
  }

  saveData();

  wxString odt = readLayoutODT(layout);

  int indexTop;
  int indexBottom;
  int indexWakeTop;
  int indexWakeBottom;
  int indexTop1;
  int indexBottom1;
  int indexWakeTop1;
  int indexWakeBottom1;

  wxString topODT;
  wxString bottomODT;
  wxString middleODT;
  wxString topWakeODT;
  wxString bottomWakeODT;
  wxString middleWakeODT;
  wxString headerODT;
  wxString middleData = "";

  wxString seperatorTop = "[[";
  wxString seperatorBottom = "]]";
  wxString seperatorWakeTop = "{{";
  wxString seperatorWakeBottom = "}}";

  if (!odt.Contains(seperatorWakeTop)) {
    indexTop = odt.First(seperatorTop);
    topODT = odt.substr(0, indexTop);
    indexTop1 = topODT.find_last_of('<');
    topODT = odt.substr(0, indexTop1);
    indexBottom = odt.First(seperatorBottom);
    bottomODT = odt.substr(indexBottom);
    indexBottom1 = bottomODT.find_first_of('>');
    bottomODT = bottomODT.substr(indexBottom1 + 1);
    middleODT = odt.substr(indexTop + 11, indexBottom);
    indexTop = middleODT.First(seperatorBottom);
    middleODT = middleODT.substr(0, indexTop);
    indexTop = middleODT.find_last_of('<');
    middleODT = middleODT.substr(0, indexTop);
  } else if (!odt.Contains(seperatorTop)) {
    indexWakeTop = odt.First(seperatorWakeTop);
    topODT = odt.substr(0, indexWakeTop);
    indexWakeTop1 = topODT.find_last_of('<');
    topODT = topODT.substr(0, indexWakeTop1);
    indexWakeBottom = odt.First(seperatorWakeBottom);
    bottomWakeODT = odt.substr(indexWakeBottom);
    indexWakeBottom1 = bottomWakeODT.find_first_of('>');
    bottomODT = bottomWakeODT.substr(indexWakeBottom1 + 1);
    middleODT = odt.substr(indexWakeTop + 11, indexWakeBottom);
    indexWakeTop = middleODT.First(seperatorWakeBottom);
    middleODT = middleODT.substr(0, indexWakeTop);
    indexWakeTop = middleODT.find_last_of('<');
    middleWakeODT = middleODT.substr(0, indexWakeTop);
    topODT.Replace("#LFROM#", _("from"));
    topODT.Replace("#LTO#", _("to"));
    topODT.Replace("#LWATCH#",
                   Export::replaceNewLine(
                       mode, dialog->m_gridGlobal->GetColLabelValue(4), true));
  } else {
    indexTop = odt.First(seperatorTop);
    topODT = odt.substr(0, indexTop);
    indexTop1 = topODT.find_last_of('<');
    topODT = topODT.substr(0, indexTop1);
    indexBottom = odt.First(seperatorBottom);
    bottomODT = odt.substr(indexBottom);
    indexBottom1 = bottomODT.find_first_of('>');
    bottomODT = bottomODT.substr(indexBottom1 + 1);

    middleODT = odt.substr(indexTop + 11, indexBottom);
    indexTop = middleODT.First(seperatorBottom);
    middleODT = middleODT.substr(0, indexTop);
    indexTop = middleODT.find_last_of('<');
    middleODT = middleODT.substr(0, indexTop);

    middleData = bottomODT;
    indexTop = middleData.Find(seperatorWakeTop);
    middleData = middleData.substr(0, indexTop);
    middleData = middleData.substr(0, middleData.find_last_of('<'));
    indexWakeTop = odt.First(seperatorWakeTop);

    indexWakeBottom = odt.First(seperatorWakeBottom);
    bottomWakeODT = odt.substr(indexWakeBottom);
    indexWakeBottom1 = bottomWakeODT.find_first_of('>');
    bottomODT = bottomWakeODT.substr(indexWakeBottom1 + 1);
    middleWakeODT = odt.substr(indexWakeTop + 11, indexWakeBottom);
    indexWakeTop = middleWakeODT.First(seperatorWakeBottom);
    middleWakeODT = middleWakeODT.substr(0, indexWakeTop);
    indexWakeTop = middleWakeODT.find_last_of('<');
    middleWakeODT = middleWakeODT.substr(0, indexWakeTop);

    middleData.Replace("#LFROM#", _("from"));
    middleData.Replace("#LTO#", _("to"));
    middleData.Replace(
        "#LWATCH#", Export::replaceNewLine(
                        mode, dialog->m_gridGlobal->GetColLabelValue(4), true));
  }

  topODT.Replace("#TYPE#", Export::replaceNewLine(
                               mode, dialog->boatType->GetValue(), false));
  topODT.Replace("#BOATNAME#", Export::replaceNewLine(
                                   mode, dialog->boatName->GetValue(), false));
  topODT.Replace("#HOMEPORT#", Export::replaceNewLine(
                                   mode, dialog->homeport->GetValue(), false));
  topODT.Replace("#CALLSIGN#", Export::replaceNewLine(
                                   mode, dialog->callsign->GetValue(), false));
  topODT.Replace(
      "#REGISTRATION#",
      Export::replaceNewLine(mode, dialog->registration->GetValue(), false));

  topODT.Replace(
      "#LTYPE#",
      Export::replaceNewLine(mode, dialog->m_staticText128->GetLabel(), true));
  topODT.Replace("#LBOATNAME#",
                 Export::replaceNewLine(mode, dialog->bname->GetLabel(), true));
  topODT.Replace(
      "#LHOMEPORT#",
      Export::replaceNewLine(mode, dialog->m_staticText114->GetLabel(), true));
  topODT.Replace(
      "#LCALLSIGN#",
      Export::replaceNewLine(mode, dialog->m_staticText115->GetLabel(), true));
  topODT.Replace(
      "#LREGISTRATION#",
      Export::replaceNewLine(mode, dialog->m_staticText118->GetLabel(), true));
  topODT.Replace(
      "#LCREWLIST#",
      Export::replaceNewLine(mode, dialog->m_logbook->GetPageText(2), true));

  path = data_locn;
  wxTextFile* logFile = new wxTextFile(path);
  if (mode != 0)
    path.Replace("txt", "odt");
  else
    path = savePath;

  if (::wxFileExists(path)) ::wxRemoveFile(path);

  ODT_locn = path;

  logFile->Open();

  wxString newMiddleODT;
  wxString newWakeODT;

  unique_ptr<wxFFileInputStream> in(
      new wxFFileInputStream(layout_locn + layout + ".odt"));
  wxTempFileOutputStream out(path);

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

  if (odt.Contains(seperatorTop)) {
    int rowsMax = dialog->m_gridCrew->GetNumberRows();
    int colsMax = dialog->m_gridCrew->GetNumberCols();
    for (int row = 0; row < rowsMax; row++) {
      if (dialog->m_menu2->IsChecked(MENUCREWONBOARD) &&
          dialog->m_gridCrew->GetCellValue(row, ONBOARD) == "")
        continue;
      newMiddleODT = middleODT;
#ifdef __WXOSX__
      wxArrayString watch;
      for (int col = 0; col < colsMax; col++)
        newMiddleODT = replacePlaceholder(newMiddleODT, headerODT, 0, row, col,
                                          mode, watch, 0);
#else
      for (int col = 0; col < colsMax; col++)
        newMiddleODT = replacePlaceholder(newMiddleODT, headerODT, 0, row, col,
                                          mode, (wxArrayString)0, 0);
#endif
      odtFile << newMiddleODT;
    }
  }

  if (!middleData.IsEmpty()) odtFile << middleData;

  if (odt.Contains(seperatorWakeTop)) {
    int lineno;
    unsigned int dDay = dialog->m_choiceWakeDisplay->GetSelection();
    unsigned int lineCount = watchListFile->GetLineCount();
    wxString s;
    int col = 1;
    unsigned int lastday = 1;
    wxString tmp;
    wxArrayString watch;

    middleWakeODT.Replace("#LWATCH#",
                          dialog->m_gridGlobal->GetColLabelValue(4));
    newWakeODT = middleWakeODT;

    if (day == 0) day = 1;

    if (dDay == 0)
      dDay = -1;
    else
      dDay = (day + dDay);

    if ((lineno = getDayOne(day)) == -1) return;

    s = watchListFile->GetLine(lineno);
    wxStringTokenizer tkz(s, "\t");
    wxString t = tkz.GetNextToken();
    lastday = wxAtoi(t);

    do {
      watch.clear();

      if (lineno >= (int)lineCount) break;
      s = watchListFile->GetLine(lineno);
      s = dialog->restoreDangerChar(s);

      wxStringTokenizer tkz(s, "\t");
      wxString t = tkz.GetNextToken();
      unsigned int z = wxAtoi(t);
      if (dDay == z) break;
      if (z != lastday) {
        col = 1;
        lastday++;
        newWakeODT = deleteODTCols(newWakeODT);
        odtFile << newWakeODT;
        newWakeODT = middleWakeODT;
      }

      watch.Add(t);  // day
      tkz.GetNextToken();
      watch.Add(tkz.GetNextToken());  // watchlength

      wxString date = Logbook::makeDateFromFile(
          tkz.GetNextToken(), dialog->logbookPlugIn->opt->sdateformat);
      watch.Add(date);  // date

      wxString time = Logbook::makeWatchtimeFromFile(
          tkz.GetNextToken(), dialog->logbookPlugIn->opt->stimeformat);
      watch.Add(time);  // watch start/end

      watch.Add(tkz.GetNextToken());  // member

      lineno++;
      newWakeODT = replacePlaceholder(newWakeODT, headerODT, 2, 0, col++, mode,
                                      watch, 0);
    } while (!watchListFile->Eof());
    newWakeODT = deleteODTCols(newWakeODT);
    odtFile << newWakeODT;
  }
  odtFile << bottomODT;
  inzip.Eof() && outzip.Close() && out.Commit();
  logFile->Close();
}

wxString CrewList::deleteODTCols(wxString newWakeODT) {
  if (!newWakeODT.Contains("#")) return newWakeODT;

  int first = 1;
  int last;
  wxString del;
  wxString w = dialog->m_gridGlobal->GetColLabelValue(4);

  while (true) {
    first = newWakeODT.find_first_of('#', first);
    if (first < 0) break;
    last = newWakeODT.find_first_of('#', first + 1);
    del = newWakeODT.substr(first, (last - first) + 1);
    if (del.Contains("#N")) del += "." + w;
    first = last;
    newWakeODT.Replace(del, wxEmptyString, false);
  }

  return newWakeODT;
}

wxString CrewList::readLayoutODT(wxString layout) {
  wxString odt = "";

  wxString filename = layout_locn + layout + ".odt";

  if (wxFileExists(filename)) {
#ifdef __WXOSX__
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
#else
    /*static const wxString fn = "content.xml";
    wxFileInputStream in(filename);
    wxZipInputStream zip(in);
    wxTextInputStream txt(zip);
    while(!zip.Eof())
            odt += txt.ReadLine();*/
#endif
  }
  return odt;
}

void CrewList::deleteRow(int row) {
  int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"), row + 1),
                            _("Confirm"), wxYES_NO | wxCANCEL, dialog);
  if (answer == wxYES) {
    gridCrew->DeleteRows(row);
    modified = true;
  }
}

void CrewList::saveXML(wxString path) {
  wxString s = "";
  wxString line;
  wxString temp;

  wxTextFile* xmlFile = new wxTextFile(path);

  if (xmlFile->Exists()) {
    ::wxRemoveFile(path);
    xmlFile->Create();
  }

  crewListFile->Open();

  if (crewListFile->GetLineCount() <= 0) {
    wxMessageBox(_("Sorry, Logbook has no lines"), _("Information"), wxOK);
    return;
  }

  xmlFile->AddLine(dialog->xmlHead);
  for (unsigned int i = 0; i < crewListFile->GetLineCount(); i++) {
    line = crewListFile->GetLine(i);
    wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);
    s = wxString::Format("<Row ss:Height=\"%u\">",
                         dialog->m_gridGlobal->GetRowHeight(i));

    while (tkz.HasMoreTokens()) {
      s += "<Cell>\n";
      s += "<Data ss:Type=\"String\">#DATA#</Data>\n";
      temp = tkz.GetNextToken().RemoveLast();
      temp.Replace("\\n", "&#10;");
      temp.Replace("&", "&amp;");
      temp.Replace("\"", "&quot;");
      temp.Replace("<", "&lt;");
      temp.Replace(">", "&gt;");
      temp.Replace("'", "&apos;");
      s.Replace("#DATA#", temp);
      s += "</Cell>";
    }
    s += "</Row>>";
    xmlFile->AddLine(s);
  }

  xmlFile->AddLine(dialog->xmlEnd);
  xmlFile->Write();
  crewListFile->Close();
  xmlFile->Close();
}

void CrewList::backup(wxString path) { wxCopyFile(data_locn, path); }

void CrewList::saveODS(wxString path) {
  wxString s = "";
  wxString line;
  wxString temp;

  wxFileInputStream input(data_locn);
  wxTextInputStream* stream = new wxTextInputStream(input);

  wxFFileOutputStream out(path);
  wxZipOutputStream zip(out);
  wxTextOutputStream txt(zip);
  wxString sep(wxFileName::GetPathSeparator());

  temp = dialog->content;
  temp.Replace("table:number-columns-repeated=\"33\"",
               "table:number-columns-repeated=\"14\"");
  temp.Replace("Logbook", "CrewList");
  zip.PutNextEntry("content.xml");
  txt << temp;

  txt << "<table:table-row table:style-name=\"ro2\">";

  for (int i = 0; i < dialog->m_gridCrew->GetNumberCols(); i++) {
    txt << "<table:table-cell office:value-type=\"string\">";
    txt << "<text:p>";
    txt << dialog->m_gridCrew->GetColLabelValue(i);
    txt << "</text:p>";
    txt << "</table:table-cell>";
  }

  txt << "</table:table-row>";

  long emptyCol = 0;

  while (input.Eof()) {
    line = stream->ReadLine();
    while (true) {
      wxString line = stream->ReadLine();
      if (input.Eof()) break;
      txt << "<table:table-row table:style-name=\"ro2\">";
      wxStringTokenizer tkz(line, "\t", wxTOKEN_RET_EMPTY);

      while (tkz.HasMoreTokens()) {
        wxString s = dialog->restoreDangerChar(tkz.GetNextToken().RemoveLast());
        if (s == "") {
          txt << "<table:table-cell />";
          emptyCol++;
          continue;
        }

        txt << "<table:table-cell office:value-type=\"string\">";

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
      }
      txt << "</table:table-row>";
      ;
    }
  }
  txt << dialog->contentEnd;

  zip.PutNextEntry("mimetype");
  txt << "application/vnd.oasis.opendocument.spreadsheet";

  zip.PutNextEntry("styles.xml");
  txt << dialog->styles;

  zip.PutNextEntry("meta.xml");
  txt << dialog->meta;

  zip.PutNextEntry("META-INF" + sep + "manifest.xml");
  txt << dialog->manifest;

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

void LogbookDialog::OnGridBeginDragWatch(wxGridEvent& event) {
  int row = event.GetRow();
  int col = event.GetCol();
  ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->col = col;

  if (row != 3) return;

  wxString s = m_gridCrewWake->GetCellValue(row, col);

  if (s.IsEmpty()) return;

  wxTextDataObject txtData(s);
  wxDropSource src(txtData, m_gridCrewWake);
  ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->source =
      m_gridCrewWake;
  ((DnDCrew*)m_gridCrew->GetGridWindow()->GetDropTarget())->source =
      m_gridCrewWake;
  wxDragResult res = src.DoDragDrop(wxDrag_AllowMove);

  if (res != wxDragNone &&
      ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->col != col)
    m_gridCrewWake->SetCellValue(row, col, " ");

  m_gridCrewWake->SetGridCursor(
      3, ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->col);
}

void LogbookDialog::OnGridBeginDragCrew(wxGridEvent& event) {
  wxString s;
  int row = crewList->selRow;

  for (int i = 0; i < m_gridCrew->GetNumberCols(); i++)
    s += m_gridCrew->GetCellValue(row, i) + "#";
  s.RemoveLast();
  s.RemoveLast();
  if (s.IsEmpty()) return;

#ifdef __WXOSX__
  // OSX needs it or textcttrl is droptarget, too
  m_textCtrlWatchStartDate->Enable(false);
  m_textCtrlWatchStartTime->Enable(false);
  m_textCtrlWakeDay->Enable(false);
  m_textCtrlWakeTrip->Enable(false);
#endif
  wxTextDataObject txtData(s);
  wxDropSource src(txtData, m_gridCrew);
  ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->source =
      m_gridCrew;
  ((DnDCrew*)m_gridCrew->GetGridWindow()->GetDropTarget())->source = m_gridCrew;
  wxDragResult res = src.DoDragDrop(wxDrag_AllowMove);

  s = ((DnDCrew*)m_gridCrew->GetGridWindow()->GetDropTarget())->moveStr;
  if ((res == wxDragMove || res == wxDragCopy) && !s.IsEmpty()) {
    for (int i = 0; i < m_gridCrew->GetNumberCols(); i++)
      m_gridCrew->SetCellValue(row, i, wxEmptyString);
    int i = 0;
    wxStringTokenizer tkz(s, "#");
    while (tkz.HasMoreTokens())
      m_gridCrew->SetCellValue(row, i++, tkz.GetNextToken());
    ((DnDCrew*)m_gridCrew->GetGridWindow()->GetDropTarget())->moveStr =
        wxEmptyString;
  }

  if (((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->col != -1)
    m_gridCrewWake->SetGridCursor(
        3, ((DnDWatch*)m_gridCrewWake->GetGridWindow()->GetDropTarget())->col);

#ifdef __WXOSX__
  // OSX needs it or textcttrl is droptarget, too
  m_textCtrlWatchStartDate->Enable(true);
  m_textCtrlWatchStartTime->Enable(true);
  m_textCtrlWakeDay->Enable(true);
  m_textCtrlWakeTrip->Enable(true);
#endif
}

bool DnDWatch::OnDropText(wxCoord x, wxCoord y, const wxString& text) {
  if (m_pOwner == NULL) return false;

  int xx, yy;
  wxString fname, name, oldTxt;

  m_pOwner->CalcUnscrolledPosition(x, y, &xx, &yy);

  int col = m_pOwner->XToCol(xx);
  int row = m_pOwner->YToRow(yy);

  if (this->col == col && source == crewList->gridWake) return false;
  this->col = col;
  this->row = row;

  if (col == wxNOT_FOUND || row == wxNOT_FOUND) return false;

  oldTxt = m_pOwner->GetCellValue(3, col);
  if (oldTxt.Length() == 1 && oldTxt.GetChar(0) == ' ') oldTxt.RemoveLast();

  if (text.Contains("#")) {
    wxStringTokenizer tkz(text, "#");
    tkz.GetNextToken(), name = tkz.GetNextToken();
    tkz.GetNextToken();
    fname = tkz.GetNextToken();

    if (!oldTxt.IsEmpty()) oldTxt += "\n";

    int sel = crewList->dialog->m_choiceCrewNames->GetSelection();

    switch (sel) {
      case 0:
        if (oldTxt.Contains((fname + " " + name))) return false;
        if (!fname.IsEmpty()) {
          m_pOwner->SetCellValue(3, col, oldTxt + fname + " " + name);
          crewList->checkMemberIsInMenu(fname + " " + name);
        } else {
          m_pOwner->SetCellValue(3, col, oldTxt + name);
          crewList->checkMemberIsInMenu(name);
        }
        break;
      case 1:
        if (oldTxt.Contains(fname)) return false;
        m_pOwner->SetCellValue(3, col, oldTxt + fname);
        crewList->checkMemberIsInMenu(oldTxt + fname);
        break;
      case 2:
        if (oldTxt.Contains(name)) return false;
        m_pOwner->SetCellValue(3, col, oldTxt + name);
        crewList->checkMemberIsInMenu(oldTxt + name);
        break;
    }
  } else if (text.Contains("\n")) {
    if (oldTxt.IsEmpty())
      oldTxt += text;
    else
      oldTxt += "\n" + text;

    m_pOwner->SetCellValue(3, col, oldTxt);
    m_pOwner->SetCellValue(3, crewList->selColWake, " ");

    if (crewList->day == ActualWatch::day && crewList->selColWake)
      ActualWatch::member = wxEmptyString;
  } else {
    wxString t = text;
    t.Replace("\n", " ");
    if (oldTxt.IsEmpty())
      oldTxt += t;
    else
      oldTxt += "\n" + t;

    m_pOwner->SetCellValue(3, col, oldTxt);

    oldTxt = m_pOwner->GetCellValue(3, crewList->selColWake);
    if (oldTxt.Contains("\n" + text))
      oldTxt.Replace("\n" + text, "");
    else if (oldTxt.Contains(text + "\n"))
      oldTxt.Replace(text + "\n", "");
    else
      oldTxt.Replace(text, " ");

    m_pOwner->SetCellValue(3, crewList->selColWake, oldTxt);

    if (crewList->day == ActualWatch::day && ActualWatch::col != -1) {
      wxString m = m_pOwner->GetCellValue(3, ActualWatch::col);
      ActualWatch::member = m;
    }
  }

  m_pOwner->AutoSizeRow(3);
  m_pOwner->AutoSizeColumn(col);
  m_pOwner->SetRowSize(3, m_pOwner->GetRowHeight(3) + 10);

  crewList->updateLine();
  if (ActualWatch::col == col && ActualWatch::day == crewList->day)
    ActualWatch::member = m_pOwner->GetCellValue(3, col);
  if (row == 3) crewList->statusText(CrewList::HITCALCULATE);

  m_pOwner->Refresh();
  return true;
}

wxDragResult DnDWatch::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
  int xx, yy;
  m_pOwner->CalcUnscrolledPosition(x, y, &xx, &yy);

  int col = m_pOwner->XToCol(xx);
  int row = m_pOwner->YToRow(yy);

  if (row == wxNOT_FOUND || col == wxNOT_FOUND) return def;

  m_pOwner->SetFocus();
  m_pOwner->SetGridCursor(3, col);
  return wxDragCopy;
}

wxDragResult DnDCrew::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
  int xx, yy;
  m_pOwner->CalcUnscrolledPosition(x, y, &xx, &yy);

  int col = m_pOwner->XToCol(xx);
  int row = m_pOwner->YToRow(yy);

  if (row == wxNOT_FOUND || col == wxNOT_FOUND) return def;
  m_pOwner->SetFocus();
  m_pOwner->SetGridCursor(row, col);
  if (m_pOwner == crewList->gridWake)
    return wxDragCopy;
  else
    return def;
}

bool DnDCrew::OnDropText(wxCoord x, wxCoord y, const wxString& text) {
  this->col = -1;

  if (m_pOwner == NULL || source == crewList->gridWake) return false;

  int xx, yy;
  m_pOwner->CalcUnscrolledPosition(x, y, &xx, &yy);

  int col = m_pOwner->XToCol(xx);
  int row = m_pOwner->YToRow(yy);

  if ((row == wxNOT_FOUND) || (col == wxNOT_FOUND)) return false;
  this->col = col;

  moveStr = wxEmptyString;
  for (int i = 0; i < m_pOwner->GetNumberCols(); i++)
    moveStr += m_pOwner->GetCellValue(row, i) + "#";
  moveStr.RemoveLast();
  moveStr.RemoveLast();

  wxStringTokenizer tkz(text, "#");
  int n = 0;
  while (tkz.HasMoreTokens())
    m_pOwner->SetCellValue(row, n++, tkz.GetNextToken());

  m_pOwner->Refresh();
  return true;
}
