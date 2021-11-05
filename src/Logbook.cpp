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

#define PI 3.14159265

#include <math.h>

#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/filefn.h>
#include <wx/fs_inet.h>
#include <wx/generic/gridctrl.h>
#include <wx/grid.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/object.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "Logbook.h"
#include "LogbookDialog.h"
#include "LogbookHTML.h"
#include "LogbookOptions.h"
#include "Options.h"
#include "logbook_pi.h"
#include "nmea0183/nmea0183.h"

Logbook::Logbook(LogbookDialog* parent, wxString data, wxString layout,
                 wxString layoutODT)
    : LogbookHTML(this, parent, data, layout) {
#ifdef PBVE_DEBUG
  pbvecount = 0;
#endif
  oldLogbook = false;
  OCPN_Message = false;
  noSentence = true;
  modified = false;
  wxString logLay;
  lastWayPoint = "";
  logbookDescription = wxEmptyString;
  sLinesReminder =
      _("Your Logbook has %i lines\n\nYou should create a new logbook to "
        "minimize loadingtime.");

  dialog = parent;
  opt = dialog->logbookPlugIn->opt;

  wxString logData = data;
  logData.Append("logbook.txt");
  dialog->backupFile = "logbook";

  wxFileName wxHomeFiledir = logData;
  if (!wxHomeFiledir.FileExists()) {
    logbookFile = new wxTextFile(logData);
    logbookFile->Create();
  } else
    logbookFile = new wxTextFile(logData);

  data_locn = logData;
  logbookData_actual = logData;

  if (dialog->m_radioBtnHTML->GetValue())
    logLay = layout;
  else
    logLay = layoutODT;
  setLayoutLocation(logLay);

  // weatherCol = dialog->m_gridGlobal->GetNumberCols();
  // sailsCol   = dialog->m_gridGlobal->GetNumberCols()+weatherCol-1;

  noAppend = false;
  gpsStatus = false;
  waypointArrived = false;
  WP_skipped = false;
  bCOW = false;
  dCOW = -1;
  dCOG = -1;
  courseChange = false;
  everySM = false;
  guardChange = false;
  dLastMinute = -1;
  oldPosition.latitude = 500;
  activeRoute = wxEmptyString;
  activeRouteGUID = wxEmptyString;
  activeMOB = wxEmptyString;
  MOBIsActive = false;
  routeIsActive = false;
  trackIsActive = false;
  wimdaSentence = false;
  bSOW = false;
  bTemperatureWater = false;
  bTemperatureAir = false;
  bWindA = false;
  bWindT = false;
  bDepth = false;
  dtEngine1Off = -1;
  bRPM1 = false;
  dtEngine2Off = -1;
  bRPM2 = false;
  dtGeneratorOff = -1;
  bGEN = false;
  sRPM1Shaft = wxEmptyString;
  sRPM1Source = wxEmptyString;
  sRPM2Shaft = wxEmptyString;
  sRPM2Source = wxEmptyString;
  rpmSentence = false;
  sVolume = wxEmptyString;
  dVolume = 0;
}

Logbook::~Logbook(void) { update(); }

void Logbook::setTrackToNewID(wxString target) {
  if (mergeList.Count() == 0) return;

  wxDir dir;
  wxArrayString files;
  dir.GetAllFiles(parent->data, &files, "until*.txt", wxDIR_FILES);

  for (unsigned int i = 0; i < files.Count(); i++) {
    wxFileInputStream file(files[i]);
    wxTextInputStream txt(file);

    wxString data = wxEmptyString;
    while (!file.Eof()) data += txt.ReadLine() + "\n";

    for (unsigned int n = 0; n < mergeList.GetCount(); n++)
      data.Replace(mergeList.Item(n), target);

    wxFileOutputStream fileo(files[i]);
    wxTextOutputStream txto(fileo);
    txto << data;
    fileo.Close();
  }
}

void Logbook::setLayoutLocation(wxString loc) {
  bool radio = dialog->m_radioBtnHTML->GetValue();
  loc.Append("logbook");
  dialog->appendOSDirSlash(&loc);
  layout_locn = loc;
  setFileName(data_locn, layout_locn);
  dialog->loadLayoutChoice(LogbookDialog::LOGBOOK, layout_locn,
                           dialog->logbookChoice,
                           opt->layoutPrefix[LogbookDialog::LOGBOOK]);
  if (radio)
    dialog->logbookChoice->SetSelection(
        dialog->logbookPlugIn->opt->navGridLayoutChoice);
  else
    dialog->logbookChoice->SetSelection(
        dialog->logbookPlugIn->opt->navGridLayoutChoiceODT);
}

void Logbook::SetPosition(PlugIn_Position_Fix& pfix) {
  if (opt->traditional)
    sLat = this->toSDMM(1, pfix.Lat, true);
  else
    sLat = this->toSDMMOpenCPN(1, pfix.Lat, true);

  if (opt->traditional)
    sLon = this->toSDMM(2, pfix.Lon, true);
  else
    sLon = this->toSDMMOpenCPN(2, pfix.Lon, true);

  if (pfix.FixTime != 0) {
    double factor = 1;
    double tspeed = 1;

    switch (opt->showBoatSpeedchoice) {
      case 0:
        factor = 1;
        break;
      case 1:
        factor = 0.51444;
        break;
      case 2:
        factor = 1.852;
        break;
    }

    tspeed = pfix.Sog * factor;

    sSOG = wxString::Format("%5.2f %s", tspeed, opt->showBoatSpeed.c_str());
    sCOG = wxString::Format("%5.2f %s", pfix.Cog, opt->Deg.c_str());
    SetGPSStatus(true);
  } else
    SetGPSStatus(false);

  mUTCDateTime.Set(pfix.FixTime);
  //	dialog->GPSTimer->Start(5000);
}
void Logbook::clearNMEAData() { noSentence = true; }

void Logbook::SetSentence(wxString& sentence) {
  wxDateTime dt;
  wxString onOff[2];
  onOff[0] = _(" off");
  onOff[1] = _(" on");

  m_NMEA0183 << sentence;

#ifdef PBVE_DEBUG
  if (sentence.Contains("$PBVE")) {
    if (pvbe != NULL && pbvecount < 15) {
      pvbe->m_textCtrlPVBE->AppendText(sentence);
      pvbe->SetFocus();
      pbvecount++;
    }
  }
#endif

  if (m_NMEA0183.PreParse()) {
    noSentence = false;
    if (m_NMEA0183.LastSentenceIDReceived == "GGA") {
      if (m_NMEA0183.Parse()) {
        if (m_NMEA0183.Gga.GPSQuality > 0) {
          SetGPSStatus(true);
          setPositionString(m_NMEA0183.Gga.Position.Latitude.Latitude,
                            m_NMEA0183.Gga.Position.Latitude.Northing,
                            m_NMEA0183.Gga.Position.Longitude.Longitude,
                            m_NMEA0183.Gga.Position.Longitude.Easting);
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "GLL") {
      if (m_NMEA0183.Parse()) {
        if (m_NMEA0183.Gll.IsDataValid == NTrue) {
          SetGPSStatus(true);
          setPositionString(m_NMEA0183.Gll.Position.Latitude.Latitude,
                            m_NMEA0183.Gll.Position.Latitude.Northing,
                            m_NMEA0183.Gll.Position.Longitude.Longitude,
                            m_NMEA0183.Gll.Position.Longitude.Easting);
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "ZDA") {
      if (m_NMEA0183.Parse()) {
        dt = dt.Set(m_NMEA0183.Zda.Day,
                    (wxDateTime::Month)(m_NMEA0183.Zda.Month - 1),
                    m_NMEA0183.Zda.Year);
        // dt.ParseTime((const
        // char)dt.ParseFormat(m_NMEA0183.Zda.UTCTime,"%H%M%S"));
        dt.ParseFormat(m_NMEA0183.Zda.UTCTime, "%H%M%S");
        setDateTimeString(dt);
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "HDT") {
      if (m_NMEA0183.Parse()) {
        if (opt->showHeading == 0)
          sCOW = wxString::Format("%5.2f%s", m_NMEA0183.Hdt.DegreesTrue,
                                  opt->Deg.c_str());
        dCOW = m_NMEA0183.Hdt.DegreesTrue;
        bCOW = true;
        dtCOW = wxDateTime::Now();
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "HDM") {
      if (m_NMEA0183.Parse()) {
        if (opt->showHeading == 1)
          sCOW = wxString::Format("%5.2f%s", m_NMEA0183.Hdm.DegreesMagnetic,
                                  opt->Deg.c_str());
        dCOW = m_NMEA0183.Hdm.DegreesMagnetic;
        bCOW = true;
        dtCOW = wxDateTime::Now();
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "HDG") {
      if (m_NMEA0183.Parse()) {
        if (opt->showHeading == 0) {
          dCOW = m_NMEA0183.Hdg.MagneticVariationDirection == East
                     ? m_NMEA0183.Hdg.MagneticSensorHeadingDegrees +
                           m_NMEA0183.Hdg.MagneticVariationDegrees
                     : m_NMEA0183.Hdg.MagneticSensorHeadingDegrees -
                           m_NMEA0183.Hdg.MagneticVariationDegrees;
          sCOW = wxString::Format("%5.2f%s", dCOW, opt->Deg.c_str());
        } else {
          sCOW = wxString::Format("%5.2f%s",
                                  m_NMEA0183.Hdg.MagneticSensorHeadingDegrees,
                                  opt->Deg.c_str());
          dCOW = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
        }
        bCOW = true;
        dtCOW = wxDateTime::Now();
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "RMB") {
      if (opt->waypointArrived) {
        if (m_NMEA0183.Parse()) {
          if (m_NMEA0183.Rmb.IsDataValid == NTrue) {
            if (m_NMEA0183.Rmb.IsArrivalCircleEntered == NTrue) {
              if (m_NMEA0183.Rmb.From != lastWayPoint) {
                checkWayPoint(m_NMEA0183.Rmb);
              }
            }
          }
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "RMC") {
      if (m_NMEA0183.Parse()) {
        double factor = 1;
        double tboatspeed = 1;
        if (m_NMEA0183.Rmc.IsDataValid == NTrue) {
          SetGPSStatus(true);
          setPositionString(m_NMEA0183.Rmc.Position.Latitude.Latitude,
                            m_NMEA0183.Rmc.Position.Latitude.Northing,
                            m_NMEA0183.Rmc.Position.Longitude.Longitude,
                            m_NMEA0183.Rmc.Position.Longitude.Easting);

          if (m_NMEA0183.Rmc.SpeedOverGroundKnots != 999.0)
            switch (opt->showBoatSpeedchoice) {
              case 0:
                factor = 1;
                break;
              case 1:
                factor = 0.51444;
                break;
              case 2:
                factor = 1.852;
                break;
            }

          tboatspeed = m_NMEA0183.Rmc.SpeedOverGroundKnots * factor;

          sSOG = wxString::Format("%5.2f %s", tboatspeed,
                                  opt->showBoatSpeed.c_str());

          if (m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
            sCOG = wxString::Format("%5.2f%s",
                                    m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue,
                                    opt->Deg.c_str());
          if (m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue != 999.0)
            dCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

          long day, month, year;
          m_NMEA0183.Rmc.Date.SubString(0, 1).ToLong(&day);
          m_NMEA0183.Rmc.Date.SubString(2, 3).ToLong(&month);
          m_NMEA0183.Rmc.Date.SubString(4, 5).ToLong(&year);
          dt.Set(((int)day), (wxDateTime::Month)(month - 1),
                 ((int)year + 2000));
          // dt.ParseTime((const
          // char)dt.ParseFormat(m_NMEA0183.Rmc.UTCTime,"%H%M%S"));
          dt.ParseFormat(m_NMEA0183.Rmc.UTCTime, "%H%M%S");

          setDateTimeString(dt);

          if (!dialog->logbookPlugIn->eventsEnabled && opt->courseChange)
            checkCourseChanged();
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "VHW") {
      if (m_NMEA0183.Parse()) {
        double factor = 1;
        double tboatspeed = 1;
        if (m_NMEA0183.Vhw.Knots != 999.0) {
          switch (opt->showBoatSpeedchoice) {
            case 0:
              factor = 1;
              break;
            case 1:
              factor = 0.51444;
              break;
            case 2:
              factor = 1.852;
              break;
          }
        }
        tboatspeed = m_NMEA0183.Vhw.Knots * factor;

        sSOW = wxString::Format("%5.2f %s", tboatspeed,
                                opt->showBoatSpeed.c_str());
        dtSOW = wxDateTime::Now();
        bSOW = true;
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "MWV") {
      if (m_NMEA0183.Parse()) {
        double dWind = 0;
        double factor = 1, twindspeed = 1;

        switch (opt->showWindSpeedchoice) {
          case 0:
            if (m_NMEA0183.Mwv.WindSpeedUnits == 'N')
              factor = 1;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'M')
              factor = 1.94384;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'K')
              factor = 0.53995;
            break;
          case 1:
            if (m_NMEA0183.Mwv.WindSpeedUnits == 'N')
              factor = 0.51444;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'M')
              factor = 1;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'K')
              factor = 0.27777;
            break;
          case 2:
            if (m_NMEA0183.Mwv.WindSpeedUnits == 'N')
              factor = 1.852;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'M')
              factor = 3.6;
            else if (m_NMEA0183.Mwv.WindSpeedUnits == 'K')
              factor = 1;
            break;
        }
        twindspeed = m_NMEA0183.Mwv.WindSpeed * factor;

        if (m_NMEA0183.Mwv.Reference == "T") {
          if (opt->showWindHeading && bCOW) {
            dWind = m_NMEA0183.Mwv.WindAngle + dCOW;
            if (dWind > 360) {
              dWind -= 360;
            }
          } else
            dWind = m_NMEA0183.Mwv.WindAngle;

          sWindT = wxString::Format("%3.0f%s", dWind, opt->Deg.c_str());
          sWindSpeedT = wxString::Format("%3.1f %s", twindspeed,
                                         opt->showWindSpeed.c_str());
          dtWindT = wxDateTime::Now();
          bWindT = true;
          if (minwindT > twindspeed) minwindT = twindspeed;
          if (maxwindT < twindspeed) maxwindT = twindspeed;
          avgwindT = (avgwindT + twindspeed) / 2;
          swindspeedsT = wxString::Format("%03.1f|%03.1f|%03.1f", minwindT,
                                          avgwindT, maxwindT);
        } else {
          dWind = m_NMEA0183.Mwv.WindAngle;
          sWindA = wxString::Format("%3.0f%s", dWind, opt->Deg.c_str());
          sWindSpeedA = wxString::Format("%3.1f %s", twindspeed,
                                         opt->showWindSpeed.c_str());
          dtWindA = wxDateTime::Now();
          bWindA = true;
          if (minwindA > twindspeed) minwindA = twindspeed;
          if (maxwindA < twindspeed) maxwindA = twindspeed;
          avgwindA = (avgwindA + twindspeed) / 2;
          swindspeedsA = wxString::Format("%03.1f|%03.1f|%03.1f", minwindA,
                                          avgwindA, maxwindA);
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "VWT") {
      if (m_NMEA0183.Parse()) {
        double dWind = 0;
        dWind = m_NMEA0183.Vwt.WindDirectionMagnitude;

        if (m_NMEA0183.Vwt.DirectionOfWind == Left) {
          dWind = 360 - dWind;
        }

        if (opt->showWindHeading && bCOW) {
          dWind = dWind + dCOW;
          if (dWind > 360) {
            dWind -= 360;
          }
        }

        sWindT = wxString::Format("%3.0f%s", dWind, opt->Deg.c_str());

        double factor, twindspeed;

        switch (opt->showWindSpeedchoice) {
          case 0:
            factor = 1;
            break;
          case 1:
            factor = 0.51444;
            break;
          case 2:
            factor = 1.852;
            break;
        }
        twindspeed = m_NMEA0183.Vwt.WindSpeedKnots * factor;

        sWindSpeedT = wxString::Format("%3.1f %s", twindspeed,
                                       opt->showWindSpeed.c_str());
        dtWindT = wxDateTime::Now();
        bWindT = true;
        if (minwindT > twindspeed) minwindT = twindspeed;
        if (maxwindT < twindspeed) maxwindT = twindspeed;
        avgwindT = (avgwindT + twindspeed) / 2;
        swindspeedsT = wxString::Format("%03.1f|%03.1f|%03.1f", minwindT,
                                        avgwindT, maxwindT);
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "VWR") {
      if (m_NMEA0183.Parse()) {
        double dWind = 0;
        dWind = m_NMEA0183.Vwr.WindDirectionMagnitude;

        if (m_NMEA0183.Vwr.DirectionOfWind == Left) {
          dWind = 360 - dWind;
        }

        sWindA = wxString::Format("%3.0f%s", dWind, opt->Deg.c_str());

        double factor, twindspeed;

        switch (opt->showWindSpeedchoice) {
          case 0:
            factor = 1;
            break;
          case 1:
            factor = 0.51444;
            break;
          case 2:
            factor = 1.852;
            break;
        }
        twindspeed = m_NMEA0183.Vwt.WindSpeedKnots * factor;

        sWindSpeedA = wxString::Format("%3.1f %s", twindspeed,
                                       opt->showWindSpeed.c_str());
        dtWindA = wxDateTime::Now();
        bWindA = true;
        if (minwindA > twindspeed) minwindA = twindspeed;
        if (maxwindA < twindspeed) maxwindA = twindspeed;
        avgwindA = (avgwindA + twindspeed) / 2;
        swindspeedsA = wxString::Format("%03.1f|%03.1f|%03.1f", minwindA,
                                        avgwindA, maxwindA);
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "MTW") {
      if (m_NMEA0183.Parse()) {
        double t;
        if (opt->temperature == "F")
          t = ((m_NMEA0183.Mtw.Temperature * 9) / 5) + 32;
        else
          t = m_NMEA0183.Mtw.Temperature;
        sTemperatureWater = wxString::Format("%4.1f %s %s", t, opt->Deg.c_str(),
                                             opt->temperature.c_str());
        dtTemperatureWater = wxDateTime::Now();
        bTemperatureWater = true;
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "DBT") {
      m_NMEA0183.Parse();
      dtDepth = wxDateTime::Now();
      bDepth = true;
      if (m_NMEA0183.Dbt.ErrorMessage.Contains("Invalid") ||
          (m_NMEA0183.Dbt.DepthMeters == m_NMEA0183.Dbt.DepthFathoms)) {
        sDepth = "-----";
      } else {
        switch (opt->showDepth) {
          case 0:
            sDepth = wxString::Format("%5.1f %s", m_NMEA0183.Dbt.DepthMeters,
                                      opt->meter.c_str());
            break;
          case 1:
            sDepth = wxString::Format("%5.1f %s", m_NMEA0183.Dbt.DepthFeet,
                                      opt->feet.c_str());
            break;
          case 2:
            sDepth = wxString::Format("%5.1f %s", m_NMEA0183.Dbt.DepthFathoms,
                                      opt->fathom.c_str());
            break;
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived == "DPT") {
      m_NMEA0183.Parse();
      dtDepth = wxDateTime::Now();
      bDepth = true;
      if (m_NMEA0183.Dpt.ErrorMessage.Contains("Invalid")) {
        sDepth = "-----";
      } else {
        switch (opt->showDepth) {
          case 0:
            sDepth = wxString::Format("%5.1f %s", m_NMEA0183.Dpt.DepthMeters,
                                      opt->meter.c_str());
            break;
          case 1:
            sDepth = wxString::Format("%5.1f %s",
                                      m_NMEA0183.Dpt.DepthMeters / 0.3048,
                                      opt->feet.c_str());
            break;
          case 2:
            sDepth = wxString::Format("%5.1f %s",
                                      m_NMEA0183.Dpt.DepthMeters / 1.8288,
                                      opt->fathom.c_str());
            break;
        }
      }
    } else if (m_NMEA0183.LastSentenceIDReceived ==
               "XDR") {  // Transducer measurement
      /* XDR Transducer types
       * AngularDisplacementTransducer = 'A',
       * TemperatureTransducer = 'C',
       * LinearDisplacementTransducer = 'D',
       * FrequencyTransducer = 'F',
       * HumidityTransducer = 'H',
       * ForceTransducer = 'N',
       * PressureTransducer = 'P',
       * FlowRateTransducer = 'R',
       * TachometerTransducer = 'T',
       * VolumeTransducer = 'V'
       */

      if (m_NMEA0183.Parse()) {
        double xdrdata;
        wxString tempopt;

        for (int i = 0; i < m_NMEA0183.Xdr.TransducerCnt; i++) {
          wimdaSentence = true;
          dtWimda = wxDateTime::Now();

          xdrdata = m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData;
          // XDR Airtemp
          if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerType == "C") {
            if (opt->temperature == "F") xdrdata = ((xdrdata * 9) / 5) + 32;
            sTemperatureAir =
                wxString::Format("%2.2f%s %s", xdrdata, opt->Deg.c_str(),
                                 opt->temperature.c_str());
          }
          // XDR Pressure
          if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerType == "P") {
            if (m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement == "B") {
              xdrdata *= 1000;
            }
            sPressure =
                wxString::Format("%4.1f %s", xdrdata, opt->baro.c_str());
          }
          // XDR Humidity
          if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerType == "H") {
            sHumidity = wxString::Format("%3.1f ", xdrdata);
          }
          // XDR Volume
          if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerType == "V") {
            tempopt = opt->vol.SubString(0, 0).Upper();
            if (m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement == "M") {
              xdrdata *= 1000;
              if (tempopt == "G") xdrdata = xdrdata * 0.264172;
            }
            if (m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement == "L") {
              if (tempopt == "G") xdrdata = xdrdata * 0.264172;
            }
            if (m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement == "G") {
              if (tempopt == "L") xdrdata = xdrdata * 3.7854;
            }
            dVolume += xdrdata;
            sVolume = wxString::Format("%4.2f ", dVolume);
          }
        }
      }
    }
  }

  /*  Propietary NMEA sentences
   */
  /*											*/
  /*  use function appendRow to add the values to the grid
   */
  /*  For motorhours and/or fuel recalculate the grid with
   */
  /*	changeCellValue(lastRow, 0,0)
   */
  /*	In function checkGPS(bool appendClick) set the strings to emtpy string
   * when GPS */
  /*  is off.
   */

  wxStringTokenizer tkz(sentence, ",");
  wxString sentenceInd = tkz.GetNextToken();

  if (sentenceInd.Right(3) == "MDA") {
    wimdaSentence = true;
    dtWimda = wxDateTime::Now();

    double t;
    double p;
    double h;

    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken().ToDouble(&p);
    p = p * 1000;
    sPressure = wxString::Format("%4.1f %s", p, opt->baro.c_str());
    tkz.GetNextToken();

    tkz.GetNextToken().ToDouble(&t);
    if (opt->temperature == "F") t = ((t * 9) / 5) + 32;
    sTemperatureAir = wxString::Format("%2.2f%s %s", t, opt->Deg.c_str(),
                                       opt->temperature.c_str());

    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    if (tkz.GetNextToken().ToDouble(&h))
      sHumidity = wxString::Format("%3.1f ", h);
    else
      sHumidity = wxEmptyString;
  } else if (opt->bRPMIsChecked && sentenceInd.Right(3) == "RPM") {
    rpmSentence = true;
    if (opt->bRPMCheck)
      parent->logbookPlugIn->optionsDialog->setRPMSentence(sentence);
    long Umin1 = 0, Umin2 = 0;

    dtRPM = wxDateTime::Now();

    wxString source = tkz.GetNextToken();
    wxString engineNr = tkz.GetNextToken();
    wxString speed = tkz.GetNextToken();
    wxString pitch = tkz.GetNextToken();

    if (engineNr == opt->engine1Id && opt->bEng1RPMIsChecked) {
      speed.ToLong(&Umin1);
      if (source == "E") sRPM1 = speed;
      sRPM1Source = source;

      if (Umin1 != 0L) {
        if (source == "E") {
          if (!opt->engine1Running) {
            if (opt->engineMessageSails && opt->engineAllwaysSailsDown)
              dialog->resetSails();
            dialog->startEngine1(false, false, true);
            dialog->m_toggleBtnEngine1->SetLabel(
                dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR) +
                onOff[1]);
          }
        }
        if (source == "S") {
          bRPM1 = true;
          sRPM1Shaft = speed;
        }
      } else {
        if (opt->engine1Running) {
          if (opt->engineMessageSails) dialog->stateSails();
          dialog->stopEngine1(false, true);
          dialog->m_toggleBtnEngine1->SetLabel(
              dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR) +
              onOff[0]);
        }
      }
    }

    if (engineNr == opt->engine2Id && opt->bEng2RPMIsChecked) {
      speed.ToLong(&Umin2);
      if (source == "E") sRPM2 = speed;

      if (Umin2 != 0L) {
        if (source == "E") {
          if (!opt->engine2Running) {
            if (opt->engineMessageSails && opt->engineAllwaysSailsDown)
              dialog->resetSails();
            dialog->startEngine2(false, false, true);
            dialog->m_toggleBtnEngine2->SetLabel(
                dialog->m_gridMotorSails->GetColLabelValue(
                    LogbookHTML::MOTOR1) +
                onOff[1]);
          }
        }
        if (source == "S") {
          bRPM2 = true;
          sRPM2Shaft = speed;
        }
      } else {
        if (opt->engine2Running) {
          dialog->stopEngine2(false, true, true);
          dialog->m_toggleBtnEngine2->SetLabel(
              dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR1) +
              onOff[0]);
        }
      }
    }

    if (engineNr == opt->generatorId && opt->bGenRPMIsChecked) {
      speed.ToLong(&Umin2);

      if (Umin2 != 0L) {
        if (source == "E") {
          if (!opt->generatorRunning) {
            dialog->startGenerator(false, false, true);
            dialog->m_toggleBtnGenerator->SetLabel(
                dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::GENE) +
                onOff[1]);
          }
        }
      } else {
        if (opt->generatorRunning) {
          dialog->stopGenerator(false, true, true);
          dialog->m_toggleBtnGenerator->SetLabel(
              dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::GENE) +
              onOff[0]);
        }
      }
    }
  }
}

void Logbook::setDateTimeString(wxDateTime dt) {
  mUTCDateTime = dt;

  if (opt->gpsAuto) {
    if (newPosition.NSflag == 'E')
      opt->tzIndicator = 0;
    else
      opt->tzIndicator = 1;

    opt->tzHour = (int)newPosition.longitude / 15;
  }

  wxTimeSpan span(opt->tzHour, 0, 0, 0);
  if (opt->tzIndicator == 0)
    mCorrectedDateTime = mUTCDateTime + span;
  else
    mCorrectedDateTime = mUTCDateTime - span;

  if (opt->UTC) mCorrectedDateTime = mUTCDateTime;

  sDate = mCorrectedDateTime.Format(opt->sdateformat);
  sTime = mCorrectedDateTime.Format(opt->stimeformat);
}

void Logbook::setPositionString(double dLat, int iNorth, double dLon,
                                int iEast) {
  double lat, lon;
  float llt = dLat;
  int lat_deg_int = (int)(llt / 100);
  float lat_deg = lat_deg_int;
  float lat_min = llt - (lat_deg * 100);
  lat = lat_deg + (lat_min / 60.);
  if (iNorth == South) lat = -lat;
  if (opt->traditional)
    sLat = this->toSDMM(1, lat, true);
  else
    sLat = this->toSDMMOpenCPN(1, lat, true);

  float lln = dLon;
  int lon_deg_int = (int)(lln / 100);
  float lon_deg = lon_deg_int;
  float lon_min = lln - (lon_deg * 100);
  lon = lon_deg + (lon_min / 60.);
  if (iEast == West) lon = -lon;
  if (opt->traditional)
    sLon = this->toSDMM(2, lon, false);
  else
    sLon = this->toSDMMOpenCPN(2, lon, true);

  SetGPSStatus(true);
  //	dialog->GPSTimer->Start(5000);

  if (opt->everySM) checkDistance();
}

void Logbook::newLogbook() {
  bool zero = false;

  if (data_locn != this->logbookData_actual) this->switchToActualLogbook();

  int i = wxMessageBox(_("Are you sure ?"), _("New Logbook"), wxYES_NO);
  if (i == wxNO) {
    dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
    return;
  }

  i = wxMessageBox(_("Reset all Values to zero ?"), "", wxYES_NO);
  if (i == wxYES) zero = true;

  if (dialog->m_gridGlobal->GetNumberRows() <= 0) {
    wxMessageBox(_("Your Logbook has no lines ?"), _("New Logbook"), wxOK);
    return;
  }

  update();

  wxFileName fn = data_locn;
  wxString temp = fn.GetPath();
  dialog->appendOSDirSlash(&temp);
  wxString sn;
  wxString ss = wxDateTime::Now().FormatISOTime();
  ss.Replace(":", "_");
  ss = wxString::Format("until_%s_%s_logbook.txt",
                        wxDateTime::Now().FormatISODate().c_str(), ss.c_str());
  sn = temp + ss;

  wxCopyFile(data_locn, sn);

  wxArrayString s;
  for (int i = 0; i < dialog->numPages; i++) {
    for (int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++) {
      s.Add(dialog->logGrids[i]->GetCellValue(
          dialog->logGrids[i]->GetNumberRows() - 1, n));
    }
  }

  logbookFile->Open();
  logbookFile->Clear();
  logbookFile->Write();
  logbookFile->Close();

  clearAllGrids();

  int offset = 0;
  dialog->selGridRow = 0;
  for (int i = 0; i < dialog->numPages; i++) {
    if (zero) break;
    dialog->logGrids[i]->AppendRows();
    if (i > 0) offset += dialog->logGrids[i - 1]->GetNumberCols();

    for (int n = 0; n < dialog->logGrids[i]->GetNumberCols(); n++) {
      dialog->logGrids[i]->SetCellValue(0, n, s[n + offset]);
    }
  }
  if (!zero) {
    dialog->logGrids[0]->SetCellValue(0, 13,
                                      _("Last line from Logbook\n") + ss);
    dialog->logGrids[0]->SetCellValue(0, 6,
                                      dialog->logGrids[0]->GetCellValue(0, 6));
    wxString t = "0.00 " + opt->showDistance;
    dialog->logGrids[0]->SetCellValue(0, 5, t);
  } else {
    appendRow(true, false);
    dialog->logGrids[0]->SetCellValue(0, 13, _("Last Logbook is\n") + ss);
  }

  update();

  dialog->setEqualRowHeight(0);
  setCellAlign(0);
  dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
}

void Logbook::selectLogbook() {
  wxString path = dialog->Home_Locn;

  update();
  SelectLogbook selLogbook(dialog, path);

  if (selLogbook.ShowModal() == wxID_CANCEL) {
    dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
    return;
  }

  if (selLogbook.selRow == -1) {
    dialog->logGrids[dialog->m_logbook->GetSelection()]->SetFocus();
    return;
  }

  wxString s = selLogbook.files[selLogbook.selRow];

  for (int i = 0; i < LOGGRIDS; i++)
    if (dialog->logGrids[i]->GetNumberRows() != 0)
      dialog->logGrids[i]->DeleteRows(0, dialog->logGrids[i]->GetNumberRows());

  loadSelectedData(s);
}

void Logbook::loadSelectedData(wxString path) {
  data_locn = path;
  logbookFile = new wxTextFile(path);
  setFileName(path, layout_locn);
  wxFileName fn(path);
  path = fn.GetName();
  dialog->backupFile = path;
  if (path == "logbook") {
    path = _("Active Logbook");
    oldLogbook = false;
  } else {
    wxDateTime dt = dialog->getDateTo(path);
    path = wxString::Format(_("Old Logbook until %s"), dt.FormatDate().c_str());
    oldLogbook = true;
  }
  title = path;
  dialog->SetTitle(title);

  loadData();
}
void Logbook::clearAllGrids() {
  if (dialog->m_gridGlobal->GetNumberRows() > 0) {
    dialog->m_gridGlobal->DeleteRows(0, dialog->m_gridGlobal->GetNumberRows(),
                                     false);
    dialog->m_gridWeather->DeleteRows(0, dialog->m_gridWeather->GetNumberRows(),
                                      false);
    dialog->m_gridMotorSails->DeleteRows(
        0, dialog->m_gridMotorSails->GetNumberRows(), false);
  }
}

void Logbook::loadData() {
  wxString s = "", t;
  wxString nullhstr = "00:00";
  double nullval = 0.0;
  wxString dateFormat;

  dialog->selGridCol = dialog->selGridRow = 0;
  if (title.IsEmpty()) title = _("Active Logbook");

  clearAllGrids();

  int row = 0;

  /** make a backup of 0.910 */
  wxString sep = wxFileName::GetPathSeparator();
  wxString source = dialog->Home_Locn;
  wxString dest = dialog->Home_Locn + "910_Backup";

  wxFileInputStream input1(data_locn);
  wxTextInputStream* stream1 = new wxTextInputStream(input1);

  t = stream1->ReadLine();
  if (t.IsEmpty()) return;  // first install only
  if (t.Contains("#1.2#")) {
    dateFormat = t;
    t = stream1->ReadLine();
  } else {
    wxArrayString files;

    wxDir dir;
    wxString path = dialog->data;
    wxString dest = path + "Backup_1_1";
    wxDir destDir(dest);

    if (!wxDir::Exists(dest)) ::wxMkdir(dest);

    wxMessageBox(
        wxString::Format(_("Start converting to new Date/Time-Format\nand "
                           "backup all datafiles from version 1.1 to\n\n%s"),
                         dest.c_str()));

    dir.GetAllFiles(path, &files, "*.txt", wxDIR_FILES);
    dest += wxFileName::GetPathSeparator();

    for (unsigned int i = 0; i < files.Count(); i++) {
      wxFileName fn(files[i]);
      ::wxCopyFile(path + fn.GetFullName(), dest + fn.GetFullName());
    }

    convertTo_1_2();
  }
  wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);

  if (tkz.CountTokens() == 33 && !wxDir::Exists(dest)) {
    ::wxMkdir(dest);
    wxArrayString files;
    wxDir dir;
    dir.GetAllFiles(source.RemoveLast(), &files, "*.txt", wxDIR_FILES);
    for (unsigned int i = 0; i < files.Count(); i++) {
      wxFileName fn(files[i]);
      ::wxCopyFile(files[i], dest + sep + fn.GetFullName(), true);
    }
  }

  /***************************/

  wxFileInputStream input(data_locn);
  wxTextInputStream* stream = new wxTextInputStream(input, "\n", wxConvUTF8);

  wxString firstrow = stream->ReadLine();  // for #1.2#
  wxStringTokenizer first(firstrow, "\t", wxTOKEN_RET_EMPTY);
  first.GetNextToken();
  logbookDescription = first.GetNextToken();

  wxDateTime dt;
  int month = 0, day = 0, year = 0, hour = 0, min = 0, sec = 0;
  dialog->m_gridGlobal->BeginBatch();
  dialog->m_gridWeather->BeginBatch();
  dialog->m_gridMotorSails->BeginBatch();
  int lines = 0;
  while (!(t = stream->ReadLine()).IsEmpty()) {
    if (input.Eof()) break;
    lines++;
    dialog->m_gridGlobal->AppendRows();
    dialog->m_gridWeather->AppendRows();
    dialog->m_gridMotorSails->AppendRows();

    setCellAlign(row);

    wxStringTokenizer tkz(t, "\t", wxTOKEN_RET_EMPTY);
    int c = 0;
    int fields = tkz.CountTokens();

    while (tkz.HasMoreTokens()) {
      s = dialog->restoreDangerChar(tkz.GetNextToken());
      s.RemoveLast();

      switch (c) {
        case 0:
          dialog->m_gridGlobal->SetCellValue(row, ROUTE, s);
          break;
        case 1:
          month = wxAtoi(s);
          break;
        case 2:
          day = wxAtoi(s);
          break;
        case 3:
          year = wxAtoi(s);
          if (month >= 0 && day != 0 && year != 0) {
            dt.Set(day, (wxDateTime::Month)month, year);
            dialog->m_gridGlobal->SetCellValue(row, RDATE,
                                               dt.Format(opt->sdateformat));
          }
          break;
        case 4:
          if (s.IsEmpty())
            hour = -1;
          else
            hour = wxAtoi(s);
          break;
        case 5:
          if (s.IsEmpty())
            min = -1;
          else
            min = wxAtoi(s);
          break;
        case 6:
          if (hour == -1 || min == -1) continue;
          sec = wxAtoi(s);
          dt.Set(hour, min, sec);
          dialog->m_gridGlobal->SetCellValue(row, RTIME,
                                             dt.Format(opt->stimeformat));
          break;
        case 7:
          dialog->m_gridGlobal->SetCellValue(row, STATUS, s);
          break;
        case 8:
          dialog->m_gridGlobal->SetCellValue(row, WAKE, s);
          break;
        case 9:
          dialog->m_gridGlobal->SetCellValue(row, DISTANCE, s);
          break;
        case 10:
          dialog->m_gridGlobal->SetCellValue(row, DTOTAL, s);
          dialog->m_gridGlobal->SetReadOnly(row, DTOTAL);
          break;
        case 11:
          dialog->m_gridGlobal->SetCellValue(row, POSITION, s);
          break;
        case 12:
          dialog->m_gridGlobal->SetCellValue(row, COG, s);
          break;
        case 13:
          dialog->m_gridGlobal->SetCellValue(row, COW, s);
          break;
        case 14:
          dialog->m_gridGlobal->SetCellValue(row, SOG, s);
          break;
        case 15:
          dialog->m_gridGlobal->SetCellValue(row, SOW, s);
          break;
        case 16:
          dialog->m_gridGlobal->SetCellValue(row, DEPTH, s);
          break;
        case 17:
          dialog->m_gridGlobal->SetCellValue(row, REMARKS, s);
          break;
        case 18:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::BARO, s);
          break;
        case 19:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WIND, s);
          break;
        case 20:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WSPD, s);
          break;
        case 21:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::CURRENT, s);
          break;
        case 22:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::CSPD, s);
          break;
        case 23:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WAVE, s);
          break;
        case 24:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::SWELL, s);
          break;
        case 25:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WEATHER, s);
          break;
        case 26:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::CLOUDS, s);
          break;
        case 27:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::VISIBILITY, s);
          break;
        case 28:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::MOTOR, s);
          break;
        case 29:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::MOTORT, s);
          break;
        case 30:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::FUEL, s);
          break;
        case 31:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::FUELT, s);
          break;
        case 32:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::SAILS, s);
          break;
        case 33:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::REEF, s);
          break;
        case 34:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::WATER, s);
          break;
        case 35:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::WATERT, s);
          break;

        case 36:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::MREMARKS, s);
          break;
        case 37:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::HYDRO, s);
          break;
        case 38:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::AIRTE, s);
          break;
        case 39:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WATERTE, s);
          break;
        case 40:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::MOTOR1, s);
          break;
        case 41:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::MOTOR1T, s);
          break;
        case 42:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::GENE, s);
          break;
        case 43:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::GENET, s);
          break;
        case 44:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::BANK1, s);
          break;
        case 45:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::BANK1T, s);
          break;
        case 46:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::BANK2, s);
          break;
        case 47:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::BANK2T, s);
          break;
        case 48:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::WATERM, s);
          break;
        case 49:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::WATERMT, s);
          break;
        case 50:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::WATERMO, s);
          break;
        case 51:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::ROUTEID, s);
          break;
        case 52:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::TRACKID, s);
          break;
        case 53:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::RPM1, s);
          break;
        case 54:
          dialog->m_gridMotorSails->SetCellValue(row, LogbookHTML::RPM2, s);
          break;
        case 55:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WINDR, s);
          break;
        case 56:
          dialog->m_gridWeather->SetCellValue(row, LogbookHTML::WSPDR, s);
          //    int in =  0;
          break;
      }
      c++;
    }
    wxString temp = dialog->m_gridGlobal->GetCellValue(row, DISTANCE);
    temp.Replace(",", ".");
    double dist = wxAtof(temp);
    if ((dialog->m_gridGlobal->GetCellValue(row, STATUS) == wxEmptyString ||
         dialog->m_gridGlobal->GetCellValue(row, STATUS).GetChar(0) == ' ') &&
        dist > 0)
      dialog->m_gridGlobal->SetCellValue(row, STATUS, "S");

    if (fields <
        50)  // data from 0.910 ? need zero-values to calculate the columns
    {
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::MOTOR1,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::MOTOR1T,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::GENE,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::GENET,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::BANK1,
          wxString::Format("%2.2f %s", nullval, opt->ampereh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::BANK1T,
          wxString::Format("%2.2f %s", nullval, opt->ampereh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::BANK2,
          wxString::Format("%2.2f %s", nullval, opt->ampereh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::BANK2T,
          wxString::Format("%2.2f %s", nullval, opt->ampereh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::WATERM,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::WATERMT,
          wxString::Format("%s %s", nullhstr.c_str(), opt->motorh.c_str()));
      dialog->m_gridMotorSails->SetCellValue(
          row, LogbookHTML::WATERMO,
          wxString::Format("%2.2f %s", nullval, opt->vol.c_str()));
    }

    dialog->setEqualRowHeight(row);
    row++;
  }

  dialog->selGridRow = 0;
  dialog->selGridCol = 0;

  for (int i = 0; i < LOGGRIDS; i++) {
    dialog->logGrids[i]->Refresh();
    row = dialog->logGrids[i]->GetNumberRows() - 1;
    dialog->logGrids[i]->MakeCellVisible(row, 0);
    dialog->logGrids[i]->SetGridCursor(0, 0);
  }

  dialog->m_gridGlobal->EndBatch();
  dialog->m_gridWeather->EndBatch();
  dialog->m_gridMotorSails->EndBatch();

  if (!oldLogbook && lines >= 500) {
    wxString str = wxString::Format(sLinesReminder, lines);
    LinesReminderDlg* dlg = new LinesReminderDlg(str, dialog);
    dlg->Show();
  }
}

wxString Logbook::makeDateFromFile(wxString date, wxString dateformat) {
  wxStringTokenizer tkzd(date, "/");
  wxDateTime dt;
  wxDateTime::Month month = (wxDateTime::Month)wxAtoi(tkzd.GetNextToken());
  wxDateTime::wxDateTime_t day =
      (wxDateTime::wxDateTime_t)wxAtoi(tkzd.GetNextToken());
  int year = wxAtoi(tkzd.GetNextToken());
  dt.Set(day, month, year);

  return dt.Format(dateformat);
}

wxString Logbook::makeWatchtimeFromFile(wxString time, wxString timeformat) {
  wxStringTokenizer tkzt(time, ",");
  wxDateTime dts, dte;
  wxDateTime::wxDateTime_t hours =
      (wxDateTime::wxDateTime_t)wxAtoi(tkzt.GetNextToken());
  wxDateTime::wxDateTime_t mins =
      (wxDateTime::wxDateTime_t)wxAtoi(tkzt.GetNextToken());
  wxDateTime::wxDateTime_t houre =
      (wxDateTime::wxDateTime_t)wxAtoi(tkzt.GetNextToken());
  wxDateTime::wxDateTime_t mine =
      (wxDateTime::wxDateTime_t)wxAtoi(tkzt.GetNextToken());
  dts.Set(hours, mins);
  dte.Set(houre, mine);

  return dts.Format(timeformat) + "-" + dte.Format(timeformat);
}

void Logbook::convertTo_1_2() {
  wxString path = dialog->data;
  wxArrayString files;
  wxDir dir;
  int dtformat = opt->dateformat;
  int timeFormat = opt->timeformat;
  wxDateTime dt = wxDateTime::Now();
  bool b = true;

  wxString datePattern = LogbookDialog::datePattern;
  opt->dateformat = 0;
  opt->timeformat = 1;

  opt->setDateFormat();
  opt->setTimeFormat(1);

  update();

  dir.GetAllFiles(path, &files, "*logbook*.txt", wxDIR_FILES);

  for (unsigned int i = 0; i < files.Count(); i++) {
    wxFileName fn(files[i]);
    wxFileInputStream stream(path + fn.GetFullName());
    wxTextInputStream* in = new wxTextInputStream(stream, "\n", wxConvUTF8);
    wxFileOutputStream stream1(path + fn.GetFullName() + "_");
    wxTextOutputStream* out =
        new wxTextOutputStream(stream1, wxEOL_NATIVE, wxConvUTF8);

    int l = 0;

    while (true) {
      wxString s = in->ReadLine();
      if (stream.Eof() || s.IsEmpty()) break;

      wxStringTokenizer tkz(s, "\t");
      tkz.GetNextToken();
      wxString d = tkz.GetNextToken();
      b = LogbookDialog::myParseDate(d, dt);
      s.Replace(d, wxString::Format("%i \t%i \t%i ", dt.GetMonth(), dt.GetDay(),
                                    dt.GetYear()));
      wxString t = tkz.GetNextToken();
      LogbookDialog::myParseTime(t, dt);
      s.Replace(t, wxString::Format("%i \t%i \t%i ", dt.GetHour(),
                                    dt.GetMinute(), dt.GetSecond()));

      if (l == 0) *out << "#1.2#\n";
      l++;
      *out << s + "\n";
    }
    stream1.Close();
    if (b) {
      ::wxCopyFile(path + fn.GetFullName() + "_", path + fn.GetFullName());
      ::wxRemoveFile(path + fn.GetFullName() + "_");
    }
  }

  wxString m = "service.txt";
  wxFileInputStream streams(path + m);
  wxTextInputStream* in = new wxTextInputStream(streams, "\n", wxConvUTF8);
  wxFileOutputStream stream2(path + m + "_");
  wxTextOutputStream* out =
      new wxTextOutputStream(stream2, wxEOL_NATIVE, wxConvUTF8);

  int l = 0;
  while (true) {
    int i = 0;
    wxString tmp;
    wxString s = in->ReadLine();
    if (streams.Eof() || s.IsEmpty()) break;

    wxStringTokenizer tkz(s, "\t");
    tkz.GetNextToken();
    tkz.GetNextToken();
    wxString d = tkz.GetNextToken();
    d.RemoveLast();
    for (i = 0; i < dialog->maintenance->m_choicesCount; i++) {
      if (d == dialog->maintenance->m_choices[i]) break;
    }

    if (i == 8) {
      tmp = tkz.GetNextToken();
      tmp.RemoveLast();
      LogbookDialog::myParseDate(tmp, dt);
      s.Replace(tmp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                      dt.GetYear()));
      tmp = tkz.GetNextToken();
      LogbookDialog::myParseDate(tmp, dt);
      s.Replace(tmp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                      dt.GetYear()));
    } else if (i > 8) {
      tmp = tkz.GetNextToken();
      tmp = tkz.GetNextToken();
      tmp = tkz.GetNextToken();
      tmp.RemoveLast();
      if (!tmp.IsEmpty()) {
        LogbookDialog::myParseDate(tmp, dt);
        s.Replace(tmp, wxString::Format("%i\t%i\t%i", dt.GetMonth(),
                                        dt.GetDay(), dt.GetYear()));
      }
    }
    if (l == 0) *out << "#1.2#\n";
    l++;
    *out << s + "\n";
  }
  stream2.Close();
  if (b) {
    ::wxCopyFile(path + m + "_", path + m);
    ::wxRemoveFile(path + m + "_");
  }

  m = "crewlist.txt";
  wxFileInputStream streamc(path + m);
  in = new wxTextInputStream(streamc, "\n", wxConvUTF8);
  wxFileOutputStream stream3(path + m + "_");
  out = new wxTextOutputStream(stream3, wxEOL_NATIVE, wxConvUTF8);

  l = 0;
  while (true) {
    wxString tmp;
    wxString s = in->ReadLine();
    if (streamc.Eof() || s.IsEmpty()) break;

    wxStringTokenizer tkz(s, "\t");
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    wxString d = tkz.GetNextToken();
    if (!d.IsEmpty() && d.GetChar(0) != ' ') {
      d.RemoveLast();
      tmp = d;
      LogbookDialog::myParseDate(d, dt);
      s.Replace(tmp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                      dt.GetYear()));
    }
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    d = tkz.GetNextToken();
    if (!d.IsEmpty() && d.GetChar(0) != ' ') {
      d.RemoveLast();
      tmp = d;
      LogbookDialog::myParseDate(d, dt);
      s.Replace(tmp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                      dt.GetYear()));
    }
    if (l == 0) *out << "#1.2#\n";
    l++;
    *out << s + "\n";
  }
  stream3.Close();

  ::wxCopyFile(path + m + "_", path + m);
  ::wxRemoveFile(path + m + "_");

  m = "boat.txt";
  wxFileInputStream streamb(path + m);
  in = new wxTextInputStream(streamb, "\n", wxConvUTF8);
  wxFileOutputStream stream4(path + m + "_");
  out = new wxTextOutputStream(stream4, wxEOL_NATIVE, wxConvUTF8);

  l = 0;
  while (true) {
    wxString tmp;
    wxString s = in->ReadLine();
    if (streamb.Eof() || s.IsEmpty()) break;

    wxStringTokenizer tkz(s, "\t");
    for (int x = 0; x < 18; x++) tkz.GetNextToken();
    wxString d = tkz.GetNextToken();
    if (!d.IsEmpty() && d.GetChar(0) != ' ') {
      d.RemoveLast();
      tmp = d;
      LogbookDialog::myParseDate(d, dt);
      s.Replace(tmp, wxString::Format("%i/%i/%i", dt.GetMonth(), dt.GetDay(),
                                      dt.GetYear()));
    }

    if (l == 0) *out << "#1.2#\n";
    l++;
    *out << s + "\n";
  }
  stream4.Close();

  ::wxCopyFile(path + m + "_", path + m);
  ::wxRemoveFile(path + m + "_");

  opt->dateformat = dtformat;
  opt->timeformat = timeFormat;
  opt->setDateFormat();
  opt->setTimeFormat(opt->timeformat);
}

void Logbook::setCellAlign(int i) {
  dialog->m_gridGlobal->SetCellAlignment(i, ROUTE, wxALIGN_LEFT, wxALIGN_TOP);
  dialog->m_gridGlobal->SetCellAlignment(i, RDATE, wxALIGN_CENTRE, wxALIGN_TOP);
  dialog->m_gridGlobal->SetCellAlignment(i, RTIME, wxALIGN_CENTRE, wxALIGN_TOP);
  dialog->m_gridGlobal->SetCellAlignment(i, STATUS, wxALIGN_CENTRE,
                                         wxALIGN_TOP);
  dialog->m_gridGlobal->SetCellAlignment(i, WAKE, wxALIGN_LEFT, wxALIGN_TOP);
  dialog->m_gridGlobal->SetCellAlignment(i, REMARKS, wxALIGN_LEFT, wxALIGN_TOP);
  if (opt->windspeeds) {
    dialog->m_gridWeather->SetCellAlignment(i, LogbookHTML::WSPD,
                                            wxALIGN_CENTRE, wxALIGN_TOP);
    dialog->m_gridWeather->SetCellAlignment(i, LogbookHTML::WSPDR,
                                            wxALIGN_CENTRE, wxALIGN_TOP);
  }
  dialog->m_gridWeather->SetCellAlignment(i, LogbookHTML::WEATHER, wxALIGN_LEFT,
                                          wxALIGN_TOP);
  dialog->m_gridWeather->SetCellAlignment(i, LogbookHTML::CLOUDS, wxALIGN_LEFT,
                                          wxALIGN_TOP);
  dialog->m_gridWeather->SetCellAlignment(i, LogbookHTML::VISIBILITY,
                                          wxALIGN_LEFT, wxALIGN_TOP);
  dialog->m_gridMotorSails->SetCellAlignment(i, LogbookHTML::SAILS,
                                             wxALIGN_LEFT, wxALIGN_TOP);
  dialog->m_gridMotorSails->SetCellAlignment(i, LogbookHTML::REEF, wxALIGN_LEFT,
                                             wxALIGN_TOP);
  dialog->m_gridMotorSails->SetCellAlignment(i, LogbookHTML::MREMARKS,
                                             wxALIGN_LEFT, wxALIGN_TOP);

  dialog->m_gridGlobal->SetReadOnly(i, POSITION, true);
}

void Logbook::switchToActualLogbook() {
  dialog->selGridRow = 0;
  dialog->selGridCol = 0;
  logbookFile = new wxTextFile(logbookData_actual);
  data_locn = logbookData_actual;
  setFileName(logbookData_actual, layout_locn);
  dialog->SetTitle(_("Active Logbook"));
  loadData();
}

void Logbook::appendRow(bool showlastline, bool autoline) {
  wxString s;

  if (dialog->m_gridGlobal->IsSelection()) dialog->deselectAllLogbookGrids();

  checkGPS(autoline);

  if (noAppend) return;
  modified = true;

  wxFileName fn(logbookFile->GetName());
  if (fn.GetName() != ("logbook")) {
    switchToActualLogbook();
    noAppend = true;
    NoAppendDialog* x = new NoAppendDialog(dialog);
    x->Show();

    noAppend = false;
    oldLogbook = false;
  }

  int lastRow = dialog->logGrids[0]->GetNumberRows();
  if (lastRow >= 499) {
    static int repeat = lastRow;
    // dialog->timer->Stop();
    if (lastRow == repeat) {
      repeat += 50;
      wxString str = wxString::Format(sLinesReminder, lastRow + 1);
      LinesReminderDlg* dlg = new LinesReminderDlg(str, dialog);
      dlg->Show();

      wxMessageBox(wxString::Format(_("Your Logbook has %i lines\n\n\
			You should create a new logbook to minimize loadingtime."),
                                    lastRow),
                   _("Information"));
    }
    //	dialog->logbookPlugIn->opt->timer = false;

    /*	wxFileConfig *pConf = (wxFileConfig *)dialog->logbookPlugIn->m_pconfig;

    if(pConf)
    {
    pConf->SetPath ( _T ( "/PlugIns/Logbook" ) );
    pConf->Write ( "Timer", dialog->logbookPlugIn->opt->timer );
    }
    */
  }

  for (int i = 0; i < dialog->numPages; i++) dialog->logGrids[i]->AppendRows();

  if (lastRow > 0) {
    dialog->logGrids[0]->SetCellValue(
        lastRow, ROUTE, dialog->logGrids[0]->GetCellValue(lastRow - 1, ROUTE));
    // if(gpsStatus)
    dialog->logGrids[0]->SetCellValue(lastRow, POSITION, sLat + sLon);
    // else
    //	dialog->logGrids[0]->SetCellValue(lastRow,POSITION,dialog->logGrids[0]->GetCellValue(lastRow-1,POSITION));
    changeCellValue(lastRow, 0, 0);
    dialog->logGrids[0]->SetCellValue(
        lastRow, DTOTAL,
        dialog->logGrids[0]->GetCellValue(lastRow - 1, DTOTAL));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::MOTORT,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::MOTORT));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::MOTOR1T,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::MOTOR1T));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::GENET,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::GENET));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::FUELT,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::FUELT));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::WATERT,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::WATERT));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::WATERMT,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::WATERMT));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::BANK1T,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::BANK1T));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::BANK2T,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::BANK2T));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::TRACKID,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::TRACKID));
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::ROUTEID,
        dialog->logGrids[2]->GetCellValue(lastRow - 1, LogbookHTML::ROUTEID));
  } else {
    dialog->logGrids[0]->SetCellValue(lastRow, ROUTE, _("unnamed Route"));
    if (gpsStatus)
      dialog->logGrids[0]->SetCellValue(lastRow, POSITION, sLat + sLon);
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::FUELT,
                                      opt->fuelTank.c_str());
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::WATERT,
                                      opt->waterTank.c_str());
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::BANK1T,
                                      opt->bank1.c_str());
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::BANK2T,
                                      opt->bank2.c_str());
  }

  if (sDate != "") {
    dialog->logGrids[0]->SetCellValue(lastRow, RDATE, sDate);
    dialog->logGrids[0]->SetCellValue(lastRow, RTIME, sTime);
  } else {
    if (!opt->UTC)
      mCorrectedDateTime = wxDateTime::Now();
    else
      mCorrectedDateTime = wxDateTime::Now().ToUTC();
    dialog->logGrids[0]->SetCellValue(
        lastRow, RDATE, mCorrectedDateTime.Format(opt->sdateformat));
    dialog->logGrids[0]->SetCellValue(
        lastRow, RTIME, mCorrectedDateTime.Format(opt->stimeformat));
  }

  if (MOBIsActive)
    dialog->logGrids[0]->SetCellValue(lastRow, REMARKS,
                                      _("*** MAN OVERBOARD ***\n"));
  else
    dialog->logGrids[0]->SetCellValue(lastRow, REMARKS, sLogText);

  if (routeIsActive) {
    if (activeRoute != wxEmptyString)
      dialog->logGrids[0]->SetCellValue(lastRow, ROUTE, activeRoute);

    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::ROUTEID,
                                      activeRouteGUID);
  } else
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::ROUTEID,
                                      wxEmptyString);

  if (trackIsActive) {
    if (!routeIsActive)
      dialog->logGrids[0]->SetCellValue(lastRow, ROUTE,
                                        _("Track ") + activeTrack);

    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::TRACKID,
                                      activeTrackGUID);
  } else
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::TRACKID,
                                      wxEmptyString);

  dialog->logGrids[0]->SetCellValue(lastRow, COG, sCOG);
  dialog->logGrids[0]->SetCellValue(lastRow, COW, sCOW);
  dialog->logGrids[0]->SetCellValue(lastRow, SOG, sSOG);
  dialog->logGrids[0]->SetCellValue(lastRow, SOW, sSOW);
  dialog->logGrids[0]->SetCellValue(lastRow, DEPTH, sDepth);
  dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WATERTE,
                                    sTemperatureWater);
  dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WIND, sWindT);
  if (opt->windspeeds) {
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WSPD, swindspeedsT);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WINDR, sWindA);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WSPDR,
                                      swindspeedsA);
    minwindA = minwindT = 99;
    maxwindA = maxwindT = 0;
  } else {
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WSPD, sWindSpeedT);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WINDR, sWindA);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::WSPDR, sWindSpeedA);
  }
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR, "00.00");
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR1, "00.00");
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::GENE, "00.00");
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::WATERM, "00.00");
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::WATERMO, "0");
  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS, " ");

  if (wimdaSentence) {
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::AIRTE,
                                      sTemperatureAir);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::BARO, sPressure);
    dialog->logGrids[1]->SetCellValue(lastRow, LogbookHTML::HYDRO, sHumidity);
  }

  dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::FUEL, sVolume);
  sVolume = wxEmptyString;
  dVolume = 0;
  getModifiedCellValue(2, lastRow, 0, LogbookHTML::FUEL);

  if (bRPM1) {
    if (!opt->engine1Running)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                        _("Engine #1 started"));
    else {
      if (opt->engineMessageRunning)
        dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                          _("Engine #1 running"));
      if (opt->NMEAUseERRPM || opt->toggleEngine1) {
        dtEngine1Off = wxDateTime::Now().Subtract(opt->dtEngine1On);
        // wxMessageBox(dtEngine1Off.Format("%H:%M:%S"));
        opt->dtEngine1On = wxDateTime::Now();
        dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR,
                                          dtEngine1Off.Format("%H:%M"));
        //			wxMessageBox(dtEngine1Off.Format("%H:%M:%S"));
      }
    }
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::RPM1, sRPM1);
  }
  if (!bRPM1 && opt->engine1Running) {
    if (opt->NMEAUseERRPM || !opt->toggleEngine1)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR,
                                        dtEngine1Off.Format("%H:%M"));
    dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                      _("Engine #1 stopped"));
  }
  if (bRPM2) {
    if (!opt->engine2Running)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                        _("Engine #2 started"));
    else {
      if (opt->engineMessageRunning) {
        if (dialog->logGrids[2]
                ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                .IsEmpty() ||
            dialog->logGrids[2]
                    ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                    .GetChar(0) == ' ')
          dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                            _("Engine #2 running"));
        else
          dialog->logGrids[2]->SetCellValue(
              lastRow, LogbookHTML::MREMARKS,
              dialog->logGrids[2]->GetCellValue(lastRow,
                                                LogbookHTML::MREMARKS) +
                  _("\nEngine #2 running"));
      }
      if (opt->NMEAUseERRPM || engine2Manual) {
        dtEngine2Off = wxDateTime::Now().Subtract(opt->dtEngine2On);
        opt->dtEngine2On = wxDateTime::Now();
        dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR1,
                                          dtEngine2Off.Format("%H:%M"));
        //	wxMessageBox(dtEngine2Off.Format("%H:%M:%S"));
      }
    }
  }
  if (!bRPM2 && opt->engine2Running) {
    if (opt->NMEAUseERRPM || !opt->toggleEngine2)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MOTOR1,
                                        dtEngine2Off.Format("%H:%M"));
    if (dialog->logGrids[2]
            ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
            .IsEmpty() ||
        dialog->logGrids[2]
                ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                .GetChar(0) == ' ')
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                        _("Engine #2 stopped"));
    else
      dialog->logGrids[2]->SetCellValue(
          lastRow, LogbookHTML::MREMARKS,
          dialog->logGrids[2]->GetCellValue(lastRow, LogbookHTML::MREMARKS) +
              _("\nEngine #2 stopped"));
  }

  if (bGEN) {
    if (!opt->generatorRunning)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                        _("Generator started"));
    else {
      if (opt->engineMessageRunning) {
        if (dialog->logGrids[2]
                ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                .IsEmpty() ||
            dialog->logGrids[2]
                    ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                    .GetChar(0) == ' ')
          dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                            _("Generator running"));
        else
          dialog->logGrids[2]->SetCellValue(
              lastRow, LogbookHTML::MREMARKS,
              dialog->logGrids[2]->GetCellValue(lastRow,
                                                LogbookHTML::MREMARKS) +
                  _("\nGenerator running"));
      }
      if (opt->NMEAUseERRPM || generatorManual) {
        dtGeneratorOff = wxDateTime::Now().Subtract(opt->dtGeneratorOn);
        opt->dtGeneratorOn = wxDateTime::Now();
        dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::GENE,
                                          dtGeneratorOff.Format("%H:%M"));
        //	wxMessageBox(dtEngine2Off.Format("%H:%M:%S"));
      }
    }
  }

  if (!bGEN && opt->generatorRunning) {
    if (opt->NMEAUseERRPM || !opt->toggleGenerator)
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::GENE,
                                        dtGeneratorOff.Format("%H:%M"));
    if (dialog->logGrids[2]
            ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
            .IsEmpty() ||
        dialog->logGrids[2]
                ->GetCellValue(lastRow, LogbookHTML::MREMARKS)
                .GetChar(0) == ' ')
      dialog->logGrids[2]->SetCellValue(lastRow, LogbookHTML::MREMARKS,
                                        _("Generator stopped"));
    else
      dialog->logGrids[2]->SetCellValue(
          lastRow, LogbookHTML::MREMARKS,
          dialog->logGrids[2]->GetCellValue(lastRow, LogbookHTML::MREMARKS) +
              _("\nGenerator stopped"));
  }

  //	wxString sEngine = " "+opt->rpm+" ("+opt->engine+")";
  //	wxString sShaft =  " "+opt->rpm+" ("+opt->shaft+")";
  wxString sEngine = " (" + opt->engine + ")";
  wxString sShaft = " (" + opt->shaft + ")";

  if (!sRPM1.IsEmpty())
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::RPM1,
        sRPM1 + sEngine +
            ((sRPM1Shaft.IsEmpty()) ? "" : "\n" + sRPM1Shaft + sShaft));
  if (!sRPM2.IsEmpty())
    dialog->logGrids[2]->SetCellValue(
        lastRow, LogbookHTML::RPM2,
        sRPM2 + sEngine +
            ((sRPM2Shaft.IsEmpty()) ? "" : "\n" + sRPM2Shaft + sShaft));

  if (sailsMessage && opt->engineMessageSails) {
    wxString temp =
        dialog->logGrids[2]->GetCellValue(lastRow, LogbookHTML::MREMARKS);
    if (temp.Len() == 1 && temp.GetChar(0) == ' ') temp.Remove(0, 1);

    if ((oldSailsState == 0 || oldSailsState == -1) && sailsState == 1) {
      dialog->logGrids[2]->SetCellValue(
          lastRow, LogbookHTML::MREMARKS,
          temp + ((temp.IsEmpty()) ? "" : "\n") + _("Sails hoisted"));
      oldSailsState = 1;
    } else if ((oldSailsState == 1 || oldSailsState == -1) && sailsState == 1)
      dialog->logGrids[2]->SetCellValue(
          lastRow, LogbookHTML::MREMARKS,
          temp + ((temp.IsEmpty()) ? "" : "\n") + _("Sails changed"));
    else if ((oldSailsState == 1 || oldSailsState == -1) && sailsState == 0) {
      dialog->logGrids[2]->SetCellValue(
          lastRow, LogbookHTML::MREMARKS,
          temp + ((temp.IsEmpty()) ? "" : "\n") + _("Sails down"));
      oldSailsState = 0;
    }

    sailsMessage = false;
  }

  if (ActualWatch::active == true)
    dialog->logGrids[0]->SetCellValue(lastRow, WAKE, ActualWatch::member);

  wxString sails = wxEmptyString;
  unsigned int n = 0;
  for (int i = 0; i < opt->numberSails; i++) {
    if (dialog->checkboxSails[i]->IsChecked()) {
      sails += opt->sailsName.Item(i);
      if (n == 1) {
        sails += "\n";
        n = 0;
      } else {
        sails += ", ";
        n++;
      }
    }
  }

  if (!sails.IsEmpty() && n == 1)
    sails.RemoveLast(2);
  else if (!sails.IsEmpty())
    sails.RemoveLast(1);

  dialog->m_gridMotorSails->SetCellValue(lastRow, LogbookHTML::SAILS, sails);

  changeCellValue(lastRow, 0, 1);
  setCellAlign(lastRow);
  dialog->setEqualRowHeight(lastRow);

  dialog->m_gridGlobal->SetReadOnly(lastRow, 6);

  update(); /* Save to file with every newline */

  if (showlastline) {
    dialog->m_gridGlobal->MakeCellVisible(lastRow, 0);
    dialog->m_gridWeather->MakeCellVisible(lastRow, 0);
    dialog->m_gridMotorSails->MakeCellVisible(lastRow, 0);
  }
}

void Logbook::resetEngineManualMode(int enginenumber) {
  /* engine number 0=all, 1=engine#1, 2=engine#2, 3=generator */
  bool t = opt->bRPMCheck;
  wxString onOff[2];
  onOff[0] = _(" off");
  onOff[1] = _(" on");

  if (enginenumber == 1 || enginenumber == 0) {
    parent->m_toggleBtnEngine1->SetValue(false);
    opt->toggleEngine1 = false;
    bRPM1 = false;
    dtEngine1Off = wxDateTime::Now().Subtract(opt->dtEngine1On);
    parent->m_toggleBtnEngine1->SetLabel(
        parent->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR) +
        onOff[0]);
  }
  if (enginenumber == 2 || enginenumber == 0) {
    parent->m_toggleBtnEngine2->SetValue(false);
    opt->toggleEngine2 = false;
    bRPM2 = false;
    dtEngine2Off = wxDateTime::Now().Subtract(opt->dtEngine2On);
    parent->m_toggleBtnEngine2->SetLabel(
        parent->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR1) +
        onOff[0]);
  }
  if (enginenumber == 3 || enginenumber == 0) {
    parent->m_toggleBtnGenerator->SetValue(false);
    opt->toggleGenerator = false;
    bGEN = false;
    dtGeneratorOff = wxDateTime::Now().Subtract(opt->dtGeneratorOn);
    parent->m_toggleBtnGenerator->SetLabel(
        parent->m_gridMotorSails->GetColLabelValue(LogbookHTML::GENE) +
        onOff[0]);
  }

  appendRow(true, false);

  if (enginenumber == 1 || enginenumber == 0) {
    opt->dtEngine1On = -1;
    engine1Manual = false;
    opt->engine1Running = false;
  }
  if (enginenumber == 2 || enginenumber == 0) {
    opt->dtEngine2On = -1;
    engine2Manual = false;
    opt->engine2Running = false;
  }
  if (enginenumber == 3 || enginenumber == 0) {
    opt->dtGeneratorOn = -1;
    generatorManual = false;
    opt->generatorRunning = false;
  }

  opt->bRPMCheck = t;
}

void Logbook::checkNMEADeviceIsOn() {
  wxDateTime dtn = wxDateTime::Now();
  wxString onOff[2];
  onOff[0] = _(" off");
  onOff[1] = _(" on");

  if (bDepth && dtn.Subtract(dtDepth).GetSeconds() > DEVICE_TIMEOUT)  // Sounder
  {
    sDepth = wxEmptyString;
    bDepth = false;
  }
  if (bSOW && dtn.Subtract(dtSOW).GetSeconds() > DEVICE_TIMEOUT)  // Speedo
  {
    sSOW = wxEmptyString;
    bSOW = false;
  }
  if (bWindA &&
      dtn.Subtract(dtWindA).GetSeconds() > DEVICE_TIMEOUT)  // Wind Rel
  {
    sWindA = wxEmptyString;
    sWindSpeedA = wxEmptyString;
    swindspeedsA = wxEmptyString;
    bWindA = false;
  }
  if (bWindT &&
      dtn.Subtract(dtWindT).GetSeconds() > DEVICE_TIMEOUT)  // Wind True
  {
    sWindT = wxEmptyString;
    sWindSpeedT = wxEmptyString;
    swindspeedsT = wxEmptyString;
    bWindT = false;
  }
  if (bCOW && dtn.Subtract(dtCOW).GetSeconds() > DEVICE_TIMEOUT)  // Heading
  {
    sCOW = wxEmptyString;
    bCOW = false;
  }
  if (bTemperatureWater && dtn.Subtract(dtTemperatureWater).GetSeconds() >
                               DEVICE_TIMEOUT)  // Watertemperature
  {
    sTemperatureWater = wxEmptyString;
    bTemperatureWater = false;
  }

  if (wimdaSentence &&
      dtn.Subtract(dtWimda).GetSeconds() > DEVICE_TIMEOUT)  // WeatherStation
  {
    sPressure = wxEmptyString;
    sTemperatureAir = wxEmptyString;
    sHumidity = wxEmptyString;
    wimdaSentence = false;
  }
  if (rpmSentence && dtn.Subtract(dtRPM).GetSeconds() >
                         DEVICE_TIMEOUT)  // Engine RPM and Engine elapsed time
  {
    rpmSentence = false;
    wxDateTime now = wxDateTime::Now();

    if (opt->bEng1RPMIsChecked) {
      bRPM1 = false;
      dtEngine1Off = now.Subtract(opt->dtEngine1On);
      opt->dtEngine1On = -1;
      sRPM1 = wxEmptyString;
      sRPM1Shaft = wxEmptyString;
      dialog->m_toggleBtnEngine1->SetLabel(
          dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR) +
          onOff[0]);
    }

    if (opt->bEng2RPMIsChecked) {
      bRPM2 = false;
      dtEngine2Off = now.Subtract(opt->dtEngine2On);
      opt->dtEngine2On = -1;
      sRPM2 = wxEmptyString;
      sRPM2Shaft = wxEmptyString;
      dialog->m_toggleBtnEngine2->SetLabel(
          dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::MOTOR1) +
          onOff[0]);
    }

    if (opt->bGenRPMIsChecked) {
      bGEN = false;
      dtGeneratorOff = now.Subtract(opt->dtGeneratorOn);
      opt->dtGeneratorOn = -1;
      dialog->m_toggleBtnGenerator->SetLabel(
          dialog->m_gridMotorSails->GetColLabelValue(LogbookHTML::GENE) +
          onOff[0]);
    }

    appendRow(true, true);

    if (opt->bEng1RPMIsChecked) {
      opt->engine1Running = false;
    }
    if (opt->bEng1RPMIsChecked) {
      opt->engine2Running = false;
    }
    if (opt->bGenRPMIsChecked) {
      opt->generatorRunning = false;
    }
  }
}

void Logbook::recalculateLogbook(int row) {
  int cells[] = {
      LogbookHTML::POSITION, LogbookHTML::MOTOR,  LogbookHTML::MOTOR1,
      LogbookHTML::FUEL,     LogbookHTML::GENE,   LogbookHTML::BANK1,
      LogbookHTML::BANK2,    LogbookHTML::WATERM, LogbookHTML::WATER};
  int grid;

  if (row < 0) return;
  if (row == 0 && dialog->m_gridGlobal->GetNumberRows() > 1) row = 1;

  int len = sizeof(cells) / sizeof(cells[0]);
  for (int i = 0; i < len; i++) {
    if (i == 0)
      grid = 0;
    else
      grid = 2;

    if (dialog->m_gridGlobal->GetNumberRows() >= 2)
      getModifiedCellValue(grid, row, 0, cells[i]);
  }
}

void Logbook::checkCourseChanged() {
  static wxDateTime dt;
  static bool timer = true;

  wxDouble cog;
  wxGrid* grid = dialog->m_gridGlobal;

  wxString temp = grid->GetCellValue(grid->GetNumberRows() - 1, 8);
  temp.Replace(",", ".");
  temp.ToDouble(&cog);

  if ((cog == dCOG) || (oldLogbook || temp.IsEmpty())) return;

#ifdef __WXOSX__
  wxDouble result = labs(cog - dCOG);
#else
  wxDouble result = abs(cog - dCOG);
#endif
  if (result > 180) result -= 360;

#ifdef __WXOSX__
  if (labs(result) >= opt->dCourseChangeDegrees &&
      !dialog->logbookPlugIn->eventsEnabled)
#else
  if (abs(result) >= opt->dCourseChangeDegrees &&
      !dialog->logbookPlugIn->eventsEnabled)
#endif
  {
    if (timer) {
      timer = false;
      dt = mCorrectedDateTime;
      long min;
      opt->courseTextAfterMinutes.ToLong(&min);
      wxTimeSpan t(0, (int)min);
      dt.Add(t);
    }

    if (mCorrectedDateTime >= dt) {
      dialog->logbookTimerWindow->popUp();
      timer = true;
      courseChange = true;
      appendRow(true, true);
      courseChange = false;
    }
  }
}

void Logbook::checkWayPoint(RMB rmb) {
  if (lastWayPoint == rmb.From) return;

  dialog->logbookTimerWindow->popUp();
  tempRMB = rmb;
  waypointArrived = true;
  appendRow(true, true);
  waypointArrived = false;
  lastWayPoint = rmb.From;
}

void Logbook::checkGuardChanged() {
  if (dLastMinute == -1) {
    dLastMinute = (long)mCorrectedDateTime.GetMinute() + 1;
    return;
  }

  long hour, minute;
  long m_minute = (long)mCorrectedDateTime.GetMinute();
  long m_hour = (long)mCorrectedDateTime.GetHour();
  bool append = false;

  if (m_minute >= dLastMinute) {
    for (int row = 0; row < dialog->m_gridCrewWake->GetNumberRows(); row++) {
      for (int col = 2; col < dialog->m_gridCrewWake->GetNumberCols();
           col += 2) {
        wxString s = dialog->m_gridCrewWake->GetCellValue(row, col);
        if (s.IsEmpty()) continue;
        wxStringTokenizer tkz(s, ":");
        tkz.GetNextToken().ToLong(&hour);
        tkz.GetNextToken().ToLong(&minute);
        if (hour != m_hour) continue;
        if (minute == m_minute) append = true;
      }
    }
    if (append) {
      guardChange = true;
      appendRow(true, true);
      guardChange = false;
    }
    dLastMinute = m_minute + 1;
  }
}

void Logbook::checkDistance() {
  if (oldPosition.latitude == 500) oldPosition = newPosition;

  double fromlat = oldPosition.posLat * PI / 180;
  double fromlon = oldPosition.posLon * PI / 180;
  double tolat = newPosition.posLat * PI / 180;
  double tolon = newPosition.posLon * PI / 180;
  if (oldPosition.NSflag == 'S') fromlat = -fromlat;
  if (oldPosition.WEflag == 'W') fromlon = -fromlon;
  if (newPosition.NSflag == 'S') tolat = -fromlat;
  if (newPosition.WEflag == 'W') tolon = -fromlon;

  double sm = acos(cos(fromlat) * cos(fromlon) * cos(tolat) * cos(tolon) +
                   cos(fromlat) * sin(fromlon) * cos(tolat) * sin(tolon) +
                   sin(fromlat) * sin(tolat)) *
              3443.9;

  double factor = 1;
  double tdistance = 1;

  switch (opt->showDistanceChoice) {
    case 0:
      factor = 1;
      break;
    case 1:
      factor = 1852;
      break;
    case 2:
      factor = 1.852;
      break;
  }

  tdistance = sm * factor;

  if (tdistance >= opt->dEverySM && !dialog->logbookPlugIn->eventsEnabled) {
    dialog->logbookTimerWindow->popUp();
    everySM = true;
    appendRow(true, true);
    everySM = false;
    oldPosition = newPosition;
  }
}

wxString Logbook::calculateDistance(wxString fromstr, wxString tostr) {
  if ((fromstr.IsEmpty() || tostr.IsEmpty()) || fromstr == tostr)
    return wxString("0.00 " + opt->showDistance);

  wxString sLat, sLon, sLatto, sLonto;
  wxDouble fromlat, fromlon, tolat, tolon, sm;

  wxStringTokenizer tkz(fromstr, "\n");
  sLat = tkz.GetNextToken();
  sLon = tkz.GetNextToken();
  wxStringTokenizer tkzto(tostr, "\n");
  sLatto = tkzto.GetNextToken();
  sLonto = tkzto.GetNextToken();

  if (opt->traditional) {
    fromlat = positionStringToDezimal(sLat) * (PI / 180);
    fromlon = positionStringToDezimal(sLon) * (PI / 180);
    tolat = positionStringToDezimal(sLatto) * (PI / 180);
    tolon = positionStringToDezimal(sLonto) * (PI / 180);
  } else {
    fromlat = positionStringToDezimalModern(sLat) * (PI / 180);
    fromlon = positionStringToDezimalModern(sLon) * (PI / 180);
    tolat = positionStringToDezimalModern(sLatto) * (PI / 180);
    tolon = positionStringToDezimalModern(sLonto) * (PI / 180);
  }
  if (oldPosition.NSflag == 'S') fromlat = -fromlat;
  if (oldPosition.WEflag == 'W') fromlon = -fromlon;
  if (newPosition.NSflag == 'S') tolat = -fromlat;
  if (newPosition.WEflag == 'W') tolon = -fromlon;

  ///////
  sm = acos(cos(fromlat) * cos(fromlon) * cos(tolat) * cos(tolon) +
            cos(fromlat) * sin(fromlon) * cos(tolat) * sin(tolon) +
            sin(fromlat) * sin(tolat)) *
       3443.9;
  ////// code snippet from http://www2.nau.edu/~cvm/latlongdist.html#formats

  double factor = 1;
  double tdistance = 1;

  switch (opt->showDistanceChoice) {
    case 0:
      factor = 1;
      break;
    case 1:
      factor = 1852;
      break;
    case 2:
      factor = 1.852;
      break;
  }

  tdistance = sm * factor;

  wxString ret =
      wxString::Format("%.2f %s", tdistance, opt->showDistance.c_str());
  ret.Replace(".", dialog->decimalPoint);
  return ret;
}

wxDouble Logbook::positionStringToDezimal(wxString pos) {
  wxDouble resdeg, resmin, ressec = 0;
  wxString temp = pos;

  wxStringTokenizer tkz(pos, " ");
  temp = tkz.GetNextToken();
  temp.Replace(",", ".");
  temp.ToDouble(&resdeg);
  if (pos.Contains("S")) resdeg = -resdeg;
  if (pos.Contains("W")) resdeg = -resdeg;
  temp = tkz.GetNextToken();
  temp.Replace(",", ".");
  temp.ToDouble(&resmin);
  if (pos.Contains("S")) resmin = -resmin;
  if (pos.Contains("W")) resmin = -resmin;
  temp = tkz.GetNextToken();
  temp.Replace(",", ".");
  temp.ToDouble(&ressec);
  if (pos.Contains("S")) ressec = -ressec;
  if (pos.Contains("W")) ressec = -ressec;
  resmin = (resmin / 60 + ressec / 3600);

  return resdeg + resmin;
}

wxDouble Logbook::positionStringToDezimalModern(wxString pos) {
  wxDouble resdeg, resmin;
  wxString temp = pos;

  wxStringTokenizer tkz(pos, " ");
  temp = tkz.GetNextToken();
  temp.Replace(",", ".");
  temp.ToDouble(&resdeg);
  if (pos.Contains("S")) resdeg = -resdeg;
  if (pos.Contains("W")) resdeg = -resdeg;
  temp = tkz.GetNextToken();
  temp.Replace(",", ".");
  temp.ToDouble(&resmin);
  if (pos.Contains("S")) resmin = -resmin;
  if (pos.Contains("W")) resmin = -resmin;

  return resdeg + (resmin / 60);
}

void Logbook::deleteRow(int row) {
  dialog->logGrids[dialog->m_notebook8->GetSelection()]->SelectRow(row, true);
  int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"), row + 1),
                            _("Confirm"), wxYES_NO | wxCANCEL, dialog);
  if (answer == wxYES) {
    deleteRows();
    modified = true;
  }
}

void Logbook::changeCellValue(int row, int col, int mode) {
  if (mode)
    for (int g = 0; g < LOGGRIDS; g++)
      for (int i = 0; i < dialog->logGrids[g]->GetNumberCols(); i++)
        getModifiedCellValue(g, row, i, i);
  else
    getModifiedCellValue(dialog->m_notebook8->GetSelection(), row, col, col);
}

void Logbook::update() {
  if (!modified) return;
  modified = false;

  dialog->logGrids[0]->Refresh();
  dialog->logGrids[1]->Refresh();
  dialog->logGrids[2]->Refresh();

  int count;
  if ((count = dialog->logGrids[0]->GetNumberRows()) == 0) {
    wxFile f;
    f.Create(data_locn, true);
    return;
  }

  wxString s = "", temp;

  wxString newLocn = data_locn;
  newLocn.Replace("txt", "Bak");
  wxRename(data_locn, newLocn);

  wxFileOutputStream output(data_locn);
  wxTextOutputStream* stream =
      new wxTextOutputStream(output, wxEOL_NATIVE, wxConvUTF8);

  stream->WriteString("#1.2#\t" + logbookDescription + "\n");
  for (int r = 0; r < count; r++) {
    for (int g = 0; g < LOGGRIDS; g++) {
      for (int c = 0; c < dialog->logGrids[g]->GetNumberCols(); c++) {
        if (g == 1 && (c == LogbookHTML::HYDRO || c == LogbookHTML::WATERTE ||
                       c == LogbookHTML::AIRTE || c == LogbookHTML::WINDR ||
                       c == LogbookHTML::WSPDR))
          continue;
        if (g == 2 && (c == LogbookHTML::MOTOR1 || c == LogbookHTML::MOTOR1T ||
                       c == LogbookHTML::RPM1 || c == LogbookHTML::RPM2 ||
                       c == LogbookHTML::GENE || c == LogbookHTML::GENET ||
                       c == LogbookHTML::WATERM || c == LogbookHTML::WATERMT ||
                       c == LogbookHTML::WATERMO || c == LogbookHTML::BANK1 ||
                       c == LogbookHTML::BANK1T || c == LogbookHTML::BANK2 ||
                       c == LogbookHTML::BANK2T || c == LogbookHTML::TRACKID ||
                       c == LogbookHTML::ROUTEID))
          continue;
        if (g == 0 && c == RDATE) {
          wxString t = dialog->logGrids[g]->GetCellValue(r, c);
          if (!t.IsEmpty()) {
            wxDateTime dt;
            dialog->myParseDate(t, dt);
            temp = wxString::Format("%i \t%i \t%i", dt.GetMonth(), dt.GetDay(),
                                    dt.GetYear());
          } else
            temp = wxString::Format(" \t \t");
        } else if (g == 0 && c == RTIME) {
          wxString t = dialog->logGrids[g]->GetCellValue(r, c);
          if (!t.IsEmpty()) {
            wxDateTime dt;
            dialog->myParseTime(t, dt);
            temp = wxString::Format("%i \t%i \t%i", dt.GetHour(),
                                    dt.GetMinute(), dt.GetSecond());
          } else
            temp = wxString::Format(" \t \t");
        } else
          temp = dialog->logGrids[g]->GetCellValue(r, c);

        s += dialog->replaceDangerChar(temp);
        s += " \t";
      }
    }

    for (int ext = LogbookHTML::HYDRO; ext != LogbookHTML::WIND;
         ext++)  // extended 3 columns in weathergrid
    {
      temp = dialog->logGrids[1]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::MOTOR1; ext <= LogbookHTML::MOTOR1T;
         ext++)  // extend MOTOR #1
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::GENE; ext <= LogbookHTML::BANK2T;
         ext++)  // extend for GENERATOR and Battery-Banks
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::WATERM; ext <= LogbookHTML::WATERMO;
         ext++)  // extend WATERMAKER
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::ROUTEID;
         ext < parent->m_gridMotorSails->GetNumberCols();
         ext++)  // extend GUID's
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::RPM1; ext < LogbookHTML::MOTOR1;
         ext++)  // extend RPM #1
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::RPM2; ext < LogbookHTML::FUEL;
         ext++)  // extend RPM #2
    {
      temp = dialog->logGrids[2]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    for (int ext = LogbookHTML::WINDR; ext < LogbookHTML::CURRENT;
         ext++)  // extend WINDR
    {
      temp = dialog->logGrids[1]->GetCellValue(r, ext);
      s += dialog->replaceDangerChar(temp);
      s += " \t";
    }

    s.RemoveLast();
    s += "\n";
    stream->WriteString(s);
    s = "";
  }
  output.Close();
}

void Logbook::getModifiedCellValue(int grid, int row, int selCol, int col) {
  wxString s, wind, depth;

  modified = true;

  s = dialog->logGrids[grid]->GetCellValue(row, col);

  if ((grid == 0 && (col == WAKE || col == REMARKS)) ||
      (grid == 1 &&
       (col == LogbookHTML::WEATHER || col == LogbookHTML::CLOUDS ||
        col == LogbookHTML::VISIBILITY)) ||
      (grid == 2 && (col == LogbookHTML::SAILS || col == LogbookHTML::REEF ||
                     col == LogbookHTML::MREMARKS))) {
    return;
  }

  if (grid == 0 && col == ROUTE) {
    if (s.IsEmpty()) return;
    if (s.Last() == '\n') {
      s.RemoveLast();
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 0 && col == RDATE) {
    wxDateTime dt;

    if (!dialog->myParseDate(s, dt)) {
      dt = dt.Now();
      wxMessageBox(
          wxString::Format(_("Please enter the Date in the format:\n      %s"),
                           dt.Format(opt->sdateformat).c_str()),
          _("Information"));
      dialog->logGrids[grid]->SetCellValue(row, col, "");
    } else {
      dialog->logGrids[grid]->SetCellValue(row, col,
                                           dt.Format(opt->sdateformat));

      if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
        dialog->maintenance->checkService(row);
    }
  } else if (grid == 0 && col == RTIME) {
    if (s.IsEmpty()) return;
    wxDateTime dt;
    bool c;
    s.Replace(",", ":");
    s.Replace(".", ":");
    c = dialog->myParseTime(s, dt);

    if (!c) {
      wxMessageBox(
          wxString::Format(_("Please enter the Time in the format:\n   %s"),
                           dt.Format(opt->stimeformat).c_str()));
      dialog->logGrids[grid]->SetCellValue(row, col, "");
    } else {
      dialog->logGrids[grid]->SetCellValue(row, col,
                                           dt.Format(opt->stimeformat));
      if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
        dialog->maintenance->checkService(row);
    }
  } else if (grid == 0 && col == DISTANCE) {
    s.Replace(",", ".");

    s = wxString::Format("%.2f %s", wxAtof(s), opt->showDistance.c_str());

    s.Replace(".", dialog->decimalPoint);
    dialog->logGrids[grid]->SetCellValue(row, col, s);

    computeCell(grid, row, col, s, true);
    if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
      dialog->maintenance->checkService(row);

    s.Replace(",", ".");
    if (wxAtof(s) >= 0.1)
      dialog->m_gridGlobal->SetCellValue(row, STATUS, "S");
    else
      dialog->m_gridGlobal->SetCellValue(row, STATUS, "");
  }

  else if (grid == 0 && col == STATUS) {
    dialog->logGrids[grid]->SetCellValue(row, col, s.Upper());
    if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
      dialog->maintenance->checkService(row);
  } else if (grid == 0 && col == POSITION) {
    if (s != "" && !s.Contains(opt->Deg) && !s.Contains(opt->Min) &&
        !s.Contains(opt->Sec)) {
      if (opt->traditional && s.length() != 22) {
        wxMessageBox(_("Please enter 0544512.15n0301205.15e for\n054Deg 45Min "
                       "12.15Sec N 030Deg 12Min 05.15Sec E"),
                     _("Information"), wxOK);
        s = "";
      } else if (!opt->traditional && s.length() != 22) {
        wxMessageBox(_("Please enter 05445.1234n03012.0504e for\n054Deg "
                       "45.1234Min N 030Deg 12.0504Min E"),
                     _("Information"), wxOK);
        s = "";
      }
      if (s == "") return;
      s.Replace(",", ".");

      if (opt->traditional) {
        wxString temp = s.SubString(0, 2) + opt->Deg + " ";
        temp += s.SubString(3, 4) + opt->Min + " ";
        temp += s.SubString(5, 9) + opt->Sec + " ";
        temp += s.SubString(10, 10).Upper() + "\n";
        temp += s.SubString(11, 13) + opt->Deg + " ";
        temp += s.SubString(14, 15) + opt->Min + " ";
        temp += s.SubString(16, 20) + opt->Sec + " ";
        temp += s.SubString(21, 21).Upper();
        s = temp;
      } else {
        wxString temp = s.SubString(0, 2) + opt->Deg + " ";
        temp += s.SubString(3, 9) + opt->Min + " ";
        temp += s.SubString(10, 10).Upper() + "\n";
        temp += s.SubString(11, 13) + opt->Deg + " ";
        temp += s.SubString(14, 20) + opt->Min + " ";
        temp += s.SubString(21, 22).Upper();
        s = temp;
      }
    }
    s.Replace(".", dialog->decimalPoint);
    dialog->logGrids[grid]->SetCellValue(row, col, s);
    if (row != 0) {
      for (int i = row; i < dialog->logGrids[grid]->GetNumberRows(); i++) {
        double distTotal, dist;
        dialog->logGrids[grid]->SetCellValue(
            i, 5,
            calculateDistance(dialog->logGrids[grid]->GetCellValue(i - 1, col),
                              s));
        wxString temp = dialog->logGrids[grid]->GetCellValue(i - 1, 6);
        temp.Replace(",", ".");
        temp.ToDouble(&distTotal);
        temp = dialog->logGrids[grid]->GetCellValue(i, 5);
        temp.Replace(",", ".");
        temp.ToDouble(&dist);
        s = wxString::Format("%9.2f %s", distTotal + dist,
                             opt->showDistance.c_str());
        s.Replace(".", dialog->decimalPoint);
        dialog->logGrids[grid]->SetCellValue(i, 6, s);

        if (dist >= 0.1)
          dialog->m_gridGlobal->SetCellValue(i, 3, "S");
        else
          dialog->m_gridGlobal->SetCellValue(i, 3, "");

        if (i < dialog->m_gridGlobal->GetNumberRows() - 1) {
          s = dialog->logGrids[grid]->GetCellValue(i + 1, col);
          if (s.IsEmpty() || s == " ") break;
        }
      }
    }
  } else if (grid == 0 && col == COG) {
    if (s != "") {
      s.Replace(",", ".");
      s = wxString::Format("%3.2f%s", wxAtof(s), opt->Deg.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 0 && col == COW) {
    if (s != "") {
      s.Replace(",", ".");
      s = wxString::Format("%3.2f%s %s", wxAtof(s), opt->Deg.c_str(),
                           (opt->showHeading) ? "M" : "T");
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 0 && (col == SOG || col == SOW)) {
    if (s != "") {
      s.Replace(",", ".");
#ifdef __WXOSX__
      s = wxString::Format("%2.2f %s", wxAtof(s),
                           (const wchar_t*)opt->showBoatSpeed.c_str());
#else
      s = wxString::Format("%2.2f %s", wxAtof(s), opt->showBoatSpeed.c_str());
#endif
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 0 && col == DEPTH) {
    if (s != "") {
      switch (opt->showDepth) {
        case 0:
          depth = opt->meter;
          break;
        case 1:
          depth = opt->feet;
          break;
        case 2:
          depth = opt->fathom;
          break;
      }
      if (s.Contains(opt->meter) || s.Contains(opt->feet) ||
          s.Contains(opt->fathom.c_str()) || s.Contains("--")) {
        s.Replace(".", dialog->decimalPoint);
        dialog->logGrids[grid]->SetCellValue(row, col, s);
      } else {
        s.Replace(",", ".");
        s = wxString::Format("%3.1f %s", wxAtof(s), depth.c_str());
        s.Replace(".", dialog->decimalPoint);
        dialog->logGrids[grid]->SetCellValue(row, col, s);
      }
    }
  } else if (grid == 1 && col == LogbookHTML::BARO) {
    if (s != "") {
      s = wxString::Format("%4.1f %s", wxAtof(s), opt->baro.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::HYDRO) {
    if (s != "") {
      s.Replace(",", ".");
      s = wxString::Format("%4.1f%%", wxAtof(s));
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::AIRTE) {
    if (s != "" && !s.Contains(opt->Deg)) {
      s = wxString::Format("%3.0f %s %s", wxAtof(s), opt->Deg.c_str(),
                           opt->temperature.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::WATERTE) {
    if (s != "" && !s.Contains(opt->Deg)) {
      s = wxString::Format("%3.0f %s %s", wxAtof(s), opt->Deg.c_str(),
                           opt->temperature.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::WIND) {
    if (s != "" && !s.Contains(opt->Deg)) {
      s = wxString::Format("%3.0f%s %s", wxAtof(s), opt->Deg.c_str(),
                           opt->showWindDir ? "R" : "T");
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::WSPD) {
    if (s != "") {
      if (!opt->windspeeds) {
        if (!s.Contains(opt->showWindSpeed)) {
          s.Replace(",", ".");
          s = wxString::Format("%3.2f %s", wxAtof(s),
                               opt->showWindSpeed.c_str());
          s.Replace(".", dialog->decimalPoint);
        }
        dialog->logGrids[grid]->SetCellValue(row, col, s);
      }
    }
  } else if (grid == 1 && col == LogbookHTML::CURRENT) {
    if (s != "" && !s.Contains(opt->Deg)) {
      s.Replace(",", ".");
      s = wxString::Format("%3.0f%s", wxAtof(s), opt->Deg.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 && col == LogbookHTML::CSPD) {
    if (s != "") {
      s.Replace(",", ".");
      s = wxString::Format("%3.2f %s", wxAtof(s), opt->showBoatSpeed.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 1 &&
             (col == LogbookHTML::WAVE || col == LogbookHTML::SWELL)) {
    wxString d;
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
    if (s != "") {
      s.Replace(",", ".");
      s = wxString::Format("%3.2f %s", wxAtof(s), d.c_str());
      s.Replace(".", dialog->decimalPoint);
      dialog->logGrids[grid]->SetCellValue(row, col, s);
    }
  } else if (grid == 2 &&
             ((col == LogbookHTML::MOTORT || col == LogbookHTML::MOTOR1T ||
               col == LogbookHTML::GENET || col == LogbookHTML::WATERMT) &&
              !s.IsEmpty())) {
    wxString pre, cur;
    double hp, hc, mp, mc;
    double res, hp_, hc_;

    if (!s.Contains(":") && !s.Contains(",") && !s.Contains(".")) s += ":";

    if (s.Contains(",") || s.Contains(".")) {
      double d;
      s.Replace(",", ".");
      s.ToDouble(&d);
      int h = (int)d;
      int m = (60 * (d - h));
      s = wxString::Format("%i:%i", h, m);
    }

    if (row > 0) {
      pre = dialog->m_gridMotorSails->GetCellValue(row - 1, col);
      wxStringTokenizer tkz(pre, ":");
      tkz.GetNextToken().ToDouble(&hp);
      tkz.GetNextToken().ToDouble(&mp);

    } else {
      hp = 0;
      mp = 0;
    }

    cur = s;
    wxStringTokenizer tkz1(cur, ":");
    tkz1.GetNextToken().ToDouble(&hc);
    tkz1.GetNextToken().ToDouble(&mc);

    hc_ = hc + ((mc * (100.0 / 60.0)) / 100);
    hp_ = hp + ((mp * (100.0 / 60.0)) / 100);

    res = hc_ - hp_;

    if (row == 0 || res <= 0.0)
      dialog->m_gridMotorSails->SetCellValue(
          row, col - 1, wxString::Format("00:00 %s", opt->motorh.c_str()));
    else
      dialog->m_gridMotorSails->SetCellValue(row, col - 1,
                                             decimalToHours(res, false));

    dialog->m_gridMotorSails->SetCellValue(row, col, decimalToHours(hc_, true));

    if (row < dialog->m_gridMotorSails->GetNumberRows() - 1)
      computeCell(grid, row + 1, col - 1,
                  dialog->m_gridMotorSails->GetCellValue(row + 1, col - 1),
                  true);
    if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
      dialog->maintenance->checkService(row);
  } else if (grid == 2 &&
             ((col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
               col == LogbookHTML::GENE || col == LogbookHTML::WATERM) &&
              !s.IsEmpty())) {
    double watermaker;
    opt->watermaker.ToDouble(&watermaker);

    bool t = false;
    wxString sep;

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

    if (true != t) s.Append(":0");

    wxStringTokenizer tkz(s, sep);
    wxString h = tkz.GetNextToken();
    wxString m = wxEmptyString;
    if (tkz.HasMoreTokens()) m = tkz.GetNextToken();

    if (wxAtoi(m) > 59) {
      wxMessageBox(_("Minutes greater than 59"), "");
      dialog->logGrids[grid]->SetCellValue(row, col, "00:00");
      return;
    } else {
      s = wxString::Format("%s:%s", h.c_str(), m.c_str());
      s = wxString::Format("%s %s", s.c_str(), opt->motorh.c_str());
      dialog->logGrids[grid]->SetCellValue(row, col, s);
      computeCell(grid, row, col, s, true);
      if (row == dialog->m_gridGlobal->GetNumberRows() - 1)
        dialog->maintenance->checkService(row);
      if (col == LogbookHTML::WATERM) {
        wxString t =
            dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::WATERM);
        wxStringTokenizer tkz(t, ":");
        double h, m;
        tkz.GetNextToken().ToDouble(&h);
        tkz.GetNextToken().ToDouble(&m);
        h = h + (m * (100 / 60) / 100);
        double output = watermaker * h;
        dialog->m_gridMotorSails->SetCellValue(
            row, LogbookHTML::WATERMO,
            wxString::Format("+%2.2f %s", output, opt->vol.c_str()));
        computeCell(
            grid, row, LogbookHTML::WATERMO,
            dialog->m_gridMotorSails->GetCellValue(row, LogbookHTML::WATERMO),
            false);
      }
    }
  }

  else if (grid == 2 &&
           (col == LogbookHTML::FUELT || col == LogbookHTML::WATERT ||
            col == LogbookHTML::BANK1T || col == LogbookHTML::BANK2T) &&
           !s.IsEmpty()) {
    double div = 1.0;
    long capacity;
    wxString ap;
    double t, c;
    wxString ind;

    if (col == LogbookHTML::BANK1T || col == LogbookHTML::BANK2T)
      ap = opt->ampereh;
    else
      ap = opt->vol;

    if (row > 0)
      dialog->m_gridMotorSails->GetCellValue(row - 1, col).ToDouble(&t);
    else {
      t = 0;
      c = 0;
    }
    dialog->m_gridMotorSails->GetCellValue(row, col).ToDouble(&c);

    if (s.Contains("/")) {
      double a, b;
      wxStringTokenizer tkz(s, "/");
      tkz.GetNextToken().ToDouble(&a);
      tkz.GetNextToken().ToDouble(&b);
      div = a / b;
      if (col == LogbookHTML::FUELT)
        opt->fuelTank.ToLong(&capacity);
      else if (col == LogbookHTML::BANK1T)
        opt->bank1.ToLong(&capacity);
      else if (col == LogbookHTML::BANK2T)
        opt->bank2.ToLong(&capacity);
      else
        opt->waterTank.ToLong(&capacity);

      c = capacity * div;
    }

    s.Replace(",", ".");
    ind = (c < t) ? "-" : "+";

    if (row != 0)
      dialog->m_gridMotorSails->SetCellValue(
          row, col - 1,
          wxString::Format("%s%.2f %s", ind.c_str(), fabs(t - c), ap.c_str()));
    else
      dialog->m_gridMotorSails->SetCellValue(
          row, col - 1, wxString::Format("%s0.00 %s", ind.c_str(), ap.c_str()));

    dialog->m_gridMotorSails->SetCellValue(
        row, col, wxString::Format("%.2f %s", c, ap.c_str()));

    int x;
    if (col == LogbookHTML::WATERT)
      x = 2;
    else
      x = 1;
    if (row < dialog->m_gridMotorSails->GetNumberRows() - 1)
      computeCell(grid, row + 1, col - x,
                  dialog->m_gridMotorSails->GetCellValue(row + 1, col - x),
                  false);

    dialog->maintenance->checkService(row);
  } else if (grid == 2 &&
             (col == LogbookHTML::FUEL || col == LogbookHTML::WATER ||
              col == LogbookHTML::WATERMO || col == LogbookHTML::BANK1 ||
              col == LogbookHTML::BANK2) &&
             !s.IsEmpty()) {
    wxChar ch;
    wxString ap;

    if (col == LogbookHTML::BANK1 || col == LogbookHTML::BANK2)
      ap = opt->ampereh;
    else
      ap = opt->vol;

    s.Replace(",", ".");
    if (col != LogbookHTML::WATERMO)
      ch = s.GetChar(0);
    else
      ch = '+';

    s = wxString::Format("%.2f %s", wxAtof(s), ap.c_str());
    s.Replace(".", dialog->decimalPoint);

    if (ch != '-' && ch != '+')
      dialog->logGrids[grid]->SetCellValue(row, col, "-" + s);
    else {
      if (ch == '+')
        dialog->logGrids[grid]->SetCellValue(row, col, wxString(ch) + s);
      else
        dialog->logGrids[grid]->SetCellValue(row, col, s);
    }

    computeCell(grid, row, col, s, false);
    dialog->maintenance->checkService(row);
  }
  return;
}

void Logbook::deleteRows() {
  wxArrayInt rows;
  unsigned int rowsCount;
  int tab = dialog->m_notebook8->GetSelection();

  rows = dialog->logGrids[tab]->GetSelectedRows();
  rowsCount = rows.GetCount();

  if (rowsCount == 0)  // complete grid
  {
    wxGridCellCoordsArray art =
        dialog->logGrids[tab]->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray arb =
        dialog->logGrids[tab]->GetSelectionBlockBottomRight();
    int start = art[0].GetRow();
    int end = arb[0].GetRow();
    for (int grid = 0; grid < LOGGRIDS; grid++) {
      dialog->logGrids[grid]->DeleteRows(start, (end - start) + 1);
      dialog->logGrids[grid]->ForceRefresh();
    }

    if (start == dialog->m_gridGlobal->GetNumberRows() - 1) start--;

    if (dialog->logGrids[tab]->GetNumberRows() != 0) {
      dialog->selGridRow = start;
      dialog->logGrids[tab]->SetGridCursor(start, 0);
      recalculateLogbook(start);
    } else {
      dialog->selGridRow = 0;
    }
    modified = true;
    return;
  }

  bool sort = true;
  if (rowsCount > 1) {
    while (sort) {
      sort = false;
      for (unsigned int i = 0; i < rowsCount - 1; i++) {
        if (rows[i + 1] > rows[i]) {
          sort = true;
          int temp = rows[i];
          rows[i] = rows[i + 1];
          rows[i + 1] = temp;
        }
      }
    }
  }

  for (int grid = 0; grid < LOGGRIDS; grid++) {
    for (unsigned int i = 0; i < rowsCount; i++)
      dialog->logGrids[grid]->DeleteRows(rows[i]);
  }
  dialog->selGridRow = rows[rowsCount - 1] - 1;
  if (dialog->logGrids[tab]->GetNumberRows() > 0)
    dialog->logGrids[tab]->SetGridCursor(rows[rowsCount - 1] - 1, 0);

  modified = true;
  if (dialog->logGrids[0]->GetNumberRows() > 0)
    recalculateLogbook(rows[rows.GetCount() - 1] - 1);
}

wxString Logbook::decimalToHours(double res, bool b) {
  int h = (int)res;
  double m = res - h;
  m = m * (60.0 / 100.0) * 100;

  wxString fmt = (b) ? "%05i:%02.0f %s" : "%02i:%02.0f %s";
  wxString str = wxString::Format(fmt, h, m, opt->motorh.c_str());
  return str;
}

wxString Logbook::computeCell(int grid, int row, int col, wxString s,
                              bool mode) {
  double current = 0, last = 0.0;
  long hourCur = 0, minCur = 0, hourLast = 0, minLast = 0;
  int count;
  wxString cur;
  wxString abrev;

  s.Replace(",", ".");

  if (col == DISTANCE)
    abrev = opt->showDistance;
  else if (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
           col == LogbookHTML::GENE || col == LogbookHTML::WATERM)
    abrev = opt->motorh;
  else if (col == LogbookHTML::FUEL || col == LogbookHTML::WATER ||
           col == LogbookHTML::WATERMO)
    abrev = opt->vol;
  else if (col == LogbookHTML::BANK1 || col == LogbookHTML::BANK2)
    abrev = opt->ampereh;

  count = dialog->m_gridGlobal->GetNumberRows();

  for (int i = row; i < count; i++) {
    if (col != LogbookHTML::WATERMO && col != LogbookHTML::WATER &&
        col != LogbookHTML::FUEL && col != LogbookHTML::BANK1 &&
        col != LogbookHTML::BANK2) {
      s = dialog->logGrids[grid]->GetCellValue(i, col);
      s.Replace(",", ".");
      if (s == "0000") s = "00:00";
      if (grid == 2 &&
          (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
           col == LogbookHTML::GENE || col == LogbookHTML::WATERM)) {
        wxArrayString time = wxStringTokenize(s, ":");
        time[0].ToLong(&hourCur);
        time[1].ToLong(&minCur);
      } else {
        s.ToDouble(&current);
      }
    } else {
      double t, t1 = 0.0, t2 = 0.0;

      s = dialog->logGrids[grid]->GetCellValue(i, col);
      s.Replace(",", ".");
      s.ToDouble(&t);

      if (col == LogbookHTML::WATERMO) {
        s = dialog->logGrids[grid]->GetCellValue(i, LogbookHTML::WATER);
        s.Replace(",", ".");
        s.ToDouble(&t1);

        if (i == 0) {
          s = dialog->logGrids[grid]->GetCellValue(i, LogbookHTML::WATERT);
          s.Replace(",", ".");
          s.ToDouble(&t2);
        }
        current = t + t1 + t2;
      } else if (col == LogbookHTML::WATER) {
        s = dialog->logGrids[grid]->GetCellValue(i, LogbookHTML::WATERMO);
        s.Replace(",", ".");
        s.ToDouble(&t1);

        if (i == 0) {
          s = dialog->logGrids[grid]->GetCellValue(i, LogbookHTML::WATERT);
          s.Replace(",", ".");
          s.ToDouble(&t2);

          current = t + t2;
        } else {
          current = t + t1 + t2;
        }

      } else {
        current = t + t1;
      }
    }

    if (i > 0) {
      wxString temp;
      if (col != LogbookHTML::WATERMO)
        temp = dialog->logGrids[grid]->GetCellValue(i - 1, col + 1);
      else
        temp = dialog->logGrids[grid]->GetCellValue(i - 1, col + 2);

      temp.Replace(",", ".");
      if (grid == 2 &&
          (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
           col == LogbookHTML::GENE || col == LogbookHTML::WATERM)) {
        if (temp.Contains(":")) {
          wxArrayString time = wxStringTokenize(temp, ":");
          time[0].ToLong(&hourLast);
          time[1].ToLong(&minLast);
        } else {
          hourLast = 0;
          minLast = 0;
        }
      } else
        temp.ToDouble(&last);
    } else {
      last = 0.0f;
      hourLast = 0;
      minLast = 0;
    }

    if (grid == 2 && (col == LogbookHTML::MOTOR || col == LogbookHTML::MOTOR1 ||
                      col == LogbookHTML::GENE || col == LogbookHTML::WATERM)) {
      hourLast += hourCur;
      minLast += minCur;
      if (minLast >= 60) {
        hourLast++;
        minLast -= 60;
      }
#ifdef __WXOSX__
      s = wxString::Format("%05ld:%02ld %s", (wchar_t)hourLast,
                           (wchar_t)minLast, abrev.c_str());
#else
      s = wxString::Format("%05ld:%02ld %s", hourLast, minLast, abrev.c_str());
#endif
      dialog->logGrids[grid]->SetCellValue(i, col + 1, s);
#ifdef __WXOSX__
      cur = wxString::Format("%02ld:%02ld %s", (wchar_t)hourCur,
                             (wchar_t)minCur, abrev.c_str());
#else
      cur = wxString::Format("%02ld:%02ld %s", hourCur, minCur, abrev.c_str());
#endif
      dialog->logGrids[grid]->SetCellValue(i, col, cur);
    } else {
#ifdef __WXOSX__
      s = wxString::Format("%10.2f %s", last + current, abrev.c_str());
#else
      s = wxString::Format("%10.2f %s", last + current, abrev.c_str());
#endif
      s.Replace(".", dialog->decimalPoint);
      if (col != LogbookHTML::WATERMO)
        dialog->logGrids[grid]->SetCellValue(i, col + 1, s);
      else
        dialog->logGrids[grid]->SetCellValue(i, col + 2, s);
    }
  }
  return cur;
}

wxString Logbook::toSDMM(int NEflag, double a, bool mode) {
  short neg = 0;
  int d;
  long m;
  wxDouble sec;

  if (a < 0.0) {
    a = -a;
    neg = 1;
  }
  d = (int)a;
  m = (long)((a - (double)d) * 60000.0);
  double z = (m % 1000);
  sec = 60 * (z / 1000);

  if (neg) d = -d;

  wxString s;

  if (!NEflag)
    s.Printf(_T ( "%d%02ld%02ld" ), d, m / 1000, m % 1000);
  else {
    if (NEflag == 1) {
      char c = 'N';

      if (neg) {
        d = -d;
        c = 'S';
      }
      newPosition.posLat = a;
      newPosition.latitude = d;
      newPosition.latmin = m / 1000.0;
      newPosition.WEflag = c;

      s.Printf("%03d%02ld%05.2f%c", d, m / 1000, sec, c);
    } else if (NEflag == 2) {
      char c = 'E';

      if (neg) {
        d = -d;
        c = 'W';
      }
      newPosition.posLon = a;
      newPosition.longitude = d;
      newPosition.lonmin = m / 1000.0;
      newPosition.NSflag = c;
      s.Printf("%03d%02ld%05.2f%c", d, m / 1000, sec, c);
    }
  }
  return s;
}

wxString Logbook::toSDMMOpenCPN(int NEflag, double a, bool hi_precision) {
  wxString s;
  double mpy;
  short neg = 0;
  int d;
  long m;
  double ang = a;
  char c = 'N';
  int g_iSDMMFormat = 0;

  if (a < 0.0) {
    a = -a;
    neg = 1;
  }
  d = (int)a;
  if (neg) d = -d;
  if (NEflag) {
    if (NEflag == 1) {
      c = 'N';

      if (neg) {
        d = -d;
        c = 'S';
      }
    } else if (NEflag == 2) {
      c = 'E';

      if (neg) {
        d = -d;
        c = 'W';
      }
    }
  }

  switch (g_iSDMMFormat) {
    case 0:
      mpy = 600.0;
      if (hi_precision) mpy = mpy * 1000;

      m = (long)wxRound((a - (double)d) * mpy);

      if (!NEflag || NEflag < 1 || NEflag > 2)  // Does it EVER happen?
      {
        if (hi_precision)
          s.Printf(_T ( "%d %02ld.%04ld'" ), d, m / 10000, m % 10000);
        else
          s.Printf(_T ( "%d %02ld.%01ld'" ), d, m / 10, m % 10);
      } else {
        if (NEflag == 1) {
          newPosition.posLat = a;
          newPosition.latitude = d;
          newPosition.latmin = m / 1000.0;
          newPosition.WEflag = c;
        } else {
          newPosition.posLon = a;
          newPosition.longitude = d;
          newPosition.lonmin = m / 1000.0;
          newPosition.NSflag = c;
        }
        if (hi_precision)
          s.Printf(_T ( "%03d%02ld.%04ld%c" ), d, m / 10000, (m % 10000), c);
        else
          s.Printf(_T ( "%03d%02ld.%01ld%c" ), d, m / 10, (m % 10), c);
      }
      break;
    case 1:
      if (hi_precision)
        s.Printf(_T ( "%03.6f" ),
                 ang);  // cca 11 cm - the GPX precision is higher, but as we
                        // use hi_precision almost everywhere it would be a
                        // little too much....
      else
        s.Printf(_T ( "%03.4f" ), ang);  // cca 11m
      break;
    case 2:
      m = (long)((a - (double)d) * 60);
      mpy = 10.0;
      if (hi_precision) mpy = mpy * 100;
      long sec = (long)((a - (double)d - (((double)m) / 60)) * 3600 * mpy);

      if (!NEflag || NEflag < 1 || NEflag > 2)  // Does it EVER happen?
      {
        if (hi_precision)
          s.Printf(_T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 1000, sec % 1000);
        else
          s.Printf(_T ( "%d %ld'%ld.%ld\"" ), d, m, sec / 10, sec % 10);
      } else {
        if (hi_precision)
          s.Printf(_T ( "%03d %02ld %02ld.%03ld %c" ), d, m, sec / 1000,
                   sec % 1000, c);
        else
          s.Printf(_T ( "%03d %02ld %02ld.%ld %c" ), d, m, sec / 10, sec % 10,
                   c);
      }
      break;
  }
  return s;
}

bool Logbook::checkGPS(bool autoLine) {
  sLogText = "";

  if (gpsStatus) {
    if (opt->showWindHeading == 1 && !bCOW) {
      sLogText =
          _("Wind is set to Heading,\nbut GPS sends no Heading Data.\nWind is "
            "set now to Relative to boat\n\n");
      opt->showWindHeading = 0;
    }
    if (courseChange && autoLine)
      sLogText += opt->courseChangeText + opt->courseChangeDegrees + opt->Deg;
    else if (guardChange)
      sLogText += opt->guardChangeText;
    else if (waypointArrived) {
      wxString s, ext;

      if (!OCPN_Message) {
        /* s = wxString::Format(_("\nName of Waypoint: %s\nTrue bearing to
        destination: %4.1f%s\nRange to destination: %4.2f%s"),
        tempRMB.From.c_str(),
        tempRMB.BearingToDestinationDegreesTrue,opt->Deg.c_str(),
        tempRMB.RangeToDestinationNauticalMiles,opt->distance.c_str());
        s.Replace(".",dialog->decimalPoint);*/
      } else {
        setWayPointArrivedText();
      }

    } else if (everySM && autoLine)
      sLogText += opt->everySMText + opt->everySMAmount + opt->showDistance;
    else if ((dialog->timer->IsRunning() || opt->timerType != 0) && autoLine)
      sLogText += opt->ttext;

    return true;
  } else {
    //	sLat = sLon = sDate = sTime = "";
    //		sCOG = sCOW = sSOG = sSOW = sDepth = sWind = sWindSpeed =
    //sTemperatureWater = sTemperatureAir = sPressure = sHumidity = "";
    sCOG = sSOG = "";
    bCOW = false;

    if (opt->noGPS)
      sLogText = _("No GPS-Signal !");
    else
      sLogText = "";
    if (waypointArrived) {
      setWayPointArrivedText();
    }
    return false;
  }
}

void Logbook::setWayPointArrivedText() {
  wxString ext;
  wxString msg;

  if (tempRMB.To != "-1") {
    msg = _("Next WP Name: ");
  } else {
    msg = _("Last waypoint of the Route");
    tempRMB.To = wxEmptyString;
  }

  wxString s =
      wxString::Format(_("\nName of Waypoint: %s\n%s %s"), tempRMB.From.c_str(),
                       msg.c_str(), tempRMB.To.c_str());

  if (WP_skipped)
    ext = _("Waypoint skipped");
  else
    ext = _("WayPoint arrived");

  if (sLogText != "")
    sLogText += wxString::Format("\n%s\n%s%s", opt->waypointText.c_str(),
                                 ext.c_str(), s.c_str());
  else
    sLogText += wxString::Format("%s\n%s%s", opt->waypointText.c_str(),
                                 ext.c_str(), s.c_str());
}

class ActualWatch;
void Logbook::SetGPSStatus(bool status) {
  if (!status) sDate = "";

  if (status != gpsStatus) dialog->crewList->dayNow(false);

  gpsStatus = status;
}

void Logbook::showSearchDlg(int row, int col) {
  LogbookSearch* dlg = new LogbookSearch(dialog, row, col);
  dlg->Show(true);
}

////////////////////////////////////////////////////
NoAppendDialog::NoAppendDialog(wxWindow* parent, wxWindowID id,
                               const wxString& title, const wxPoint& pos,
                               const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer20;
  bSizer20 = new wxBoxSizer(wxVERTICAL);

  m_staticText73 = new wxStaticText(
      this, wxID_ANY, _("It's not allowed to append Data to a old Logbook\n\n\
														 OpenCPN switchs to actual logbook"),
      wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticText73->Wrap(-1);
  bSizer20->Add(m_staticText73, 0, wxALL | wxEXPAND, 5);

  m_sdbSizer5 = new wxStdDialogButtonSizer();
  m_sdbSizer5OK = new wxButton(this, wxID_OK);
  m_sdbSizer5->AddButton(m_sdbSizer5OK);
  m_sdbSizer5->Realize();
  bSizer20->Add(m_sdbSizer5, 0, wxALIGN_CENTER, 5);

  this->SetSizer(bSizer20);
  this->Layout();

  this->Centre(wxBOTH);
}

NoAppendDialog::~NoAppendDialog() {}

////////////////////////////
// PVBE-DIALOG
////////////////////////////
PBVEDialog::PBVEDialog(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style) {
  dialog = (LogbookDialog*)parent;
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer21;
  bSizer21 = new wxBoxSizer(wxVERTICAL);

  m_textCtrlPVBE =
      new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxDefaultSize, wxTE_MULTILINE);
  bSizer21->Add(m_textCtrlPVBE, 1, wxALL | wxEXPAND, 5);

  this->SetSizer(bSizer21);
  this->Layout();

  this->Centre(wxBOTH);
  m_textCtrlPVBE->AppendText(
      "this is a test if you have received PBVE-Sentences\nthey are "
      "manufacturer-specific\nit's use is for engine-hours and "
      "fuel-consumption\n");
}

PBVEDialog::~PBVEDialog() { dialog->logbook->pvbe = NULL; }

void PBVEDialog::PBVEDialogOnClose(wxCloseEvent& event) {
  dialog->logbook->pvbe = NULL;
}

void PBVEDialog::OnCloseWindow(wxCloseEvent& ev) {
  dialog->logbook->pvbe = NULL;
}

/////////////////////// LogbookSearchDlg
///////////////////////////////////////////////////////

LogbookSearch::LogbookSearch(wxWindow* parent, int row, int col, wxWindowID id,
                             const wxString& title, const wxPoint& pos,
                             const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->parent = (LogbookDialog*)parent;
  this->row = row;
  this->col = col;

  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer23;
  bSizer23 = new wxBoxSizer(wxVERTICAL);

  wxFlexGridSizer* fgSizer41;
  fgSizer41 = new wxFlexGridSizer(0, 3, 0, 0);
  fgSizer41->SetFlexibleDirection(wxBOTH);
  fgSizer41->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText96 = new wxStaticText(this, wxID_ANY, _("Search in"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText96->Wrap(-1);
  fgSizer41->Add(m_staticText96, 0, wxALL, 5);

  m_radioBtnActual = new wxRadioButton(this, wxID_ANY, _("Actual Logbook"),
                                       wxDefaultPosition, wxDefaultSize, 0);
  fgSizer41->Add(m_radioBtnActual, 0, wxALL, 5);

  m_radioBtnAll = new wxRadioButton(this, wxID_ANY, _("All Logbooks"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  fgSizer41->Add(m_radioBtnAll, 0, wxALL, 5);

  bSizer23->Add(fgSizer41, 0, wxALIGN_CENTER, 5);

  m_staticline32 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  bSizer23->Add(m_staticline32, 0, wxEXPAND | wxALL, 5);

  wxFlexGridSizer* fgSizer411;
  fgSizer411 = new wxFlexGridSizer(0, 2, 0, 0);
  fgSizer411->SetFlexibleDirection(wxBOTH);
  fgSizer411->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText108 = new wxStaticText(this, wxID_ANY, _("Searchstring"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText108->Wrap(-1);
  fgSizer411->Add(m_staticText108, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrl72 =
      new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxSize(250, -1), wxTE_LEFT | wxTE_MULTILINE);
  fgSizer411->Add(m_textCtrl72, 0, wxALL, 5);

  m_staticText110 = new wxStaticText(this, wxID_ANY, _("In Column"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText110->Wrap(-1);
  fgSizer411->Add(m_staticText110, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxArrayString m_choice23Choices;
  m_choice23 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(250, -1),
                            m_choice23Choices, 0);
  m_choice23->SetSelection(0);
  fgSizer411->Add(m_choice23, 0, wxALL, 5);

  m_staticText97 = new wxStaticText(this, wxID_ANY, _("Date"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText97->Wrap(-1);
  fgSizer411->Add(m_staticText97, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxFlexGridSizer* fgSizer42;
  fgSizer42 = new wxFlexGridSizer(0, 3, 0, 0);
  fgSizer42->SetFlexibleDirection(wxBOTH);
  fgSizer42->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxString m_choiceGreaterEqualChoices[] = {">=", "<="};
  int m_choiceGreaterEqualNChoices =
      sizeof(m_choiceGreaterEqualChoices) / sizeof(wxString);
  m_choiceGreaterEqual = new wxChoice(
      this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      m_choiceGreaterEqualNChoices, m_choiceGreaterEqualChoices, 0);
  m_choiceGreaterEqual->SetSelection(0);
  fgSizer42->Add(m_choiceGreaterEqual, 0, wxALL, 5);

  m_datePicker =
      new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition,
                           wxDefaultSize, wxDP_DEFAULT);
  fgSizer42->Add(m_datePicker, 0, wxALL, 5);

  m_buttonSelectDate = new wxButton(this, wxID_ANY, _("Select"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  fgSizer42->Add(m_buttonSelectDate, 0, wxALL, 5);

  fgSizer411->Add(fgSizer42, 1, wxEXPAND, 5);

  bSizer23->Add(fgSizer411, 0, wxEXPAND, 5);

  m_staticline39 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  bSizer23->Add(m_staticline39, 0, wxEXPAND | wxALL, 5);

  wxFlexGridSizer* fgSizer43;
  fgSizer43 = new wxFlexGridSizer(0, 2, 0, 0);
  fgSizer43->SetFlexibleDirection(wxBOTH);
  fgSizer43->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_buttonBack =
      new wxButton(this, wxID_ANY, "<<", wxDefaultPosition, wxDefaultSize, 0);
  fgSizer43->Add(m_buttonBack, 0, wxALL, 5);

  m_buttonForward =
      new wxButton(this, wxID_ANY, ">>", wxDefaultPosition, wxDefaultSize, 0);
  fgSizer43->Add(m_buttonForward, 0, wxALIGN_CENTER | wxALL, 5);

  bSizer23->Add(fgSizer43, 0, wxALIGN_CENTER, 0);

  this->SetSizer(bSizer23);
  this->Layout();

  this->Centre(wxBOTH);

  // Connect Events
  this->Connect(wxEVT_INIT_DIALOG,
                wxInitDialogEventHandler(LogbookSearch::OnInitDialog));
  m_buttonBack->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                        wxCommandEventHandler(LogbookSearch::OnButtonClickBack),
                        NULL, this);
  m_buttonForward->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookSearch::OnButtonClickForward), NULL, this);
  m_buttonSelectDate->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookSearch::OnButtonClickSelectDate), NULL,
      this);
}

LogbookSearch::~LogbookSearch() {
  // Disconnect Events
  this->Disconnect(wxEVT_INIT_DIALOG,
                   wxInitDialogEventHandler(LogbookSearch::OnInitDialog));
  m_buttonBack->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookSearch::OnButtonClickBack), NULL, this);
  m_buttonForward->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookSearch::OnButtonClickForward), NULL, this);
  m_buttonSelectDate->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookSearch::OnButtonClickSelectDate), NULL,
      this);
}

void LogbookSearch::OnInitDialog(wxInitDialogEvent& event) {
  wxDateTime dt;
  searchrow = 0;

  parent->myParseDate(parent->m_gridGlobal->GetCellValue(0, 1), dt);
  m_datePicker->SetValue(dt);

  int gridNo = parent->m_notebook8->GetSelection();
  for (int i = 0; i < parent->logGrids[gridNo]->GetNumberCols(); i++)
    this->m_choice23->Append(parent->logGrids[gridNo]->GetColLabelValue(i));

  m_choice23->SetSelection(col);
  m_textCtrl72->SetFocus();

  m_radioBtnAll->Hide();
  Fit();
}

void LogbookSearch::OnButtonClickSelectDate(wxCommandEvent& event) {
  DateDialog dateDlg(this);
  if (dateDlg.ShowModal() == wxID_OK)
    this->m_datePicker->SetValue(dateDlg.m_calendar2->GetDate());
}

void LogbookSearch::OnButtonClickForward(wxCommandEvent& event) {
  int gridNo = parent->m_notebook8->GetSelection();
  int col = this->m_choice23->GetSelection();
  wxString ss = this->m_textCtrl72->GetValue().Lower();
  wxDateTime dt, dlgDt;

#ifdef __OCPN__ANDROID__
  dlgDt = m_datePicker->GetDateCtrlValue();
#else
  dlgDt = m_datePicker->GetValue();
#endif
  if (searchrow < 0) searchrow = 0;
  if (!direction) searchrow++;
  direction = true;

  for (; searchrow < parent->logGrids[gridNo]->GetNumberRows(); searchrow++) {
    parent->myParseDate(
        parent->logGrids[0]->GetCellValue(searchrow, LogbookHTML::RDATE), dt);

    if (m_choiceGreaterEqual->GetSelection() == 0) {
      if (dt < dlgDt) continue;
    } else {
      if (dt > dlgDt) continue;
    }

    if (parent->logGrids[gridNo]
            ->GetCellValue(searchrow, col)
            .Lower()
            .Contains(ss)) {
      parent->logGrids[gridNo]->SetFocus();
      parent->logGrids[gridNo]->SetGridCursor(searchrow, col);
      searchrow++;
      break;
    }
  }
}

void LogbookSearch::OnButtonClickBack(wxCommandEvent& event) {
  int gridNo = parent->m_notebook8->GetSelection();
  int col = this->m_choice23->GetSelection();
  wxString ss = this->m_textCtrl72->GetValue().Lower();
  wxDateTime dt, dlgDt;

  if (direction) searchrow--;
  direction = false;

#ifdef __OCPN__ANDROID__
  dlgDt = m_datePicker->GetDateCtrlValue();
#else
  dlgDt = m_datePicker->GetValue();
#endif
  if (searchrow > parent->logGrids[gridNo]->GetNumberRows() - 1) searchrow--;

  for (; searchrow >= 0; searchrow--) {
    parent->myParseDate(
        parent->logGrids[0]->GetCellValue(searchrow, LogbookHTML::RDATE), dt);
    if (m_choiceGreaterEqual->GetSelection() == 0) {
      if (m_choiceGreaterEqual->GetSelection() == 0) {
        if (dt < dlgDt) continue;
      } else {
        if (dt > dlgDt) continue;
      }
    }

    if (parent->logGrids[gridNo]
            ->GetCellValue(searchrow, col)
            .Lower()
            .Contains(ss)) {
      parent->logGrids[gridNo]->SetFocus();
      parent->logGrids[gridNo]->SetGridCursor(searchrow, col);
      searchrow--;
      break;
    }
  }
}

////////////////////  Reminder Dlg //////////////////////
LinesReminderDlg::LinesReminderDlg(wxString str, wxWindow* parent,
                                   wxWindowID id, const wxString& title,
                                   const wxPoint& pos, const wxSize& size,
                                   long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer38;
  bSizer38 = new wxBoxSizer(wxVERTICAL);

  m_staticTextreminder = new wxStaticText(
      this, wxID_ANY, str, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticTextreminder->Wrap(-1);
  bSizer38->Add(m_staticTextreminder, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_sdbSizer9 = new wxStdDialogButtonSizer();
  m_sdbSizer9OK = new wxButton(this, wxID_OK);
  m_sdbSizer9->AddButton(m_sdbSizer9OK);
  m_sdbSizer9->Realize();
  bSizer38->Add(m_sdbSizer9, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  this->SetSizer(bSizer38);
  this->Layout();

  this->Fit();
  this->Centre(wxBOTH);
}

LinesReminderDlg::~LinesReminderDlg() {}
