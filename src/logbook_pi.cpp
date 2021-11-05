/**
 * Copyright (C) 2010  David S. Register
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

//#define _2_9_x_ // uncomment this to compile for 2.9.x

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // precompiled headers

#include <memory>

#include "config.h"
#include "Logbook.h"
#include "LogbookDialog.h"
#include "LogbookOptions.h"
#include "Options.h"
#include "icons.h"
#include "logbook_pi.h"

#include <wx/dir.h>
#include <wx/event.h>
#include <wx/filefn.h>
#include <wx/jsonreader.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/timer.h>
#include <wx/wxprec.h>

using namespace std;

#ifndef DECL_EXP
#ifdef __WXMSW__
#define DECL_EXP __declspec(dllexport)
#else
#define DECL_EXP
#endif
#endif

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void* ppimgr) {
  return (opencpn_plugin*)new logbookkonni_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) { delete p; }


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------
logbookkonni_pi::logbookkonni_pi(void* ppimgr) : opencpn_plugin_116(ppimgr) {
  // Create the PlugIn icons
  initialize_images();
  opt = new Options();
  m_timer = NULL;
  state = 0;
}

logbookkonni_pi::~logbookkonni_pi() {
  if (m_timer != NULL && m_timer->IsRunning()) {
    m_timer->Stop();
    m_timer = NULL;
  }
  if (opt != NULL) delete opt;
}

int logbookkonni_pi::Init(void) {
  state = OFF;
  dlgShow = false;

  AddLocaleCatalog("opencpn-logbookkonni_pi");

  m_plogbook_window = NULL;
  lastWaypointInRoute = "-1";
  eventsEnabled = true;

  opt = new Options();
  // Get a pointer to the opencpn display canvas, to use as a parent for windows
  // created
  m_parent_window = GetOCPNCanvasWindow();

  m_pconfig = GetOCPNConfigObject();

  LoadConfig();
  if (m_bLOGShowIcon)

  // FOR SVG ICONS  - CMakeLists.txt line 72  PLUGIN_USE_SVG=ON

#ifdef PLUGIN_USE_SVG
    m_leftclick_tool_id = InsertPlugInToolSVG(
        "Logbook", _svg_logbookkonni, _svg_logbookkonni_toggled,
        _svg_logbookkonni_toggled, wxITEM_CHECK, _("Logbook"), "", NULL,
        LOGBOOK_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id = InsertPlugInTool("", _img_logbook_pi, _img_logbook_pi,
                                           wxITEM_CHECK, _("Logbook"), "", NULL,
                                           LOGBOOK_TOOL_POSITION, 0, this);
#endif

  /*  OLD _IMG Ccde
          m_leftclick_tool_id  = InsertPlugInTool( "", _img_logbook_pi,
     _img_logbook_pi, wxITEM_NORMAL,
                                 _( "Logbook" ), "", NULL,
                                 LOGBOOK_TOOL_POSITION, 0, this );
  */

  // Create the Context Menu Items

  //    In order to avoid an ASSERT on msw debug builds,
  //    we need to create a dummy menu to act as a surrogate parent of the
  //    created MenuItems The Items will be re-parented when added to the real
  //    context meenu
  wxMenu dummy_menu;

  timer = new LogbookTimer(this);
  m_timer = new wxTimer(timer, ID_LOGTIMER);
  timer->Connect(wxEVT_TIMER, wxObjectEventFunction(&LogbookTimer::OnTimer));

  SendPluginMessage("LOGBOOK_READY_FOR_REQUESTS", "TRUE");

  return (WANTS_CURSOR_LATLON | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL |
          WANTS_CONFIG | WANTS_PREFERENCES | WANTS_NMEA_SENTENCES |
          WANTS_NMEA_EVENTS | USES_AUI_MANAGER | WANTS_PLUGIN_MESSAGING);
}

bool logbookkonni_pi::DeInit(void) {
  shutdown(false);
  return true;
}

void logbookkonni_pi::shutdown(bool menu) {
  SendPluginMessage("LOGBOOK_READY_FOR_REQUESTS", "FALSE");

  if (m_timer)
    if (m_timer->IsRunning()) m_timer->Stop();
  if (timer) {
    timer->Disconnect(wxEVT_TIMER,
                      wxObjectEventFunction(&LogbookTimer::OnTimer));
    delete timer;
  }

  timer = NULL;
  m_timer = NULL;

  if (m_plogbook_window != NULL) {
    if (m_plogbook_window->IsIconized()) m_plogbook_window->Iconize(false);
    m_plogbook_window->setIniValues();

    if ((opt->engine1Running && opt->toggleEngine1) ||
        (opt->engine2Running && opt->toggleEngine2) ||
        (opt->generatorRunning && opt->toggleGenerator)) {
      int a = wxMessageBox(
          _("Your engine(s) are still running\n\nStop engine(s) ?"), "",
          wxYES_NO | wxICON_QUESTION, NULL);
      if (a == wxYES) m_plogbook_window->logbook->resetEngineManualMode(0);
    }
    SaveConfig();
    m_plogbook_window->Close();
    m_plogbook_window->Destroy();
    m_plogbook_window = NULL;
    dlgShow = false;
    //	SetToolbarItemState( m_leftclick_tool_id, dlgShow );
  }
}

void logbookkonni_pi::SetPluginMessage(wxString& message_id,
                                       wxString& message_body) {
  if (message_id == "OCPN_MAN_OVERBOARD") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    m_plogbook_window->logbook->MOB_GUID = data.Item("GUID").AsString();
    m_plogbook_window->logbook->MOBIsActive = true;

    m_plogbook_window->logbook->appendRow(true, false);
  } else if (message_id == "POLAR_SAVE_LOGBOOK") {
    if (m_plogbook_window) m_plogbook_window->logbook->update();
  } else if (message_id == "LOGBOOK_LOG_LASTLINE_REQUEST") {
    if (!m_plogbook_window) startLogbook();

    wxJSONValue key;
    int tcol = 0;
    int lastRow = m_plogbook_window->logGrids[0]->GetNumberRows() - 1;
    for (unsigned int g = 0; g < LOGGRIDS; g++)
      for (int col = 0; col < m_plogbook_window->logGrids[g]->GetNumberCols();
           col++)
        key[tcol++] =
            m_plogbook_window->logGrids[g]->GetCellValue(lastRow, col);
    wxJSONWriter w;
    wxString out;
    w.Write(key, out);
    wxString id = "LOGBOOK_LOG_LASTLINE_RESPONSE";
    SetPluginMessage(id, out);
    return;
  } else if (message_id == "LOGBOOK_IS_READY_FOR_REQUEST") {
    SendPluginMessage("LOGBOOK_READY_FOR_REQUESTS", "TRUE");
  } else if (message_id == "LOGBOOK_BUYPARTS_ADDLINE_REQUEST") {
    wxJSONReader reader;
    wxJSONValue data;
    int priority, amount;
    wxString category, title, unit, text, plugin;
    wxString prText[6];

    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    m_plogbook_window->Show();
    m_plogbook_window->m_logbook->SetSelection(4);    // Maintenance
    m_plogbook_window->m_notebook6->SetSelection(2);  // BuyParts

    for (int i = 0; i < data.Size(); i++) {
      priority = data[i].Item("Priority").AsInt();
      category = data[i].Item("Category").AsString();
      title = _("from ");
      plugin = data[i].Item("PluginName").AsString();
      title += plugin + _("-Plugin");
      amount = data[i].Item("Amount").AsInt();
      unit = data[i].Item("Unit").AsString();
      text = data[i].Item("Text").AsString();

      prText[priority] += wxString::Format("%4i  %-15s %-30s\n", amount,
                                           unit.c_str(), text.c_str());
    }

    if (plugin == "FindIt")
      m_plogbook_window->maintenance->deleteFindItRow(category, plugin);

    for (int i = 0; i < 6; i++) {
      if (prText[i] != wxEmptyString) {
        m_plogbook_window->maintenance->addLineBuyParts();

        int lastRow =
            m_plogbook_window->m_gridMaintenanceBuyParts->GetNumberRows() - 1;

        m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue(
            lastRow, 0, wxString::Format("%i", i));
        m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue(lastRow, 1,
                                                                   category);
        m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue(lastRow, 2,
                                                                   title);
        m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue(
            lastRow, 3, prText[i].RemoveLast());
        m_plogbook_window->m_gridMaintenanceBuyParts->AutoSizeRow(lastRow,
                                                                  false);
      }
    }

    m_plogbook_window->maintenance->checkBuyParts();

    return;
  } else if (message_id == "LOGBOOK_LOG_ADDLINE_REQUEST") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    m_plogbook_window->logbook->appendRow(true, true);
    int lastRow = m_plogbook_window->m_gridGlobal->GetNumberRows() - 1;

    m_plogbook_window->m_gridGlobal->SetCellValue(
        lastRow, 13, data.Item("Remarks").AsString());
    m_plogbook_window->m_gridMotorSails->SetCellValue(
        lastRow, 8, data.Item("MotorRemarks").AsString());
    return;
  } else if (message_id == "OCPN_WPT_ARRIVED") {
    if (!opt->waypointArrived || eventsEnabled) return;

    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    RMB rmb;
    rmb.From = data.Item("WP_arrived").AsString();
    rmb.To = lastWaypointInRoute = data.Item("Next_WP").AsString();
    m_plogbook_window->logbook->WP_skipped = data.Item("isSkipped").AsBool();
    m_plogbook_window->logbook->OCPN_Message = true;

    m_plogbook_window->logbook->checkWayPoint(rmb);

    m_plogbook_window->logbook->OCPN_Message = false;
    m_plogbook_window->logbook->WP_skipped = false;
  } else if (message_id == "OCPN_RTE_ENDED") {
    if (!opt->waypointArrived) return;

    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    RMB rmb;
    rmb.From = lastWaypointInRoute;
    rmb.To = "-1";
    m_plogbook_window->logbook->WP_skipped = false;
    m_plogbook_window->logbook->OCPN_Message = true;

    m_plogbook_window->logbook->checkWayPoint(rmb);
    m_plogbook_window->logbook->OCPN_Message = false;
    lastWaypointInRoute = "-1";
    m_plogbook_window->logbook->lastWayPoint = wxEmptyString;
    m_plogbook_window->logbook->routeIsActive = false;
  } else if (message_id == "OCPN_RTE_DEACTIVATED") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    m_plogbook_window->logbook->activeRoute = wxEmptyString;
    m_plogbook_window->logbook->activeRouteGUID = wxEmptyString;
    m_plogbook_window->logbook->routeIsActive = false;
    if (!m_plogbook_window->logbook->activeMOB.IsEmpty())
      m_plogbook_window->logbook->MOBIsActive = false;

  } else if (message_id == "OCPN_RTE_ACTIVATED") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    m_plogbook_window->logbook->activeRoute =
        data.Item("Route_activated").AsString();
    m_plogbook_window->logbook->activeRouteGUID = data.Item("GUID").AsString();
    m_plogbook_window->logbook->routeIsActive = true;
  } else if (message_id == "OCPN_TRK_ACTIVATED") {
    if (!m_plogbook_window) startLogbook();

    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    m_plogbook_window->logbook->activeTrack = data.Item("Name").AsString();
    m_plogbook_window->logbook->activeTrackGUID = data.Item("GUID").AsString();
    m_plogbook_window->logbook->trackIsActive = true;
  } else if (message_id == "OCPN_TRK_DEACTIVATED") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    if (!m_plogbook_window) startLogbook();

    if (m_plogbook_window) {
      m_plogbook_window->logbook->activeTrack = wxEmptyString;
      m_plogbook_window->logbook->activeTrackGUID = wxEmptyString;
      m_plogbook_window->logbook->trackIsActive = false;
    }

  } else if (message_id == "OCPN_TRACKPOINTS_COORDS") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    bool error = data["error"].AsBool();

    if (!error) {
      double lat = data["lat"].AsDouble();
      double lon = data["lon"].AsDouble();
      int total = data["TotalNodes"].AsInt();
      int i = data["NodeNr"].AsInt();
      if (i == 1) {
        wxString ph = m_plogbook_window->kmlPathHeader;
        ph.Replace("#NAME#", "Trackline");
        ph.Replace("#LINE#", "#LineTrack");
        *m_plogbook_window->logbook->kmlFile << ph;
      }
      if (i <= total)
        *m_plogbook_window->logbook->kmlFile
            << wxString::Format("%f,%f\n", lon, lat);
      if (i == total)
        *m_plogbook_window->logbook->kmlFile
            << m_plogbook_window->kmlPathFooter;
    }
    //		m_plogbook_window->logbook->writeTrackToKML(data);
  } else if (message_id == "OCPN_TRACKS_MERGED") {
    if (!m_plogbook_window) startLogbook();

    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    unsigned int i = 1;
    wxString target = data["targetTrack"].AsString();
    while (true) {
      if (data.HasMember("mergeTrack" + wxString::Format("%d", i)))
        m_plogbook_window->logbook->mergeList.Add(
            data["mergeTrack" + wxString::Format("%d", i++)].AsString());
      else
        break;
    }
    m_plogbook_window->logbook->setTrackToNewID(target);
  } else if (message_id == "OCPN_ROUTE_RESPONSE") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    bool error = data[0]["error"].AsBool();

    if (!error) m_plogbook_window->logbook->writeRouteToKML(data);
  } else if (message_id == "OCPN_ROUTELIST_RESPONSE") {
    wxJSONReader reader;
    wxJSONValue data;
    int numErrors = reader.Parse(message_body, &data);
    if (numErrors != 0) return;

    m_plogbook_window->writeToRouteDlg(data);
  }
}

void logbookkonni_pi::startLogbook() {
  if (!m_plogbook_window) {
    m_plogbook_window = new LogbookDialog(
        this, m_timer, timer, m_parent_window, wxID_ANY, _("Active Logbook"),
        wxDefaultPosition, wxSize(opt->dlgWidth, opt->dlgHeight),
        wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxMINIMIZE_BOX |
            wxRESIZE_BORDER);
    m_plogbook_window->init();
    m_plogbook_window->SetPosition(
        wxPoint(-1, this->m_parent_window->GetParent()->GetPosition().y + 80));

    if (m_plogbook_window->IsShown())
      SendPluginMessage(wxString("LOGBOOK_WINDOW_SHOWN"), wxEmptyString);
    else
      SendPluginMessage("LOGBOOK_WINDOW_HIDDEN", wxEmptyString);
  }
}

void logbookkonni_pi::SendLogbookMessage(wxString message_id,
                                         wxString message_body) {
  SendPluginMessage(message_id, message_body);
}

void logbookkonni_pi::GetOriginalColors() {
  mcol = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
  mcol1 = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVEBORDER);
  muitext = wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
  mgridline = m_plogbook_window->m_gridGlobal->GetGridLineColour();
  mudkrd = m_plogbook_window->m_gridGlobal->GetCellTextColour(0, 0);
  mback_color = wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  mtext_color = wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
}

void logbookkonni_pi::SetOriginalColors() {
  col = mcol;
  col1 = mcol1;
  gridline = mgridline;
  uitext = muitext;
  udkrd = mudkrd;
  back_color = mback_color;
  text_color = mtext_color;
}

void logbookkonni_pi::SetColorScheme(PI_ColorScheme cs) {
  if (NULL != m_plogbook_window) {
    if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB) {
      SetOriginalColors();
    } else {
      GetGlobalColor("DILG0", &col);         // Dialog Background white
      GetGlobalColor("DILG1", &col1);        // Dialog Background
      GetGlobalColor("DILG2", &back_color);  // Control Background
      GetGlobalColor("DILG3", &text_color);  // Text
      GetGlobalColor("UITX1", &uitext);      // Menu Text, derived from UINFF
      GetGlobalColor("UDKRD", &udkrd);
      GetGlobalColor("GREY2", &gridline);
    }

    if (cs == 0 || cs == 1)
      m_plogbook_window->SetBackgroundColour(
          wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    else
      m_plogbook_window->SetBackgroundColour(col);

    m_plogbook_window->SetForegroundColour(uitext);
    dialogDimmer(cs, m_plogbook_window, col, col1, back_color, text_color,
                 uitext, udkrd);
    m_plogbook_window->Refresh();
  }
}

void logbookkonni_pi::dialogDimmer(PI_ColorScheme cs, wxWindow* ctrl,
                                   wxColour col, wxColour col1,
                                   wxColour back_color, wxColour text_color,
                                   wxColour uitext, wxColour udkrd) {
  wxWindowList kids = ctrl->GetChildren();
  for (unsigned int i = 0; i < kids.GetCount(); i++) {
    wxWindowListNode* node = kids.Item(i);
    wxWindow* win = node->GetData();

    if (win->IsKindOf(CLASSINFO(wxListBox)))
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB)
        ((wxListBox*)win)->SetBackgroundColour(wxNullColour);
      else
        ((wxListBox*)win)->SetBackgroundColour(col1);

    else if (win->IsKindOf(CLASSINFO(wxChoice)))
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB)
        ((wxChoice*)win)
            ->SetBackgroundColour(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
      else
        ((wxChoice*)win)->SetBackgroundColour(col1);

    else if (win->IsKindOf(CLASSINFO(wxRadioButton)))
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB)
        ((wxRadioButton*)win)
            ->SetForegroundColour(
                wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
      else
        ((wxRadioButton*)win)->SetForegroundColour(col1);

    else if (win->IsKindOf(CLASSINFO(wxNotebook))) {
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB)
        ((wxNotebook*)win)
            ->SetBackgroundColour(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
      else
        ((wxNotebook*)win)->SetBackgroundColour(col1);
      ((wxNotebook*)win)->SetForegroundColour(text_color);
    }

    else if (win->IsKindOf(CLASSINFO(wxGrid))) {
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB)
        ((wxGrid*)win)
            ->SetDefaultCellBackgroundColour(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
      else

        ((wxGrid*)win)->SetDefaultCellBackgroundColour(col1);
      ((wxGrid*)win)->SetDefaultCellTextColour(uitext);
      ((wxGrid*)win)->SetLabelBackgroundColour(col);
      ((wxGrid*)win)->SetLabelTextColour(uitext);
      // ( ( wxGrid* )win )->GetRowGridLinePen( wxPen( col ) );
      ((wxGrid*)win)->SetGridLineColour(gridline);

    }

    else if (win->IsKindOf(CLASSINFO(wxButton))) {
      if (cs == PI_GLOBAL_COLOR_SCHEME_DAY ||
          cs == PI_GLOBAL_COLOR_SCHEME_RGB) {
        ((wxButton*)win)->SetForegroundColour(wxNullColour);
        ((wxButton*)win)->SetBackgroundColour(wxNullColour);
      } else {
        ((wxButton*)win)->SetBackgroundColour(col1);
      }

    }

    else {
      ;
    }

    if (win->GetChildren().GetCount() > 0) {
      wxWindow* w = win;
      dialogDimmer(cs, w, col, col1, back_color, text_color, uitext, udkrd);
    }
  }
}

void logbookkonni_pi::UpdateAuiStatus(void) {}

int logbookkonni_pi::GetAPIVersionMajor() { return API_VERSION_MAJOR; }

int logbookkonni_pi::GetAPIVersionMinor() { return API_VERSION_MINOR; }

int logbookkonni_pi::GetPlugInVersionMajor() { return PLUGIN_VERSION_MAJOR; }

int logbookkonni_pi::GetPlugInVersionMinor() { return PLUGIN_VERSION_MINOR; }

wxString logbookkonni_pi::GetCommonName() { return _("Logbook"); }

wxString logbookkonni_pi::GetShortDescription() {
  return _(CPACK_PACKAGE_DESCRIPTION_SUMMARY);
}

wxString logbookkonni_pi::GetLongDescription() {
  return _(CPACK_PACKAGE_DESCRIPTION);
}

void logbookkonni_pi::OnContextMenuItemCallback(int id) {}

void logbookkonni_pi::SetNMEASentence(wxString& sentence) {
  if (m_plogbook_window) {
    m_plogbook_window->logbook->SetSentence(sentence);
  }
}

void logbookkonni_pi::SetPositionFix(PlugIn_Position_Fix& pfix) {
  if (m_plogbook_window) {
    if (m_plogbook_window->logbook)
      m_plogbook_window->logbook->SetPosition(pfix);
  }
}

void logbookkonni_pi::SetDefaults(void) {
  // If the config somehow says NOT to show the icon, override it so the user
  // gets good feedback
  if (!m_bLOGShowIcon) {
    m_bLOGShowIcon = true;

#ifdef PLUGIN_USE_SVG
    m_leftclick_tool_id = InsertPlugInToolSVG(
        "Logbook", _svg_logbookkonni, _svg_logbookkonni_toggled,
        _svg_logbookkonni_toggled, wxITEM_CHECK, _("Logbook"), "", NULL,
        LOGBOOK_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id = InsertPlugInTool("", _img_logbook_pi, _img_logbook_pi,
                                           wxITEM_CHECK, _("Logbook"), "", NULL,
                                           LOGBOOK_TOOL_POSITION, 0, this);
#endif

    /*    OLD _img code
            m_leftclick_tool_id  = InsertPlugInTool( "", _img_logbook_pi,
       _img_logbook_pi, wxITEM_NORMAL,
                                   _( "Logbook" ), "", NULL,
                                   LOGBOOK_TOOL_POSITION, 0, this );
    */
  }
}

wxString logbookkonni_pi::StandardPath(void) {
  wxString s = wxFileName::GetPathSeparator();
  wxString stdPath = (*GetpPrivateApplicationDataLocation());

  stdPath += s + "plugins";

  if (!wxDirExists(stdPath)) wxMkdir(stdPath);

  stdPath += s + "logbook";

  if (!wxDirExists(stdPath)) wxMkdir(stdPath);

  return stdPath;
}

wxBitmap* logbookkonni_pi::GetPlugInBitmap() { return _img_logbook_pi; }

int logbookkonni_pi::GetToolbarToolCount(void) { return 1; }

void logbookkonni_pi::ShowPreferencesDialog(wxWindow* parent) {
  dlgShow = false;

  //#ifdef __WXOSX__
  // Not tested yet
  //#else
  if (opt->firstTime) {
    //		loadLayouts(parent);
    //		delete opt;
    //		opt = new Options();
    //		LoadConfig();
    opt->firstTime = false;
  }
  //#endif

#ifdef __WXMSW__
  optionsDialog = new LogbookOptions(
      parent, opt, this, -1, _("Logbook Preferences"), wxDefaultPosition,
      wxSize(692, 660), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
#elif defined __WXOSX__
  optionsDialog = new LogbookOptions(
      parent, opt, this, -1, _("Logbook Preferences"), wxDefaultPosition,
      wxSize(710, 685), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
#else
  optionsDialog = new LogbookOptions(
      parent, opt, this, -1, _("Logbook Preferences"), wxDefaultPosition,
      wxSize(740, 700), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
#endif
  optionsDialog->m_checkBoxShowLogbook->SetValue(m_bLOGShowIcon);

  if (optionsDialog->ShowModal() == wxID_OK) {
    optionsDialog->getValues();
    //    Show Icon changed value?
    if (m_bLOGShowIcon != optionsDialog->m_checkBoxShowLogbook->GetValue()) {
      m_bLOGShowIcon = optionsDialog->m_checkBoxShowLogbook->GetValue();

      if (m_bLOGShowIcon)

#ifdef PLUGIN_USE_SVG
        m_leftclick_tool_id = InsertPlugInToolSVG(
            "Logbook", _svg_logbookkonni, _svg_logbookkonni_toggled,
            _svg_logbookkonni_toggled, wxITEM_CHECK, _("Logbook"), "", NULL,
            LOGBOOK_TOOL_POSITION, 0, this);
#else
        m_leftclick_tool_id = InsertPlugInTool(
            "", _img_logbook_pi, _img_logbook_pi, wxITEM_CHECK, _("Logbook"),
            "", NULL, LOGBOOK_TOOL_POSITION, 0, this);
#endif

      /*    OLD _img code
                      m_leftclick_tool_id  = InsertPlugInTool( "",
         _img_logbook_pi, _img_logbook_pi, wxITEM_NORMAL,
                                             _( "Logbook" ), "", NULL,
         LOGBOOK_TOOL_POSITION, 0, this );
      */

      else
        RemovePlugInTool(m_leftclick_tool_id);
    }
    SaveConfig();
  }
  delete optionsDialog;
}

void logbookkonni_pi::OnToolbarToolCallback(int id) {
#ifdef __WXOSX__
  if (dlgShow && !m_plogbook_window->IsIconized()) {
    m_plogbook_window->Raise();
    return;
  }
#endif
  dlgShow = !dlgShow;
  // show the Logbook dialog
  if (NULL == m_plogbook_window) {
    if (m_timer == NULL) {
      if (timer == NULL) timer = new LogbookTimer(this);
      m_timer = new wxTimer(timer, ID_LOGTIMER);
      timer->Connect(wxEVT_TIMER,
                     wxObjectEventFunction(&LogbookTimer::OnTimer));
    }
    m_plogbook_window = new LogbookDialog(
        this, m_timer, timer, m_parent_window, wxID_ANY, _("Active Logbook"),
        wxDefaultPosition, wxSize(opt->dlgWidth, opt->dlgHeight),
        wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxMINIMIZE_BOX |
            wxRESIZE_BORDER);
    m_plogbook_window->init();
    m_plogbook_window->CenterOnParent();
    m_plogbook_window->Show();
    dlgShow = true;
  } else {
    if (m_plogbook_window->IsIconized()) {
      m_plogbook_window->Iconize(false);
      m_plogbook_window->Show(true);
      dlgShow = true;
    } else {
      m_plogbook_window->Show(dlgShow);
    }
  }

  if (m_plogbook_window->IsShown())
    SendPluginMessage(wxString("LOGBOOK_WINDOW_SHOWN"), wxEmptyString);
  else
    SendPluginMessage("LOGBOOK_WINDOW_HIDDEN", wxEmptyString);

  if (state == OFF) {
    if (!opt->checkStateOfEvents())
      state = ONNOEVENTS;
    else
      state = ONWITHEVENTS;
  } else {
    state = OFF;
  }

  // Toggle is handled by the toolbar but we must keep plugin manager b_toggle
  // updated to actual status to ensure correct status upon toolbar rebuild
  // SetToolbarItemState( m_leftclick_tool_id, dlgShow );
}

void logbookkonni_pi::SaveConfig() {
  wxFileConfig* pConf = (wxFileConfig*)m_pconfig;

  if (pConf) {
    pConf->SetPath(_T ( "/PlugIns/Logbook" ));
    pConf->Write("ShowLOGIcon", m_bLOGShowIcon);
    pConf->Write("Traditional", opt->traditional);
    pConf->Write("ToolTips", opt->showToolTips);

    pConf->Write(_T ( "FirstTime" ), opt->firstTime);
    if (m_plogbook_window) {
      pConf->Write("DlgWidth", m_plogbook_window->GetSize().GetX());
      pConf->Write("DlgHeight", m_plogbook_window->GetSize().GetY());
    }

    pConf->Write(_T ( "Popup" ), opt->popup);
    pConf->Write(_T ( "AutoStartTimer" ), opt->autostarttimer);
    pConf->Write(_T ( "DateFormat" ), opt->dateformat);
    pConf->Write(_T ( "DateSepIndiv" ), opt->dateseparatorindiv);
    pConf->Write(_T ( "DateSepLocale" ), opt->dateseparatorlocale);
    pConf->Write(_T ( "Date1" ), opt->date1);
    pConf->Write(_T ( "Date2" ), opt->date2);
    pConf->Write(_T ( "Date3" ), opt->date3);
    pConf->Write(_T ( "NoEngines" ), opt->engines);

    pConf->Write(_T ( "TimeFormat" ), opt->timeformat);
    pConf->Write(_T ( "NoSeconds" ), opt->noseconds);

    pConf->Write(_T ( "GuardChange" ), opt->guardChange);
    pConf->Write(_T ( "GuardChangeText" ), opt->guardChangeText);
    pConf->Write(_T ( "WaypointArrived" ), opt->waypointArrived);
    pConf->Write(_T ( "WayPointText" ), opt->waypointText);
    pConf->Write(_T ( "CourseChange" ), opt->courseChange);
    pConf->Write(_T ( "CouseChangeDegrees" ), opt->courseChangeDegrees);
    pConf->Write(_T ( "CouseChangeAfter" ), opt->courseTextAfterMinutes);
    pConf->Write(_T ( "CourseChangeText" ), opt->courseChangeText);
    pConf->Write(_T ( "EverySM" ), opt->everySM);
    pConf->Write(_T ( "EverySMAmount" ), opt->everySMAmount);
    pConf->Write(_T ( "everySMText" ), opt->everySMText);

    pConf->Write(_T ( "Timer" ), opt->timer);
    pConf->Write(_T ( "TimerType" ), opt->timerType);
    pConf->Write(_T ( "Local" ), opt->local);
    pConf->Write(_T ( "UTC" ), opt->UTC);
    pConf->Write(_T ( "GPSAuto" ), opt->gpsAuto);
    pConf->Write(_T ( "TzIndicator" ), opt->tzIndicator);
    pConf->Write(_T ( "TzHours" ), opt->tzHour);
    pConf->Write(_T ( "TimerHours" ), opt->thour);
    pConf->Write(_T ( "TimerMin" ), opt->tmin);
    pConf->Write(_T ( "TimerSec" ), opt->tsec);
    pConf->Write(_T ( "TimerText" ), opt->ttext);

    pConf->Write(_T ( "NavDegrees" ), opt->Deg);
    pConf->Write(_T ( "NavMin" ), opt->Min);
    pConf->Write(_T ( "NavSec" ), opt->Sec);
    pConf->Write(_T ( "ShowDistance" ), opt->showDistance);
    pConf->Write(_T ("ShowDistanceInd"), opt->showDistanceChoice);
    pConf->Write(_T ("ShowBoatSpeedInd"), opt->showBoatSpeedchoice);
    pConf->Write(_T ( "showBoatSpeed" ), opt->showBoatSpeed);
    pConf->Write(_T ( "NavMeter" ), opt->meter);
    pConf->Write(_T ( "NavFeet" ), opt->feet);
    pConf->Write(_T ( "NavFathom" ), opt->fathom);

    pConf->Write(_T ( "Baro" ), opt->baro);
    pConf->Write(_T ( "Temperature" ), opt->temperature);

    pConf->Write(_T ( "Vol" ), opt->vol);
    pConf->Write(_T ( "Motorhours" ), opt->motorh);
    pConf->Write(_T ( "Engine" ), opt->engine);
    pConf->Write(_T ( "Shaft" ), opt->shaft);
    pConf->Write(_T ( "RPM" ), opt->rpm);

    pConf->Write(_T ( "Days" ), opt->days);
    pConf->Write(_T ( "Weeks" ), opt->weeks);
    pConf->Write(_T ( "Month" ), opt->month);

    pConf->Write(_T ( "Watermaker" ), opt->watermaker);
    pConf->Write(_T ( "FuelTank" ), opt->fuelTank);
    pConf->Write(_T ( "WaterTank" ), opt->waterTank);
    pConf->Write(_T ( "Ampere" ), opt->ampere);
    pConf->Write(_T ( "Bank1" ), opt->bank1);
    pConf->Write(_T ( "Bank2" ), opt->bank2);

    pConf->Write(_T ( "ShowDepth" ), opt->showDepth);
    pConf->Write(_T ( "ShowWaveSwell" ), opt->showWaveSwell);
    pConf->Write(_T ( "ShowWindSpeedInd" ), opt->showWindSpeedchoice);
    pConf->Write(_T ( "ShowWindSpeed" ), opt->showWindSpeed);
    pConf->Write(_T ( "ShowWindDir" ), opt->showWindDir);
    pConf->Write(_T ( "ShowHeading" ), opt->showHeading);
    pConf->Write(_T ( "ShowWindHeading" ), opt->showWindHeading);

    pConf->Write(_T ( "NavHTML" ), opt->navHTML);
    pConf->Write(_T ( "CrewHTML" ), opt->crewHTML);
    pConf->Write(_T ( "BoatHTML" ), opt->boatHTML);
    pConf->Write(_T ( "overviewHTML" ), opt->overviewHTML);
    pConf->Write(_T ( "serviceHTML" ), opt->serviceHTML);
    pConf->Write(_T ( "repairsHTML" ), opt->repairsHTML);
    pConf->Write(_T ( "buypartsHTML" ), opt->buypartsHTML);
    pConf->Write(_T ( "OverViewAll" ), opt->overviewAll);

    pConf->Write(_T ( "NavGridLayout" ), opt->navGridLayoutChoice);
    pConf->Write(_T ( "CrewGridLayout" ), opt->crewGridLayoutChoice);
    pConf->Write(_T ( "BoatGridLayout" ), opt->boatGridLayoutChoice);
    pConf->Write(_T ( "overviewGridLayout" ), opt->overviewGridLayoutChoice);
    pConf->Write(_T ( "serviceGridLayout" ), opt->serviceGridLayoutChoice);
    pConf->Write(_T ( "repairsGridLayout" ), opt->repairsGridLayoutChoice);
    pConf->Write(_T ( "buypartsGridLayout" ), opt->buypartsGridLayoutChoice);

    pConf->Write(_T ( "NavGridLayoutODT" ), opt->navGridLayoutChoiceODT);
    pConf->Write(_T ( "CrewGridLayoutODT" ), opt->crewGridLayoutChoiceODT);
    pConf->Write(_T ( "BoatGridLayoutODT" ), opt->boatGridLayoutChoiceODT);
    pConf->Write(_T ( "overviewGridLayoutODT" ),
                 opt->overviewGridLayoutChoiceODT);
    pConf->Write(_T ( "serviceGridLayoutODT" ),
                 opt->serviceGridLayoutChoiceODT);
    pConf->Write(_T ( "repairsGridLayoutODT" ),
                 opt->repairsGridLayoutChoiceODT);
    pConf->Write(_T ( "buypartsGridLayoutODT" ),
                 opt->buypartsGridLayoutChoiceODT);

    pConf->Write(_T ( "HTMLEditor" ), opt->htmlEditor);
    pConf->Write(_T ( "ODTEditor" ), opt->odtEditor);
    pConf->Write(_T ( "DataManager" ), opt->dataManager);
    pConf->Write(_T ( "MailClient" ), opt->mailClient);

    pConf->Write(_T ( "GPSWarning" ), opt->noGPS);
    pConf->Write(_T ( "EngineMessageSails" ), opt->engineMessageSails);
    pConf->Write(_T ( "WriteEngineRun" ), opt->engineMessageRunning);
    pConf->Write(_T ( "SailsDown" ), opt->engineAllwaysSailsDown);
    pConf->Write(_T ( "StatusBar" ), opt->statusbar);
    pConf->Write(_T ( "WindSpeeds" ), opt->windspeeds);
    pConf->Write(_T ( "OverviewLines" ), opt->overviewlines);

    wxString str = wxEmptyString;
    for (int i = 0; i < 7; i++)
      str += wxString::Format("%i,%s,", opt->filterLayout[i],
                              opt->layoutPrefix[i].c_str());
    str.RemoveLast();
    pConf->Write(_T ( "PrefixLayouts" ), str);

    wxString kmlRouteTrack =
        wxString::Format("%i,%i", opt->kmlRoute, opt->kmlTrack);
    pConf->Write(_T ( "KMLRouteTrack" ), kmlRouteTrack);
    pConf->Write(_T ( "KMLWidth" ), opt->kmlLineWidth);
    pConf->Write(_T ( "KMLTransp" ), opt->kmlLineTransparancy);
    pConf->Write(_T ( "KMLRouteColor" ), opt->kmlRouteColor);
    pConf->Write(_T ( "KMLTrackColor" ), opt->kmlTrackColor);

    pConf->Write(_T ( "RPMIsChecked" ), opt->bRPMIsChecked);
    pConf->Write(_T ( "Eng1RPMIsChecked" ), opt->bEng1RPMIsChecked);
    pConf->Write(_T ( "Eng2RPMIsChecked" ), opt->bEng2RPMIsChecked);
    pConf->Write(_T ( "GenRPMIsChecked" ), opt->bGenRPMIsChecked);

    pConf->Write(_T ( "NMEAUseRPM" ), opt->NMEAUseERRPM);
    pConf->Write(_T ( "Engine1" ), opt->engine1Id);
    pConf->Write(_T ( "Engine2" ), opt->engine2Id);
    pConf->Write(_T ( "Engine1Runs" ), opt->engine1Running);
    pConf->Write(_T ( "Engine2Runs" ), opt->engine2Running);

    pConf->Write(_T ( "Generator" ), opt->generator);
    pConf->Write(_T ( "GeneratorId" ), opt->generatorId);

    pConf->Write(_T ( "GeneratorRuns" ), opt->generatorRunning);

    pConf->Write(_T ( "ShowLayoutP" ), opt->layoutShow);

    pConf->Write(_T ( "toggleEngine1" ), opt->toggleEngine1);
    pConf->Write(_T ( "toggleEngine2" ), opt->toggleEngine2);
    pConf->Write(_T ( "toggleGenerator" ), opt->toggleGenerator);
    pConf->Write(_T ( "numberofSails" ), opt->numberSails);

    wxString sails = wxEmptyString;
    sails = wxString::Format("%i,%i,", opt->rowGap, opt->colGap);
    for (int i = 0; i < opt->numberSails; i++)
      sails += wxString::Format("%s,%s,%i,", opt->abrSails.Item(i).c_str(),
                                opt->sailsName.Item(i).c_str(),
                                opt->bSailIsChecked[i]);
    sails.RemoveLast();
    pConf->Write(_T ( "Sails" ), sails);

    if (opt->dtEngine1On.IsValid())
      pConf->Write(_T ( "Engine1TimeStart" ),
                   opt->dtEngine1On.FormatISODate() + " " +
                       opt->dtEngine1On.FormatISOTime());
    else
      pConf->Write(_T ( "Engine1TimeStart" ), wxEmptyString);

    if (opt->dtEngine2On.IsValid())
      pConf->Write(_T ( "Engine2TimeStart" ),
                   opt->dtEngine2On.FormatISODate() + " " +
                       opt->dtEngine2On.FormatISOTime());
    else
      pConf->Write(_T ( "Engine2TimeStart" ), wxEmptyString);

    if (opt->dtGeneratorOn.IsValid())
      pConf->Write(_T ( "GeneratorTimeStart" ),
                   opt->dtGeneratorOn.FormatISODate() + " " +
                       opt->dtGeneratorOn.FormatISOTime());
    else
      pConf->Write(_T ( "GeneratorTimeStart" ), wxEmptyString);

    writeCols(pConf, opt->NavColWidth, "NavGridColWidth");
    writeCols(pConf, opt->WeatherColWidth, "WeatherGridColWidth");
    writeCols(pConf, opt->MotorColWidth, "MotorGridColWidth");
    writeCols(pConf, opt->CrewColWidth, "CrewGridColWidth");
    writeCols(pConf, opt->WakeColWidth, "WakeGridColWidth");
    writeCols(pConf, opt->EquipColWidth, "EquipGridColWidth");
    writeCols(pConf, opt->OverviewColWidth, "OverviewGridColWidth");
    writeCols(pConf, opt->ServiceColWidth, "ServiceGridColWidth");
    writeCols(pConf, opt->RepairsColWidth, "RepairsGridColWidth");
    writeCols(pConf, opt->BuyPartsColWidth, "BuyPartsGridColWidth");
  }
}

void logbookkonni_pi::writeCols(wxFileConfig* pConf, ArrayOfGridColWidth ar,
                                wxString entry) {
  wxString str = wxEmptyString;
  for (unsigned int i = 0; i < ar.Count(); i++)
    str += wxString::Format("%i,", ar[i]);
  str.RemoveLast();
  pConf->Write(entry, str);
}

void logbookkonni_pi::LoadConfig() {
  wxFileConfig* pConf = (wxFileConfig*)m_pconfig;

  if (pConf) {
    pConf->SetPath("/PlugIns/Logbook");
    pConf->Read("ShowLOGIcon", &m_bLOGShowIcon, 1);
    pConf->Read("Traditional", &opt->traditional, 1);
    pConf->Read("ToolTips", &opt->showToolTips);
    pConf->Read("FirstTime", &opt->firstTime);
#ifdef __WXMSW__
    pConf->Read("DlgWidth", &opt->dlgWidth, 1010);
#elif defined __WXGTK__
    pConf->Read("DlgWidth", &opt->dlgWidth, 1085);
#elif defined __WXOSX__
    pConf->Read("DlgWidth", &opt->dlgWidth, 1085);
#endif
    pConf->Read("DlgHeight", &opt->dlgHeight, 535);
    pConf->Read("Popup", &opt->popup, true);
    pConf->Read("AutoStartTimer", &opt->autostarttimer, false);

    pConf->Read(_T ( "DateFormat" ), &opt->dateformat, 0);
    pConf->Read(_T ( "DateSepIndiv" ), &opt->dateseparatorindiv);
    pConf->Read(_T ( "DateSepLocale" ), &opt->dateseparatorlocale);
    pConf->Read(_T ( "Date1" ), &opt->date1, 0);
    pConf->Read(_T ( "Date2" ), &opt->date2, 1);
    pConf->Read(_T ( "Date3" ), &opt->date3, 2);
    pConf->Read(_T ( "NoEngines" ), &opt->engines, 0);

    pConf->Read(_T ( "TimeFormat" ), &opt->timeformat, -1);
    pConf->Read(_T ( "NoSeconds" ), &opt->noseconds);

    pConf->Read(_T ( "GuardChange" ), &opt->guardChange);
    pConf->Read(_T ( "GuardChangeText" ), &opt->guardChangeText);
    pConf->Read(_T ( "WaypointArrived" ), &opt->waypointArrived);
    pConf->Read(_T ( "WayPointText" ), &opt->waypointText);
    pConf->Read(_T ( "CourseChange" ), &opt->courseChange);
    pConf->Read(_T ( "CouseChangeDegrees" ), &opt->courseChangeDegrees);
    pConf->Read(_T ( "CouseChangeAfter" ), &opt->courseTextAfterMinutes);
    pConf->Read(_T ( "CourseChangeText" ), &opt->courseChangeText);
    pConf->Read(_T ( "EverySM" ), &opt->everySM);
    pConf->Read(_T ( "EverySMAmount" ), &opt->everySMAmount);
    pConf->Read(_T ( "everySMText" ), &opt->everySMText);
    opt->courseChangeDegrees.ToDouble(&opt->dCourseChangeDegrees);
    opt->everySMAmount.ToDouble(&opt->dEverySM);

    pConf->Read(_T ( "Timer" ), &opt->timer);
    pConf->Read(_T ( "TimerType" ), &opt->timerType);
    pConf->Read(_T ( "Local" ), &opt->local);
    pConf->Read(_T ( "UTC" ), &opt->UTC);
    pConf->Read(_T ( "GPSAuto" ), &opt->gpsAuto);
    pConf->Read(_T ( "TzIndicator" ), &opt->tzIndicator);
    pConf->Read(_T ( "TzHours" ), &opt->tzHour);
    pConf->Read(_T ( "TimerHours" ), &opt->thour, "0");
    pConf->Read(_T ( "TimerMin" ), &opt->tmin, "1");
    pConf->Read(_T ( "TimerSec" ), &opt->tsec, "0");
    pConf->Read(_T ( "TimerText" ), &opt->ttext);

    opt->timerSec = (wxAtol(opt->thour) * 3600000 + wxAtol(opt->tmin) * 60000 +
                     wxAtol(opt->tsec) * 1000);

    pConf->Read(_T ( "NavDegrees" ), &opt->Deg);
    pConf->Read(_T ( "NavMin" ), &opt->Min);
    pConf->Read(_T ( "NavSec" ), &opt->Sec);

    pConf->Read(_T ( "ShowDistance" ), &opt->showDistance);
    pConf->Read(_T ( "showBoatSpeed" ), &opt->showBoatSpeed);
    pConf->Read(_T ("ShowDistanceInd"), &opt->showDistanceChoice);
    pConf->Read(_T ("ShowBoatSpeedInd"), &opt->showBoatSpeedchoice);

    pConf->Read(_T ( "Baro" ), &opt->baro);
    pConf->Read(_T ( "Temperature" ), &opt->temperature);
    pConf->Read(_T ( "NavMeter" ), &opt->meter);
    pConf->Read(_T ( "NavFeet" ), &opt->feet);
    pConf->Read(_T ( "NavFathom" ), &opt->fathom);

    pConf->Read(_T ( "Vol" ), &opt->vol);
    pConf->Read(_T ( "Motorhours" ), &opt->motorh);
    pConf->Read(_T ( "Engine" ), &opt->engine, "E");
    pConf->Read(_T ( "Shaft" ), &opt->shaft, "S");
    pConf->Read(_T ( "RPM" ), &opt->rpm, "RPM");

    pConf->Read(_T ( "Days" ), &opt->days);
    pConf->Read(_T ( "Weeks" ), &opt->weeks);
    pConf->Read(_T ( "Month" ), &opt->month);

    pConf->Read(_T ( "Watermaker" ), &opt->watermaker);
    pConf->Read(_T ( "FuelTank" ), &opt->fuelTank);
    pConf->Read(_T ( "WaterTank" ), &opt->waterTank);
    pConf->Read(_T ( "Ampere" ), &opt->ampere);
    pConf->Read(_T ( "Bank1" ), &opt->bank1);
    pConf->Read(_T ( "Bank2" ), &opt->bank2);

    pConf->Read(_T ( "ShowDepth" ), &opt->showDepth);
    pConf->Read(_T ( "ShowWaveSwell" ), &opt->showWaveSwell);
    pConf->Read(_T ( "ShowWindSpeedInd" ), &opt->showWindSpeedchoice);
    pConf->Read(_T ( "ShowWindSpeed" ), &opt->showWindSpeed);
    pConf->Read(_T ( "ShowWindDir" ), &opt->showWindDir);
    pConf->Read(_T ( "ShowHeading" ), &opt->showHeading);
    pConf->Read(_T ( "ShowWindHeading" ), &opt->showWindHeading);

    pConf->Read(_T ( "NavHTML" ), &opt->navHTML, 1);
    pConf->Read(_T ( "CrewHTML" ), &opt->crewHTML, 1);
    pConf->Read(_T ( "BoatHTML" ), &opt->boatHTML, 1);
    pConf->Read(_T ( "overviewHTML" ), &opt->overviewHTML, 1);
    pConf->Read(_T ( "serviceHTML" ), &opt->serviceHTML, 1);
    pConf->Read(_T ( "repairsHTML" ), &opt->repairsHTML, 1);
    pConf->Read(_T ( "buypartsHTML" ), &opt->buypartsHTML, 1);
    pConf->Read(_T ( "OverViewAll" ), &opt->overviewAll, 0);

    pConf->Read(_T ( "NavGridLayout" ), &opt->navGridLayoutChoice, 0);
    pConf->Read(_T ( "CrewGridLayout" ), &opt->crewGridLayoutChoice, 0);
    pConf->Read(_T ( "BoatGridLayout" ), &opt->boatGridLayoutChoice, 0);
    pConf->Read(_T ( "overviewGridLayout" ), &opt->overviewGridLayoutChoice, 0);
    pConf->Read(_T ( "serviceGridLayout" ), &opt->serviceGridLayoutChoice, 0);
    pConf->Read(_T ( "repairsGridLayout" ), &opt->repairsGridLayoutChoice, 0);
    pConf->Read(_T ( "buypartsGridLayout" ), &opt->buypartsGridLayoutChoice, 0);

    pConf->Read(_T ( "NavGridLayoutODT" ), &opt->navGridLayoutChoiceODT, 0);
    pConf->Read(_T ( "CrewGridLayoutODT" ), &opt->crewGridLayoutChoiceODT, 0);
    pConf->Read(_T ( "BoatGridLayoutODT" ), &opt->boatGridLayoutChoiceODT, 0);
    pConf->Read(_T ( "overviewGridLayoutODT" ),
                &opt->overviewGridLayoutChoiceODT, 0);
    pConf->Read(_T ( "serviceGridLayoutODT" ), &opt->serviceGridLayoutChoiceODT,
                0);
    pConf->Read(_T ( "repairsGridLayoutODT" ), &opt->repairsGridLayoutChoiceODT,
                0);
    pConf->Read(_T ( "buypartsGridLayout" ), &opt->buypartsGridLayoutChoiceODT,
                0);

    pConf->Read(_T ( "HTMLEditor" ), &opt->htmlEditor);
    pConf->Read(_T ( "ODTEditor" ), &opt->odtEditor);
    pConf->Read(_T ( "DataManager" ), &opt->dataManager);
    pConf->Read(_T ( "MailClient" ), &opt->mailClient);

    pConf->Read(_T ( "GPSWarning" ), &opt->noGPS);
    pConf->Read(_T ( "EngineMessageSails" ), &opt->engineMessageSails);
    pConf->Read(_T ( "WriteEngineRun" ), &opt->engineMessageRunning);
    pConf->Read(_T ( "SailsDown" ), &opt->engineAllwaysSailsDown);
    pConf->Read(_T ( "StatusBar" ), &opt->statusbar);
    pConf->Read(_T ( "WindSpeeds" ), &opt->windspeeds);
    pConf->Read(_T ( "OverviewLines" ), &opt->overviewlines);

    wxString str = wxEmptyString;
    pConf->Read(_T ( "PrefixLayouts" ), &str);
    if (str.Contains(",")) {
      wxStringTokenizer tkz(str, ",");
      for (int i = 0; i < 7; i++) {
        opt->filterLayout[i] = (wxAtoi(tkz.GetNextToken())) ? true : false;
        opt->layoutPrefix[i] = tkz.GetNextToken();
      }
    }

    wxString kmlRouteTrack = wxEmptyString;
    pConf->Read(_T ( "KMLRouteTrack" ), &kmlRouteTrack, "1,1");
    wxStringTokenizer tkz(kmlRouteTrack, ",");
    opt->kmlRoute = wxAtoi(tkz.GetNextToken());
    opt->kmlTrack = wxAtoi(tkz.GetNextToken());
    pConf->Read(_T ( "KMLWidth" ), &opt->kmlLineWidth, "4");
    pConf->Read(_T ( "KMLTransp" ), &opt->kmlLineTransparancy, 0);
    pConf->Read(_T ( "KMLRouteColor" ), &opt->kmlRouteColor, 0);
    pConf->Read(_T ( "KMLTrackColor" ), &opt->kmlTrackColor, 3);

    pConf->Read(_T ( "RPMIsChecked" ), &opt->bRPMIsChecked, false);
    pConf->Read(_T ( "Eng1RPMIsChecked" ), &opt->bEng1RPMIsChecked, false);
    pConf->Read(_T ( "Eng2RPMIsChecked" ), &opt->bEng2RPMIsChecked, false);
    pConf->Read(_T ( "GenRPMIsChecked" ), &opt->bGenRPMIsChecked, false);

    pConf->Read(_T ( "NMEAUseRPM" ), &opt->NMEAUseERRPM, false);
    pConf->Read(_T ( "Engine1" ), &opt->engine1Id, "");
    pConf->Read(_T ( "Engine2" ), &opt->engine2Id, "");
    pConf->Read(_T ( "Engine1Runs" ), &opt->engine1Running);
    pConf->Read(_T ( "Engine2Runs" ), &opt->engine2Running);

    pConf->Read(_T ( "Generator" ), &opt->generator, false);
    pConf->Read(_T ( "GeneratorId" ), &opt->generatorId, "");
    pConf->Read(_T ( "GeneratorRuns" ), &opt->generatorRunning);

    pConf->Read(_T ( "ShowLayoutP" ), &opt->layoutShow, true);

    pConf->Read(_T ( "toggleEngine1" ), &opt->toggleEngine1);
    pConf->Read(_T ( "toggleEngine2" ), &opt->toggleEngine2);
    pConf->Read(_T ( "toggleGenerator" ), &opt->toggleGenerator);
    pConf->Read(_T ( "numberofSails" ), &opt->numberSails, 14);

    wxString sails = wxEmptyString;
    pConf->Read(_T ( "Sails" ), &sails);
    if (!sails.IsEmpty()) {
      wxStringTokenizer tkz(sails, ",");
      if (wxString(sails.GetChar(0)).IsNumber()) {
        opt->rowGap = wxAtoi(tkz.GetNextToken());
        opt->colGap = wxAtoi(tkz.GetNextToken());
      }

      opt->abrSails.Empty();
      opt->sailsName.Empty();

      opt->abrSails.SetCount(opt->numberSails);
      opt->sailsName.SetCount(opt->numberSails);

      for (int i = 0; i < opt->numberSails; i++) {
        opt->abrSails.Item(i) = tkz.GetNextToken();
        opt->sailsName.Item(i) = tkz.GetNextToken();
        opt->bSailIsChecked[i] = (wxAtoi(tkz.GetNextToken()) ? true : false);
      }
    }

    wxString engine1 = wxEmptyString, engine2 = wxEmptyString,
             genny = wxEmptyString;
    pConf->Read(_T ( "Engine1TimeStart" ), &engine1);
    pConf->Read(_T ( "Engine2TimeStart" ), &engine2);
    pConf->Read(_T ( "GeneratorTimeStart" ), &genny);

    if (!engine1.IsEmpty()) {
      wxStringTokenizer tkz(engine1, " ");
      wxString date = tkz.GetNextToken();
      wxString time = tkz.GetNextToken();

      wxDateTime dt;
      dt.ParseDate(date);
      dt.ParseTime(time);

      if (dt.GetYear() != 1970) opt->dtEngine1On = dt;
    }

    if (!engine2.IsEmpty()) {
      wxStringTokenizer tkz(engine2, " ");
      wxString date = tkz.GetNextToken();
      wxString time = tkz.GetNextToken();

      wxDateTime dt;
      dt.ParseDate(date);
      dt.ParseTime(time);

      if (dt.GetYear() != 1970) opt->dtEngine2On = dt;
    }

    if (!genny.IsEmpty()) {
      wxStringTokenizer tkz(genny, " ");
      wxString date = tkz.GetNextToken();
      wxString time = tkz.GetNextToken();

      wxDateTime dt;
      dt.ParseDate(date);
      dt.ParseTime(time);

      if (dt.GetYear() != 1970) opt->dtGeneratorOn = dt;
    }

    bool r;
    r = pConf->Read(_T ( "NavGridColWidth" ), &str);
    if (r)
      opt->NavColWidth = readCols(opt->NavColWidth, str);
    else
      opt->NavColWidth =
          readColsOld(pConf, opt->NavColWidth, _T ( "NavGridColWidth" ));

    r = pConf->Read(_T ( "WeatherGridColWidth" ), &str);
    if (r)
      opt->WeatherColWidth = readCols(opt->WeatherColWidth, str);
    else
      opt->WeatherColWidth = readColsOld(pConf, opt->WeatherColWidth,
                                         _T ( "WeatherGridColWidth" ));

    r = pConf->Read(_T ( "MotorGridColWidth" ), &str);
    if (r)
      opt->MotorColWidth = readCols(opt->MotorColWidth, str);
    else
      opt->MotorColWidth =
          readColsOld(pConf, opt->MotorColWidth, _T ( "MotorGridColWidth" ));

    r = pConf->Read(_T ( "CrewGridColWidth" ), &str);
    if (r)
      opt->CrewColWidth = readCols(opt->CrewColWidth, str);
    else
      opt->CrewColWidth =
          readColsOld(pConf, opt->CrewColWidth, _T ( "CrewGridColWidth" ));

    r = pConf->Read(_T ( "WakeGridColWidth" ), &str);
    if (r)
      opt->WakeColWidth = readCols(opt->WakeColWidth, str);
    else
      opt->WakeColWidth =
          readColsOld(pConf, opt->WakeColWidth, _T ( "WakeGridColWidth" ));

    r = pConf->Read(_T ( "EquipGridColWidth" ), &str);
    if (r)
      opt->EquipColWidth = readCols(opt->EquipColWidth, str);
    else
      opt->EquipColWidth =
          readColsOld(pConf, opt->EquipColWidth, _T ( "EquipGridColWidth" ));

    r = pConf->Read(_T ( "OverviewGridColWidth" ), &str);
    if (r)
      opt->OverviewColWidth = readCols(opt->OverviewColWidth, str);
    else
      opt->OverviewColWidth = readColsOld(pConf, opt->OverviewColWidth,
                                          _T ( "OverviewGridColWidth" ));

    r = pConf->Read(_T ( "ServiceGridColWidth" ), &str);
    if (r)
      opt->ServiceColWidth = readCols(opt->ServiceColWidth, str);
    else
      opt->ServiceColWidth = readColsOld(pConf, opt->ServiceColWidth,
                                         _T ( "ServiceGridColWidth" ));

    r = pConf->Read(_T ( "RepairsGridColWidth" ), &str);
    if (r)
      opt->RepairsColWidth = readCols(opt->RepairsColWidth, str);
    else
      opt->RepairsColWidth = readColsOld(pConf, opt->RepairsColWidth,
                                         _T ( "RepairsGridColWidth" ));

    r = pConf->Read(_T ( "BuyPartsGridColWidth" ), &str);
    if (r)
      opt->BuyPartsColWidth = readCols(opt->BuyPartsColWidth, str);
    else
      opt->BuyPartsColWidth = readColsOld(pConf, opt->BuyPartsColWidth,
                                          _T ( "BuyPartsGridColWidth" ));

    pConf->DeleteEntry(_T ( "ShowAllLayout" ));
    pConf->DeleteEntry(_T ( "ShowFilteredLayout" ));

    opt->ampereh = opt->ampere + opt->motorh;
  }

  if (opt->timeformat == -1) {
    wxString am, pm;
    wxDateTime::GetAmPmStrings(&am, &pm);
    if (am.IsEmpty())
      opt->timeformat = 0;
    else
      opt->timeformat = 1;
  }
}

ArrayOfGridColWidth logbookkonni_pi::readCols(ArrayOfGridColWidth ar,
                                              wxString str) {
  wxStringTokenizer tkz(str, ",");
  while (tkz.HasMoreTokens()) ar.Add(wxAtoi(tkz.GetNextToken()));

  return ar;
}

ArrayOfGridColWidth logbookkonni_pi::readColsOld(wxFileConfig* pConf,
                                                 ArrayOfGridColWidth ar,
                                                 wxString entry) {
  int val;
  bool r;
  int i = 0;

  while (true) {
    r = pConf->Read(wxString::Format(entry + _T ( "/%i" ), i++), &val);
    if (!r) break;
    ar.Add(val);
  }
  pConf->DeleteGroup(wxString("/PlugIns/Logbook/") + entry);

  return ar;
}

static std::string get_layoutdir(const std::string base,
                                 const std::string subdir) {
  wxString path = base;
  const wxString sep = wxFileName::GetPathSeparator();
  path.append(sep);
  path.append("data");
  path.append(sep);
  path.append(subdir);
  if (!wxDir::Exists(path)) {
    wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }
  return path.ToStdString();
}

void logbookkonni_pi::loadLayouts(wxWindow* parent) {
  wxString FILE = "LogbookKonni*.zip";
  std::unique_ptr<wxZipEntry> entry;
  wxString path;
  wxString sep = wxFileName::GetPathSeparator();
  wxString basedir = GetPluginDataDir("LogbookKonni_pi") + sep + "data";

#ifdef __WXOSX__
  wxFileDialog* openFileDialog = new wxFileDialog(
      parent, _("Select zipped Layout-Files"), basedir, "", "*.zip");
#else
  wxFileDialog* openFileDialog = new wxFileDialog(
      parent, _("Select zipped Layout-Files"), basedir, "", "*.zip", wxFD_OPEN);
#endif
  int ret = true;
  if (openFileDialog->ShowModal() == wxID_OK) {
    wxFFileInputStream in(openFileDialog->GetPath());
    wxZipInputStream zip(in);
    wxString ok = wxString(_("Layouts installed at\n"));
    std::string configdir(StandardPath().ToStdString());

    while (entry.reset(zip.GetNextEntry()), entry.get() != NULL) {
      if (entry->GetName().Contains("HTMLLayouts"))
        path = get_layoutdir(configdir, "HTMLLayouts");
      else if (entry->GetName().Contains("ODTLayouts"))
        path = get_layoutdir(configdir, "ODTLayouts");
      else if (entry->GetName().Contains("Clouds"))
        path = get_layoutdir(configdir, "Clouds");
      else
        path = get_layoutdir(configdir, "Images");
      path += sep;

      wxString name = entry->GetName();
      if (!name.Contains(".htm") && !name.Contains(".odt") &&
          !name.Contains(".jpg") && !name.Contains(".PNG")) {
        continue;
      }
      if (ok.Find(path) == wxNOT_FOUND) {
        ok += path + "\n";
      }
      wxString fn = name.AfterLast(wxFileName::GetPathSeparator());

      if (name.Contains(sep + "boat"))
        path.append("boat");
      else if (name.Contains(sep + "logbook"))
        path.append("logbook");
      else if (name.Contains(sep + "crew"))
        path.append("crew");
      else if (name.Contains(sep + "overview"))
        path.append("overview");
      else if (name.Contains(sep + "service"))
        path.append("service");
      else if (name.Contains(sep + "repairs"))
        path.append("repairs");
      else if (name.Contains(sep + "buyparts"))
        path.append("buyparts");

      if (!name.Contains("Help")) {
        path.append(sep);
        if (!wxFileName::DirExists(path)) wxMkdir(path);
      } else {
        path = StandardPath() + sep + "data" + sep;
      }
      path.append(fn);

      wxFileOutputStream out(path);
      if (zip.OpenEntry(*entry) != true) {
        out.Close();
        ret = false;
        break;
      }
      zip.Read(out);
      out.Close();
    }
    if (m_plogbook_window) {
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::LOGBOOK, m_plogbook_window->logbook->layout_locn,
          m_plogbook_window->logbookChoice,
          opt->layoutPrefix[LogbookDialog::LOGBOOK]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::CREW, m_plogbook_window->crewList->layout_locn,
          m_plogbook_window->crewChoice,
          opt->layoutPrefix[LogbookDialog::CREW]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::BOAT, m_plogbook_window->boat->layout_locn,
          m_plogbook_window->boatChoice,
          opt->layoutPrefix[LogbookDialog::BOAT]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::OVERVIEW, m_plogbook_window->logbook->layout_locn,
          m_plogbook_window->overviewChoice,
          opt->layoutPrefix[LogbookDialog::OVERVIEW]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::GSERVICE,
          m_plogbook_window->maintenance->layout_locnService,
          m_plogbook_window->m_choiceSelectLayoutService,
          opt->layoutPrefix[LogbookDialog::GSERVICE]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::GREPAIRS,
          m_plogbook_window->maintenance->layout_locnRepairs,
          m_plogbook_window->m_choiceSelectLayoutRepairs,
          opt->layoutPrefix[LogbookDialog::GREPAIRS]);
      m_plogbook_window->loadLayoutChoice(
          LogbookDialog::GBUYPARTS,
          m_plogbook_window->maintenance->layout_locnBuyParts,
          m_plogbook_window->m_choiceSelectLayoutBuyParts,
          opt->layoutPrefix[LogbookDialog::GBUYPARTS]);
    }
    ok = ret ? ok : "";
    wxMessageBox(ok);
    if (ret) {
      opt->navGridLayoutChoice = 0;
      opt->crewGridLayoutChoice = 0;
      opt->boatGridLayoutChoice = 0;
    }
  }
}

////////////////////////////////////////////////////////
void LogbookTimer::OnTimer(wxTimerEvent& ev) { timerEvent(); }

void LogbookTimer::timerEvent() {
  if (popUp()) plogbook_pi->m_plogbook_window->logbook->appendRow(true, true);
}

bool LogbookTimer::popUp() {
  if (plogbook_pi->eventsEnabled || NULL == plogbook_pi->m_plogbook_window)
    return false;

  wxFrame* frame = (wxFrame*)plogbook_pi->m_parent_window->GetParent();
  if ((frame->IsIconized() || plogbook_pi->m_plogbook_window->IsIconized()) &&
      plogbook_pi->opt->popup) {
    if (frame->IsIconized()) frame->Iconize(false);

    plogbook_pi->m_plogbook_window->Iconize(false);
    plogbook_pi->m_parent_window->SetFocus();
    return true;
  }

  if (!plogbook_pi->m_plogbook_window->IsShown() && plogbook_pi->opt->popup) {
    plogbook_pi->m_plogbook_window->Show();
    plogbook_pi->SendLogbookMessage("LOGBOOK_WINDOW_SHOWN", wxEmptyString);
    plogbook_pi->dlgShow = true;
  }

  if (plogbook_pi->opt->popup) plogbook_pi->m_plogbook_window->Raise();

  return true;
}
