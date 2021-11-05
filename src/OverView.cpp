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

#include <map>


#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/generic/gridctrl.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>

#include "LogbookDialog.h"
#include "Logbook.h"
#include "logbook_pi.h"
#include "Options.h"
#include "OverView.h"

using namespace std;
OverView::OverView(LogbookDialog* d, wxString data, wxString lay,
                   wxString layoutODT)
    : Export(d) {
  parent = d;

  ODTLayout_locn = layoutODT;
  HTMLLayout_locn = lay;
  data_locn = data;
  data_file = data + "overview.html";
  grid = d->m_gridOverview;
  opt = d->logbookPlugIn->opt;
  selectedRow = 0;
  logbook = d->logbook;
  collection t_coll;
  setLayoutLocation();
  loadAllLogbooks();
}

OverView::~OverView(void) {}

void OverView::refresh() {
  grid->DeleteRows(0, grid->GetNumberRows());
  row = -1;
  if (parent->m_radioBtnActualLogbook->GetValue())
    actualLogbook();
  else if (parent->m_radioBtnAllLogbooks->GetValue())
    allLogbooks();
  else if (parent->m_radioBtnSelectLogbook->GetValue())
    if (!selectedLogbook.IsEmpty()) loadLogbookData(selectedLogbook, false);
}

void OverView::viewODT(wxString path, wxString layout, int mode) {
  wxString fn;

  if (opt->filterLayout[LogbookDialog::OVERVIEW])
    layout.Prepend(opt->layoutPrefix[LogbookDialog::OVERVIEW]);

  fn = toODT(path, layout, mode);

  if (layout != "") {
    fn.Replace("txt", "odt");
    parent->startApplication(fn, ".odt");
  }
}

void OverView::viewHTML(wxString path, wxString layout, int mode) {
  wxString fn;

  if (opt->filterLayout[LogbookDialog::OVERVIEW])
    layout.Prepend(opt->layoutPrefix[LogbookDialog::OVERVIEW]);

  fn = toHTML(path, layout, mode);

  if (layout != "") {
    fn.Replace("txt", "html");
    parent->startBrowser(fn);
  }
}

wxString OverView::toODT(wxString path, wxString layout, int mode) {
  wxString top;
  wxString header;
  wxString middle;
  wxString bottom;
  wxString tempPath = data_file;

  wxString odt = readLayoutODT(layout_locn, layout);
  if (!cutInPartsODT(odt, &top, &header, &middle, &bottom)) return "";

  if (mode == 1) tempPath.Replace("html", "txt");
  wxTextFile* text = setFiles(path, &tempPath, mode);
  writeToODT(text, parent->m_gridOverview, tempPath,
             layout_locn + layout + ".odt", top, header, middle, bottom, mode);

  return tempPath;
}

wxString OverView::toHTML(wxString path, wxString layout, int mode) {
  wxString top;
  wxString header;
  wxString middle;
  wxString bottom;
  wxString tempPath = data_file;

  wxString html = readLayoutHTML(layout_locn, layout);
  if (!cutInPartsHTML(html, &top, &header, &middle, &bottom)) return "";

  wxTextFile* text = setFiles(path, &tempPath, mode);
  writeToHTML(text, parent->m_gridOverview, tempPath,
              layout_locn + layout + ".html", top, header, middle, bottom,
              mode);

  return tempPath;
}

void OverView::loadAllLogbooks() {
  wxArrayString files;

  logbooks.clear();
  int i = wxDir::GetAllFiles(data_locn, &files, "*logbook*.txt", wxDIR_FILES);

  for (int f = 0; f < i; f++) {
    // wxFileName name(files.Item(f));
    // if(name.GetName().Contains("logbook"))
    logbooks.Add(files[f]);
  }
}

void OverView::selectLogbook() {
  SelectLogbook selLogbook(parent, data_locn);

  if (selLogbook.ShowModal() == wxID_CANCEL) return;

  if (selLogbook.selRow == -1) return;

  parent->m_radioBtnSelectLogbook->SetValue(true);
  grid->DeleteRows(0, grid->GetNumberRows());
  row = -1;

  selectedLogbook = selLogbook.files[selLogbook.selRow];
  showAllLogbooks = false;
  loadLogbookData(selectedLogbook, false);
  opt->overviewAll = 2;
}

void OverView::actualLogbook() {
  showAllLogbooks = false;
  clearGrid();
  for (unsigned int i = 0; i < logbooks.size(); i++)
    if (!logbooks[i].Contains("until")) {
      loadLogbookData(logbooks[i], false);
      break;
    }
  opt->overviewAll = 0;
}

void OverView::allLogbooks() {
  showAllLogbooks = true;
  loadAllLogbooks();
  clearGrid();
  for (unsigned int i = 0; i < logbooks.Count(); i++) {
    if (i % 2)
      loadLogbookData(logbooks[i], true);
    else
      loadLogbookData(logbooks[i], false);
  }
  opt->overviewAll = 1;
}

void OverView::clearGrid() {
  if (row != -1) grid->DeleteRows(0, grid->GetNumberRows());
  row = -1;
}

void OverView::loadLogbookData(wxString logbook, bool colour) {
  wxString t, s, temp;
  bool test = true;
  bool overviewflag = false;
  //	bool write = true;
  double x = 0;
  wxStringTokenizer tkz1;
  wxTimeSpan span;
  wxDateSpan oneday(0, 0, 0, 1);
  wxDateTime enddt;

  collection::iterator it;

  resetValues();

  wxFileInputStream input(logbook);
  wxTextInputStream* stream = new wxTextInputStream(input);
  wxString path = logbook;
  wxFileName fn(logbook);
  logbook = fn.GetName();
  if (logbook == "logbook")
    logbook = _("Active Logbook");
  else {
    wxDateTime dt = parent->getDateTo(logbook);
    logbook = _("Logbook until ") + dt.FormatDate();
  }

  int lastrow = 0;
  oneLogbookTotalReset();

  wxString route = "xxx";
  int rowNewLogbook = -1;

  stream->ReadLine();  // skip line with #1.2#
  int month = 0, day = 0, year = 0, hour = 0, min = 0, sec = 0;

  while ((!(t = stream->ReadLine()).IsEmpty())) {
    sign = wxEmptyString;
    rowNewLogbook++;
    wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;
    while (tkz.HasMoreTokens()) {
      s = parent->restoreDangerChar(tkz.GetNextToken());
      s.RemoveLast();
      s.Replace(",", ".");

      switch (c) {
        case ROUTE:
          if (route != s) {
            // write = true;
            resetValues();
            grid->AppendRows();
            route = s;
            row++;
            lastrow = row;
            test = true;
            grid->SetCellValue(row, FROUTE, s);
          } else
            ;
          // write = false;

          break;
        case DATEM:
          if (s.IsEmpty())
            month = -1;
          else
            month = wxAtoi(s);
          break;
        case DATED:
          if (s.IsEmpty())
            day = -1;
          else
            day = wxAtoi(s);
          break;
        case DATEY: {
          year = wxAtoi(s);
          if (day == -1 || month == -1) continue;

          wxDateTime tmp;
          tmp.Set(day, (wxDateTime::Month)month, year);
          s = tmp.Format(opt->sdateformat);

          if (s != startdate && !test && !showAllLogbooks &&
              opt->overviewlines) {
            temp = route;
            endtime = "00:00";
            parent->myParseDate(startdate, enddt);
            enddt.Add(oneday);
            enddate = enddt.Format(opt->sdateformat);
            // write = true;
            writeSumColumn(lastrow, logbook, path, colour);
            resetValues();
            grid->AppendRows();
            route = temp;
            overviewflag = true;
            row++;
            lastrow = row;
            test = true;
            grid->SetCellValue(row, FROUTE, route);
          }

          if (test) {
            startdate = s;
            enddate = s;
            if (rowNewLogbook == 0)
              oneLogbookTotal.logbookStart = oneLogbookTotal.logbookEnd = s;
          } else
            enddate = s;

          if (etmaldate != s) {
            etmaldate = s;
            etmalcount++;
            bestetmaltemp = 0;
          }

          date = s;
        } break;
        case TIMEH:
          if (s.IsEmpty())
            hour = -1;
          else
            hour = wxAtoi(s);
          break;
        case TIMEM:
          if (s.IsEmpty())
            min = -1;
          else
            min = wxAtoi(s);
          break;
        case TIMES: {
          if (hour == -1 || min == -1) continue;
          sec = wxAtoi(s);
          wxDateTime tmp;
          tmp.Set(hour, min, sec);
          s = tmp.Format(opt->stimeformat);

          if (test) {
            if (overviewflag)
              starttime = "00:00";
            else
              starttime = s;
            endtime = s;
            if (rowNewLogbook == 0)
              oneLogbookTotal.logbookTimeStart =
                  oneLogbookTotal.logbookTimeEnd = s;
          } else
            endtime = s;
        } break;
        case STATUS:
          sign = s;
          break;
        case WATCH:
          break;
        case DISTANCE:
          if (!s.IsEmpty() && sign != 'S') break;
          s.ToDouble(&x);

          distance += x;
          etmal = distance / etmalcount;

          if (date == etmaldate) {
            bestetmaltemp += x;
            if (bestetmaltemp > bestetmal) bestetmal = bestetmaltemp;
            if (bestetmaltemp > oneLogbookTotal.bestetmal)
              oneLogbookTotal.bestetmal = bestetmaltemp;
          }
          oneLogbookTotal.distance += x;
          // allLogbooksTotal.distance += x;
          break;
        case DISTANCETOTAL:
        case POSITION:
        case COG:
        case HEADING:
          break;
        case SOG:
          if (!s.IsEmpty() && sign == "S") {
            s.ToDouble(&x);
            speed += x;
            oneLogbookTotal.speed += x;
            speedcount++;
            oneLogbookTotal.speedcount++;
            if (x > speedpeak) speedpeak = x;
            if (x > oneLogbookTotal.speedpeak) oneLogbookTotal.speedpeak = x;
          }
          break;
        case STW:
          if (!s.IsEmpty() && sign == "S") {
            s.ToDouble(&x);
            speedSTW += x;
            oneLogbookTotal.speedSTW += x;
            speedcountSTW++;
            oneLogbookTotal.speedcountSTW++;
            if (x > speedpeakSTW) speedpeakSTW = x;
            if (x > oneLogbookTotal.speedpeakSTW)
              oneLogbookTotal.speedpeakSTW = x;
          }
          break;
        case DEPTH:
        case REMARKS:
        case BAROMETER:
        case HYDRO:
        case TEMPAIR:
        case TEMPWATER:
          break;
        case WIND:
          if (!s.IsEmpty()) {
            s.ToDouble(&x);
            winddir += x;
            oneLogbookTotal.winddir += x;
            windcount++;
            oneLogbookTotal.windcount++;
          }
          break;
        case WINDFORCE:
          if (!s.IsEmpty()) {
            if (s.Length() < 11) {
              s.ToDouble(&x);
              wind += x;
              oneLogbookTotal.wind += x;
              if (x > windpeak) windpeak = x;
              if (x > oneLogbookTotal.windpeak) oneLogbookTotal.windpeak = x;
            } else {
              wxArrayString WS;
              WS = wxStringTokenize(s, "|");
              WS[1].ToDouble(&x);
              wind += x;
              oneLogbookTotal.wind += x;
              WS[2].ToDouble(&x);
              if (x > windpeak) windpeak = x;
              if (x > oneLogbookTotal.windpeak) oneLogbookTotal.windpeak = x;
            }
          }
          break;
        case CURRENT:
          if (!s.IsEmpty()) {
            s.ToDouble(&x);
            currentdir += x;
            oneLogbookTotal.currentdir += x;
            currentcount++;
            oneLogbookTotal.currentcount++;
          }
          break;
        case CURRENTFORCE:
          if (!s.IsEmpty()) {
            s.ToDouble(&x);
            current += x;
            oneLogbookTotal.current += x;
            if (x > currentpeak) currentpeak = x;
            if (x > oneLogbookTotal.currentpeak)
              oneLogbookTotal.currentpeak = x;
          }
          break;
        case WAVE:
          if (!s.IsEmpty()) {
            s.ToDouble(&x);
            wave += x;
            oneLogbookTotal.wave += x;
            wavecount++;
            oneLogbookTotal.wavecount++;
            if (x > wavepeak) wavepeak = x;
            if (x > oneLogbookTotal.wavepeak) oneLogbookTotal.wavepeak = x;
          }
          break;
        case SWELL:
          if (!s.IsEmpty()) {
            s.ToDouble(&x);
            swell += x;
            oneLogbookTotal.swell += x;
            swellcount++;
            oneLogbookTotal.swellcount++;
            if (x > swellpeak) swellpeak = x;
            if (x > oneLogbookTotal.swellpeak) oneLogbookTotal.swellpeak = x;
          }
          break;
        case WEATHER:
        case CLOUDS:
        case VISIBILITY:
          break;
        case ENGINE1:
          if (s.IsEmpty()) continue;

          tkz1.SetString(s, ":", wxTOKEN_RET_EMPTY);
          long hours, minutes;
          tkz1.GetNextToken().ToLong(&hours);
          tkz1.GetNextToken().ToLong(&minutes);
          enginehours += hours;
          enginemin += minutes;
          if (enginemin >= 60) {
            enginehours++;
            enginemin -= 60;
          }
          oneLogbookTotal.enginehours += hours;
          oneLogbookTotal.enginemin += minutes;
          if (oneLogbookTotal.enginemin >= 60) {
            oneLogbookTotal.enginehours++;
            oneLogbookTotal.enginemin -= 60;
          }
          break;
        case ENGINE2:
          if (s.IsEmpty()) continue;

          {
            tkz1.SetString(s, ":", wxTOKEN_RET_EMPTY);
            long hours, minutes;
            tkz1.GetNextToken().ToLong(&hours);
            tkz1.GetNextToken().ToLong(&minutes);
            enginehours2 += hours;
            enginemin2 += minutes;
            if (enginemin2 >= 60) {
              enginehours2++;
              enginemin2 -= 60;
            }
            oneLogbookTotal.enginehours2 += hours;
            oneLogbookTotal.enginemin2 += minutes;
            if (oneLogbookTotal.enginemin2 >= 60) {
              oneLogbookTotal.enginehours2++;
              oneLogbookTotal.enginemin2 -= 60;
            }
          }
          break;
        case GENERATOR:
          if (s.IsEmpty()) continue;

          {
            tkz1.SetString(s, ":", wxTOKEN_RET_EMPTY);
            long hours, minutes;
            tkz1.GetNextToken().ToLong(&hours);
            tkz1.GetNextToken().ToLong(&minutes);
            generatorhours += hours;
            generatormin += minutes;
            if (generatormin >= 60) {
              generatorhours++;
              generatormin -= 60;
            }
            oneLogbookTotal.generatorhours += hours;
            oneLogbookTotal.generatormin += minutes;
            if (oneLogbookTotal.generatormin >= 60) {
              oneLogbookTotal.generatorhours++;
              oneLogbookTotal.generatormin -= 60;
            }
          }
          break;
        case WATERM:
          if (s.IsEmpty()) continue;

          {
            tkz1.SetString(s, ":", wxTOKEN_RET_EMPTY);
            long hours, minutes;
            tkz1.GetNextToken().ToLong(&hours);
            tkz1.GetNextToken().ToLong(&minutes);
            watermhours += hours;
            watermmin += minutes;
            if (watermmin >= 60) {
              watermhours++;
              watermmin -= 60;
            }
            oneLogbookTotal.watermhours += hours;
            oneLogbookTotal.watermmin += minutes;
            if (oneLogbookTotal.watermmin >= 60) {
              oneLogbookTotal.watermhours++;
              oneLogbookTotal.watermmin -= 60;
            }
          }
          break;
        case ENGINE1T:
          break;
        case FUEL:
          if (s.IsEmpty()) continue;

          if (s.GetChar(0) == '+') break;
          s.ToDouble(&x);
          if (s.GetChar(0) != '-')
            s.Prepend("-");  // version 0.910 has no minus sign

          //	if(x < 0)
          //	{
          fuel += x;
          oneLogbookTotal.fuel += x;
          //	}
          break;
        case WATERMO:
          if (s.IsEmpty()) continue;
          s.ToDouble(&x);
          //	if(x < 0)
          //	{
          watermo += x;
          oneLogbookTotal.watermo += x;
          //	}
          break;
        case FUELTOTAL:
          break;
        case SAILS:
          if (!s.empty()) {
            bool found = false;
            wxString result;
            for (it = t_coll.begin(); it != t_coll.end(); ++it) {
              if (s == it->first) {
                int a = it->second;
                it->second = ++a;
                result += it->first + "\n";
                found = true;
              }
            }
            if (!found) {
              t_coll.insert(pair(s, 1));
              wxString result;
              for (it = t_coll.begin(); it != t_coll.end(); ++it) {
                result = it->first;
              }
            }
          }
          break;
        case REEF:
          break;
        case WATER:
          if (s.IsEmpty()) continue;
          if (s.GetChar(0) == '+') break;  // add water used only
          if (s.GetChar(0) != '-')
            s.Prepend("-");  // version 0.910 has no minus sign
          s.ToDouble(&x);

          x = fabs(x);
          //	if(x < 0)
          //	{
          water += x;
          oneLogbookTotal.water += x;
          //	}
          break;
        case BANK1:
          if (s.IsEmpty()) continue;
          s.ToDouble(&x);
          if (x >= 0) {
            bank1g += x;
            oneLogbookTotal.bank1g += x;
          } else {
            bank1u += fabs(x);
            oneLogbookTotal.bank1u += bank1u;
          }
          break;
        case BANK2:
          if (s.IsEmpty()) continue;
          s.ToDouble(&x);
          if (x >= 0) {
            bank2g += x;
            oneLogbookTotal.bank2g += x;
          } else {
            bank2u += fabs(x);
            oneLogbookTotal.bank2u += bank2u;
          }
          break;
        case WATERTOTAL:
          break;
        case MREMARKS:
          break;
      }
      c++;
    }
    // if(test)
    writeSumColumn(lastrow, logbook, path, colour);
    test = false;
    overviewflag = false;
  }
  if (!showAllLogbooks)
    writeSumColumnLogbook(oneLogbookTotal, lastrow, logbook, colour);
}

void OverView::resetValues() {
  startdate = "";
  enddate = "";
  starttime = "";
  endtime = "";
  etmaldate = "";
  etmal = 0;
  bestetmal = 0;
  bestetmaltemp = 0;
  enginehours = 0;
  enginemin = 0;
  enginehours2 = 0;
  enginemin2 = 0;
  watermhours = 0;
  watermmin = 0;
  watermo = 0;
  generatorhours = 0;
  generatormin = 0;
  bank1u = 0;
  bank2u = 0;
  bank1g = 0;
  bank2g = 0;
  distance = 0;
  speed = 0;
  speedpeak = 0;
  speedSTW = 0;
  speedpeakSTW = 0;
  water = 0;
  fuel = 0;
  wind = 0;
  winddir = 0;
  wave = 0;
  current = 0;
  currentdir = 0;
  swell = 0;
  windpeak = 0;
  wavepeak = 0;
  currentpeak = 0;
  swellpeak = 0;

  windcount = 0;
  wavecount = 0;
  swellcount = 0;
  currentcount = 0;
  etmalcount = 0;
  speedcount = 0;
  speedcountSTW = 0;
  t_coll.clear();

  sign = wxEmptyString;
}

void OverView::oneLogbookTotalReset() {
  oneLogbookTotal.sails.Clear();

  oneLogbookTotal.logbookStart = wxEmptyString;
  oneLogbookTotal.logbookEnd = wxEmptyString;
  oneLogbookTotal.logbookTimeStart = wxEmptyString;
  oneLogbookTotal.logbookTimeEnd = wxEmptyString;
  oneLogbookTotal.distance = 0;
  oneLogbookTotal.bestetmal = 0;
  oneLogbookTotal.speed = 0;
  oneLogbookTotal.speedpeak = 0;
  oneLogbookTotal.speedSTW = 0;
  oneLogbookTotal.speedpeakSTW = 0;
  oneLogbookTotal.enginehours = 0;
  oneLogbookTotal.enginemin = 0;
  oneLogbookTotal.enginehours2 = 0;
  oneLogbookTotal.enginemin2 = 0;
  oneLogbookTotal.watermhours = 0;
  oneLogbookTotal.watermmin = 0;
  oneLogbookTotal.watermo = 0;
  oneLogbookTotal.generatorhours = 0;
  oneLogbookTotal.generatormin = 0;
  oneLogbookTotal.bank1u = 0;
  oneLogbookTotal.bank2u = 0;
  oneLogbookTotal.bank1g = 0;
  oneLogbookTotal.bank2g = 0;
  oneLogbookTotal.fuel = 0;
  oneLogbookTotal.water = 0;
  oneLogbookTotal.winddir = 0;
  oneLogbookTotal.wind = 0;
  oneLogbookTotal.wave = 0;
  oneLogbookTotal.swell = 0;
  oneLogbookTotal.current = 0;
  oneLogbookTotal.currentdir = 0;
  oneLogbookTotal.windpeak = 0;
  oneLogbookTotal.wavepeak = 0;
  oneLogbookTotal.swellpeak = 0;
  oneLogbookTotal.currentpeak = 0;

  oneLogbookTotal.windcount = 0;
  oneLogbookTotal.currentcount = 0;
  oneLogbookTotal.wavecount = 0;
  oneLogbookTotal.swellcount = 0;
  oneLogbookTotal.etmalcount = 0;
  oneLogbookTotal.speedcount = 0;
  oneLogbookTotal.speedcountSTW = 0;
}

void OverView::writeSumColumn(int row, wxString logbook, wxString path,
                              bool colour) {
  wxString d, sail;
  wxString nothing = "-----";

  switch (opt->showWaveSwell) {
    case 0:
      d = opt->meter;
      break;
    case 1:
      d = opt->feet;
      break;
    case 2:
      d = opt->fathom;
      break;
  }

  grid->SetCellAlignment(row, FLOG, wxALIGN_LEFT, wxALIGN_TOP);
  grid->SetCellAlignment(row, FROUTE, wxALIGN_LEFT, wxALIGN_TOP);
  grid->SetCellAlignment(row, FSAILS, wxALIGN_LEFT, wxALIGN_TOP);

  grid->SetCellValue(row, FLOG, logbook);
  grid->SetCellValue(row, FSTART, startdate);
  grid->SetCellValue(row, FEND, enddate);

  wxString temp =
      wxString::Format("%6.2f %s", distance, opt->showDistance.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FDISTANCE, temp);
  temp = wxString::Format("%6.2f %s", etmal, opt->showDistance.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FETMAL, temp);
  temp = wxString::Format("%6.2f %s", bestetmal, opt->showDistance.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBESTETMAL, temp);

  temp = wxString::Format("%6.2f %s", fabs(fuel), opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FFUEL, temp);
  temp = wxString::Format("%6.2f %s", fabs(water), opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATER, temp);

  temp = wxString::Format("%3.2f %s", bank1u, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK1U, temp);
  temp = wxString::Format("%3.2f %s", bank1g, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK1G, temp);
  temp = wxString::Format("%3.2f %s", bank2u, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK2U, temp);
  temp = wxString::Format("%3.2f %s", bank2g, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK2G, temp);

  if (windcount)
    temp = wxString::Format("%6.2f %s", wind / windcount, "kts");
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWIND, temp);

  if (windcount)
    temp = wxString::Format("%6.2f %s", winddir / windcount, opt->Deg.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWINDDIR, temp);

  temp = wxString::Format("%6.2f %s", windpeak, "kts");
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWINDPEAK, temp);

  if (wavecount)
    temp = wxString::Format("%6.2f %s", wave / wavecount, d.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWAVE, temp);

  temp = wxString::Format("%6.2f %s", wavepeak, d.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWAVEPEAK, temp);

  if (swellcount)
    temp = wxString::Format("%6.2f %s", swell / swellcount, d.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSWELL, temp);

  temp = wxString::Format("%6.2f %s", swellpeak, d.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSWELLPEAK, temp);

  if (currentcount)
    temp = wxString::Format("%6.2f %s", currentdir / currentcount,
                            opt->Deg.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENTDIR, temp);

  if (currentcount)
    temp = wxString::Format("%6.2f %s", current / currentcount,
                            opt->showBoatSpeed.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENT, temp);

  temp = wxString::Format("%6.2f %s", currentpeak, opt->showBoatSpeed.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENTPEAK, temp);

  temp = wxString::Format("%0002i:%02i %s", enginehours, enginemin,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FENGINE1, temp);

  temp = wxString::Format("%0002i:%02i %s", enginehours2, enginemin2,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FENGINE2, temp);

  temp = wxString::Format("%0002i:%02i %s", generatorhours, generatormin,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FGENERATOR, temp);

  temp = wxString::Format("%0002i:%02i %s", watermhours, watermmin,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATERM, temp);

  temp = wxString::Format("%3.2f %s", watermo, opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATERMO, temp);

  if (speedcount)
    temp = wxString::Format("%6.2f %s", speed / speedcount,
                            opt->showBoatSpeed.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSPEED, temp);

  temp = wxString::Format("%6.2f %s", speedpeak, opt->showBoatSpeed.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBSPEED, temp);

  //	if(speedcountSTW)
  //		temp = wxString::Format("%6.2f
  //%s",speedSTW/speedcountSTW,opt->speed.c_str()); 	else 		temp = nothing;
  //	temp.Replace(".",parent->decimalPoint);
  //	grid->SetCellValue(row,FSPEEDSTW,temp);

  //	temp = wxString::Format("%6.2f %s",speedpeakSTW,opt->speed.c_str());
  //	temp.Replace(".",parent->decimalPoint);
  //	grid->SetCellValue(row,FBSPEEDSTW,temp);

  grid->SetCellValue(row, FPATH, path);

  wxDateTime startdt, enddt, timet;

  if ((startdate.IsEmpty() || enddate.IsEmpty()) ||
      (starttime.IsEmpty() || endtime.IsEmpty()))
    return;

  parent->myParseTime(starttime, timet);
  wxTimeSpan timesp(timet.GetHour(), timet.GetMinute());
  parent->myParseDate(startdate, startdt);
  startdt.Add(timesp);

  parent->myParseTime(endtime, timet);
  wxTimeSpan timespe(timet.GetHour(), timet.GetMinute());
  parent->myParseDate(enddate, enddt);
  enddt.Add(timespe);

  wxTimeSpan journey = enddt.Subtract(startdt);
  wxTimeSpan time;
  if (journey.GetDays() > 0) {
    journey.GetMinutes();
    wxTimeSpan t(0, journey.GetMinutes() - (journey.GetDays() * 24 * 60));
    time = t;
  } else {
    wxTimeSpan t(0, journey.GetMinutes());
    time = t;
  }
  grid->SetCellValue(
      row, FJOURNEY,
      wxString::Format("%s %s %s %s", journey.Format("%D").c_str(),
                       opt->days.c_str(), time.Format("%H:%M").c_str(),
                       opt->motorh.c_str()));

  int max = 0;
  wxString result;
  collection::iterator it;

  for (it = t_coll.begin(); it != t_coll.end(); ++it)
    if (it->second >= max) {
      sail = it->first;
      max = it->second;
    }
  grid->SetCellValue(row, FSAILS, sail);

  if (colour)
    for (int i = 0; i < grid->GetNumberCols(); i++)
      grid->SetCellBackgroundColour(row, i, wxColour(230, 230, 230));
}

void OverView::writeSumColumnLogbook(total data, int row, wxString logbook,
                                     bool colour) {
  wxString nothing = "-----";

  parent->m_gridOverview->AppendRows();
  row = parent->m_gridOverview->GetNumberRows() - 1;
  for (int i = 0; i < parent->m_gridOverview->GetNumberCols(); i++)
    grid->SetCellBackgroundColour(row, i, wxColour(156, 156, 156));

  wxString d, sail;
  switch (opt->showWaveSwell) {
    case 0:
      d = opt->meter;
      break;
    case 1:
      d = opt->feet;
      break;
    case 2:
      d = opt->fathom;
      break;
  }

  grid->SetCellAlignment(row, FLOG, wxALIGN_LEFT, wxALIGN_TOP);
  grid->SetCellAlignment(row, FROUTE, wxALIGN_LEFT, wxALIGN_TOP);
  grid->SetCellAlignment(row, FSAILS, wxALIGN_LEFT, wxALIGN_TOP);

  grid->SetCellValue(row, FLOG, _("Logbook Total"));
  grid->SetCellValue(row, FSTART, data.logbookStart);
  grid->SetCellValue(row, FEND, enddate);

  wxString temp =
      wxString::Format("%6.2f %s", data.distance, opt->showDistance.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FDISTANCE, temp);

  temp =
      wxString::Format("%6.2f %s", data.bestetmal, opt->showDistance.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBESTETMAL, temp);

  temp = wxString::Format("%6.2f %s", fabs(data.fuel), opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FFUEL, temp);
  temp = wxString::Format("%6.2f %s", fabs(data.water), opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATER, temp);

  temp = wxString::Format("%3.2f %s", data.bank1u, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK1U, temp);
  temp = wxString::Format("%3.2f %s", data.bank1g, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK1G, temp);
  temp = wxString::Format("%3.2f %s", data.bank2u, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK2U, temp);
  temp = wxString::Format("%3.2f %s", data.bank2g, opt->ampereh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBANK2G, temp);

  if (data.windcount)
    temp = wxString::Format("%6.2f %s", data.wind / data.windcount, "kts");
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWIND, temp);

  if (data.windcount)
    temp = wxString::Format("%6.2f %s", data.winddir / data.windcount,
                            opt->Deg.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWINDDIR, temp);

  temp = wxString::Format("%6.2f %s", data.windpeak, "kts");
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWINDPEAK, temp);

  if (data.wavecount)
    temp = wxString::Format("%6.2f %s", data.wave / data.wavecount, d.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWAVE, temp);

  temp = wxString::Format("%6.2f %s", data.wavepeak, d.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWAVEPEAK, temp);

  if (data.swellcount)
    temp =
        wxString::Format("%6.2f %s", data.swell / data.swellcount, d.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSWELL, temp);

  temp = wxString::Format("%6.2f %s", data.swellpeak, d.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSWELLPEAK, temp);

  if (data.currentcount)
    temp = wxString::Format("%6.2f %s", data.currentdir / data.currentcount,
                            opt->Deg.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENTDIR, temp);

  if (data.currentcount)
    temp = wxString::Format("%6.2f %s", data.current / data.currentcount,
                            opt->showBoatSpeed.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENT, temp);

  temp = wxString::Format("%6.2f %s", data.currentpeak,
                          opt->showBoatSpeed.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FCURRENTPEAK, temp);

  temp = wxString::Format("%0002i:%02i %s", data.enginehours, data.enginemin,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FENGINE1, temp);

  temp = wxString::Format("%0002i:%02i %s", data.enginehours2, data.enginemin2,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FENGINE2, temp);

  temp = wxString::Format("%0002i:%02i %s", data.generatorhours,
                          data.generatormin, opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FGENERATOR, temp);

  temp = wxString::Format("%0002i:%02i %s", data.watermhours, data.watermmin,
                          opt->motorh.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATERM, temp);

  temp = wxString::Format("%3.2f %s", data.watermo, opt->vol.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FWATERMO, temp);

  if (data.speedcount)
    temp = wxString::Format("%6.2f %s", data.speed / data.speedcount,
                            opt->showBoatSpeed.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FSPEED, temp);

  temp =
      wxString::Format("%6.2f %s", data.speedpeak, opt->showBoatSpeed.c_str());
  temp.Replace(".", parent->decimalPoint);
  grid->SetCellValue(row, FBSPEED, temp);

  if (data.speedcountSTW)
    temp = wxString::Format("%6.2f %s", data.speedSTW / data.speedcountSTW,
                            opt->showBoatSpeed.c_str());
  else
    temp = nothing;
  temp.Replace(".", parent->decimalPoint);

  temp = wxString::Format("%6.2f %s", data.speedpeakSTW,
                          opt->showBoatSpeed.c_str());
  temp.Replace(".", parent->decimalPoint);

  wxDateTime startdt, enddt, startdtd, enddtd, time;

  if ((data.logbookStart.IsEmpty() || enddate.IsEmpty()) ||
      (data.logbookTimeStart.IsEmpty() || endtime.IsEmpty()))
    return;

  parent->myParseTime(data.logbookTimeStart, time);
  wxTimeSpan timesp(time.GetHour(), time.GetMinute());
  parent->myParseDate(data.logbookStart, startdt);
  startdt.Add(timesp);

  parent->myParseTime(endtime, time);
  wxTimeSpan timespe(time.GetHour(), time.GetMinute());
  parent->myParseDate(enddate, enddt);
  enddt.Add(timespe);

  wxTimeSpan journey = enddt.Subtract(startdt);
  if (journey.GetWeeks() > 3) {
    int years = 0;
    int month;
    int days;

    // years = journey.GetDays() / 365 ;

    years = enddt.GetYear() - startdt.GetYear();

    int bd = startdt.GetDay(), bm = startdt.GetMonth(), by = startdt.GetYear(),
        cd = enddt.GetDay(), cm = enddt.GetMonth(), cy = enddt.GetYear(), ad,
        am, ay;
    if (cd < bd) {
      cm = cm - 1;
      cd = cd + startdt.GetLastMonthDay().GetDay();
    }
    if (cm < bm) {
      cy = cy - 1;
      cm = cm + 12;
    }
    ad = cd - bd;
    am = cm - bm;
    ay = cy - by;

    month = startdt.GetMonth() - enddt.GetMonth();
    if (month < 0 && startdt.GetYear() != enddt.GetYear()) {
      month = 12 + month;
      years--;
    } else
      month = enddt.GetMonth() - startdt.GetMonth();

    days = enddt.GetDay() - startdt.GetDay();
    if (days < 0) {
      days = startdt.GetDay() - days;
      month--;
    }
    /*		else
    {
    month--;
    days = enddt.GetLastMonthDay((wxDateTime::Month)month).GetDay() -
    (startdt.GetDay() - enddt.GetDay());
    }
    */
    grid->SetCellValue(
        row, FJOURNEY,
        wxString::Format(_("%i Year(s) %i Month(s) %i Day(s)"), ay, am, ad));
  } else
    grid->SetCellValue(
        row, FJOURNEY,
        journey.Format(_("%E Week(s) %D Day(s) %H:%M ")) + opt->motorh);

  int max = 0;
  wxString result;
  collection::iterator it;

  for (it = t_coll.begin(); it != t_coll.end(); ++it)
    if (it->second >= max) {
      sail = it->first;
      max = it->second;
    }
  grid->SetCellValue(row, FSAILS, sail);

  if (colour)
    for (int i = 0; i < grid->GetNumberCols(); i++)
      grid->SetCellBackgroundColour(row, i, wxColour(230, 230, 230));
}

void OverView::setLayoutLocation() {
  bool radio = parent->m_radioBtnHTMLOverview->GetValue();
  if (radio)
    layout_locn = HTMLLayout_locn;
  else
    layout_locn = ODTLayout_locn;
  this->layout_locn = layout_locn;

  this->layout_locn.Append("overview");
  parent->appendOSDirSlash(&layout_locn);

  parent->loadLayoutChoice(LogbookDialog::OVERVIEW, layout_locn,
                           parent->overviewChoice,
                           opt->layoutPrefix[LogbookDialog::OVERVIEW]);
  if (radio)
    parent->overviewChoice->SetSelection(opt->overviewGridLayoutChoice);
  else
    parent->overviewChoice->SetSelection(opt->overviewGridLayoutChoiceODT);
}

void OverView::setSelectedRow(int row) { selectedRow = row; }

void OverView::gotoRoute() {
  wxString file = grid->GetCellValue(selectedRow, FLOG);
  wxString route = grid->GetCellValue(selectedRow, FROUTE);
  wxString date = grid->GetCellValue(selectedRow, FSTART);
  wxString path = grid->GetCellValue(selectedRow, FPATH);

  if (logbook->data_locn != path) {
    logbook->data_locn = path;
    logbook->loadSelectedData(path);
  }

  int i;
  for (i = 0; i < parent->m_gridGlobal->GetNumberRows(); i++) {
    if (parent->m_gridGlobal->GetCellValue(i, 0) == route &&
        parent->m_gridGlobal->GetCellValue(i, 1) == date)
      break;
  }

  parent->m_gridGlobal->MakeCellVisible(i, 0);
  parent->m_gridGlobal->SelectRow(i);
  parent->m_gridWeather->MakeCellVisible(i, 0);
  parent->m_gridWeather->SelectRow(i);
  parent->m_gridMotorSails->MakeCellVisible(i, 0);
  parent->m_gridMotorSails->SelectRow(i);

  parent->m_logbook->SetSelection(0);
}

wxString OverView::setPlaceHolders(int mode, wxGrid* grid, int row,
                                   wxString middle) {
  wxString newMiddleODT = middle;

  newMiddleODT.Replace(
      "#FLOG#", replaceNewLine(mode, grid->GetCellValue(row, FLOG), false));
  newMiddleODT.Replace("#LLOG#", grid->GetTable()->GetColLabelValue(FLOG));
  newMiddleODT.Replace(
      "#FROUTE#", replaceNewLine(mode, grid->GetCellValue(row, FROUTE), false));
  newMiddleODT.Replace("#LROUTE#", grid->GetTable()->GetColLabelValue(FROUTE));
  newMiddleODT.Replace(
      "#FSTART#", replaceNewLine(mode, grid->GetCellValue(row, FSTART), false));
  newMiddleODT.Replace("#LSTART#", grid->GetTable()->GetColLabelValue(FSTART));
  newMiddleODT.Replace(
      "#FEND#", replaceNewLine(mode, grid->GetCellValue(row, FEND), false));
  newMiddleODT.Replace("#LEND#", grid->GetTable()->GetColLabelValue(FEND));
  newMiddleODT.Replace(
      "#FJOURNEY#",
      replaceNewLine(mode, grid->GetCellValue(row, FJOURNEY), false));
  newMiddleODT.Replace("#LJOURNEY#",
                       grid->GetTable()->GetColLabelValue(FJOURNEY));
  newMiddleODT.Replace(
      "#FDISTANCE#",
      replaceNewLine(mode, grid->GetCellValue(row, FDISTANCE), false));
  newMiddleODT.Replace("#LDISTANCE#",
                       grid->GetTable()->GetColLabelValue(FDISTANCE));
  newMiddleODT.Replace(
      "#FETMAL#", replaceNewLine(mode, grid->GetCellValue(row, FETMAL), false));
  newMiddleODT.Replace("#LETMAL#", grid->GetTable()->GetColLabelValue(FETMAL));
  newMiddleODT.Replace(
      "#FBESTETMAL#",
      replaceNewLine(mode, grid->GetCellValue(row, FBESTETMAL), false));
  newMiddleODT.Replace("#LBESTETMAL#",
                       grid->GetTable()->GetColLabelValue(FBESTETMAL));
  newMiddleODT.Replace(
      "#FSPEED#", replaceNewLine(mode, grid->GetCellValue(row, FSPEED), false));
  newMiddleODT.Replace("#LSPEED#", grid->GetTable()->GetColLabelValue(FSPEED));
  newMiddleODT.Replace(
      "#FBSPEED#",
      replaceNewLine(mode, grid->GetCellValue(row, FBSPEED), false));
  newMiddleODT.Replace("#LBSPEED#",
                       grid->GetTable()->GetColLabelValue(FBSPEED));
  newMiddleODT.Replace(
      "#FENGINE1#",
      replaceNewLine(mode, grid->GetCellValue(row, FENGINE2), false));
  newMiddleODT.Replace("#LENGINE1#",
                       grid->GetTable()->GetColLabelValue(FENGINE2));
  newMiddleODT.Replace(
      "#FGENERATOR#",
      replaceNewLine(mode, grid->GetCellValue(row, FGENERATOR), false));
  newMiddleODT.Replace("#LGENERATOR#",
                       grid->GetTable()->GetColLabelValue(FGENERATOR));
  newMiddleODT.Replace(
      "#FWATERM#",
      replaceNewLine(mode, grid->GetCellValue(row, FWATERM), false));
  newMiddleODT.Replace("#LWATERM#",
                       grid->GetTable()->GetColLabelValue(FWATERM));
  newMiddleODT.Replace(
      "#FWATERMO#",
      replaceNewLine(mode, grid->GetCellValue(row, FWATERMO), false));
  newMiddleODT.Replace("#LWATERMO#",
                       grid->GetTable()->GetColLabelValue(FWATERMO));
  newMiddleODT.Replace(
      "#FBANK1G#",
      replaceNewLine(mode, grid->GetCellValue(row, FBANK1G), false));
  newMiddleODT.Replace("#LBANK1G#",
                       grid->GetTable()->GetColLabelValue(FBANK1G));
  newMiddleODT.Replace(
      "#FBANK1U#",
      replaceNewLine(mode, grid->GetCellValue(row, FBANK1U), false));
  newMiddleODT.Replace("#LBANK1U#",
                       grid->GetTable()->GetColLabelValue(FBANK1U));
  newMiddleODT.Replace(
      "#FBANK2G#",
      replaceNewLine(mode, grid->GetCellValue(row, FBANK2G), false));
  newMiddleODT.Replace("#LBANK2G#",
                       grid->GetTable()->GetColLabelValue(FBANK2G));
  newMiddleODT.Replace(
      "#FBANK2U#",
      replaceNewLine(mode, grid->GetCellValue(row, FBANK2U), false));
  newMiddleODT.Replace("#LBANK2U#",
                       grid->GetTable()->GetColLabelValue(FBANK2U));

  newMiddleODT.Replace(
      "#FENGINE#",
      replaceNewLine(mode, grid->GetCellValue(row, FENGINE1), false));
  newMiddleODT.Replace("#LENGINE#",
                       grid->GetTable()->GetColLabelValue(FENGINE1));
  newMiddleODT.Replace(
      "#FFUEL#", replaceNewLine(mode, grid->GetCellValue(row, FFUEL), false));
  newMiddleODT.Replace("#LFUEL#", grid->GetTable()->GetColLabelValue(FFUEL));
  newMiddleODT.Replace(
      "#FWATER#", replaceNewLine(mode, grid->GetCellValue(row, FWATER), false));
  newMiddleODT.Replace("#LWATER#", grid->GetTable()->GetColLabelValue(FWATER));
  newMiddleODT.Replace(
      "#FWINDDIR#",
      replaceNewLine(mode, grid->GetCellValue(row, FWINDDIR), false));
  newMiddleODT.Replace("#LWINDDIR#",
                       grid->GetTable()->GetColLabelValue(FWINDDIR));
  newMiddleODT.Replace(
      "#FWIND#", replaceNewLine(mode, grid->GetCellValue(row, FWIND), false));
  newMiddleODT.Replace("#LWIND#", grid->GetTable()->GetColLabelValue(FWIND));
  newMiddleODT.Replace(
      "#FWINDPEAK#",
      replaceNewLine(mode, grid->GetCellValue(row, FWINDPEAK), false));
  newMiddleODT.Replace("#LWINDPEAK#",
                       grid->GetTable()->GetColLabelValue(FWINDPEAK));
  newMiddleODT.Replace(
      "#FCURRENTDIR#",
      replaceNewLine(mode, grid->GetCellValue(row, FCURRENTDIR), false));
  newMiddleODT.Replace("#LCURRENTDIR#",
                       grid->GetTable()->GetColLabelValue(FCURRENTDIR));

  newMiddleODT.Replace(
      "#FCURRENT#",
      replaceNewLine(mode, grid->GetCellValue(row, FCURRENT), false));
  newMiddleODT.Replace("#LCURRENT#",
                       grid->GetTable()->GetColLabelValue(FCURRENT));
  newMiddleODT.Replace(
      "#FCURRENTPEAK#",
      replaceNewLine(mode, grid->GetCellValue(row, FCURRENTPEAK), false));
  newMiddleODT.Replace("#LCURRENTPEAK#",
                       grid->GetTable()->GetColLabelValue(FCURRENTPEAK));
  newMiddleODT.Replace(
      "#FWAVE#", replaceNewLine(mode, grid->GetCellValue(row, FWAVE), false));
  newMiddleODT.Replace("#LWAVE#", grid->GetTable()->GetColLabelValue(FWAVE));
  newMiddleODT.Replace(
      "#FWAVEPEAK#",
      replaceNewLine(mode, grid->GetCellValue(row, FWAVEPEAK), false));
  newMiddleODT.Replace("#LWAVEPEAK#",
                       grid->GetTable()->GetColLabelValue(FWAVEPEAK));
  newMiddleODT.Replace(
      "#FSWELL#", replaceNewLine(mode, grid->GetCellValue(row, FSWELL), false));
  newMiddleODT.Replace("#LSWELL#", grid->GetTable()->GetColLabelValue(FSWELL));
  newMiddleODT.Replace(
      "#FSWELLPEAK#",
      replaceNewLine(mode, grid->GetCellValue(row, FSWELLPEAK), false));
  newMiddleODT.Replace("#LSWELLPEAK#",
                       grid->GetTable()->GetColLabelValue(FSWELLPEAK));
  newMiddleODT.Replace(
      "#FSAILS#", replaceNewLine(mode, grid->GetCellValue(row, FSAILS), false));
  newMiddleODT.Replace("#LSAILS#", grid->GetTable()->GetColLabelValue(FSAILS));

  return newMiddleODT;
}
