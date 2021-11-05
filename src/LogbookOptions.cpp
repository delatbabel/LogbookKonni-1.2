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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LogbookOptions.h"

#include <memory>

#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/tooltip.h>


#include "Logbook.h"
#include "LogbookDialog.h"
#include "Options.h"
#include "folder.xpm"
#include "logbook_pi.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
LogbookOptions::LogbookOptions(wxWindow* parent, Options* opt,
                               logbookkonni_pi* log_pi, wxWindowID id,
                               const wxString& title, const wxPoint& pos,
                               const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->opt = opt;
  this->log_pi = log_pi;

  // for small screens: don't let boat dialog be larger than screen
  int w, h;
  wxDisplaySize(&w, &h);
  w = wxMin(w, GetMinWidth());
  h = wxMin(h - 32, GetMinHeight());
  SetMinSize(wxSize(w / 2, h / 2));  // allow to half normal dimensions
  SetSize(wxSize(w, h));

  //	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* bSizer21;
  bSizer21 = new wxBoxSizer(wxVERTICAL);

  m_scrolledWindow1 = new wxScrolledWindow(
      this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
  m_scrolledWindow1->SetScrollRate(5, 5);
  wxBoxSizer* bSizer29;
  bSizer29 = new wxBoxSizer(wxVERTICAL);

  m_notebook4 = new wxNotebook(m_scrolledWindow1, wxID_ANY, wxDefaultPosition,
                               wxDefaultSize, 0);
  m_notebook4->SetBackgroundColour(
      wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  m_panel15 = new wxPanel(m_notebook4, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer51;
  bSizer51 = new wxBoxSizer(wxVERTICAL);

  wxFlexGridSizer* fgSizer10;
  fgSizer10 = new wxFlexGridSizer(12, 1, 0, 0);
  fgSizer10->SetFlexibleDirection(wxBOTH);
  fgSizer10->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer* fgSizer51;
  fgSizer51 = new wxFlexGridSizer(2, 6, 0, 0);
  fgSizer51->SetFlexibleDirection(wxBOTH);
  fgSizer51->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer51->Add(10, 0, 1, wxEXPAND, 0);

  m_staticText132 = new wxStaticText(m_panel15, wxID_ANY, _("Date-Format:"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText132->Wrap(-1);
  fgSizer51->Add(m_staticText132, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceDateFormatChoices[] = {_("Local Format"), _("Individual")};
  int m_choiceDateFormatNChoices =
      sizeof(m_choiceDateFormatChoices) / sizeof(wxString);
  m_choiceDateFormat =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceDateFormatNChoices, m_choiceDateFormatChoices, 0);
  m_choiceDateFormat->SetSelection(0);
  fgSizer51->Add(m_choiceDateFormat, 0, wxALIGN_CENTER_VERTICAL | wxTOP, 0);

  wxFlexGridSizer* fgSizer52;
  fgSizer52 = new wxFlexGridSizer(1, 3, 0, 0);
  fgSizer52->SetFlexibleDirection(wxBOTH);
  fgSizer52->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxString m_choiceDate1Choices[] = {_("Month"), _("Day"), _("Year")};
  int m_choiceDate1NChoices = sizeof(m_choiceDate1Choices) / sizeof(wxString);
  m_choiceDate1 =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(-1, -1),
                   m_choiceDate1NChoices, m_choiceDate1Choices, 0);
  m_choiceDate1->SetSelection(0);
  fgSizer52->Add(m_choiceDate1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceDate2Choices[] = {_("Month"), _("Day"), _("Year")};
  int m_choiceDate2NChoices = sizeof(m_choiceDate2Choices) / sizeof(wxString);
  m_choiceDate2 =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(-1, -1),
                   m_choiceDate2NChoices, m_choiceDate2Choices, 0);
  m_choiceDate2->SetSelection(0);
  fgSizer52->Add(m_choiceDate2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceDate3Choices[] = {_("Month"), _("Day"), _("Year")};
  int m_choiceDate3NChoices = sizeof(m_choiceDate3Choices) / sizeof(wxString);
  m_choiceDate3 =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(-1, -1),
                   m_choiceDate3NChoices, m_choiceDate3Choices, 0);
  m_choiceDate3->SetSelection(0);
  fgSizer52->Add(m_choiceDate3, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  fgSizer51->Add(fgSizer52, 1, wxEXPAND, 0);

  wxFlexGridSizer* fgSizer53;
  fgSizer53 = new wxFlexGridSizer(0, 2, 0, 0);
  fgSizer53->SetFlexibleDirection(wxBOTH);
  fgSizer53->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText128 = new wxStaticText(m_panel15, wxID_ANY, _("Separator"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText128->Wrap(-1);
  fgSizer53->Add(m_staticText128, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT,
                 5);

  m_textCtrlDateSeparator = new wxTextCtrl(
      m_panel15, wxID_ANY, _("."), wxDefaultPosition, wxSize(20, -1), 0);
  m_textCtrlDateSeparator->SetMaxLength(1);
  fgSizer53->Add(m_textCtrlDateSeparator, 0, wxALIGN_CENTER_VERTICAL, 0);

  fgSizer51->Add(fgSizer53, 0, wxALIGN_CENTER_VERTICAL, 5);

  m_staticTextDate = new wxStaticText(m_panel15, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_staticTextDate->Wrap(-1);
  fgSizer51->Add(m_staticTextDate, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

  fgSizer51->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText133 = new wxStaticText(m_panel15, wxID_ANY, _("Time-Format:"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText133->Wrap(-1);
  fgSizer51->Add(m_staticText133, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceTimeFormatChoices[] = {_("24 Hour Format"),
                                          _("12 Hour Format")};
  int m_choiceTimeFormatNChoices =
      sizeof(m_choiceTimeFormatChoices) / sizeof(wxString);
  m_choiceTimeFormat =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceTimeFormatNChoices, m_choiceTimeFormatChoices, 0);
  m_choiceTimeFormat->SetSelection(0);
  fgSizer51->Add(m_choiceTimeFormat, 0, wxLEFT, 0);

  m_checkBoxNoSeconds = new wxCheckBox(m_panel15, wxID_ANY, _("No Seconds"),
                                       wxDefaultPosition, wxDefaultSize, 0);
  fgSizer51->Add(m_checkBoxNoSeconds, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

  fgSizer51->Add(0, 0, 1, wxEXPAND, 5);

  m_staticTextTime = new wxStaticText(m_panel15, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_staticTextTime->Wrap(-1);
  fgSizer51->Add(m_staticTextTime, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

  fgSizer10->Add(fgSizer51, 0,
                 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxTOP,
                 5);

  wxFlexGridSizer* fgSizer18;
  fgSizer18 = new wxFlexGridSizer(1, 7, 0, 0);
  fgSizer18->SetFlexibleDirection(wxBOTH);
  fgSizer18->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer18->Add(100, 0, 1, wxEXPAND, 5);

  m_radioBtnUTC = new wxRadioButton(m_panel15, wxID_ANY, _("UTC"),
                                    wxDefaultPosition, wxSize(80, -1), 0);
  fgSizer18->Add(m_radioBtnUTC, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_radioBtnLocal = new wxRadioButton(m_panel15, wxID_ANY, _("Local = UTC"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_radioBtnLocal->SetValue(true);
  fgSizer18->Add(m_radioBtnLocal, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceTzIndicatorChoices[] = {"+", "-"};
  int m_choiceTzIndicatorNChoices =
      sizeof(m_choiceTzIndicatorChoices) / sizeof(wxString);
#ifdef __WXOSX__
  m_choiceTzIndicator =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(50, -1),
                   m_choiceTzIndicatorNChoices, m_choiceTzIndicatorChoices, 0);
#else
  m_choiceTzIndicator =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(40, -1),
                   m_choiceTzIndicatorNChoices, m_choiceTzIndicatorChoices, 0);
#endif
  m_choiceTzIndicator->SetSelection(0);
  fgSizer18->Add(m_choiceTzIndicator, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceTzHoursChoices[] = {"0",  "1",  "2",  "3",  "4",
                                       "5",  "6",  "7",  "8",  "9",
                                       "10", "11", "12", "13", "14"};
  int m_choiceTzHoursNChoices =
      sizeof(m_choiceTzHoursChoices) / sizeof(wxString);
#ifdef __WXOSX__
  m_choiceTzHours =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(50, -1),
                   m_choiceTzHoursNChoices, m_choiceTzHoursChoices, 0);
#else
  m_choiceTzHours =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxSize(40, -1),
                   m_choiceTzHoursNChoices, m_choiceTzHoursChoices, 0);
#endif
  fgSizer18->Add(m_choiceTzHours, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_staticText71 = new wxStaticText(m_panel15, wxID_ANY, _("h"),
                                    wxDefaultPosition, wxSize(35, -1), 0);
  m_staticText71->Wrap(-1);
  fgSizer18->Add(m_staticText71, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_radioBtnGPSAuto = new wxRadioButton(m_panel15, wxID_ANY, _("GPS Auto"),
                                        wxDefaultPosition, wxDefaultSize, 0);
  m_radioBtnGPSAuto->SetToolTip(_("every 15 Degrees +/- 1 hour"));

  fgSizer18->Add(m_radioBtnGPSAuto, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  fgSizer10->Add(fgSizer18, 1, wxEXPAND, 5);

  m_staticline44 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer10->Add(m_staticline44, 0, wxEXPAND | wxBOTTOM, 5);

  wxFlexGridSizer* fgSizer29;
  fgSizer29 = new wxFlexGridSizer(8, 4, 0, 0);
  fgSizer29->SetFlexibleDirection(wxBOTH);
  fgSizer29->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxShowLogbook =
      new wxCheckBox(m_panel15, wxID_ANY, _("Show Logbook Icon"),
                     wxDefaultPosition, wxSize(-1, -1), wxCHK_2STATE);
  m_checkBoxShowLogbook->SetValue(true);
  fgSizer29->Add(m_checkBoxShowLogbook, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);

  m_staticText76 =
      new wxStaticText(m_panel15, wxID_ANY, _("Show Position Format"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText76->Wrap(-1);
  fgSizer29->Add(m_staticText76, 0,
                 wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 0);

  wxString m_choicePositionFormatChoices[] = {
      wxString::Format("054%s 12.1234%s", opt->Deg.c_str(), opt->Min.c_str()),
      wxString::Format(
          "054%s 12%s.12,34%s", opt->Deg.c_str(), opt->Min.c_str(),
          opt->Sec.c_str())};  //"054° 12.1234'", "054° 12' 12.34\"" };
  int m_choicePositionFormatNChoices =
      sizeof(m_choicePositionFormatChoices) / sizeof(wxString);
  m_choicePositionFormat = new wxChoice(
      m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      m_choicePositionFormatNChoices, m_choicePositionFormatChoices, 0);
  m_choicePositionFormat->SetSelection(0);
  fgSizer29->Add(m_choicePositionFormat, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxToolTips = new wxCheckBox(m_panel15, wxID_ANY, _("Show ToolTips"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxToolTips->SetValue(true);
  fgSizer29->Add(m_checkBoxToolTips, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxStaticText* m_staticText1391 =
      new wxStaticText(m_panel15, wxID_ANY, _("No. of Inboard Engines"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1391->Wrap(-1);
  fgSizer29->Add(m_staticText1391, 0,
                 wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxLEFT, 5);

  wxString m_choiceNoEnginesChoices[] = {_("1"), _("2")};
  int m_choiceNoEnginesNChoices =
      sizeof(m_choiceNoEnginesChoices) / sizeof(wxString);
  m_choiceNoEngines =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceNoEnginesNChoices, m_choiceNoEnginesChoices, 0);
  m_choiceNoEngines->SetSelection(0);
  fgSizer29->Add(m_choiceNoEngines, 0, wxRIGHT, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxStatusBar =
      new wxCheckBox(m_panel15, wxID_ANY, _("StatusBar at bottom of the page"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxStatusBar->SetValue(true);
  m_checkBoxStatusBar->SetToolTip(
      _("If unchecked the StatusBar will be placed above the "
        "Navigation/Meteo/Motor-Sails buttons"));
  fgSizer29->Add(m_checkBoxStatusBar, 0, 0, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxGenerator =
      new wxCheckBox(m_panel15, wxID_ANY, _("Generator On Board"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxGenerator->SetValue(true);
  m_checkBoxGenerator->SetToolTip(
      _("Check if there is a Generator onboard.\nIf unchecked the Generator "
        "columns will not show in Logbook"));
  fgSizer29->Add(m_checkBoxGenerator, 0, wxALIGN_CENTER_VERTICAL | wxTOP, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxWindspeeds =
      new wxCheckBox(m_panel15, wxID_ANY, _("Show Min|Avg|Max wind speeds"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxWindspeeds->SetValue(false);
  m_checkBoxWindspeeds->SetToolTip(
      _("If checked the TWS and AWS will display Min|Avg|Max windspeeds"));
  fgSizer29->Add(m_checkBoxWindspeeds, 0, wxALIGN_CENTER_VERTICAL | wxTOP, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxEngine = new wxCheckBox(m_panel15, wxID_ANY, _("Sails Messages"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxEngine->SetValue(true);
  fgSizer29->Add(m_checkBoxEngine, 0, wxTOP, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxOverview =
      new wxCheckBox(m_panel15, wxID_ANY, _("Show daily entries in Overview"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxOverview->SetValue(false);
  m_checkBoxOverview->SetToolTip(
      _("If checked the Overview for Active or Selected logbook will show "
        "daily lines-"));
  fgSizer29->Add(m_checkBoxOverview, 0, wxTOP, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxNoGPS =
      new wxCheckBox(m_panel15, wxID_ANY, _("Write warning \"No GPS\""),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxNoGPS->SetValue(true);
  m_checkBoxNoGPS->SetToolTip(
      _("Uncheck if You have no GPS\nIf GPS is out of order, uncheck "
        "to\nprevent that the message repeats."));

  fgSizer29->Add(m_checkBoxNoGPS, 0, wxTOP | wxALIGN_CENTER_VERTICAL, 0);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxEngineRunning = new wxCheckBox(
      m_panel15, wxID_ANY, _("Write message \"Engine running\" "),
      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxEngineRunning->SetValue(true);
  m_checkBoxEngineRunning->SetToolTip(
      _("If checked the message will be in Remarks when engine is started."));
  fgSizer29->Add(m_checkBoxEngineRunning, 0, wxALIGN_CENTER_VERTICAL | wxTOP,
                 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxSailsDown = new wxCheckBox(
      m_panel15, wxID_ANY, _("On engine start set always all sails down"),
      wxDefaultPosition, wxDefaultSize, 0);
  fgSizer29->Add(m_checkBoxSailsDown, 0, 0, 5);

  fgSizer29->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText741 =
      new wxStaticText(m_panel15, wxID_ANY, _("     Winddirection set to:"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText741->Wrap(-1);
  fgSizer29->Add(m_staticText741, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceWindToChoices[] = {_("default"), _("Heading")};
  int m_choiceWindToNChoices = sizeof(m_choiceWindToChoices) / sizeof(wxString);
  m_choiceWindTo =
      new wxChoice(m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceWindToNChoices, m_choiceWindToChoices, 0);
  m_choiceWindTo->SetSelection(0);
  m_choiceWindTo->SetToolTip(
      _("default = Relative to Boat\nHeading = Actual wind direction"));

  fgSizer29->Add(m_choiceWindTo, 0, wxRIGHT, 5);

  fgSizer10->Add(fgSizer29, 1, 0, 0);

  m_staticline25 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer10->Add(m_staticline25, 0, wxEXPAND | wxTOP, 5);

  m_staticText54 =
      new wxStaticText(m_panel15, wxID_ANY, _("Events"), wxDefaultPosition,
                       wxDefaultSize, wxALIGN_CENTRE);
  m_staticText54->Wrap(-1);
  m_staticText54->SetFont(wxFont(10, 74, 90, 92, false, "Tahoma"));

  fgSizer10->Add(m_staticText54, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);

  m_staticline22 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer10->Add(m_staticline22, 0, wxEXPAND | wxBOTTOM, 5);

  m_staticText123 =
      new wxStaticText(m_panel15, wxID_ANY,
                       _("Writes this text to the logbook when the event "
                         "occurs, alter texts as desired"),
                       wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticText123->Wrap(-1);
  fgSizer10->Add(m_staticText123, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL,
                 5);

  wxFlexGridSizer* fgSizer14;
  fgSizer14 = new wxFlexGridSizer(6, 3, 0, 0);
  fgSizer14->SetFlexibleDirection(wxBOTH);
  fgSizer14->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer14->Add(10, 0, 1, wxEXPAND, 5);

  m_staticText56 = new wxStaticText(m_panel15, wxID_ANY, _("Timer"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText56->Wrap(-1);
  fgSizer14->Add(m_staticText56, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  m_timerText =
      new wxTextCtrl(m_panel15, wxID_ANY,
                     _("automatic line by timer\nYou can set/unset a timer\n "
                       "by clicking the bullet\nChange this text in options"),
                     wxDefaultPosition, wxSize(250, 40), wxTE_MULTILINE);
  fgSizer14->Add(m_timerText, 0, 0, 5);

  fgSizer14->Add(10, 0, 1, wxEXPAND, 5);

  m_staticText121 = new wxStaticText(m_panel15, wxID_ANY, _("Waypoint"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText121->Wrap(-1);
  fgSizer14->Add(m_staticText121, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlWayPoint = new wxTextCtrl(
      m_panel15, wxID_ANY, _("automatic line\nWaypoint arrived:"),
      wxDefaultPosition, wxSize(250, 40), wxTE_MULTILINE);
  fgSizer14->Add(m_textCtrlWayPoint, 0, wxALL | wxEXPAND, 0);

  fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText122 = new wxStaticText(m_panel15, wxID_ANY, _("Watch"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText122->Wrap(-1);
  fgSizer14->Add(m_staticText122, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlGuradChanged =
      new wxTextCtrl(m_panel15, wxID_ANY, _("automatic line\nchange of guards"),
                     wxDefaultPosition, wxSize(250, 40), wxTE_MULTILINE);
  //	m_textCtrlGuradChanged->SetMaxLength( 0 );
  fgSizer14->Add(m_textCtrlGuradChanged, 0, wxALL | wxEXPAND, 0);

  fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText1231 = new wxStaticText(m_panel15, wxID_ANY, _("Distance"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1231->Wrap(-1);
  fgSizer14->Add(m_staticText1231, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlEverySMText =
      new wxTextCtrl(m_panel15, wxID_ANY, _("automatic line\nDistance > "),
                     wxDefaultPosition, wxSize(250, 40), wxTE_MULTILINE);
  //	m_textCtrlEverySMText->SetMaxLength( 0 );
  fgSizer14->Add(m_textCtrlEverySMText, 0, wxALL | wxEXPAND, 0);

  fgSizer14->Add(10, 0, 1, wxEXPAND, 5);

  m_staticText124 = new wxStaticText(m_panel15, wxID_ANY, _("Course changes"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText124->Wrap(-1);
  fgSizer14->Add(m_staticText124, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlChancedCourse = new wxTextCtrl(
      m_panel15, wxID_ANY, _("automatic line\nchange of course >"),
      wxDefaultPosition, wxSize(250, 40), wxTE_MULTILINE);
  //	m_textCtrlChancedCourse->SetMaxLength( 0 );
  fgSizer14->Add(m_textCtrlChancedCourse, 0, wxALL | wxEXPAND, 0);

  fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxPopUp =
      new wxCheckBox(m_panel15, wxID_ANY, _("Popup logbook on event"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxPopUp->SetValue(true);
  fgSizer14->Add(m_checkBoxPopUp, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_checkBoxAutoStartTimer =
      new wxCheckBox(m_panel15, wxID_ANY, _("Auto restart Timer(s)"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxAutoStartTimer->SetValue(false);
  m_checkBoxAutoStartTimer->SetToolTip(
      "This will automatically start the timer(s) if they were suspended when "
      "starting OpenCPN");

  fgSizer14->Add(m_checkBoxAutoStartTimer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL,
                 5);

  fgSizer10->Add(fgSizer14, 0,
                 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_staticline10 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer10->Add(m_staticline10, 0, wxEXPAND | wxTOP, 5);

  wxFlexGridSizer* fgSizer28;
  fgSizer28 = new wxFlexGridSizer(3, 2, 0, 0);
  fgSizer28->SetFlexibleDirection(wxBOTH);
  fgSizer28->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText731 = new wxStaticText(m_panel15, wxID_ANY, _("Install Layouts"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText731->Wrap(-1);
  fgSizer28->Add(m_staticText731, 0, wxTOP | wxRIGHT | wxLEFT, 5);

  m_staticText751 =
      new wxStaticText(m_panel15, wxID_ANY, _("Uninstall Logbook"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText751->Wrap(-1);
  fgSizer28->Add(m_staticText751, 0, wxTOP | wxRIGHT | wxLEFT, 5);

#ifndef LAYOUTS_HACK
  m_buttonInstallHTMLFiles = new wxButton(m_panel15, wxID_ANY, _("Install"),
                                          wxDefaultPosition, wxDefaultSize, 0);
  m_buttonInstallHTMLFiles->SetToolTip(
      _("Install OpenCPN_Logbook_Layouts.zip"));

  fgSizer28->Add(m_buttonInstallHTMLFiles, 0, wxALL | wxALIGN_CENTER_HORIZONTAL,
                 5);

  m_buttonUninstall = new wxButton(m_panel15, wxID_ANY, _("Uninstall"),
                                   wxDefaultPosition, wxDefaultSize, 0);
  m_buttonUninstall->SetToolTip(
      _("Uninstalls\nLogbook-Plugin,\nall Logbook-Data,\nall "
        "Logbook-Layouts,\nall Logbook-Languages\nand cleans the OpenCPN.ini"));

  fgSizer28->Add(m_buttonUninstall, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
#endif

  fgSizer10->Add(fgSizer28, 0,
                 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_staticline261 = new wxStaticLine(m_panel15, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer10->Add(m_staticline261, 0, wxEXPAND, 5);

  bSizer51->Add(fgSizer10, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  m_panel15->SetSizer(bSizer51);
  m_panel15->Layout();
  fgSizer10->Fit(m_panel15);
  m_notebook4->AddPage(m_panel15, _("Behavior"), true);
  m_panel20 = new wxPanel(m_notebook4, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer30;
  bSizer30 = new wxBoxSizer(wxHORIZONTAL);

  bSizer30->Add(0, 0, 0, wxEXPAND, 5);

  wxBoxSizer* bSizer31;
  bSizer31 = new wxBoxSizer(wxVERTICAL);

  bSizer31->Add(0, 20, 0, wxEXPAND, 5);

  m_staticText1041 =
      new wxStaticText(m_panel20, wxID_ANY,
                       _("Only used when you enter 1/2, 1/4 ... in a "
                         "total-field, or to calculate the watermaker\n"),
                       wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticText1041->Wrap(-1);
  m_staticText1041->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  bSizer31->Add(m_staticText1041, 0, wxALL | wxEXPAND, 5);

  m_staticline34 = new wxStaticLine(m_panel20, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  bSizer31->Add(m_staticline34, 0, wxEXPAND | wxALL, 5);

  wxFlexGridSizer* fgSizer44;

  fgSizer44 = new wxFlexGridSizer(3, 3, 0, 0);
  fgSizer44->SetFlexibleDirection(wxBOTH);
  fgSizer44->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText99 = new wxStaticText(m_panel20, wxID_ANY, _("Water"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText99->Wrap(-1);
  m_staticText99->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));
  m_staticText99->SetMinSize(wxSize(150, -1));
  fgSizer44->Add(m_staticText99, 0, wxALL, 0);
  fgSizer44->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer44->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer44->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText100 = new wxStaticText(m_panel20, wxID_ANY, _("Tank"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText100->Wrap(-1);
  m_staticText100->SetMinSize(wxSize(120, -1));

  fgSizer44->Add(m_staticText100, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlTankWater =
      new wxTextCtrl(m_panel20, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize,
                     wxTE_PROCESS_ENTER);
  m_textCtrlTankWater->SetToolTip(_("Set gal/liter in Abbreviations "));
  fgSizer44->Add(m_textCtrlTankWater, 0, wxALL, 5);

  fgSizer44->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText102 = new wxStaticText(m_panel20, wxID_ANY, _("Watermaker"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText102->Wrap(-1);
  m_staticText102->SetMinSize(wxSize(120, -1));

  fgSizer44->Add(m_staticText102, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlWatermaker =
      new wxTextCtrl(m_panel20, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize,
                     wxTE_PROCESS_ENTER);
  m_textCtrlWatermaker->SetToolTip(_("Set gal/liter in Abbreviations "));

  fgSizer44->Add(m_textCtrlWatermaker, 0, wxALL, 5);

  bSizer31->Add(fgSizer44, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  m_staticline33 = new wxStaticLine(m_panel20, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  bSizer31->Add(m_staticline33, 0, wxEXPAND | wxALL, 5);

  wxFlexGridSizer* fgSizer43;

  fgSizer43 = new wxFlexGridSizer(2, 3, 0, 0);
  fgSizer43->SetFlexibleDirection(wxBOTH);
  fgSizer43->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText96 = new wxStaticText(m_panel20, wxID_ANY, _("Fuel"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText96->Wrap(-1);
  m_staticText96->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));
  m_staticText96->SetMinSize(wxSize(150, -1));
  fgSizer43->Add(m_staticText96, 0, wxALL, 0);
  fgSizer43->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer43->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer43->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText97 = new wxStaticText(m_panel20, wxID_ANY, _("Tank"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText97->Wrap(-1);
  m_staticText97->SetMinSize(wxSize(120, -1));

  fgSizer43->Add(m_staticText97, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlTankFuel =
      new wxTextCtrl(m_panel20, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize,
                     wxTE_PROCESS_ENTER);
  m_textCtrlTankFuel->SetToolTip(_("Set gal/liter in Abbreviations "));
  fgSizer43->Add(m_textCtrlTankFuel, 0, wxALL, 5);

  bSizer31->Add(fgSizer43, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  m_staticline32 = new wxStaticLine(m_panel20, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  bSizer31->Add(m_staticline32, 0, wxALL | wxEXPAND, 5);

  wxFlexGridSizer* fgSizer45;

  fgSizer45 = new wxFlexGridSizer(3, 3, 0, 0);
  fgSizer45->SetFlexibleDirection(wxBOTH);
  fgSizer45->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText104 = new wxStaticText(m_panel20, wxID_ANY, _("Battery"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText104->Wrap(-1);
  m_staticText104->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));
  m_staticText104->SetMinSize(wxSize(150, -1));
  fgSizer45->Add(m_staticText104, 0, wxALL, 0);
  fgSizer45->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer45->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer45->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText105 = new wxStaticText(m_panel20, wxID_ANY, _("Bank #1"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText105->Wrap(-1);
  m_staticText105->SetMinSize(wxSize(120, -1));

  fgSizer45->Add(m_staticText105, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlBank1 = new wxTextCtrl(m_panel20, wxID_ANY, "0", wxDefaultPosition,
                                   wxDefaultSize, wxTE_PROCESS_ENTER);
  fgSizer45->Add(m_textCtrlBank1, 0, wxALL, 5);

  fgSizer45->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText107 = new wxStaticText(m_panel20, wxID_ANY, _("Bank #2"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText107->Wrap(-1);
  m_staticText107->SetMinSize(wxSize(120, -1));

  fgSizer45->Add(m_staticText107, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlBank2 = new wxTextCtrl(m_panel20, wxID_ANY, "0", wxDefaultPosition,
                                   wxDefaultSize, wxTE_PROCESS_ENTER);
  fgSizer45->Add(m_textCtrlBank2, 0, wxALL, 5);

  bSizer31->Add(fgSizer45, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  bSizer30->Add(bSizer31, 1, wxEXPAND, 5);

  m_panel20->SetSizer(bSizer30);
  m_panel20->Layout();
  bSizer30->Fit(m_panel20);
  m_notebook4->AddPage(m_panel20, _("Capacity"), false);
  m_panel16 = new wxPanel(m_notebook4, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxFlexGridSizer* fgSizer30;

  fgSizer30 = new wxFlexGridSizer(6, 1, 5, 0);
  fgSizer30->SetFlexibleDirection(wxBOTH);
  fgSizer30->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer* fgSizer91;

  fgSizer91 = new wxFlexGridSizer(13, 7, 5, 0);
  fgSizer91->SetFlexibleDirection(wxBOTH);
  fgSizer91->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  fgSizer91->Add(140, 30, 1, wxEXPAND, 5);
  fgSizer91->Add(100, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(80, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(80, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(60, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(50, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(100, 0, 1, wxEXPAND, 5);

  m_staticText34 = new wxStaticText(m_panel16, wxID_ANY, _("Navigation"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText34->Wrap(-1);
  m_staticText34->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  fgSizer91->Add(m_staticText34, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_staticText31 = new wxStaticText(m_panel16, wxID_ANY, _("Degrees"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText31->Wrap(-1);
  fgSizer91->Add(m_staticText31, 0, wxALIGN_CENTER_VERTICAL, 5);
#ifdef __WXOSX__
  m_sDeg = new wxTextCtrl(m_panel16, wxID_ANY, "\xA1", wxDefaultPosition,
                          wxSize(40, -1), 0);
  // don't know if \xA1 is correct, cannot compile for apple
#else
  m_sDeg = new wxTextCtrl(m_panel16, wxID_ANY, "\xB0", wxDefaultPosition,
                          wxSize(40, -1), 0);
#endif
  fgSizer91->Add(m_sDeg, 0, 0, 5);

  m_staticText35 = new wxStaticText(m_panel16, wxID_ANY, _("Minutes"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText35->Wrap(-1);
  fgSizer91->Add(m_staticText35, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_sMin = new wxTextCtrl(m_panel16, wxID_ANY, _("'"), wxDefaultPosition,
                          wxSize(40, -1), 0);
  fgSizer91->Add(m_sMin, 0, 0, 5);

  m_staticText36 = new wxStaticText(m_panel16, wxID_ANY, _("Seconds"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText36->Wrap(-1);
  fgSizer91->Add(m_staticText36, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_sSec = new wxTextCtrl(m_panel16, wxID_ANY, _("\""), wxDefaultPosition,
                          wxSize(40, -1), 0);
  fgSizer91->Add(m_sSec, 0, 0, 5);

  fgSizer91->Add(100, 0, 1, wxEXPAND, 5);

  m_staticText38 = new wxStaticText(m_panel16, wxID_ANY, _("Distance"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText38->Wrap(-1);
  fgSizer91->Add(m_staticText38, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceDistanceChoices[] = {_("NM"), _("m"), _("km")};
  int m_choiceDistanceNChoices =
      sizeof(m_choiceDistanceChoices) / sizeof(wxString);
  m_choiceDistance =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceDistanceNChoices, m_choiceDistanceChoices, 0);
  m_choiceDistance->SetSelection(0);
  fgSizer91->Add(m_choiceDistance, 0, 0, 5);

  m_staticText39 = new wxStaticText(m_panel16, wxID_ANY, _("Speed"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText39->Wrap(-1);
  fgSizer91->Add(m_staticText39, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceBoatSpeedChoices[] = {_("kts"), _("m/s"), _("kmh")};
  int m_choiceBoatSpeedNChoices =
      sizeof(m_choiceBoatSpeedChoices) / sizeof(wxString);
  m_choiceBoatSpeed =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceBoatSpeedNChoices, m_choiceBoatSpeedChoices, 0);
  m_choiceBoatSpeed->SetSelection(0);

  fgSizer91->Add(m_choiceBoatSpeed, 0, 0, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText40 = new wxStaticText(m_panel16, wxID_ANY, _("Show Depth in"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText40->Wrap(-1);
  fgSizer91->Add(m_staticText40, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceDepthChoices[] = {_("Meter"), _("Feet"), _("Fathom")};
  int m_choiceDepthNChoices = sizeof(m_choiceDepthChoices) / sizeof(wxString);
  m_choiceDepth =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceDepthNChoices, m_choiceDepthChoices, 0);
  m_choiceDepth->SetSelection(0);
  fgSizer91->Add(m_choiceDepth, 0, 0, 5);

  m_staticText41 = new wxStaticText(m_panel16, wxID_ANY, _("Meter"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText41->Wrap(-1);
  fgSizer91->Add(m_staticText41, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_smeter = new wxTextCtrl(m_panel16, wxID_ANY, _("m"), wxDefaultPosition,
                            wxSize(40, -1), 0);
  fgSizer91->Add(m_smeter, 0, 0, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 0);

  m_staticText63 =
      new wxStaticText(m_panel16, wxID_ANY, _("Show Wave/Swell in"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText63->Wrap(-1);
  fgSizer91->Add(m_staticText63, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceWaveSwellChoices[] = {_("Meter"), _("Feet"), _("Fathom")};
  int m_choiceWaveSwellNChoices =
      sizeof(m_choiceWaveSwellChoices) / sizeof(wxString);
  m_choiceWaveSwell =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceWaveSwellNChoices, m_choiceWaveSwellChoices, 0);
  m_choiceWaveSwell->SetSelection(0);
  fgSizer91->Add(m_choiceWaveSwell, 0, wxALIGN_CENTER_VERTICAL, 5);

  m_staticText48 = new wxStaticText(m_panel16, wxID_ANY, _("Feet"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText48->Wrap(-1);
  fgSizer91->Add(m_staticText48, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_sFeet = new wxTextCtrl(m_panel16, wxID_ANY, _("ft"), wxDefaultPosition,
                           wxSize(40, -1), 0);
  fgSizer91->Add(m_sFeet, 0, 0, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 0);

  m_staticText632 = new wxStaticText(m_panel16, wxID_ANY, _("Heading"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText632->Wrap(-1);
  fgSizer91->Add(m_staticText632, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceHeadingChoices[] = {_("True"), _("Magnetic")};
  int m_choiceHeadingNChoices =
      sizeof(m_choiceHeadingChoices) / sizeof(wxString);
  m_choiceHeading =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceHeadingNChoices, m_choiceHeadingChoices, 0);
  m_choiceHeading->SetSelection(0);
  fgSizer91->Add(m_choiceHeading, 1, wxALIGN_CENTER_VERTICAL, 0);

  m_staticText49 = new wxStaticText(m_panel16, wxID_ANY, _("Fathom"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText49->Wrap(-1);
  fgSizer91->Add(m_staticText49, 0, wxALIGN_CENTER_VERTICAL | wxBOTTOM | wxLEFT,
                 5);

  m_sFathom = new wxTextCtrl(m_panel16, wxID_ANY, _("fm"), wxDefaultPosition,
                             wxSize(40, -1), 0);
  fgSizer91->Add(m_sFathom, 0, 0, 5);
  fgSizer91->Add(0, 1, 1, wxEXPAND, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 0);

  m_staticText42 = new wxStaticText(m_panel16, wxID_ANY, _("Weather"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText42->Wrap(-1);
  m_staticText42->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  fgSizer91->Add(m_staticText42, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_staticText43 = new wxStaticText(m_panel16, wxID_ANY, _("Barometer"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText43->Wrap(-1);
  fgSizer91->Add(m_staticText43, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

  m_sBaro = new wxTextCtrl(m_panel16, wxID_ANY, _("mb"), wxDefaultPosition,
                           wxSize(40, -1), 0);
  fgSizer91->Add(m_sBaro, 0, 0, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText44 = new wxStaticText(m_panel16, wxID_ANY, _("Show Windspeed in"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText44->Wrap(-1);
  fgSizer91->Add(m_staticText44, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

  wxString m_choiceSpeedChoices[] = {_("kts"), _("m/s"), _("kmh")};
  int m_choiceSpeedNChoices = sizeof(m_choiceSpeedChoices) / sizeof(wxString);
  m_choiceWindSpeed =
      new wxChoice(m_panel16, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   m_choiceSpeedNChoices, m_choiceSpeedChoices, 0);
  m_choiceWindSpeed->SetSelection(0);
  fgSizer91->Add(m_choiceWindSpeed, 1, wxALIGN_CENTER_VERTICAL, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText791 = new wxStaticText(m_panel16, wxID_ANY, _("Temperature"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText791->Wrap(-1);
  fgSizer91->Add(m_staticText791, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

  m_textCtrlTemperature = new wxTextCtrl(m_panel16, wxID_ANY, _("F"),
                                         wxDefaultPosition, wxSize(40, -1), 0);
  fgSizer91->Add(m_textCtrlTemperature, 0, wxALL, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText46 = new wxStaticText(m_panel16, wxID_ANY, _("Fuel/Water"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText46->Wrap(-1);
  m_staticText46->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  fgSizer91->Add(m_staticText46, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_staticText47 = new wxStaticText(m_panel16, wxID_ANY, _("Capacity"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText47->Wrap(-1);
  fgSizer91->Add(m_staticText47, 0, wxALIGN_CENTER_VERTICAL, 5);

  m_sLiter = new wxTextCtrl(m_panel16, wxID_ANY, _("gal"), wxDefaultPosition,
                            wxSize(40, -1), 0);
  fgSizer91->Add(m_sLiter, 0, 0, 5);

  m_staticText50 = new wxStaticText(m_panel16, wxID_ANY, _("Motor Hours"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText50->Wrap(-1);
  fgSizer91->Add(m_staticText50, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_sMotorh = new wxTextCtrl(m_panel16, wxID_ANY, _("h"), wxDefaultPosition,
                             wxSize(40, -1), 0);
  fgSizer91->Add(m_sMotorh, 0, 0, 5);

  m_staticText106 = new wxStaticText(m_panel16, wxID_ANY, _("Ampere"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText106->Wrap(-1);
  fgSizer91->Add(m_staticText106, 0, wxALL, 5);

  m_textCtrlAmpere = new wxTextCtrl(m_panel16, wxID_ANY, "A", wxDefaultPosition,
                                    wxSize(40, -1), 0);
  fgSizer91->Add(m_textCtrlAmpere, 0, wxALL, 0);
  fgSizer91->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText135 = new wxStaticText(m_panel16, wxID_ANY, _("Engine"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText135->Wrap(-1);
  fgSizer91->Add(m_staticText135, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

  m_textCtrlEngine = new wxTextCtrl(m_panel16, wxID_ANY, _("E"),
                                    wxDefaultPosition, wxSize(40, -1), 0);
  fgSizer91->Add(m_textCtrlEngine, 0, 0, 5);

  m_staticText137 = new wxStaticText(m_panel16, wxID_ANY, _("PropShaft"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText137->Wrap(-1);
  fgSizer91->Add(m_staticText137, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

  m_textCtrlPropShaft = new wxTextCtrl(m_panel16, wxID_ANY, _("S"),
                                       wxDefaultPosition, wxSize(40, -1), 0);
  fgSizer91->Add(m_textCtrlPropShaft, 0, 0, 5);

  m_staticText136 = new wxStaticText(m_panel16, wxID_ANY, _("Revolutions"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText136->Wrap(-1);
  fgSizer91->Add(m_staticText136, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrRPM = new wxTextCtrl(m_panel16, wxID_ANY, _("RPM"),
                                wxDefaultPosition, wxSize(50, -1), 0);
  fgSizer91->Add(m_textCtrRPM, 0, 0, 5);
  m_staticText70 = new wxStaticText(m_panel16, wxID_ANY, _("Maintenance"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText70->Wrap(-1);
  m_staticText70->SetFont(
      wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  fgSizer91->Add(m_staticText70, 0, wxALL, 5);

  m_staticText712 = new wxStaticText(m_panel16, wxID_ANY, _("Day(s)"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  m_staticText712->Wrap(-1);
  fgSizer91->Add(m_staticText712, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_Days = new wxTextCtrl(m_panel16, wxID_ANY, _("Day(s)"), wxDefaultPosition,
                          wxSize(60, -1), 0);
  fgSizer91->Add(m_Days, 0, wxALL, 0);

  m_staticText72 = new wxStaticText(m_panel16, wxID_ANY, _("Week(s)"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText72->Wrap(-1);
  fgSizer91->Add(m_staticText72, 0, wxALL, 5);

  m_Weeks = new wxTextCtrl(m_panel16, wxID_ANY, _("Week(s)"), wxDefaultPosition,
                           wxSize(60, -1), 0);
  fgSizer91->Add(m_Weeks, 0, wxALL, 0);

  m_staticText73 = new wxStaticText(m_panel16, wxID_ANY, _("Month "),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText73->Wrap(-1);
  fgSizer91->Add(m_staticText73, 0, wxALL, 5);

  m_textMonth = new wxTextCtrl(m_panel16, wxID_ANY, _("Month"),
                               wxDefaultPosition, wxSize(60, -1), 0);
  fgSizer91->Add(m_textMonth, 0, wxALL, 0);

  fgSizer30->Add(fgSizer91, 1, wxEXPAND, 5);

  m_staticline251 = new wxStaticLine(m_panel16, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer30->Add(m_staticline251, 0, wxEXPAND | wxALL, 5);

  m_staticText75 = new wxStaticText(
      m_panel16, wxID_ANY, _("If GPS off use for manual entry"),
      wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticText75->Wrap(-1);
  fgSizer30->Add(m_staticText75, 0, wxALL | wxEXPAND, 5);

  m_staticline26 = new wxStaticLine(m_panel16, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_HORIZONTAL);
  fgSizer30->Add(m_staticline26, 0, wxEXPAND | wxALL, 5);

  wxFlexGridSizer* fgSizer32;
  fgSizer32 = new wxFlexGridSizer(2, 4, 0, 0);
  fgSizer32->SetFlexibleDirection(wxBOTH);
  fgSizer32->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText45 =
      new wxStaticText(m_panel16, wxID_ANY, _(" Direction "), wxPoint(-1, -1),
                       wxSize(-1, -1), wxALIGN_RIGHT);
  m_staticText45->Wrap(-1);
  fgSizer32->Add(m_staticText45, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceDirChoices[] = {_("True"), _("Relative")};
  int m_choiceDirNChoices = sizeof(m_choiceDirChoices) / sizeof(wxString);
#ifdef __WXOSX__
  m_choiceDir =
      new wxChoice(m_panel16, wxID_ANY, wxPoint(-1, -1), wxSize(80, -1),
                   m_choiceDirNChoices, m_choiceDirChoices, 0);
#else
  m_choiceDir =
      new wxChoice(m_panel16, wxID_ANY, wxPoint(-1, -1), wxSize(65, -1),
                   m_choiceDirNChoices, m_choiceDirChoices, 0);
#endif
  m_choiceDir->SetSelection(1);
  fgSizer32->Add(m_choiceDir, 0, wxALIGN_CENTER_VERTICAL, 5);

  fgSizer30->Add(fgSizer32, 0,
                 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_panel16->SetSizer(fgSizer30);
  m_panel16->Layout();
  fgSizer30->Fit(m_panel16);
  m_notebook4->AddPage(m_panel16, _("Abbreviations"), false);
  m_panel17 = new wxPanel(m_notebook4, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxFlexGridSizer* fgSizer322;
#ifdef __WXOSX__
  fgSizer322 = new wxFlexGridSizer(5, 1, 0, 0);
#else
  fgSizer322 = new wxFlexGridSizer(2, 1, 0, 0);
#endif
  fgSizer322->SetFlexibleDirection(wxBOTH);
  fgSizer322->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer* fgSizer321;

  fgSizer321 = new wxFlexGridSizer(5, 3, 0, 0);
  fgSizer321->SetFlexibleDirection(wxBOTH);
  fgSizer321->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  fgSizer321->Add(0, 40, 1, wxEXPAND, 5);
  fgSizer321->Add(0, 0, 1, wxEXPAND, 5);
  fgSizer321->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText761 =
      new wxStaticText(m_panel17, wxID_ANY, _("Path to ODT-Editor"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText761->Wrap(-1);
  fgSizer321->Add(m_staticText761, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlODTEditor = new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxSize(420, -1), 0);
  m_textCtrlODTEditor->SetToolTip(
      _("to use MS-Word, append %s like "
        "this:\n\"C:\\yourPathToWinword\\WINWORD.EXE\" %s"));

  fgSizer321->Add(m_textCtrlODTEditor, 0, wxALL | wxEXPAND, 5);

  m_bpButtonODT =
      new wxBitmapButton(m_panel17, wxID_ANY, wxBitmap(folder),
                         wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
  fgSizer321->Add(m_bpButtonODT, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_staticText77 =
      new wxStaticText(m_panel17, wxID_ANY, _("Path to Datamanager"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText77->Wrap(-1);
  fgSizer321->Add(m_staticText77, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

#ifdef __WXOSX__
  m_textCtrlDataManager = new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString,
                                         wxDefaultPosition, wxSize(420, -1), 0);
#else
  m_textCtrlDataManager = new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString,
                                         wxDefaultPosition, wxSize(-1, -1), 0);
#endif
  fgSizer321->Add(m_textCtrlDataManager, 0, wxALL | wxEXPAND, 5);

  m_bpButtonDatamanager =
      new wxBitmapButton(m_panel17, wxID_ANY, wxBitmap(folder),
                         wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
  fgSizer321->Add(m_bpButtonDatamanager, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_staticText78 =
      new wxStaticText(m_panel17, wxID_ANY, _("Path to MailClient"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText78->Wrap(-1);
  fgSizer321->Add(m_staticText78, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

#ifdef __WXOSX__
  m_textCtrlMailClient = new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxSize(420, -1), 0);
#else
  m_textCtrlMailClient = new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxSize(-1, -1), 0);
#endif
  fgSizer321->Add(m_textCtrlMailClient, 0, wxALL | wxEXPAND, 5);

  m_bpButtonMail =
      new wxBitmapButton(m_panel17, wxID_ANY, wxBitmap(folder),
                         wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
  fgSizer321->Add(m_bpButtonMail, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  m_staticText79 = new wxStaticText(m_panel17, wxID_ANY,
                                    _("Path to HTML-Editor\n(optional)"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  m_staticText79->Wrap(-1);
  fgSizer321->Add(m_staticText79, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlHTMLEditorPath =
      new wxTextCtrl(m_panel17, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxSize(420, -1), 0);
  fgSizer321->Add(m_textCtrlHTMLEditorPath, 1, wxALL | wxALIGN_CENTER_VERTICAL,
                  5);

  m_bpButtonHTMLEditor =
      new wxBitmapButton(m_panel17, wxID_ANY, wxBitmap(folder),
                         wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
  fgSizer321->Add(m_bpButtonHTMLEditor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

  fgSizer322->Add(fgSizer321, 1, wxEXPAND, 5);

  m_buttonResetPath = new wxButton(m_panel17, wxID_ANY, _("Reset Paths"),
                                   wxDefaultPosition, wxDefaultSize, 0);
  fgSizer322->Add(m_buttonResetPath, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_panel17->SetSizer(fgSizer322);
  m_panel17->Layout();
  fgSizer322->Fit(m_panel17);
  m_notebook4->AddPage(m_panel17, _("Paths"), false);

  m_panel24 = new wxPanel(m_notebook4, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer42;
  bSizer42 = new wxBoxSizer(wxVERTICAL);

  m_notebook7 =
      new wxNotebook(m_panel24, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
  m_notebook7->SetBackgroundColour(
      wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

  m_panel27 = new wxPanel(m_notebook7, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer431;
  bSizer431 = new wxBoxSizer(wxVERTICAL);

  wxFlexGridSizer* fgSizer431;
  fgSizer431 = new wxFlexGridSizer(0, 3, 0, 0);
  fgSizer431->SetFlexibleDirection(wxBOTH);
  fgSizer431->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer431->Add(150, 50, 1, wxEXPAND, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText1381 = new wxStaticText(m_panel27, wxID_ANY, _("Show"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1381->Wrap(-1);
  fgSizer431->Add(m_staticText1381, 0, wxALL, 5);

  wxFlexGridSizer* fgSizer47;
  fgSizer47 = new wxFlexGridSizer(0, 3, 0, 0);
  fgSizer47->SetFlexibleDirection(wxBOTH);
  fgSizer47->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_checkBoxKMLRoute = new wxCheckBox(m_panel27, wxID_ANY, _("Route"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxKMLRoute->SetValue(true);
  fgSizer47->Add(m_checkBoxKMLRoute, 0, wxALL, 5);

  m_checkBoxKMLTrack = new wxCheckBox(m_panel27, wxID_ANY, _("Track"),
                                      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxKMLTrack->SetValue(true);
  fgSizer47->Add(m_checkBoxKMLTrack, 0, wxALL, 5);

  fgSizer431->Add(fgSizer47, 1, wxEXPAND, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText1281 =
      new wxStaticText(m_panel27, wxID_ANY, _("Set Linewidth to"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1281->Wrap(-1);
  fgSizer431->Add(m_staticText1281, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlKMLLineWidt = new wxTextCtrl(m_panel27, wxID_ANY, _("4"),
                                         wxDefaultPosition, wxSize(40, -1), 0);
  fgSizer431->Add(m_textCtrlKMLLineWidt, 0, wxALL, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText131 =
      new wxStaticText(m_panel27, wxID_ANY, _("Set Transparency to"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText131->Wrap(-1);
  fgSizer431->Add(m_staticText131, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceKMLLineTranparencyChoices[] = {_("transparent"), _("solid")};
  int m_choiceKMLLineTranparencyNChoices =
      sizeof(m_choiceKMLLineTranparencyChoices) / sizeof(wxString);
  m_choiceKMLLineTranparency = new wxChoice(
      m_panel27, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      m_choiceKMLLineTranparencyNChoices, m_choiceKMLLineTranparencyChoices, 0);
  m_choiceKMLLineTranparency->SetSelection(0);
  fgSizer431->Add(m_choiceKMLLineTranparency, 0, wxALL, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText129 =
      new wxStaticText(m_panel27, wxID_ANY, _("Set Route-Linecolor to"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText129->Wrap(-1);
  fgSizer431->Add(m_staticText129, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceKMLRouteLineColoChoices[] = {_("white"), _("black"),
                                                _("red"), _("yellow")};
  int m_choiceKMLRouteLineColoNChoices =
      sizeof(m_choiceKMLRouteLineColoChoices) / sizeof(wxString);
  m_choiceKMLRouteLineColo = new wxChoice(
      m_panel27, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      m_choiceKMLRouteLineColoNChoices, m_choiceKMLRouteLineColoChoices, 0);
  m_choiceKMLRouteLineColo->SetSelection(0);
  fgSizer431->Add(m_choiceKMLRouteLineColo, 0, wxALL, 5);

  fgSizer431->Add(0, 0, 1, wxEXPAND, 5);

  m_staticText1291 =
      new wxStaticText(m_panel27, wxID_ANY, _("Set Track-Linecolor to"),
                       wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1291->Wrap(-1);
  fgSizer431->Add(m_staticText1291, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxString m_choiceKMLTrackLineColorChoices[] = {_("white"), _("black"),
                                                 _("red"), _("yellow")};
  int m_choiceKMLTrackLineColorNChoices =
      sizeof(m_choiceKMLTrackLineColorChoices) / sizeof(wxString);
  m_choiceKMLTrackLineColor = new wxChoice(
      m_panel27, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      m_choiceKMLTrackLineColorNChoices, m_choiceKMLTrackLineColorChoices, 0);
  m_choiceKMLTrackLineColor->SetSelection(3);
  fgSizer431->Add(m_choiceKMLTrackLineColor, 0, wxALL, 5);

  bSizer431->Add(fgSizer431, 1, wxEXPAND, 5);

  m_panel27->SetSizer(bSizer431);
  m_panel27->Layout();
  bSizer431->Fit(m_panel27);
  m_notebook7->AddPage(m_panel27, _("KML"), true);
  m_panel28 = new wxPanel(m_notebook7, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxTAB_TRAVERSAL);
  wxFlexGridSizer* fgSizer48;
  fgSizer48 = new wxFlexGridSizer(0, 2, 0, 0);
  fgSizer48->SetFlexibleDirection(wxBOTH);
  fgSizer48->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer48->Add(100, 50, 1, wxEXPAND, 5);

  m_staticTextUseRPM =
      new wxStaticText(m_panel28, wxID_ANY,
                       _("Check one of the below checkboxes, if you want use "
                         "ERRPM-Sentence (see help)"),
                       wxDefaultPosition, wxDefaultSize, 0);
  fgSizer48->Add(m_staticTextUseRPM, 0, wxALL | wxALIGN_BOTTOM, 5);

  fgSizer48->Add(0, 0, 1, wxEXPAND, 5);

  wxFlexGridSizer* fgSizer511;
  fgSizer511 = new wxFlexGridSizer(0, 3, 0, 0);
  fgSizer511->SetFlexibleDirection(wxBOTH);
  fgSizer511->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  fgSizer511->Add(20, 0, 0, 0, 5);

  m_staticTextRPMSentence = new wxStaticText(
      m_panel28, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
  m_staticTextRPMSentence->Wrap(-1);
  m_staticTextRPMSentence->Enable(false);
  fgSizer511->Add(m_staticTextRPMSentence, 0, wxALL | wxALIGN_CENTER_VERTICAL,
                  5);

  m_toggleBtnRPMCheck = new wxToggleButton(m_panel28, wxID_ANY, _("Check"),
                                           wxDefaultPosition, wxDefaultSize, 0);
  m_toggleBtnRPMCheck->Enable(false);
  m_toggleBtnRPMCheck->SetToolTip(
      _("Start your engine(s) and hit the button.\nYou should see the "
        "ERRPM-Sentence\nleft to the button"));

  fgSizer511->Add(m_toggleBtnRPMCheck, 0, wxALL, 5);

  fgSizer511->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxEng1RPM = new wxCheckBox(
      m_panel28, wxID_ANY, _("Engine-No. corresponds with column Engine #1"),
      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxEng1RPM->Enable(true);
  m_checkBoxEng1RPM->SetToolTip(
      _("Check this box if the NMEA stream contains RPM messages for this "
        "engine\n"));

  fgSizer511->Add(m_checkBoxEng1RPM, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  m_textCtrlEngine1 = new wxTextCtrl(m_panel28, wxID_ANY, wxEmptyString,
                                     wxDefaultPosition, wxSize(40, -1), 0);
  m_textCtrlEngine1->Enable(false);

  fgSizer511->Add(m_textCtrlEngine1, 0, wxALL, 5);

  fgSizer511->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxEng2RPM = new wxCheckBox(
      m_panel28, wxID_ANY, _("Engine-No. corresponds with column Engine #2"),
      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxEng2RPM->Enable(true);
  m_checkBoxEng2RPM->SetToolTip(
      _("Check this box if the NMEA stream contains RPM messages for this "
        "engine\n"));

  fgSizer511->Add(m_checkBoxEng2RPM, 0, wxALL, 5);

  m_textCtrlEngine2 = new wxTextCtrl(m_panel28, wxID_ANY, wxEmptyString,
                                     wxDefaultPosition, wxSize(40, -1), 0);
  m_textCtrlEngine2->Enable(false);

  fgSizer511->Add(m_textCtrlEngine2, 0, wxALL, 5);

  fgSizer511->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxGenRPM = new wxCheckBox(
      m_panel28, wxID_ANY, _("Engine-No. corresponds with column Generator"),
      wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxGenRPM->Enable(true);
  m_checkBoxGenRPM->SetToolTip(
      _("Check this box if the NMEA stream contains RPM messages for this "
        "generator\n"));

  fgSizer511->Add(m_checkBoxGenRPM, 0, wxALL, 5);

  m_textCtrlGenerator = new wxTextCtrl(m_panel28, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxSize(40, -1), 0);
  m_textCtrlGenerator->Enable(false);

  fgSizer511->Add(m_textCtrlGenerator, 0, wxALL, 5);

  fgSizer511->Add(0, 0, 1, wxEXPAND, 5);

  m_checkBoxNMEAUseRPM =
      new wxCheckBox(m_panel28, wxID_ANY,
                     _("Use ERRPM-Sentence to calculate the engine-hours"),
                     wxDefaultPosition, wxDefaultSize, 0);
  m_checkBoxNMEAUseRPM->Enable(false);

  fgSizer511->Add(m_checkBoxNMEAUseRPM, 0, wxALL, 5);

  fgSizer48->Add(fgSizer511, 0,
                 wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

  fgSizer48->Add(0, 0, 1, wxEXPAND, 5);

  m_panel28->SetSizer(fgSizer48);
  m_panel28->Layout();
  fgSizer48->Fit(m_panel28);
  m_notebook7->AddPage(m_panel28, _("NMEA"), true);

  m_panelSails = new wxPanel(m_notebook7, wxID_ANY, wxDefaultPosition,
                             wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer46;
  bSizer46 = new wxBoxSizer(wxVERTICAL);

  m_panelSailNames = new wxPanel(m_panelSails, wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, wxTAB_TRAVERSAL);

  wxBoxSizer* bSizer461;
  bSizer461 = new wxBoxSizer(wxVERTICAL);

  bSizer461->Add(0, 20, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 5);

  m_buttonToSailsSpace =
      new wxButton(m_panelSailNames, wxID_ANY, _("Checkbox Space  >>"),
                   wxDefaultPosition, wxDefaultSize, 0);
  m_buttonToSailsSpace->SetToolTip(_("Usefull for Tablet-PC's"));

  bSizer461->Add(m_buttonToSailsSpace, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_staticText1351 = new wxStaticText(
      m_panelSailNames, wxID_ANY,
      _("Abbreviations are shown in the Sails-Checkboxes\nThe Name is used for "
        "the column \"Sails\" when added a new entry"),
      wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  m_staticText1351->Wrap(-1);
  bSizer461->Add(m_staticText1351, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_gridSailNames = new wxGrid(m_panelSailNames, wxID_ANY, wxDefaultPosition,
                               wxDefaultSize, 0);

  // Grid
  m_gridSailNames->CreateGrid(14, 2);
  m_gridSailNames->EnableEditing(true);
  m_gridSailNames->EnableGridLines(true);
  m_gridSailNames->EnableDragGridSize(false);
  m_gridSailNames->SetMargins(0, 0);

  // Columns
  m_gridSailNames->SetColSize(0, 80);
  m_gridSailNames->SetColSize(1, 175);
  m_gridSailNames->EnableDragColMove(false);
  m_gridSailNames->EnableDragColSize(true);
  m_gridSailNames->SetColLabelSize(30);
  m_gridSailNames->SetColLabelValue(0, _("Abbreviations"));
  m_gridSailNames->SetColLabelValue(1, _("Name"));
  m_gridSailNames->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

  // Rows
  m_gridSailNames->EnableDragRowSize(true);
  m_gridSailNames->SetRowLabelSize(0);
  m_gridSailNames->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);

  // Label Appearance

  // Cell Defaults
  m_gridSailNames->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
  bSizer461->Add(m_gridSailNames, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  m_panelSailNames->SetSizer(bSizer461);
  m_panelSailNames->Layout();
  bSizer461->Fit(m_panelSailNames);
  bSizer46->Add(m_panelSailNames, 1, wxEXPAND | wxALL, 5);

  m_panelSailsCheckbox = new wxPanel(m_panelSails, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxTAB_TRAVERSAL);
  m_panelSailsCheckbox->Hide();
  wxBoxSizer* bSizer47;
  bSizer47 = new wxBoxSizer(wxVERTICAL);

  bSizer47->Add(0, 20, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  m_buttonBackToSails =
      new wxButton(m_panelSailsCheckbox, wxID_ANY, _("<<  Back"),
                   wxDefaultPosition, wxDefaultSize, 0);
  m_buttonBackToSails->SetToolTip(_("Back to Sailnames"));

  bSizer47->Add(0, 20, 0, wxALIGN_CENTER_HORIZONTAL, 5);
  bSizer47->Add(m_buttonBackToSails, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

  fgSizerSailsCheckboxes = new wxFlexGridSizer(0, 5, 0, 0);
  fgSizerSailsCheckboxes->SetFlexibleDirection(wxBOTH);
  fgSizerSailsCheckboxes->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  bSizer47->Add(fgSizerSailsCheckboxes, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  wxFlexGridSizer* fgSizer49;
  fgSizer49 = new wxFlexGridSizer(0, 5, 0, 0);
  fgSizer49->SetFlexibleDirection(wxBOTH);
  fgSizer49->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_buttonSpaceRPlus = new wxButton(m_panelSailsCheckbox, wxID_ANY, _("Row +"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  fgSizer49->Add(m_buttonSpaceRPlus, 0, wxALL, 5);

  m_buttonSpaceRMinus = new wxButton(m_panelSailsCheckbox, wxID_ANY, _("Row -"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  fgSizer49->Add(m_buttonSpaceRMinus, 0, wxALL, 5);

  m_buttonSpaceCPlus =
      new wxButton(m_panelSailsCheckbox, wxID_ANY, _("Column +"),
                   wxDefaultPosition, wxDefaultSize, 0);
  fgSizer49->Add(m_buttonSpaceCPlus, 0, wxALL, 5);

  m_buttonSpaceCMinus =
      new wxButton(m_panelSailsCheckbox, wxID_ANY, _("Column -"),
                   wxDefaultPosition, wxDefaultSize, 0);
  fgSizer49->Add(m_buttonSpaceCMinus, 0, wxALL, 5);

  m_buttonSpaceReset = new wxButton(m_panelSailsCheckbox, wxID_ANY, _("Reset"),
                                    wxDefaultPosition, wxDefaultSize, 0);
  fgSizer49->Add(m_buttonSpaceReset, 0, wxALL, 5);

  bSizer47->Add(0, 20, 0, wxALIGN_CENTER_HORIZONTAL, 5);
  bSizer47->Add(fgSizer49, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  m_panelSailsCheckbox->SetSizer(bSizer47);
  m_panelSailsCheckbox->Layout();
  bSizer47->Fit(m_panelSailsCheckbox);
  bSizer46->Add(m_panelSailsCheckbox, 1, wxEXPAND | wxALL, 5);

  m_panelSails->SetSizer(bSizer46);
  m_panelSails->Layout();
  bSizer46->Fit(m_panelSails);
  m_notebook7->AddPage(m_panelSails, _("Sails"), false);

  bSizer42->Add(m_notebook7, 1, wxEXPAND | wxALL, 5);

  m_panel24->SetSizer(bSizer42);
  m_panel24->Layout();
  bSizer42->Fit(m_panel24);
  m_notebook4->AddPage(m_panel24, _("Miscellaneous"), false);

  bSizer29->Add(m_notebook4, 1, wxEXPAND | wxALL, 5);

  m_sdbSizer1 = new wxStdDialogButtonSizer();
  m_sdbSizer1OK = new wxButton(m_scrolledWindow1, wxID_OK);
  m_sdbSizer1->AddButton(m_sdbSizer1OK);
  m_sdbSizer1Cancel = new wxButton(m_scrolledWindow1, wxID_CANCEL);
  m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
  m_sdbSizer1->Realize();

  bSizer29->Add(m_sdbSizer1, 0, wxALIGN_CENTER, 5);
  m_scrolledWindow1->SetSizer(bSizer29);
  m_scrolledWindow1->Layout();
  bSizer29->Fit(m_scrolledWindow1);
  bSizer21->Add(m_scrolledWindow1, 1, wxEXPAND | wxALL, 5);

  this->SetSizer(bSizer21);
  this->Layout();

  this->Centre(wxBOTH);

  SetSize(wxSize(w, h));

  // Connect Events
  m_choicePositionFormat->Connect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::onChoicePositionFormat), NULL,
      this);
  m_checkBoxToolTips->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::onCheckBoxToolTips), NULL, this);
  m_checkBoxNoGPS->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::onCheckBoNoGPS), NULL, this);
  m_radioBtnUTC->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                         wxCommandEventHandler(LogbookOptions::onRadioBtnUTC),
                         NULL, this);
  m_radioBtnLocal->Connect(
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
      wxCommandEventHandler(LogbookOptions::onRadioBtnLocal), NULL, this);
  m_radioBtnGPSAuto->Connect(
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
      wxCommandEventHandler(LogbookOptions::onRadioBtnGPSAuto), NULL, this);
  m_bpButtonODT->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickODT), NULL, this);
  m_bpButtonDatamanager->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickDataManager), NULL,
      this);
  m_bpButtonMail->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickMail), NULL, this);
#ifndef LAYOUTS_HACK
  m_buttonInstallHTMLFiles->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickInstallHTMLFiles),
      NULL, this);
#endif
  m_bpButtonHTMLEditor->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonHTMLEditor), NULL, this);
  m_sdbSizer1OK->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(LogbookOptions::OnButtonOKClick),
                         NULL, this);
#ifndef LAYOUTS_HACK
  m_buttonUninstall->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonClickUninstall), NULL,
      this);
#endif
  m_buttonResetPath->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonResetPaths), NULL, this);
  m_sLiter->Connect(wxEVT_COMMAND_TEXT_UPDATED,
                    wxCommandEventHandler(LogbookOptions::onTextm_sLiter), NULL,
                    this);
  m_textCtrlWatermaker->Connect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterm_textCtrlWatermaker),
      NULL, this);
  m_textCtrlBank1->Connect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterBank1), NULL, this);
  m_textCtrlBank2->Connect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::onTextEnterBank2), NULL, this);
  m_textCtrlTankFuel->Connect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterFuelTank), NULL, this);
  m_textCtrlTankWater->Connect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterWaterTank), NULL, this);
  m_choiceTimeFormat->Connect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceTimeFormat), NULL, this);
  m_checkBoxNoSeconds->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckboxNoSeconds), NULL, this);
  m_choiceDate1->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
                         wxCommandEventHandler(LogbookOptions::OnChoiceDate1),
                         NULL, this);
  m_choiceDate2->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
                         wxCommandEventHandler(LogbookOptions::OnChoiceDate2),
                         NULL, this);
  m_choiceDate3->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
                         wxCommandEventHandler(LogbookOptions::OnChoiceDate3),
                         NULL, this);
  m_choiceDateFormat->Connect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceDateFormat), NULL, this);
  m_textCtrlDateSeparator->Connect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextDateSeparator), NULL, this);
  m_sdbSizer1Cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(LogbookOptions::OnCancel),
                             NULL, this);
  m_choiceNoEngines->Connect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceNoEngines), NULL, this);
  m_toggleBtnRPMCheck->Connect(
      wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnToggleButtonRPMCheck), NULL,
      this);
  m_checkBoxEng1RPM->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEng1RPM), NULL, this);
  m_checkBoxEng2RPM->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEng2RPM), NULL, this);
  m_checkBoxGenRPM->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxGenRPM), NULL, this);
  m_buttonToSailsSpace->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonToSailsSpace), NULL, this);
  m_buttonBackToSails->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonBackToSails), NULL, this);
  m_buttonSpaceRPlus->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceRPlus), NULL, this);
  m_buttonSpaceRMinus->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceRMinus), NULL, this);
  m_buttonSpaceCPlus->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceCPlus), NULL, this);
  m_buttonSpaceCMinus->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceCMinus), NULL, this);
  m_buttonSpaceReset->Connect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceReset), NULL, this);
  m_checkBoxEngine->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEngineMessage), NULL,
      this);
  m_checkBoxEngineRunning->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEngineRunning), NULL,
      this);
  m_checkBoxGenerator->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxGeneratorMessage), NULL,
      this);
  m_checkBoxOverview->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxOverview), NULL, this);
  m_textCtrlEngine1->Connect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextEng1IdEntry), NULL, this);
  m_textCtrlEngine2->Connect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextEng2IdEntry), NULL, this);
  m_textCtrlGenerator->Connect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextGenIdEntry), NULL, this);

  this->Connect(wxEVT_CLOSE_WINDOW,
                wxCloseEventHandler(LogbookOptions::OnClose));

  init();
}

LogbookOptions::~LogbookOptions() {
  if (opt->checkStateOfEvents())
    log_pi->state = log_pi->ONWITHEVENTS;
  else
    log_pi->state = log_pi->ONNOEVENTS;

  // Disconnect Events
  m_choicePositionFormat->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::onChoicePositionFormat), NULL,
      this);
  m_checkBoxToolTips->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::onCheckBoxToolTips), NULL, this);
  m_checkBoxNoGPS->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::onCheckBoNoGPS), NULL, this);
  m_radioBtnUTC->Disconnect(
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
      wxCommandEventHandler(LogbookOptions::onRadioBtnUTC), NULL, this);
  m_radioBtnLocal->Disconnect(
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
      wxCommandEventHandler(LogbookOptions::onRadioBtnLocal), NULL, this);
  m_radioBtnGPSAuto->Disconnect(
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
      wxCommandEventHandler(LogbookOptions::onRadioBtnGPSAuto), NULL, this);
  m_bpButtonODT->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickODT), NULL, this);
  m_bpButtonDatamanager->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickDataManager), NULL,
      this);
  m_bpButtonMail->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickMail), NULL, this);
#ifndef LAYOUTS_HACK
  m_buttonInstallHTMLFiles->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonClickInstallHTMLFiles),
      NULL, this);
#endif
  m_bpButtonHTMLEditor->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonHTMLEditor), NULL, this);
  m_sdbSizer1OK->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonOKClick), NULL, this);
#ifndef LAYOUTS_HACK
  m_buttonUninstall->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonClickUninstall), NULL,
      this);
#endif
  m_buttonResetPath->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonResetPaths), NULL, this);
  m_sLiter->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
                       wxCommandEventHandler(LogbookOptions::onTextm_sLiter),
                       NULL, this);
  m_textCtrlWatermaker->Disconnect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterm_textCtrlWatermaker),
      NULL, this);
  m_textCtrlBank1->Disconnect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterBank1), NULL, this);
  m_textCtrlBank2->Disconnect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::onTextEnterBank2), NULL, this);
  m_textCtrlTankFuel->Disconnect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterFuelTank), NULL, this);
  m_textCtrlTankWater->Disconnect(
      wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(LogbookOptions::OnTextEnterWaterTank), NULL, this);
  m_choiceTimeFormat->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceTimeFormat), NULL, this);
  m_checkBoxNoSeconds->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckboxNoSeconds), NULL, this);
  m_choiceDate1->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceDate1), NULL, this);
  m_choiceDate2->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceDate2), NULL, this);
  m_choiceDate3->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceDate3), NULL, this);
  m_choiceDateFormat->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceDateFormat), NULL, this);
  m_textCtrlDateSeparator->Disconnect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextDateSeparator), NULL, this);
  m_sdbSizer1Cancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                wxCommandEventHandler(LogbookOptions::OnCancel),
                                NULL, this);
  m_choiceNoEngines->Disconnect(
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxCommandEventHandler(LogbookOptions::OnChoiceNoEngines), NULL, this);
  m_toggleBtnRPMCheck->Disconnect(
      wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnToggleButtonRPMCheck), NULL,
      this);
  m_checkBoxEng1RPM->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEng1RPM), NULL, this);
  m_checkBoxEng2RPM->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEng2RPM), NULL, this);
  m_checkBoxGenRPM->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxGenRPM), NULL, this);
  m_buttonToSailsSpace->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonToSailsSpace), NULL, this);
  m_buttonBackToSails->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::onButtonBackToSails), NULL, this);
  m_buttonSpaceRPlus->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceRPlus), NULL, this);
  m_buttonSpaceRMinus->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceRMinus), NULL, this);
  m_buttonSpaceCPlus->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceCPlus), NULL, this);
  m_buttonSpaceCMinus->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceCMinus), NULL, this);
  m_buttonSpaceReset->Disconnect(
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnButtonSpaceReset), NULL, this);
  m_checkBoxEngine->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEngineMessage), NULL,
      this);
  m_checkBoxEngineRunning->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxEngineRunning), NULL,
      this);
  m_checkBoxGenerator->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxGeneratorMessage), NULL,
      this);
  m_checkBoxOverview->Disconnect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LogbookOptions::OnCheckBoxOverview), NULL, this);
  m_textCtrlEngine1->Disconnect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextEng1IdEntry), NULL, this);
  m_textCtrlEngine2->Disconnect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextEng2IdEntry), NULL, this);
  m_textCtrlGenerator->Disconnect(
      wxEVT_COMMAND_TEXT_UPDATED,
      wxCommandEventHandler(LogbookOptions::OnTextGenIdEntry), NULL, this);

  this->Disconnect(wxEVT_CLOSE_WINDOW,
                   wxCloseEventHandler(LogbookOptions::OnClose));
}

void LogbookOptions::init() {
  oldPattern = LogbookDialog::datePattern;
  opt->setDateFormat();
  oldDateFormat = opt->sdateformat;
  iOldDateFormat = opt->dateformat;
  ioldDate1 = opt->date1;
  ;
  ioldDate2 = opt->date2;
  ioldDate3 = opt->date3;
  ioldTimeFormat = opt->timeformat;
  boldNoSeconds = opt->noseconds;

  sample = wxDateTime::Now();
  opt->setOptionsTimeFormat();
  if (opt->hourFormat) this->m_choiceTimeFormat->Enable(false);

  opt->dateseparatorlocale = LogbookDialog::dateSeparator;
  setValues();

#ifdef __WXMSW__
  ;  // m_buttonInstallLanguages->Enable(false);
#else
#ifndef LAYOUTS_HACK
  m_buttonUninstall->Enable(false);
#endif
#endif

  opt->setTimeFormat(m_checkBoxNoSeconds->GetValue());

  setDateEnabled(opt->dateformat);

  modified = false;
  m_staticTextTime->SetLabel(sample.Format(opt->stimeformat));
  if (NULL != log_pi->m_plogbook_window)
    log_pi->m_plogbook_window->myParseDate(
        log_pi->m_plogbook_window->m_textCtrlWatchStartDate->GetValue(),
        textCtrlDate);

  if (opt->timeformat == -1) {
    wxString am, pm;
    wxDateTime::GetAmPmStrings(&am, &pm);
    if (am.IsEmpty())
      opt->timeformat = 0;
    else
      opt->timeformat = 1;
  }

  if (log_pi->m_plogbook_window == NULL) {
    wxString stdPath;
    stdPath = logbookkonni_pi::StandardPath();

    wxString sep = wxFileName::GetPathSeparator();

    wxString data_locn = stdPath + "data" + sep + "logbook.txt";
    if (wxFile::Exists(data_locn)) {
      wxFileInputStream input(data_locn);
      wxTextInputStream* stream = new wxTextInputStream(input);

      if (!stream->ReadLine().Contains("#1.2#")) {
        log_pi->startLogbook();
        log_pi->m_plogbook_window->myParseDate(
            log_pi->m_plogbook_window->m_textCtrlWatchStartDate->GetValue(),
            textCtrlDate);
      }
    }
  }
  setUseRPM();
}

void LogbookOptions::resetToOldDateTimeFormat() {
  LogbookDialog::datePattern = oldPattern;

  opt->sdateformat = oldDateFormat;
  opt->dateformat = iOldDateFormat;
  opt->date1 = ioldDate1;
  opt->date2 = ioldDate2;
  opt->date3 = ioldDate3;
  opt->timeformat = ioldTimeFormat;
  opt->noseconds = boldNoSeconds;

  opt->setDateFormat();
  opt->setTimeFormat(opt->timeformat);
}

void LogbookOptions::OnCheckBoxEngineRunning(wxCommandEvent& event) {}

void LogbookOptions::OnCheckBoxEngineMessage(wxCommandEvent& event) {}

void LogbookOptions::OnCheckBoxOverview(wxCommandEvent& event) {}

void LogbookOptions::OnCheckBoxGeneratorMessage(wxCommandEvent& event) {
  opt->generator = m_checkBoxGenerator->GetValue();
  if (log_pi->m_plogbook_window) {
    if (!opt->generator) {
      if (opt->generatorRunning) {
        log_pi->m_plogbook_window->stopGenerator(false, false, true);
        wxMessageBox(
            _("Your Generator is still running .\n\nGenerator stopped now."),
            _("Information"));
      } else {
        log_pi->m_plogbook_window->stopGenerator(false, false, false);
      }
    } else {
      log_pi->m_plogbook_window->stopGenerator(true, true, false);
    }
    log_pi->m_plogbook_window->setShowGenerator();
  }
  setUseRPM();
}

void LogbookOptions::OnTextEng1IdEntry(wxCommandEvent& event) {}

void LogbookOptions::OnTextEng2IdEntry(wxCommandEvent& event) {}

void LogbookOptions::OnTextGenIdEntry(wxCommandEvent& event) {}

void LogbookOptions::OnButtonToSailsSpace(wxCommandEvent& event) {
  m_panelSailNames->Hide();
  wxCheckBox* checkboxSails[14];
  fgSizerSailsCheckboxes->Clear(true);
  m_panelSails->Layout();
  m_panelSailsCheckbox->Show();
  for (int i = 0; i < opt->numberSails; i++) {
    checkboxSails[i] =
        new wxCheckBox(m_panelSailsCheckbox, wxID_ANY, opt->abrSails.Item(i),
                       wxDefaultPosition, wxDefaultSize, 0);
    checkboxSails[i]->SetValue(opt->bSailIsChecked[i]);
    checkboxSails[i]->SetToolTip(opt->sailsName.Item(i));

    fgSizerSailsCheckboxes->Add(checkboxSails[i], 0, 0, 5);
  }
  wxButton* m_buttonSailsReset =
      new wxButton(m_panelSailsCheckbox, wxID_ANY, _("none"), wxDefaultPosition,
                   wxDefaultSize, 0);
  m_buttonSailsReset->SetToolTip(_("Reset"));
  m_buttonSailsReset->SetMinSize(wxSize(50, 25));

  fgSizerSailsCheckboxes->Add(m_buttonSailsReset, 0, 0, 5);

  fgSizerSailsCheckboxes->SetVGap(opt->rowGap);
  fgSizerSailsCheckboxes->SetHGap(opt->colGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::onButtonBackToSails(wxCommandEvent& event) {
  this->m_panelSailsCheckbox->Hide();
  this->m_panelSailNames->Show();
}

void LogbookOptions::OnButtonSpaceRPlus(wxCommandEvent& event) {
  opt->rowGap += 5;
  fgSizerSailsCheckboxes->SetVGap(opt->rowGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::OnButtonSpaceRMinus(wxCommandEvent& event) {
  opt->rowGap -= 5;
  if (opt->rowGap <= 0) opt->rowGap = 0;
  fgSizerSailsCheckboxes->SetVGap(opt->rowGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::OnButtonSpaceCPlus(wxCommandEvent& event) {
  opt->colGap += 5;
  fgSizerSailsCheckboxes->SetHGap(opt->colGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::OnButtonSpaceCMinus(wxCommandEvent& event) {
  opt->colGap -= 5;
  if (opt->colGap <= 0) opt->colGap = 0;
  fgSizerSailsCheckboxes->SetHGap(opt->colGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::OnButtonSpaceReset(wxCommandEvent& event) {
  opt->rowGap = 0;
  opt->colGap = 0;
  fgSizerSailsCheckboxes->SetVGap(opt->rowGap);
  fgSizerSailsCheckboxes->SetHGap(opt->rowGap);
  m_panelSailsCheckbox->Fit();
  m_panelSails->Layout();
}

void LogbookOptions::OnCheckBoxEng1RPM(wxCommandEvent& event) {
  opt->bEng1RPMIsChecked = event.IsChecked();
  if (log_pi->m_plogbook_window) {
    if (opt->bEng1RPMIsChecked) {
      if (log_pi->m_plogbook_window->logbook->engine1Manual &&
          opt->engine1Running) {
        log_pi->m_plogbook_window->stopEngine1(false, true);
        wxMessageBox(_("Your Engine #1 is running in manual mode.\n\nEngine "
                       "stopped now."),
                     _("Information"));
      } else {
        log_pi->m_plogbook_window->stopEngine1(false, false);
      }
    } else {
      log_pi->m_plogbook_window->stopEngine1(true, false);
    }
  }
  setUseRPM();
}

void LogbookOptions::OnCheckBoxEng2RPM(wxCommandEvent& event) {
  opt->bEng2RPMIsChecked = event.IsChecked();
  if (log_pi->m_plogbook_window) {
    if (opt->bEng2RPMIsChecked) {
      if (log_pi->m_plogbook_window->logbook->engine2Manual &&
          opt->engine2Running) {
        log_pi->m_plogbook_window->stopEngine2(false, true, true);
        wxMessageBox(_("Your Engine #2 is running in manual mode.\n\nEngine "
                       "stopped now."),
                     _("Information"));
      } else {
        log_pi->m_plogbook_window->stopEngine2(false, true, false);
      }
    } else {
      log_pi->m_plogbook_window->stopEngine2(true, true, false);
    }
  }
  setUseRPM();
}

void LogbookOptions::OnCheckBoxGenRPM(wxCommandEvent& event) {
  opt->bGenRPMIsChecked = event.IsChecked();
  if (log_pi->m_plogbook_window) {
    if (opt->bGenRPMIsChecked) {
      if (log_pi->m_plogbook_window->logbook->generatorManual &&
          opt->generatorRunning) {
        log_pi->m_plogbook_window->stopGenerator(false, true, true);
        wxMessageBox(_("Your Generator is running in manual mode.\n\nGenerator "
                       "stopped now."),
                     _("Information"));
      } else {
        log_pi->m_plogbook_window->stopGenerator(false, true, false);
      }
    } else {
      log_pi->m_plogbook_window->stopGenerator(true, true, false);
    }
  }
  setUseRPM();
}

void LogbookOptions::setUseRPM() {
  bool bIsChecked;

  bIsChecked = (opt->bEng1RPMIsChecked || opt->bEng2RPMIsChecked ||
                opt->bGenRPMIsChecked);

  if (bIsChecked) {
    opt->bRPMIsChecked = true;
    m_checkBoxNMEAUseRPM->Enable(true);
    m_toggleBtnRPMCheck->Enable(true);
  } else {
    opt->bRPMIsChecked = false;
    m_checkBoxNMEAUseRPM->Enable(false);
    m_toggleBtnRPMCheck->Enable(false);
  }

  if (opt->engines == 0) {
    m_checkBoxEng2RPM->SetValue(false);
    m_checkBoxEng2RPM->Enable(false);
    m_textCtrlEngine2->Enable(false);

    if (opt->bEng1RPMIsChecked) {
      m_textCtrlEngine1->Enable(true);
    } else {
      m_textCtrlEngine1->Enable(false);
    }
  } else {
    m_checkBoxEng2RPM->Enable(true);
    if (opt->bEng1RPMIsChecked) {
      m_textCtrlEngine1->Enable(true);
    } else {
      m_textCtrlEngine1->Enable(false);
    }

    if (opt->bEng2RPMIsChecked) {
      m_textCtrlEngine2->Enable(true);
    } else {
      m_textCtrlEngine2->Enable(false);
    }
  }

  if (!opt->generator) {
    m_checkBoxGenRPM->SetValue(false);
    m_checkBoxGenRPM->Enable(false);
    m_textCtrlGenerator->Enable(false);
  } else {
    m_checkBoxGenRPM->Enable(true);
    if (opt->bGenRPMIsChecked) {
      m_textCtrlGenerator->Enable(true);
    } else {
      m_textCtrlGenerator->Enable(false);
    }
  }
}

void LogbookOptions::OnToggleButtonRPMCheck(wxCommandEvent& event) {
  log_pi->startLogbook();
  opt->bRPMCheck = event.IsChecked();
  m_staticTextRPMSentence->Enable(event.IsChecked());
}

void LogbookOptions::setRPMSentence(wxString sentence) {
  static wxString e1 = wxEmptyString, e2 = wxEmptyString;
  m_staticTextRPMSentence->SetLabel(sentence);

  wxStringTokenizer tkz(sentence, ",");
  tkz.GetNextToken();
  wxString source = tkz.GetNextToken();
  wxString t = tkz.GetNextToken();

  /*    if(e1.IsEmpty())
          e1 = t;
      if(e2.IsEmpty() && t != e1)
          e2 = t;

      m_textCtrlEngine1->SetValue(e1);
      m_textCtrlEngine2->SetValue(e2); */
  //    if(source == "E")
  //        m_checkBoxNMEAUseRPM->SetValue(true);
}

void LogbookOptions::OnCancel(wxCommandEvent& event) {
  Close();
  event.Skip();
}

void LogbookOptions::OnClose(wxCloseEvent& event) {
  resetToOldDateTimeFormat();
  opt->bRPMCheck = false;
  event.Skip();
}

void LogbookOptions::OnChoiceDateFormat(wxCommandEvent& event) {
  setDateEnabled(event.GetSelection());
  opt->dateformat = event.GetSelection();
  opt->setDateFormat();
  m_staticTextDate->SetLabel(sample.Format(opt->sdateformat));
  modified = true;
}

void LogbookOptions::setDateEnabled(int i) {
  if (!i) {
    m_choiceDate1->Enable(false);
    m_choiceDate2->Enable(false);
    m_choiceDate3->Enable(false);
    m_textCtrlDateSeparator->Enable(false);
  } else {
    m_choiceDate1->Enable(true);
    m_choiceDate2->Enable(true);
    m_choiceDate3->Enable(true);
    m_textCtrlDateSeparator->Enable(true);
  }
}

void LogbookOptions::OnChoiceDate1(wxCommandEvent& event) {
  int sel = event.GetSelection();

  opt->date1 = sel;
  opt->setDateFormat();
  m_staticTextDate->SetLabel(sample.Format(opt->sdateformat));
  // m_staticTextTime->SetLabel(sample.Format(opt->sdateformat));
  modified = true;
}

void LogbookOptions::OnChoiceDate2(wxCommandEvent& event) {
  int sel = event.GetSelection();

  opt->date2 = sel;
  opt->setDateFormat();
  m_staticTextDate->SetLabel(sample.Format(opt->sdateformat));
  // m_staticTextTime->SetLabel(sample.Format(opt->sdateformat));
  modified = true;
}

void LogbookOptions::OnChoiceDate3(wxCommandEvent& event) {
  int sel = event.GetSelection();

  opt->date3 = sel;
  opt->setDateFormat();
  m_staticTextDate->SetLabel(sample.Format(opt->sdateformat));
  // m_staticTextTime->SetLabel(sample.Format(opt->sdateformat));
  modified = true;
}

void LogbookOptions::OnTextDateSeparator(wxCommandEvent& event) {
  opt->dateseparatorindiv = event.GetString();
  opt->setDateFormat();
  m_staticTextDate->SetLabel(sample.Format(opt->sdateformat));
  // m_staticTextTime->SetLabel(sample.Format(opt->sdateformat));
  modified = true;
}

void LogbookOptions::OnChoiceTimeFormat(wxCommandEvent& event) {
  if (opt->hourFormat && m_choiceTimeFormat->GetSelection() == 1) {
    wxMessageBox(
        _("Your Country has 24 Hour Format.\nIn this case the OS doen't serve "
          "AM/PM-Strings.\n"));
    m_choiceTimeFormat->SetSelection(0);
    return;
  }
  opt->timeformat = m_choiceTimeFormat->GetSelection();

  opt->setTimeFormat(m_checkBoxNoSeconds->GetValue());
  m_staticTextTime->SetLabel(sample.Format(opt->stimeformat));
  modified = true;
}

void LogbookOptions::OnCheckboxNoSeconds(wxCommandEvent& event) {
  opt->setTimeFormat(event.IsChecked());
  m_staticTextTime->SetLabel(sample.Format(opt->stimeformat));
  modified = true;
}

void LogbookOptions::OnButtonClickUninstall(wxCommandEvent& ev) {
#ifdef __WXMSW__
  wxStandardPathsBase& stdpath = wxStandardPathsBase::Get();
  wxString s = stdpath.GetPluginsDir();
  wxString command = s + "\\plugins\\uninst_logbookkonni_pi.exe";

  wxExecute("explorer.exe /select," + command);
#endif
}

void LogbookOptions::OnButtonResetPaths(wxCommandEvent& event) {
  opt->htmlEditor = opt->htmlEditorReset;
  opt->odtEditor = opt->odtEditorReset;
  opt->mailClient = opt->mailClientReset;
  opt->dataManager = opt->dataManagerReset;

  setValues();
}

void LogbookOptions::onCheckBoxToolTips(wxCommandEvent& ev) {
  if (ev.IsChecked()) {
    this->GetToolTip()->Enable(true);
    opt->showToolTips = true;
  } else {
    this->GetToolTip()->Enable(false);
    opt->showToolTips = false;
  }
}

void LogbookOptions::OnButtonOKClick(wxCommandEvent& ev) {
  int iNewDateFormat;

  int sel1 = m_choiceDate1->GetSelection();
  int sel2 = m_choiceDate2->GetSelection();
  int sel3 = m_choiceDate3->GetSelection();

  if (opt->dateformat == 1 &&
      ((sel1 == sel2 || sel1 == sel3) || (sel2 == sel1 || sel2 == sel3) ||
       (sel3 == sel1 || sel3 == sel2))) {
    wxString sep = m_textCtrlDateSeparator->GetValue();
    wxMessageBox(wxString::Format(
        _("You cannot set this dateformat.\n\n%s%s%s%s%s\n\nPlease use a "
          "different Dateformat in all three Choiceboxes."),
        m_choiceDate1->GetString(sel1).c_str(), sep.c_str(),
        m_choiceDate2->GetString(sel2).c_str(), sep.c_str(),
        m_choiceDate3->GetString(sel3).c_str()));
    return;
  }

  getValues();
  updateChoiceBoxes();

  LogbookDialog* dlg = log_pi->m_plogbook_window;

  if (dlg) {
    dlg->setSailsGap();
    dlg->setToNumberEngine();
    dlg->setShowGenerator();
    dlg->setCheckboxLabels();
    dlg->setAbbreviations();
  }

  setUseRPM();

  wxString newPattern = LogbookDialog::datePattern;
  wxString newDateFormat = opt->sdateformat;
  LogbookDialog::datePattern = oldPattern;
  opt->sdateformat = oldDateFormat;
  iNewDateFormat = opt->dateformat;
  opt->dateformat = iOldDateFormat;

  if ((m_sDeg->IsEmpty() || m_sDeg->GetValue().Len() != 1) ||
      (m_sMin->IsEmpty() || m_sMin->GetValue().Len() != 1) ||
      (m_sSec->IsEmpty()) || m_sSec->GetValue().Len() != 1) {
    wxMessageBox(
        _("Please fill in one character in Degrees, Minutes and Seconds"));
    return;
  } else
    ev.Skip();

  if (NULL != dlg && modified) {
    dlg->logbook->update();
    dlg->maintenance->update();
    dlg->maintenance->updateBuyParts();
    dlg->crewList->saveData();
    dlg->boat->saveData();

    LogbookDialog::datePattern = newPattern;
    opt->sdateformat = newDateFormat;
    opt->dateformat = iNewDateFormat;
    opt->setDateFormat();
    opt->setTimeFormat(opt->noseconds);

    dlg->logbook->clearAllGrids();
    dlg->logbook->loadData();

    LogbookDialog::datePattern = oldPattern;
    wxDateTime dt;
    dlg->myParseTime(dlg->m_textCtrlWatchStartTime->GetValue(), dt);
    dlg->m_textCtrlWatchStartTime->SetValue(dt.Format(opt->stimeformat));
    dlg->myParseDate(dlg->m_textCtrlWatchStartDate->GetValue(), dt);
    dlg->m_textCtrlWatchStartDate->SetValue(dt.Format(opt->sdateformat));
    LogbookDialog::datePattern = newPattern;

    if (dlg->crewList->getDayOne(1) != -1)
      dlg->crewList->dayNow(false);
    else {
      bool dummy = false;

      dlg->crewList->gridWake->SetCellValue(
          1, 0, textCtrlDate.Format(opt->sdateformat));
      dlg->crewList->updateWatchTime(dlg->crewList->day, 0, &dummy);
      dlg->crewList->gridWake->AutoSizeColumns();
    }

    if (dlg->m_gridMaintanence->GetNumberRows() > 0)
      dlg->m_gridMaintanence->DeleteRows(
          0, dlg->m_gridMaintanence->GetNumberRows());
    if (dlg->m_gridMaintenanceBuyParts->GetNumberRows() > 0)
      dlg->m_gridMaintenanceBuyParts->DeleteRows(
          0, dlg->m_gridMaintenanceBuyParts->GetNumberRows());
    dlg->maintenance->loadData();

    if (dlg->m_gridCrew->GetNumberRows() > 0)
      dlg->m_gridCrew->DeleteRows(0, dlg->m_gridCrew->GetNumberRows());
    dlg->crewList->loadData();

    dlg->boat->loadData();

    dlg->loadTimerEx();
  } else {
    opt->setDateFormat();
    opt->setTimeFormat(opt->noseconds);
  }
}

void LogbookOptions::onChoicePositionFormat(wxCommandEvent& ev) {
  switch (ev.GetInt()) {
    case 0:
      opt->traditional = false;
      break;
    case 1:
      opt->traditional = true;
      break;
  }
}

void LogbookOptions::setValues() {
  m_checkBoxPopUp->SetValue(opt->popup);
  m_checkBoxAutoStartTimer->SetValue(opt->autostarttimer);

  m_choiceDateFormat->Select(opt->dateformat);
  m_textCtrlDateSeparator->SetValue(opt->dateseparatorindiv);
  m_choiceDate1->SetSelection(opt->date1);
  m_choiceDate2->SetSelection(opt->date2);
  m_choiceDate3->SetSelection(opt->date3);

  m_choiceTimeFormat->Select(opt->timeformat);
  m_checkBoxNoSeconds->SetValue(opt->noseconds);
  m_choiceNoEngines->SetSelection(opt->engines);

  m_textCtrlTankFuel->SetValue(opt->fuelTank + " " + opt->vol);
  m_textCtrlTankWater->SetValue(opt->waterTank + " " + opt->vol);
  m_textCtrlBank1->SetValue(opt->bank1 + " " + opt->ampereh);
  m_textCtrlBank2->SetValue(opt->bank2 + " " + opt->ampereh);
  m_textCtrlWatermaker->SetValue(opt->watermaker + " " + opt->vol + "/" +
                                 opt->motorh);
  m_textCtrlGuradChanged->SetValue(opt->guardChangeText);
  m_textCtrlWayPoint->SetValue(opt->waypointText);
  m_textCtrlChancedCourse->SetValue(opt->courseChangeText);
  m_textCtrlEverySMText->SetValue(opt->everySMText);

  m_timerText->SetValue(opt->ttext);

  m_sDeg->SetValue(opt->Deg);
  m_sMin->SetValue(opt->Min);
  m_sSec->SetValue(opt->Sec);

  m_choiceDistance->SetSelection(opt->showDistanceChoice);
  m_choiceBoatSpeed->SetSelection(opt->showBoatSpeedchoice);

  m_sBaro->SetValue(opt->baro);
  m_textCtrlTemperature->SetValue(opt->temperature);
  m_smeter->SetValue(opt->meter);
  m_sFeet->SetValue(opt->feet);
  m_sFathom->SetValue(opt->fathom);

  m_sLiter->SetValue(opt->vol);
  m_sMotorh->SetValue(opt->motorh);
  m_textCtrlEngine->SetValue(opt->engine);
  m_textCtrlPropShaft->SetValue(opt->shaft);
  m_textCtrRPM->SetValue(opt->rpm);

  m_textCtrlAmpere->SetValue(opt->ampere);

  m_Days->SetValue(opt->days);
  m_Weeks->SetValue(opt->weeks);
  m_textMonth->SetValue(opt->month);

  m_choiceDepth->SetSelection(opt->showDepth);
  m_choiceWaveSwell->SetSelection(opt->showWaveSwell);
  m_choiceWindSpeed->SetSelection(opt->showWindSpeedchoice);

  m_choiceDir->SetSelection(opt->showWindDir);
  m_choiceHeading->SetSelection(opt->showHeading);
  m_choiceWindTo->SetSelection(opt->showWindHeading);

  m_textCtrlHTMLEditorPath->SetValue(opt->htmlEditor);
  m_textCtrlODTEditor->SetValue(opt->odtEditor);
  m_textCtrlDataManager->SetValue(opt->dataManager);
  m_textCtrlMailClient->SetValue(opt->mailClient);

  if (opt->local == true) m_radioBtnLocal->SetValue(true);
  if (opt->gpsAuto == true) m_radioBtnGPSAuto->SetValue(true);
  if (opt->UTC == true) m_radioBtnUTC->SetValue(true);

  if (opt->traditional)
    m_choicePositionFormat->SetSelection(1);
  else
    m_choicePositionFormat->SetSelection(0);

  m_checkBoxToolTips->SetValue(opt->showToolTips);

  m_choiceTzIndicator->SetSelection(opt->tzIndicator);
  m_choiceTzHours->SetSelection(opt->tzHour);

  m_checkBoxNoGPS->SetValue(opt->noGPS);
  m_checkBoxEngine->SetValue(opt->engineMessageSails);
  m_checkBoxEngineRunning->SetValue(opt->engineMessageRunning);
  m_checkBoxSailsDown->SetValue(opt->engineAllwaysSailsDown);
  m_checkBoxGenerator->SetValue(opt->generator);
  m_checkBoxStatusBar->SetValue(opt->statusbar);
  m_checkBoxWindspeeds->SetValue(opt->windspeeds);
  m_checkBoxOverview->SetValue(opt->overviewlines);

  m_checkBoxKMLRoute->SetValue((opt->kmlRoute) ? true : false);
  m_checkBoxKMLTrack->SetValue((opt->kmlTrack) ? true : false);
  m_textCtrlKMLLineWidt->SetValue(opt->kmlLineWidth);
  m_choiceKMLLineTranparency->SetSelection(opt->kmlLineTransparancy);
  m_choiceKMLRouteLineColo->SetSelection(opt->kmlRouteColor);
  m_choiceKMLTrackLineColor->SetSelection(opt->kmlTrackColor);

  if (opt->bRPMIsChecked) {
    m_checkBoxEng1RPM->SetValue(opt->bEng1RPMIsChecked);
    m_checkBoxEng2RPM->SetValue(opt->bEng2RPMIsChecked);
    m_checkBoxGenRPM->SetValue(opt->bGenRPMIsChecked);
    m_textCtrlEngine1->SetValue(opt->engine1Id);
    m_textCtrlEngine2->SetValue(opt->engine2Id);
    m_textCtrlGenerator->SetValue(opt->generatorId);
    m_checkBoxNMEAUseRPM->SetValue(opt->NMEAUseERRPM);
  }

  int row = 0;
  for (int col = 0; col < opt->numberSails; col++) {
    m_gridSailNames->SetCellValue(row, 0, opt->abrSails.Item(col));
    m_gridSailNames->SetCellValue(row++, 1, opt->sailsName.Item(col));
  }
}

void LogbookOptions::OnChoiceNoEngines(wxCommandEvent& event) {
  opt->engines = m_choiceNoEngines->GetSelection();
  if (log_pi->m_plogbook_window) {
    if (opt->engines == 0) {
      if (opt->engine2Running) {
        log_pi->m_plogbook_window->stopEngine2(false, false, true);
        wxMessageBox(
            _("Your Engine #2 is still running .\n\nEngine #2 stopped now."),
            _("Information"));
      } else {
        log_pi->m_plogbook_window->stopEngine2(false, false, false);
      }
    } else {
      log_pi->m_plogbook_window->stopEngine2(true, true, false);
    }
    log_pi->m_plogbook_window->setToNumberEngine();
  }
  setUseRPM();
}

void LogbookOptions::getValues() {
  opt->popup = m_checkBoxPopUp->GetValue();
  opt->autostarttimer = m_checkBoxAutoStartTimer->GetValue();

  opt->dateformat = m_choiceDateFormat->GetSelection();
  opt->dateseparatorindiv = m_textCtrlDateSeparator->GetValue();
  opt->dateseparatorlocale = wxString(LogbookDialog::dateSeparator);
  opt->date1 = m_choiceDate1->GetSelection();
  opt->date2 = m_choiceDate2->GetSelection();
  opt->date3 = m_choiceDate3->GetSelection();
  opt->engines = m_choiceNoEngines->GetSelection();

  opt->timeformat = m_choiceTimeFormat->GetSelection();
  opt->noseconds = m_checkBoxNoSeconds->GetValue();

  wxString t = m_textCtrlWatermaker->GetValue();
  opt->watermaker = t.substr(0, t.find_first_of(' '));
  t = m_textCtrlBank1->GetValue();
  opt->bank1 = t.substr(0, t.find_first_of(' '));
  t = m_textCtrlBank2->GetValue();
  opt->bank2 = t.substr(0, t.find_first_of(' '));
  t = m_textCtrlTankFuel->GetValue();
  opt->fuelTank = t.substr(0, t.find_first_of(' '));
  t = m_textCtrlTankWater->GetValue();
  opt->waterTank = t.substr(0, t.find_first_of(' '));
  opt->guardChangeText = m_textCtrlGuradChanged->GetValue();
  opt->waypointText = m_textCtrlWayPoint->GetValue();
  opt->courseChangeText = m_textCtrlChancedCourse->GetValue();
  opt->everySMText = m_textCtrlEverySMText->GetValue();

  opt->ttext = m_timerText->GetValue();

  opt->timerSec = (wxAtol(opt->thour) * 3600000 + wxAtol(opt->tmin) * 60000 +
                   wxAtol(opt->tsec) * 1000);

  opt->Deg = m_sDeg->GetValue();
  opt->Min = m_sMin->GetValue();
  opt->Sec = m_sSec->GetValue();

  opt->showDistanceChoice = m_choiceDistance->GetSelection();
  switch (m_choiceDistance->GetSelection()) {
    case 0:
      opt->showDistance = "NM";
      break;
    case 1:
      opt->showDistance = "m";
      break;
    case 2:
      opt->showDistance = "km";
      break;
  }

  opt->showBoatSpeedchoice = m_choiceBoatSpeed->GetSelection();
  switch (m_choiceBoatSpeed->GetSelection()) {
    case 0:
      opt->showBoatSpeed = "kts";
      break;
    case 1:
      opt->showBoatSpeed = "m/s";
      break;
    case 2:
      opt->showBoatSpeed = "kmh";
      break;
  }

  opt->baro = m_sBaro->GetValue();
  opt->temperature = m_textCtrlTemperature->GetValue();
  opt->meter = m_smeter->GetValue();
  opt->feet = m_sFeet->GetValue();
  opt->fathom = m_sFathom->GetValue();

  opt->vol = m_sLiter->GetValue();
  opt->motorh = m_sMotorh->GetValue();
  opt->engine = m_textCtrlEngine->GetValue();
  opt->shaft = m_textCtrlPropShaft->GetValue();
  opt->rpm = m_textCtrRPM->GetValue();
  opt->ampere = m_textCtrlAmpere->GetValue();

  opt->days = m_Days->GetValue();
  opt->weeks = m_Weeks->GetValue();
  opt->month = m_textMonth->GetValue();

  opt->showDepth = m_choiceDepth->GetSelection();
  opt->showWaveSwell = m_choiceWaveSwell->GetSelection();
  opt->showWindSpeedchoice = m_choiceWindSpeed->GetSelection();

  switch (m_choiceWindSpeed->GetSelection()) {
    case 0:
      opt->showWindSpeed = "kts";
      break;
    case 1:
      opt->showWindSpeed = "m/s";
      break;
    case 2:
      opt->showWindSpeed = "kmh";
      break;
  }

  opt->showWindDir = m_choiceDir->GetSelection();
  opt->showHeading = m_choiceHeading->GetSelection();
  opt->showWindHeading = m_choiceWindTo->GetSelection();

  opt->htmlEditor = m_textCtrlHTMLEditorPath->GetValue();
  opt->odtEditor = m_textCtrlODTEditor->GetValue();
  opt->dataManager = m_textCtrlDataManager->GetValue();
  opt->mailClient = m_textCtrlMailClient->GetValue();

  if (m_radioBtnLocal->GetValue() == true) opt->local = true;
  if (m_radioBtnGPSAuto->GetValue() == true) {
    opt->gpsAuto = true;
    opt->local = true;
  }
  if (this->m_radioBtnUTC->GetValue() == true) opt->UTC = true;

  if (m_choicePositionFormat->GetSelection())
    opt->traditional = true;
  else
    opt->traditional = false;

  opt->showToolTips = m_checkBoxToolTips->GetValue();

  opt->tzIndicator = m_choiceTzIndicator->GetSelection();
  opt->tzHour = m_choiceTzHours->GetSelection();
  opt->noGPS = m_checkBoxNoGPS->GetValue();
  opt->engineMessageSails = m_checkBoxEngine->GetValue();
  opt->engineMessageRunning = m_checkBoxEngineRunning->GetValue();
  opt->engineAllwaysSailsDown = m_checkBoxSailsDown->GetValue();
  opt->generator = m_checkBoxGenerator->GetValue();
  opt->statusbar = m_checkBoxStatusBar->GetValue();
  opt->windspeeds = m_checkBoxWindspeeds->GetValue();
  opt->overviewlines = m_checkBoxOverview->GetValue();

  opt->kmlRoute = m_checkBoxKMLRoute->GetValue();
  opt->kmlTrack = m_checkBoxKMLTrack->GetValue();
  opt->kmlLineWidth = m_textCtrlKMLLineWidt->GetValue();
  opt->kmlLineTransparancy = m_choiceKMLLineTranparency->GetSelection();
  opt->kmlRouteColor = m_choiceKMLRouteLineColo->GetSelection();
  opt->kmlTrackColor = m_choiceKMLTrackLineColor->GetSelection();

  opt->engine1Id = m_textCtrlEngine1->GetValue();
  opt->engine2Id = m_textCtrlEngine2->GetValue();
  opt->generatorId = m_textCtrlGenerator->GetValue();

  opt->bEng1RPMIsChecked = m_checkBoxEng1RPM->GetValue();
  opt->bEng2RPMIsChecked = m_checkBoxEng2RPM->GetValue();
  opt->bGenRPMIsChecked = m_checkBoxGenRPM->GetValue();
  opt->NMEAUseERRPM = m_checkBoxNMEAUseRPM->GetValue();

  // int row = 0;
  wxString tempstr;
  opt->abrSails.Clear();
  opt->sailsName.Clear();
  opt->numberSails = 0;
  for (int row = 0; row < m_gridSailNames->GetNumberRows(); row++) {
    tempstr = m_gridSailNames->GetCellValue(row, 0);
    if (!tempstr.IsEmpty()) {
      opt->abrSails.Add(m_gridSailNames->GetCellValue(row, 0));
      opt->sailsName.Add(m_gridSailNames->GetCellValue(row, 1));
      opt->numberSails++;
    }
  }
}

void LogbookOptions::onRadioBtnUTC(wxCommandEvent& ev) {
  if (ev.IsChecked()) {
    opt->local = false;
    opt->gpsAuto = false;
    opt->UTC = true;
    this->m_choiceTzIndicator->Enable(false);
    this->m_choiceTzHours->Enable(false);
  }
}
void LogbookOptions::onRadioBtnLocal(wxCommandEvent& ev) {
  if (ev.IsChecked()) {
    opt->local = true;
    opt->gpsAuto = false;
    opt->UTC = false;
    this->m_choiceTzIndicator->Enable(true);
    this->m_choiceTzHours->Enable(true);
  }
}

void LogbookOptions::onRadioBtnGPSAuto(wxCommandEvent& ev) {
  if (ev.IsChecked()) {
    opt->local = true;
    opt->gpsAuto = true;
    opt->UTC = false;
    this->m_choiceTzIndicator->Enable(true);
    this->m_choiceTzHours->Enable(true);
    this->m_choiceTzHours->SetSelection(opt->tzHour);
    this->m_choiceTzIndicator->SetSelection(opt->tzIndicator);
    this->m_choiceTzHours->Refresh();
    this->m_choiceTzIndicator->Refresh();
  }
}

void LogbookOptions::onButtonClickInstallHTMLFiles(wxCommandEvent& event) {
  log_pi->loadLayouts(this);
}

void LogbookOptions::onButtonHTMLEditor(wxCommandEvent& ev) {
  wxFileDialog* openFileDialog = new wxFileDialog(
      this, _("Set Path to HTML-Editor"), "", "", "", wxFD_OPEN);

  if (openFileDialog->ShowModal() == wxID_CANCEL) return;

  m_textCtrlHTMLEditorPath->SetValue(openFileDialog->GetPath());
  opt->htmlEditor = openFileDialog->GetPath();
}

void LogbookOptions::onButtonClickDataManager(wxCommandEvent& ev) {
  wxFileDialog* openFileDialog = new wxFileDialog(
      this, _("Set Path to DataManager"), "", "", "", wxFD_OPEN);

  if (openFileDialog->ShowModal() == wxID_CANCEL) return;

  m_textCtrlDataManager->SetValue(openFileDialog->GetPath());
  opt->dataManager = openFileDialog->GetPath();
}

void LogbookOptions::onButtonClickMail(wxCommandEvent& ev) {
  wxFileDialog* openFileDialog = new wxFileDialog(
      this, _("Set Path to MailClient"), "", "", "", wxFD_OPEN);

  if (openFileDialog->ShowModal() == wxID_CANCEL) return;

  m_textCtrlMailClient->SetValue(openFileDialog->GetPath());
  opt->mailClient = openFileDialog->GetPath();
}

void LogbookOptions::onButtonClickODT(wxCommandEvent& ev) {
  wxFileDialog* openFileDialog = new wxFileDialog(
      this, _("Set Path to ODT-Editor"), "", "", "", wxFD_OPEN);

  if (openFileDialog->ShowModal() == wxID_CANCEL) return;

  m_textCtrlODTEditor->SetValue(openFileDialog->GetPath());
  opt->odtEditor = openFileDialog->GetPath();
}

void LogbookOptions::onTextm_sLiter(wxCommandEvent& event) {
  wxString t =
      wxString::Format("%i %s/%s", wxAtoi(opt->watermaker),
                       m_sLiter->GetValue().c_str(), opt->motorh.c_str());
  this->m_textCtrlWatermaker->SetValue(t);

  t = wxString::Format("%i %s", wxAtoi(m_textCtrlTankWater->GetValue()),
                       m_sLiter->GetValue().c_str());
  this->m_textCtrlTankWater->SetValue(t);

  t = wxString::Format("%i %s", wxAtoi(m_textCtrlTankFuel->GetValue()),
                       m_sLiter->GetValue().c_str());
  this->m_textCtrlTankFuel->SetValue(t);
}

void LogbookOptions::OnTextEnterm_textCtrlWatermaker(wxCommandEvent& event) {
  long a;
  wxString t = m_textCtrlWatermaker->GetValue();
  t.ToLong(&a);
  m_textCtrlWatermaker->Clear();
  t = wxString::Format("%i %s/%s", a, m_sLiter->GetValue().c_str(),
                       opt->motorh.c_str());

  m_textCtrlWatermaker->SetValue(t);
  m_textCtrlTankFuel->SetFocus();
}

void LogbookOptions::OnTextEnterFuelTank(wxCommandEvent& event) {
  long a;
  wxString t = m_textCtrlTankFuel->GetValue();
  t.ToLong(&a);
  m_textCtrlTankFuel->Clear();
  t = wxString::Format("%i %s", a, m_sLiter->GetValue().c_str());

  m_textCtrlTankFuel->SetValue(t);
  m_textCtrlBank1->SetFocus();
}

void LogbookOptions::OnTextEnterWaterTank(wxCommandEvent& event) {
  long a;
  wxString t = m_textCtrlTankWater->GetValue();
  t.ToLong(&a);
  m_textCtrlTankWater->Clear();
  t = wxString::Format("%i %s", a, m_sLiter->GetValue().c_str());

  m_textCtrlTankWater->SetValue(t);
  m_textCtrlWatermaker->SetFocus();
}

void LogbookOptions::OnTextEnterBank1(wxCommandEvent& event) {
  long a;
  wxString t = m_textCtrlBank1->GetValue();
  t.ToLong(&a);
  m_textCtrlBank1->Clear();
  t = wxString::Format("%i %s", a, opt->ampereh.c_str());

  m_textCtrlBank1->SetValue(t);
  m_textCtrlBank2->SetFocus();
}

void LogbookOptions::onTextEnterBank2(wxCommandEvent& event) {
  long a;
  wxString t = m_textCtrlBank2->GetValue();
  t.ToLong(&a);
  m_textCtrlBank2->Clear();
  t = wxString::Format("%i %s", a, opt->ampereh.c_str());

  m_textCtrlBank2->SetValue(t);
  m_textCtrlTankWater->SetFocus();
}

void LogbookOptions::onCheckBoNoGPS(wxCommandEvent& event) {
  opt->noGPS = m_checkBoxNoGPS->GetValue();
}

void LogbookOptions::updateChoiceBoxes() {
  LogbookDialog* dialog = log_pi->m_plogbook_window;

  if (dialog != NULL) {
    dialog->loadLayoutChoice(
        LogbookDialog::LOGBOOK, dialog->logbook->layout_locn,
        dialog->logbookChoice, opt->layoutPrefix[LogbookDialog::LOGBOOK]);
    dialog->loadLayoutChoice(
        LogbookDialog::OVERVIEW, dialog->overview->layout_locn,
        dialog->overviewChoice, opt->layoutPrefix[LogbookDialog::OVERVIEW]);
    dialog->loadLayoutChoice(LogbookDialog::CREW, dialog->crewList->layout_locn,
                             dialog->crewChoice,
                             opt->layoutPrefix[LogbookDialog::CREW]);
    dialog->loadLayoutChoice(LogbookDialog::BOAT, dialog->boat->layout_locn,
                             dialog->boatChoice,
                             opt->layoutPrefix[LogbookDialog::BOAT]);
    dialog->loadLayoutChoice(LogbookDialog::GSERVICE,
                             dialog->maintenance->layout_locnService,
                             dialog->m_choiceSelectLayoutService,
                             opt->layoutPrefix[LogbookDialog::GSERVICE]);
    dialog->loadLayoutChoice(LogbookDialog::GREPAIRS,
                             dialog->maintenance->layout_locnRepairs,
                             dialog->m_choiceSelectLayoutRepairs,
                             opt->layoutPrefix[LogbookDialog::GREPAIRS]);
    dialog->loadLayoutChoice(LogbookDialog::GBUYPARTS,
                             dialog->maintenance->layout_locnBuyParts,
                             dialog->m_choiceSelectLayoutBuyParts,
                             opt->layoutPrefix[LogbookDialog::GBUYPARTS]);
  }
}
