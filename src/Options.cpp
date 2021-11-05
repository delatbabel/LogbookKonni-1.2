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
#include "wx/wx.h"
#endif  // precompiled headers

#ifdef __WXMSW__
#include <windows.h>
#endif

#include <wx/mimetype.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

#include "LogbookDialog.h"
#include "Options.h"

Options::Options(void) {
  // Date/Time
  timeformat = -1;
  noseconds = true;
  dateformat = 0;
  dateseparatorlocale = "/";
  dateseparatorindiv = "/";
  sdateformat = "";
  stimeformat = "";
  date1 = date2 = date3 = 0;
  statusbarGlobal = true;
  layoutShow = false;
  statusbarWatch = true;
  kmlTrans.Add("50");
  kmlTrans.Add("FF");
  kmlColor.Add("FFFFFF");
  kmlColor.Add("000000");
  kmlColor.Add("0000FF");
  kmlColor.Add("14F0FF");
  engines = 0;
  generator = 0;
  toggleEngine1 = false;
  toggleEngine2 = false;
  toggleGenerator = false;
  dtEngine1On = -1;
  dtEngine2On = -1;
  dtGeneratorOn = -1;
  bRPMIsChecked = false;
  bEng1RPMIsChecked = false;
  bEng2RPMIsChecked = false;
  bGenRPMIsChecked = false;
  sentence = wxEmptyString;
  engine1Id = wxEmptyString;
  engine2Id = wxEmptyString;
  generatorId = wxEmptyString;
  engine1Running = false;
  engine2Running = false;
  generatorRunning = false;
  bRPMCheck = false;
  engineStr[0] = "1";
  engineStr[1] = "2";
  engineMessageSails = false;
  engineMessageRunning = false;
  engineAllwaysSailsDown = false;
  statusbar = true;
  windspeeds = false;
  numberSails = 14;

  abrSails.Add(_("Ma"));
  abrSails.Add(_("Tr"));
  abrSails.Add(_("Ge1"));
  abrSails.Add(_("Ge2"));
  abrSails.Add(_("Ge3"));
  abrSails.Add(_("Fo1"));
  abrSails.Add(_("Fo2"));
  abrSails.Add(_("Fo3"));
  abrSails.Add(_("Ji"));
  abrSails.Add(_("Mi"));
  abrSails.Add(_("Mt"));
  abrSails.Add(_("St"));
  abrSails.Add(_("Sp"));
  abrSails.Add(_("?"));

  sailsName.Add(_("Main"));
  sailsName.Add(_("Try"));
  sailsName.Add(_("Genoa 1"));
  sailsName.Add(_("Genoa 2"));
  sailsName.Add(_("Genoa 3"));
  sailsName.Add(_("Fock 1"));
  sailsName.Add(_("Fock 2"));
  sailsName.Add(_("Fock 3"));
  sailsName.Add(_("Jib"));
  sailsName.Add(_("Mizzen"));
  sailsName.Add(_("Mizzen Storm"));
  sailsName.Add(_("Staysail"));
  sailsName.Add(_("Spinnacker"));
  sailsName.Add(
      _("You can change the abreviations and the names of the sails\nin "
        "Options/Miscellaneous/Sails"));

  rowGap = 0;
  colGap = 0;

  for (int i = 0; i < numberSails; i++) bSailIsChecked[i] = false;

  // Global Options
  traditional = true;
  modern = false;
  showToolTips = true;

#ifdef __WXMSW__
  firstTime = false;
#endif
#ifdef __WXOSX__
  firstTime = false;
#endif
#ifdef __POSIX__
  firstTime = true;
#endif
  dlgWidth = 1010;
  dlgHeight = 535;
  popup = true;
  autostarttimer = false;
  for (int i = 0; i < 7; i++) {
    layoutPrefix[i] = "Label_";
    filterLayout[i] = true;
  }
  noGPS = true;
  colouredMaintenance = true;
  waypointArrived = false;
  waypointText = _("automatic line Waypoint arrived");
  guardChange = false;
  guardChangeText = _("automatic line\nchange of guards");
  courseChange = false;
  courseChangeDegrees = "90";
  courseChangeText = _("automatic line\nchange of course >");
  courseTextAfterMinutes = "5";
  everySM = false;
  everySMAmount = "10";
  everySMText = _("automatic line\nDistance > ");
  dCourseChangeDegrees = -1;
  dEverySM = -1;

  // Timer
  timer = false;
  timerType = 0;
  local = true;
  UTC = false;
  gpsAuto = false;
  tzIndicator = 0;
  tzHour = 0;
  thour = "0";
  tmin = "1";
  ;
  tsec = "0";
  ;
  ttext =
      _("Automatic line by timer\n\n\
You can set/unset a timer-interval\n\
or change this text in Toolbox/Plugins/Logbook\n\nShift+Enter appends a new line");
  timerSec = 5;

  // Grid Navigation
  Deg = "\xB0";  // Apple \xA1 ??
  Min = _("'");
  Sec = _("\"");

  meter = _("m");
  feet = _("ft");
  fathom = _("fth");

  // Grid Weather
  baro = _("mb");
  temperature = _("F");

  // Grid MotorSails
  vol = _("gal");
  motorh = _("h");
  engine = "E";
  shaft = "S";
  rpm = "RPM";

  // Maintenance
  days = _("day(s)");
  weeks = _("week(s)");
  month = _("month");

  // Capacity
  fuelTank = "0";
  waterTank = "0";
  watermaker = "0";
  bank1 = "0";
  bank2 = "0";
  ampere = "A";

  // wxChoice
  showDistanceChoice = 0;
  showDistance = _("NM");
  showBoatSpeedchoice = 0;
  showBoatSpeed = _("kts");
  showDepth = 0;
  showWaveSwell = 0;
  showWindSpeedchoice = 0;
  showWindSpeed = "kts";
  showWindHeading = 0;
  showWindDir = 0;
  showHeading = 0;

  overviewAll = 0;

  navGridLayoutChoice = 0;
  crewGridLayoutChoice = 0;
  boatGridLayoutChoice = 0;
  overviewGridLayoutChoice = 0;
  serviceGridLayoutChoice = 0;
  repairsGridLayoutChoice = 0;
  buypartsGridLayoutChoice = 0;

  navGridLayoutChoiceODT = 0;
  crewGridLayoutChoiceODT = 0;
  boatGridLayoutChoiceODT = 0;
  overviewGridLayoutChoiceODT = 0;
  serviceGridLayoutChoiceODT = 0;
  repairsGridLayoutChoiceODT = 0;
  buypartsGridLayoutChoiceODT = 0;

  navHTML = true;
  crewHTML = true;
  boatHTML = true;
  overviewHTML = true;
  serviceHTML = true;
  repairsHTML = true;
  buypartsHTML = false;

  // Paths
  htmlEditor = "";
  htmlEditorReset = htmlEditor;

  // KML
  kmlRoute = 1;
  kmlTrack = 1;
  kmlLineWidth = "4";
  ;
  kmlLineTransparancy = 0;
  kmlRouteColor = 0;
  kmlTrackColor = 3;

  // NMEA
  NMEAUseERRPM = false;

#ifdef __WXMSW__
  wxFileType *filetype1 =
      wxTheMimeTypesManager->GetFileTypeFromExtension("odt");
  wxString command;

  if (filetype1) {
    if ((command = filetype1->GetOpenCommand("x.odt")) != wxEmptyString) {
      command = command.Remove(command.find_last_of(" "));

      odtEditor = command;
      odtEditorReset = odtEditor;
    }
  } else {
    odtEditor = "";
    odtEditorReset = "";
  }
#endif
#ifdef __WXGTK__
  odtEditor = "libreoffice --writer";
  odtEditorReset = odtEditor;
#endif
#ifdef __WXOSX__
  //	wxFileType
  //*filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension("odt");
  wxFileType *filetype1 = wxTheMimeTypesManager->GetFileTypeFromMimeType(
      "application/vnd.oasis.opendocument.text-template");

  wxString command;

  if (filetype1) {
    if ((command = filetype1->GetOpenCommand("x.odt")) != wxEmptyString) {
      command = command.Remove(command.find_last_of(" "));
      odtEditor = command;
      odtEditorReset = odtEditor;
    }
  }
#endif

#ifdef __WXMSW__
  filetype1 = wxTheMimeTypesManager->GetFileTypeFromExtension("RSS");

  if (filetype1) {
    command = filetype1->GetOpenCommand(" ");
    command = command.Remove(command.find_first_of('/') - 1);
    mailClient = command;
    mailClientReset = mailClient;
  } else {
    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    if ((osvi.dwMajorVersion > 5) ||
        ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1))) {
      mailClient = "C:\\Programme\\Outlook Express\\msimn.exe";
      mailClientReset = mailClient;
    } else {
      mailClient = "";
      mailClientReset = "";
    }
  }

#endif
#ifdef __WXGTK__
  mailClient = "kmail --composer";
  mailClientReset = mailClient;
#endif
#ifdef __WXOSX__
  mailClient = "/Applications/Mail.app ";
  mailClientReset = mailClient;
#endif

#ifdef __WXMSW__
  dataManager = "explorer.exe /select,";
  dataManagerReset = dataManager;
#endif
#ifdef __WXGTK__
  dataManager = "dolphin --select";
  dataManagerReset = dataManager;

  htmlEditor = "libreoffice";
  htmlEditorReset = htmlEditor;
#endif
#ifdef __WXOSW__
  dataManager = "";
  dataManagerReset = dataManagerReset;
#endif

  stateEvents = 0;

  for (int i = 0; i < numberSails; i++) bSailIsChecked[i] = false;
}

Options::~Options(void) {}

bool Options::checkStateOfEvents() {
  if (timer || guardChange || courseChange || waypointArrived || everySM)
    return true;
  else
    return false;
}

void Options::stopAllEvents() {
  timer = false;
  guardChange = false;
  courseChange = false;
  waypointArrived = false;
  everySM = false;
}

void Options::setTimeFormat(int i) {
  wxString stimeh = "%H:";
  wxString stimei = "%I:";
  wxString stimem = "%M";
  wxString stimes = ":%S";
  wxString sam = " %p";

  if (i) {
    if (timeformat == 0) {
      stimeformat = stimeh + stimem;
      stimeformatw = stimeformat;
    } else {
      stimeformat = stimei + stimem + sam;
      stimeformatw = stimeformat;
    }
  } else {
    if (timeformat == 0) {
      stimeformat = stimeh + stimem + stimes;
      stimeformatw = stimeh + stimem;
    } else {
      stimeformat = stimei + stimem + stimes + sam;
      stimeformatw = stimei + stimem + sam;
    }
  }
}

void Options::setOptionsTimeFormat() {
  wxDateTime sample = wxDateTime::Now();
  wxString am, pm;
  sample.GetAmPmStrings(&am, &pm);

  if (am.IsEmpty()) {
    timeformat = 0;
    hourFormat = true;
  } else {
    // timeformat = 1;
    hourFormat = false;
  }
}

void Options::setDateFormat() {
  LogbookDialog::setDatePattern();

  wxArrayString sadate;
  sadate.Add("%m");
  sadate.Add("%d");
  sadate.Add("%Y");

  wxArrayString spattern;
  spattern.Add("mm");
  spattern.Add("dd");
  spattern.Add("yyyy");

  wxArrayInt ar;

  if (dateformat == 0) {
    wxStringTokenizer tkz(LogbookDialog::datePattern,
                          LogbookDialog::dateSeparator);

    wxString tmp = tkz.GetNextToken();
    if (tmp.Contains("m")) {
      sdateformat = sadate[0] + dateseparatorlocale;
      ar.Add(0);
    } else if (tmp.Contains("d")) {
      sdateformat = sadate[1] + dateseparatorlocale;
      ar.Add(1);
    } else if (tmp.Contains("y")) {
      sdateformat = sadate[2] + dateseparatorlocale;
      ar.Add(2);
    }

    tmp = tkz.GetNextToken();
    if (tmp.Contains("m")) {
      sdateformat += sadate[0] + dateseparatorlocale;
      ar.Add(0);
    } else if (tmp.Contains("d")) {
      sdateformat += sadate[1] + dateseparatorlocale;
      ar.Add(1);
    } else if (tmp.Contains("y")) {
      sdateformat += sadate[2] + dateseparatorlocale;
      ar.Add(2);
    }

    tmp = tkz.GetNextToken();
    if (tmp.Contains("m")) {
      sdateformat += sadate[0];
      ar.Add(0);
    } else if (tmp.Contains("d")) {
      sdateformat += sadate[1];
      ar.Add(1);
    } else if (tmp.Contains("y")) {
      sdateformat += sadate[2];
      ar.Add(2);
    }

    wxString sep = dateseparatorlocale;
    LogbookDialog::datePattern =
        spattern[ar[0]] + sep + spattern[ar[1]] + sep + spattern[ar[2]];
  } else {
    wxString sep = dateseparatorindiv;
    sdateformat = sadate[date1] + sep + sadate[date2] + sep + sadate[date3];
    LogbookDialog::datePattern =
        spattern[date1] + sep + spattern[date2] + sep + spattern[date3];
  }
}
