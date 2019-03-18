/******************************************************************************
* $Id: logbookkonni_pi.cpp,v 1.8 2010/06/21 01:54:37 bdbcat Exp $
*
* Project:  OpenCPN
* Purpose:  DEMO Plugin
* Author:   David Register
*
***************************************************************************
*   Copyright (C) 2010 by David S. Register   *
*   $EMAIL$   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
***************************************************************************
*/
//#define _2_9_x_ // uncomment this to compile for 2.9.x

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "wx/wxprec.h"

#include "logbook_pi.h"
#include "Logbook.h"
#include "LogbookDialog.h"
#include "LogbookOptions.h"
#include "Options.h"
#include "wx/stdpaths.h"
#include <wx/timer.h>
#include <wx/event.h>
#include <wx/sysopt.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <memory>

#include "jsonreader.h"

using namespace std;

#ifndef DECL_EXP
#ifdef __WXMSW__
#define DECL_EXP     __declspec(dllexport)
#else
#define DECL_EXP
#endif
#endif

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi( void *ppimgr )
{
    return ( opencpn_plugin * )new logbookkonni_pi( ppimgr );
}

extern "C" DECL_EXP void destroy_pi( opencpn_plugin* p )
{
    delete p;
}

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------
logbookkonni_pi::logbookkonni_pi( void *ppimgr )
    :opencpn_plugin_17( ppimgr )
{
    // Create the PlugIn icons
    initialize_images();
    opt = new Options();
    m_timer = NULL;
    state = 0;
}


logbookkonni_pi::~logbookkonni_pi()
{
    if ( m_timer != NULL && m_timer->IsRunning() )
    {
        m_timer->Stop();
        m_timer = NULL;
    }
    if ( opt != NULL )
        delete opt;
}

int logbookkonni_pi::Init( void )
{
    state = OFF;
    dlgShow = false;

    AddLocaleCatalog( _T( "opencpn-logbookkonni_pi" ) );

    m_plogbook_window = NULL;
    lastWaypointInRoute = _T( "-1" );
    eventsEnabled = true;

    opt = new Options();
    // Get a pointer to the opencpn display canvas, to use as a parent for windows created
    m_parent_window = GetOCPNCanvasWindow();

    m_pconfig = GetOCPNConfigObject();

    LoadConfig();
    if ( m_bLOGShowIcon )
        m_leftclick_tool_id  = InsertPlugInTool( _T( "" ), _img_logbook_pi, _img_logbook_pi, wxITEM_NORMAL,
                               _( "Logbook" ), _T( "" ), NULL,
                               LOGBOOK_TOOL_POSITION, 0, this );

    // Create the Context Menu Items

    //    In order to avoid an ASSERT on msw debug builds,
    //    we need to create a dummy menu to act as a surrogate parent of the created MenuItems
    //    The Items will be re-parented when added to the real context meenu
    wxMenu dummy_menu;

    timer = new LogbookTimer( this );
    m_timer = new wxTimer( timer,ID_LOGTIMER );
    timer->Connect( wxEVT_TIMER, wxObjectEventFunction( &LogbookTimer::OnTimer ) );
    
    SendPluginMessage( _T( "LOGBOOK_READY_FOR_REQUESTS" ), _T( "TRUE" ) );

    return (
               WANTS_CURSOR_LATLON       |
               WANTS_TOOLBAR_CALLBACK    |
               INSTALLS_TOOLBAR_TOOL     |
               WANTS_CONFIG              |
               WANTS_PREFERENCES         |
               WANTS_NMEA_SENTENCES      |
               WANTS_NMEA_EVENTS		 |
               USES_AUI_MANAGER			 |
               WANTS_PLUGIN_MESSAGING
           );
}

bool logbookkonni_pi::DeInit( void )
{
    shutdown( false );
    return true;
}

void logbookkonni_pi::shutdown( bool menu )
{
    SendPluginMessage( _T( "LOGBOOK_READY_FOR_REQUESTS" ), _T( "FALSE" ) );

    if ( m_timer )
        if ( m_timer->IsRunning() )
            m_timer->Stop();
    if ( timer )
    {
        timer->Disconnect( wxEVT_TIMER, wxObjectEventFunction( &LogbookTimer::OnTimer ) );
        delete timer;
    }

    timer = NULL;
    m_timer = NULL;

    if ( m_plogbook_window != NULL )
    {
        if ( m_plogbook_window->IsIconized() ) m_plogbook_window->Iconize( false );
        m_plogbook_window->setIniValues();

        if ( ( opt->engine1Running && opt->toggleEngine1 )
                || ( opt->engine2Running && opt->toggleEngine2 )
                || ( opt->generatorRunning && opt->toggleGenerator ) )
        {
            int a = wxMessageBox( _( "Your engine(s) are still running\n\nStop engine(s) ?" ),_T( "" ),wxYES_NO | wxICON_QUESTION, NULL );
            if ( a == wxYES )
                m_plogbook_window->logbook->resetEngineManualMode( 0 );
        }
        SaveConfig();
        m_plogbook_window->Close();
        m_plogbook_window->Destroy();
        m_plogbook_window = NULL;
        dlgShow = false;
        //	SetToolbarItemState( m_leftclick_tool_id, dlgShow );
    }
}


void logbookkonni_pi::SetPluginMessage( wxString &message_id, wxString &message_body )
{
    if ( message_id == _T( "OCPN_MAN_OVERBOARD" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        m_plogbook_window->logbook->MOB_GUID = data.Item( _T( "GUID" ) ).AsString();
        m_plogbook_window->logbook->MOBIsActive = true;

        m_plogbook_window->logbook->appendRow( true, false );
    }
    else if ( message_id == _T( "POLAR_SAVE_LOGBOOK" ) )
    {
        if ( m_plogbook_window )
            m_plogbook_window->logbook->update();
    }
    else if ( message_id == _T( "LOGBOOK_LOG_LASTLINE_REQUEST" ) )
    {
        if ( !m_plogbook_window )
            startLogbook();

        wxJSONValue key;
        int tcol = 0;
        int lastRow = m_plogbook_window->logGrids[0]->GetNumberRows()-1;
        for ( unsigned int g = 0; g < LOGGRIDS; g++ )
            for ( int col = 0; col < m_plogbook_window->logGrids[g]->GetNumberCols(); col++ )
                key[tcol++] = m_plogbook_window->logGrids[g]->GetCellValue( lastRow,col );
        wxJSONWriter w;
        wxString out;
        w.Write( key, out );
        wxString id = _T( "LOGBOOK_LOG_LASTLINE_RESPONSE" );
        SetPluginMessage( id,out );
        return;
    }
    else if ( message_id == _T( "LOGBOOK_IS_READY_FOR_REQUEST" ) )
    {
        SendPluginMessage( _T( "LOGBOOK_READY_FOR_REQUESTS" ), _T( "TRUE" ) );
    }
    else if ( message_id == _T( "LOGBOOK_BUYPARTS_ADDLINE_REQUEST" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int priority, amount;
        wxString category, title, unit, text, plugin;
        wxString prText[6];

        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        m_plogbook_window->Show();
        m_plogbook_window->m_logbook->SetSelection( 4 ); // Maintenance
        m_plogbook_window->m_notebook6->SetSelection( 2 ); // BuyParts

        for ( int i = 0; i < data.Size(); i++ )
        {
            priority = data[i].Item( _T( "Priority" ) ).AsInt();
            category = data[i].Item( _T( "Category" ) ).AsString();
            title    = _( "from " );
            plugin = data[i].Item( _T( "PluginName" ) ).AsString();
            title   += plugin + _( "-Plugin" );
            amount   = data[i].Item( _T( "Amount" ) ).AsInt();
            unit     = data[i].Item( _T( "Unit" ) ).AsString();
            text     = data[i].Item( _T( "Text" ) ).AsString();

            prText[priority] += wxString::Format( _T( "%4i  %-15s %-30s\n" ),amount,unit.c_str(),text.c_str() );

        }

        if ( plugin == _T( "FindIt" ) )
            m_plogbook_window->maintenance->deleteFindItRow( category,plugin );

        for ( int i = 0; i < 6; i++ )
        {
            if ( prText[i] != wxEmptyString )
            {
                m_plogbook_window->maintenance->addLineBuyParts();

                int lastRow = m_plogbook_window->m_gridMaintenanceBuyParts->GetNumberRows()-1;

                m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue( lastRow,0,wxString::Format( _T( "%i" ),i ) );
                m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue( lastRow,1,category );
                m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue( lastRow,2,title );
                m_plogbook_window->m_gridMaintenanceBuyParts->SetCellValue( lastRow,3,prText[i].RemoveLast() );
                m_plogbook_window->m_gridMaintenanceBuyParts->AutoSizeRow( lastRow,false );
            }
        }

        m_plogbook_window->maintenance->checkBuyParts();

        return;
    }
    else if ( message_id == _T( "LOGBOOK_LOG_ADDLINE_REQUEST" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        m_plogbook_window->logbook->appendRow( true, true );
        int lastRow = m_plogbook_window->m_gridGlobal->GetNumberRows()-1;

        m_plogbook_window->m_gridGlobal->SetCellValue( lastRow,13,data.Item( _T( "Remarks" ) ).AsString() );
        m_plogbook_window->m_gridMotorSails->SetCellValue( lastRow,8,data.Item( _T( "MotorRemarks" ) ).AsString() );
        return;
    }
    else if ( message_id == _T( "OCPN_WPT_ARRIVED" ) )
    {
        if ( !opt->waypointArrived || eventsEnabled ) return;

        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        RMB rmb;
        rmb.From = data.Item( _T( "WP_arrived" ) ).AsString();
        rmb.To   = lastWaypointInRoute =  data.Item( _T( "Next_WP" ) ).AsString();
        m_plogbook_window->logbook->WP_skipped = data.Item( _T( "isSkipped" ) ).AsBool();
        m_plogbook_window->logbook->OCPN_Message = true;

        m_plogbook_window->logbook->checkWayPoint( rmb );

        m_plogbook_window->logbook->OCPN_Message = false;
        m_plogbook_window->logbook->WP_skipped = false;
    }
    else if ( message_id == _T( "OCPN_RTE_ENDED" ) )
    {
        if ( !opt->waypointArrived ) return;

        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        RMB rmb;
        rmb.From = lastWaypointInRoute;
        rmb.To = _T( "-1" );
        m_plogbook_window->logbook->WP_skipped = false;
        m_plogbook_window->logbook->OCPN_Message = true;

        m_plogbook_window->logbook->checkWayPoint( rmb );
        m_plogbook_window->logbook->OCPN_Message = false;
        lastWaypointInRoute = _T( "-1" );
        m_plogbook_window->logbook->lastWayPoint = wxEmptyString;
        m_plogbook_window->logbook->routeIsActive = false;
    }
    else if ( message_id == _T( "OCPN_RTE_DEACTIVATED" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        m_plogbook_window->logbook->activeRoute = wxEmptyString;
        m_plogbook_window->logbook->activeRouteGUID = wxEmptyString;
        m_plogbook_window->logbook->routeIsActive = false;
        if ( !m_plogbook_window->logbook->activeMOB.IsEmpty() )
            m_plogbook_window->logbook->MOBIsActive = false;

    }
    else if ( message_id == _T( "OCPN_RTE_ACTIVATED" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        m_plogbook_window->logbook->activeRoute = data.Item( _T( "Route_activated" ) ).AsString();
        m_plogbook_window->logbook->activeRouteGUID = data.Item( _T( "GUID" ) ).AsString();
        m_plogbook_window->logbook->routeIsActive = true;
    }
    else if ( message_id == _T( "OCPN_TRK_ACTIVATED" ) )
    {
        if ( !m_plogbook_window )
            startLogbook();

        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        m_plogbook_window->logbook->activeTrack = data.Item( _T( "Name" ) ).AsString();
        m_plogbook_window->logbook->activeTrackGUID = data.Item( _T( "GUID" ) ).AsString();
        m_plogbook_window->logbook->trackIsActive = true;
    }
    else if ( message_id == _T( "OCPN_TRK_DEACTIVATED" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        if ( !m_plogbook_window )
            startLogbook();

        if ( m_plogbook_window )
        {
            m_plogbook_window->logbook->activeTrack = wxEmptyString;
            m_plogbook_window->logbook->activeTrackGUID = wxEmptyString;
            m_plogbook_window->logbook->trackIsActive = false;
        }

    }
    else if ( message_id == _T( "OCPN_TRACKPOINTS_COORDS" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        bool error = data[_T( "error" )].AsBool();

        if ( !error )
        {
            double lat = data[_T( "lat" )].AsDouble();
            double lon = data[_T( "lon" )].AsDouble();
            int total = data[_T( "TotalNodes" )].AsInt();
            int i = data[_T( "NodeNr" )].AsInt();
            if ( i == 1 )
            {
                wxString ph = m_plogbook_window->kmlPathHeader;
                ph.Replace( _T( "#NAME#" ),_T( "Trackline" ) );
                ph.Replace( _T( "#LINE#" ),_T( "#LineTrack" ) );
                *m_plogbook_window->logbook->kmlFile << ph;
            }
            if ( i <= total )
                *m_plogbook_window->logbook->kmlFile << wxString::Format( _T( "%f,%f\n" ),lon,lat );
            if ( i == total )
                *m_plogbook_window->logbook->kmlFile << m_plogbook_window->kmlPathFooter;
        }
        //		m_plogbook_window->logbook->writeTrackToKML(data);
    }
    else if ( message_id == _T( "OCPN_TRACKS_MERGED" ) )
    {
        if ( !m_plogbook_window )
            startLogbook();

        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        unsigned int i = 1;
        wxString target = data[_T( "targetTrack" )].AsString();
        while ( true )
        {
            if ( data.HasMember( _T( "mergeTrack" )+wxString::Format( _T( "%d" ),i ) ) )
                m_plogbook_window->logbook->mergeList.Add( data[_T( "mergeTrack" )+wxString::Format( _T( "%d" ),i++ )].AsString() );
            else
                break;

        }
        m_plogbook_window->logbook->setTrackToNewID( target );
    }
    else if ( message_id == _T( "OCPN_ROUTE_RESPONSE" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        bool error = data[0][_T( "error" )].AsBool();

        if ( !error )
            m_plogbook_window->logbook->writeRouteToKML( data );
    }
    else if ( message_id == _T( "OCPN_ROUTELIST_RESPONSE" ) )
    {
        wxJSONReader reader;
        wxJSONValue  data;
        int numErrors = reader.Parse( message_body, &data );
        if ( numErrors != 0 ) return;

        m_plogbook_window->writeToRouteDlg( data );
    }
}

void logbookkonni_pi::startLogbook()
{
    if ( !m_plogbook_window )
    {
        m_plogbook_window = new LogbookDialog( this, m_timer, timer, m_parent_window, wxID_ANY,_( "Active Logbook" ), wxDefaultPosition, wxSize( opt->dlgWidth,opt->dlgHeight ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
        m_plogbook_window->init();
        m_plogbook_window->SetPosition( wxPoint( -1,this->m_parent_window->GetParent()->GetPosition().y+80 ) );

        if ( m_plogbook_window->IsShown() )
            SendPluginMessage( wxString( _T( "LOGBOOK_WINDOW_SHOWN" ) ), wxEmptyString );
        else
            SendPluginMessage( _T( "LOGBOOK_WINDOW_HIDDEN" ), wxEmptyString );
    }

}


void logbookkonni_pi::SendLogbookMessage( wxString message_id, wxString message_body )
{
    SendPluginMessage( message_id,message_body );
}

void logbookkonni_pi::GetOriginalColors()
{
    mcol = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    mcol1 = wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVEBORDER  );
    muitext = wxColour( 0,0,0 );
    mgridline = m_plogbook_window->m_gridGlobal->GetGridLineColour();
    mudkrd = m_plogbook_window->m_gridGlobal->GetCellTextColour( 0,0 );
    mback_color = wxColour( 255,255,255 );
    mtext_color = wxColour( 0,0,0 );
}

void logbookkonni_pi::SetOriginalColors()
{
    col = mcol;
    col1 = mcol1;
    gridline = mgridline;
    uitext = muitext;
    udkrd = mudkrd;
    back_color = mback_color;
    text_color = mtext_color;
}

void logbookkonni_pi::SetColorScheme( PI_ColorScheme cs )
{
    if ( NULL != m_plogbook_window )
    {
        if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
        {
            SetOriginalColors();
        }
        else
        {
            GetGlobalColor( _T( "DILG0" ),&col );   // Dialog Background white
            GetGlobalColor( _T( "DILG1" ),&col1 );  // Dialog Background
            GetGlobalColor( _T( "DILG2" ),&back_color ); // Control Background
            GetGlobalColor( _T( "DILG3" ),&text_color ); // Text
            GetGlobalColor( _T( "UITX1" ),&uitext ); // Menu Text, derived from UINFF
            GetGlobalColor( _T( "UDKRD" ),&udkrd );
            GetGlobalColor( _T( "GREY2" ),&gridline );
        }

        if ( cs == 0 || cs == 1 )
            m_plogbook_window->SetBackgroundColour( wxColour( 255,255,255 ) );
        else
            m_plogbook_window->SetBackgroundColour( col );

        m_plogbook_window->SetForegroundColour( uitext );
        dialogDimmer( cs,m_plogbook_window,col,col1,back_color,text_color,uitext,udkrd );
        m_plogbook_window->Refresh();
    }
}

void logbookkonni_pi::dialogDimmer( PI_ColorScheme cs,wxWindow* ctrl,wxColour col, wxColour col1, wxColour back_color,wxColour text_color,
                                    wxColour uitext, wxColour udkrd )
{
    wxWindowList kids = ctrl->GetChildren();
    for ( unsigned int i = 0 ; i < kids.GetCount() ; i++ )
    {
        wxWindowListNode *node = kids.Item( i );
        wxWindow *win = node->GetData();

        if ( win->IsKindOf( CLASSINFO( wxListBox ) ) )
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                ( ( wxListBox* )win )->SetBackgroundColour( wxNullColour );
            else
                ( ( wxListBox* )win )->SetBackgroundColour( col1 );

        else if ( win->IsKindOf( CLASSINFO( wxChoice ) ) )
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                ( ( wxChoice* )win )->SetBackgroundColour( wxColour( 255,255,255 ) );
            else
                ( ( wxChoice* )win )->SetBackgroundColour( col1 );

        else if ( win->IsKindOf( CLASSINFO( wxRadioButton ) ) )
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                ( ( wxRadioButton* )win )->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
            else
                ( ( wxRadioButton* )win )->SetForegroundColour( col1 );

        else if ( win->IsKindOf( CLASSINFO( wxNotebook ) ) )
        {
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                ( ( wxNotebook* )win )->SetBackgroundColour( wxColour( 255,255,255 ) );
            else
                ( ( wxNotebook* )win )->SetBackgroundColour( col1 );
            ( ( wxNotebook* )win )->SetForegroundColour( text_color );
        }

        else if ( win->IsKindOf( CLASSINFO( wxGrid ) ) )
        {
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
                ( ( wxGrid* )win )->SetDefaultCellBackgroundColour( wxColour( 255,255,255 ) );
            else
                ( ( wxGrid* )win )->SetDefaultCellBackgroundColour( col1 );
            ( ( wxGrid* )win )->SetDefaultCellTextColour( uitext );
            ( ( wxGrid* )win )->SetLabelBackgroundColour( col );
            ( ( wxGrid* )win )->SetLabelTextColour( uitext );
            //( ( wxGrid* )win )->SetDividerPen( wxPen( col ) );
            ( ( wxGrid* )win )->SetGridLineColour( gridline );

        }

        else if ( win->IsKindOf( CLASSINFO( wxButton ) ) )
        {
            if ( cs == PI_GLOBAL_COLOR_SCHEME_DAY || cs == PI_GLOBAL_COLOR_SCHEME_RGB )
            {
                ( ( wxButton* )win )->SetForegroundColour( wxNullColour );
                ( ( wxButton* )win )->SetBackgroundColour( wxNullColour );
            }
            else
            {
                ( ( wxButton* )win )->SetBackgroundColour( col1 );
            }

        }

        else
        {
            ;
        }

        if ( win->GetChildren().GetCount() > 0 )
        {
            wxWindow * w = win;
            dialogDimmer( cs,w,col,col1,back_color,text_color,uitext,udkrd );
        }
    }
}

void logbookkonni_pi::UpdateAuiStatus( void )
{
}

int logbookkonni_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int logbookkonni_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int logbookkonni_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int logbookkonni_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxString logbookkonni_pi::GetCommonName()
{
    return _( "Logbook" );
}

wxString logbookkonni_pi::GetShortDescription()
{
    return _( "Logbook for OpenCPN" );
}

wxString logbookkonni_pi::GetLongDescription()
{
    return _( "Logbook for OpenCPN\n\n\
Thanks to the community for their helpful suggestions.\n\n\
If you find a bug post it on\nhttp://www.cruisersforum.com/forums/f134/logbook-konni-for-opencpn-68945.html\n\
Helpful videos are at Youtube, search for LogbookKonni." );
}


void logbookkonni_pi::OnContextMenuItemCallback( int id )
{
}

void logbookkonni_pi::SetNMEASentence( wxString &sentence )
{
    if ( m_plogbook_window )
    {
        m_plogbook_window->logbook->SetSentence( sentence );
    }
}

void logbookkonni_pi::SetPositionFix( PlugIn_Position_Fix &pfix )
{
    if ( m_plogbook_window )
    {
        if ( m_plogbook_window->logbook )
            m_plogbook_window->logbook->SetPosition( pfix );
    }
}

void logbookkonni_pi::SetDefaults( void )
{
    // If the config somehow says NOT to show the icon, override it so the user gets good feedback
    if ( !m_bLOGShowIcon )
    {
        m_bLOGShowIcon = true;
        m_leftclick_tool_id  = InsertPlugInTool( _T( "" ), _img_logbook_pi, _img_logbook_pi, wxITEM_NORMAL,
                               _( "Logbook" ), _T( "" ), NULL,
                               LOGBOOK_TOOL_POSITION, 0, this );

    }
}

wxString logbookkonni_pi::StandardPath( void )
{
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
    wxString s = wxFileName::GetPathSeparator();

#ifdef __WXMSW__
    wxString stdPath  = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
    wxString stdPath  = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
    wxString stdPath  = ( std_path.GetUserConfigDir() + s + _T( "opencpn" ) );
#endif

    stdPath += s + _T( "plugins" );
    if ( !wxDirExists( stdPath ) )
        wxMkdir( stdPath );

    stdPath += s + _T( "logbook" );

#ifdef __WXOSX__
    // Compatibility with pre-OCPN-4.2; move config dir to
    // ~/Library/Preferences/opencpn if it exists
    wxString oldPath = ( std_path.GetUserConfigDir() + s + _T( "plugins" ) + s + _T( "logbook" ) );
    if ( wxDirExists( oldPath ) && !wxDirExists( stdPath ) )
    {
        wxLogMessage( "logbookkonni_pi: moving config dir %s to %s", oldPath, stdPath );
        wxRenameFile( oldPath, stdPath );
    }
#endif

    if ( !wxDirExists( stdPath ) )
        wxMkdir( stdPath );

    stdPath += s; // is this necessary?
    return stdPath;
}

wxBitmap *logbookkonni_pi::GetPlugInBitmap()
{
    return _img_logbook_pi;
}

int logbookkonni_pi::GetToolbarToolCount( void )
{
    return 1;
}

void logbookkonni_pi::ShowPreferencesDialog( wxWindow* parent )
{
    dlgShow = false;

//#ifdef __WXOSX__
// Not tested yet
//#else
    if ( opt->firstTime )
    {
//		loadLayouts(parent);
//		delete opt;
//		opt = new Options();
//		LoadConfig();
        opt->firstTime = false;
    }
//#endif

#ifdef __WXMSW__
    optionsDialog = new LogbookOptions( parent, opt, this, -1, _( "Logbook Preferences" ), wxDefaultPosition,  wxSize( 692,660  ),
                                        wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#elif defined __WXOSX__
    optionsDialog = new LogbookOptions( parent, opt, this, -1, _( "Logbook Preferences" ), wxDefaultPosition,  wxSize( 710,685 ) ,
                                        wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#else
    optionsDialog = new LogbookOptions( parent, opt, this, -1, _( "Logbook Preferences" ), wxDefaultPosition,  wxSize( 740,700 ) ,
                                        wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#endif
    optionsDialog->m_checkBoxShowLogbook->SetValue( m_bLOGShowIcon );

    if ( optionsDialog->ShowModal() == wxID_OK )
    {
        optionsDialog->getValues();
        //    Show Icon changed value?
        if ( m_bLOGShowIcon != optionsDialog->m_checkBoxShowLogbook->GetValue() )
        {
            m_bLOGShowIcon= optionsDialog->m_checkBoxShowLogbook->GetValue();

            if ( m_bLOGShowIcon )
                m_leftclick_tool_id  = InsertPlugInTool( _T( "" ), _img_logbook_pi, _img_logbook_pi, wxITEM_NORMAL,
                                       _( "Logbook" ), _T( "" ), NULL, LOGBOOK_TOOL_POSITION,
                                       0, this );
            else
                RemovePlugInTool( m_leftclick_tool_id );
        }
        SaveConfig();
    }
}

void logbookkonni_pi::OnToolbarToolCallback( int id )
{
#ifdef __WXOSX__
    if ( dlgShow && !m_plogbook_window->IsIconized() )
    {
        m_plogbook_window->Raise();
        return;
    }
#endif
    dlgShow = !dlgShow;
    // show the Logbook dialog
    if ( NULL == m_plogbook_window )
    {
        if ( m_timer == NULL )
        {
            if ( timer == NULL )
                timer = new LogbookTimer( this );
            m_timer = new wxTimer( timer,ID_LOGTIMER );
            timer->Connect( wxEVT_TIMER, wxObjectEventFunction( &LogbookTimer::OnTimer ) );
        }
        m_plogbook_window = new LogbookDialog( this, m_timer, timer, m_parent_window, wxID_ANY, _( "Active Logbook" ), wxDefaultPosition, wxSize( opt->dlgWidth,opt->dlgHeight ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
        m_plogbook_window->init();
        m_plogbook_window->CenterOnParent();
        m_plogbook_window->Show();
        dlgShow = true;
    }
    else
    {
        if ( m_plogbook_window->IsIconized() )
        {
            m_plogbook_window->Iconize( false );
            m_plogbook_window->Show( true );
            dlgShow = true;
        }
        else
        {
            m_plogbook_window->Show( dlgShow );
        }
    }

    if ( m_plogbook_window->IsShown() )
        SendPluginMessage( wxString( _T( "LOGBOOK_WINDOW_SHOWN" ) ), wxEmptyString );
    else
        SendPluginMessage( _T( "LOGBOOK_WINDOW_HIDDEN" ), wxEmptyString );

    if ( state == OFF )
    {
        if ( !opt->checkStateOfEvents() )
            state = ONNOEVENTS;
        else
            state = ONWITHEVENTS;
    }
    else
    {
        state = OFF;
    }

    // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
    // to actual status to ensure correct status upon toolbar rebuild
    // SetToolbarItemState( m_leftclick_tool_id, dlgShow );
}

void logbookkonni_pi::SaveConfig()
{
    wxFileConfig *pConf = ( wxFileConfig * )m_pconfig;

    if ( pConf )
    {
        pConf->SetPath ( _T ( "/PlugIns/Logbook" ) );
        pConf->Write ( _T( "ShowLOGIcon" ), m_bLOGShowIcon );
        pConf->Write ( _T( "Traditional" ), opt->traditional );
        pConf->Write ( _T( "ToolTips" ),    opt->showToolTips );

        pConf->Write ( _T ( "FirstTime" ),  opt->firstTime );
        if ( m_plogbook_window )
        {
            pConf->Write( _T( "DlgWidth" ),  m_plogbook_window->GetSize().GetX() );
            pConf->Write ( _T( "DlgHeight" ), m_plogbook_window->GetSize().GetY() );
        }

        pConf->Write ( _T ( "Popup" ),opt->popup );
        pConf->Write ( _T ( "AutoStartTimer" ),opt->autostarttimer );
        pConf->Write ( _T ( "DateFormat" ), opt->dateformat );
        pConf->Write ( _T ( "DateSepIndiv" ), opt->dateseparatorindiv );
        pConf->Write ( _T ( "DateSepLocale" ), opt->dateseparatorlocale );
        pConf->Write ( _T ( "Date1" ), opt->date1 );
        pConf->Write ( _T ( "Date2" ), opt->date2 );
        pConf->Write ( _T ( "Date3" ), opt->date3 );
        pConf->Write ( _T ( "NoEngines" ), opt->engines );

        pConf->Write ( _T ( "TimeFormat" ), opt->timeformat );
        pConf->Write ( _T ( "NoSeconds" ), opt->noseconds );

        pConf->Write ( _T ( "GuardChange" ), opt->guardChange );
        pConf->Write ( _T ( "GuardChangeText" ), opt->guardChangeText );
        pConf->Write ( _T ( "WaypointArrived" ), opt->waypointArrived );
        pConf->Write ( _T ( "WayPointText" ), opt->waypointText );
        pConf->Write ( _T ( "CourseChange" ), opt->courseChange );
        pConf->Write ( _T ( "CouseChangeDegrees" ), opt->courseChangeDegrees );
        pConf->Write ( _T ( "CouseChangeAfter" ), opt->courseTextAfterMinutes );
        pConf->Write ( _T ( "CourseChangeText" ), opt->courseChangeText );
        pConf->Write ( _T ( "EverySM" ), opt->everySM );
        pConf->Write ( _T ( "EverySMAmount" ), opt->everySMAmount );
        pConf->Write ( _T ( "everySMText" ), opt->everySMText );

        pConf->Write ( _T ( "Timer" ), opt->timer );
        pConf->Write ( _T ( "TimerType" ), opt->timerType );
        pConf->Write ( _T ( "Local" ), opt->local );
        pConf->Write ( _T ( "UTC" ),   opt->UTC );
        pConf->Write ( _T ( "GPSAuto" ), opt->gpsAuto );
        pConf->Write ( _T ( "TzIndicator" ), opt->tzIndicator );
        pConf->Write ( _T ( "TzHours" ), opt->tzHour );
        pConf->Write ( _T ( "TimerHours" ), opt->thour );
        pConf->Write ( _T ( "TimerMin" ), opt->tmin );
        pConf->Write ( _T ( "TimerSec" ), opt->tsec );
        pConf->Write ( _T ( "TimerText" ), opt->ttext );

        pConf->Write ( _T ( "NavDegrees" ), opt->Deg );
        pConf->Write ( _T ( "NavMin" ), opt->Min );
        pConf->Write ( _T ( "NavSec" ), opt->Sec );
        pConf->Write ( _T ( "NavDistance" ), opt->distance );
        pConf->Write ( _T ( "NavSpeed" ), opt->speed );
        pConf->Write ( _T ( "NavMeter" ), opt->meter );
        pConf->Write ( _T ( "NavFeet" ), opt->feet );
        pConf->Write ( _T ( "NavFathom" ), opt->fathom );

        pConf->Write ( _T ( "Baro" ), opt->baro );
        pConf->Write ( _T ( "Temperature" ), opt->temperature );

        pConf->Write ( _T ( "Vol" ), opt->vol );
        pConf->Write ( _T ( "Motorhours" ), opt->motorh );
        pConf->Write ( _T ( "Engine" ), opt->engine );
        pConf->Write ( _T ( "Shaft" ), opt->shaft );
        pConf->Write ( _T ( "RPM" ), opt->rpm );

        pConf->Write ( _T ( "Days" ), opt->days );
        pConf->Write ( _T ( "Weeks" ), opt->weeks );
        pConf->Write ( _T ( "Month" ), opt->month );

        pConf->Write ( _T ( "Watermaker" ), opt->watermaker );
        pConf->Write ( _T ( "FuelTank" ),   opt->fuelTank );
        pConf->Write ( _T ( "WaterTank" ),  opt->waterTank );
        pConf->Write ( _T ( "Ampere" ),     opt->ampere );
        pConf->Write ( _T ( "Bank1" ),      opt->bank1 );
        pConf->Write ( _T ( "Bank2" ),      opt->bank2 );

        pConf->Write ( _T ( "ShowDepth" ), opt->showDepth );
        pConf->Write ( _T ( "ShowWaveSwell" ), opt->showWaveSwell );
        pConf->Write ( _T ( "ShowWindSpeedInd" ), opt->showWindSpeedchoice );
        pConf->Write ( _T ( "ShowWindSpeed" ), opt->showWindSpeed );
        pConf->Write ( _T ( "ShowWindDir" ), opt->showWindDir );
        pConf->Write ( _T ( "ShowHeading" ), opt->showHeading );
        pConf->Write ( _T ( "ShowWindHeading" ), opt->showWindHeading );

        pConf->Write ( _T ( "NavHTML" ), opt->navHTML );
        pConf->Write ( _T ( "CrewHTML" ), opt->crewHTML );
        pConf->Write ( _T ( "BoatHTML" ), opt->boatHTML );
        pConf->Write ( _T ( "overviewHTML" ), opt->overviewHTML );
        pConf->Write ( _T ( "serviceHTML" ), opt->serviceHTML );
        pConf->Write ( _T ( "repairsHTML" ), opt->repairsHTML );
        pConf->Write ( _T ( "buypartsHTML" ), opt->buypartsHTML );
        pConf->Write ( _T ( "OverViewAll" ), opt->overviewAll );

        pConf->Write ( _T ( "NavGridLayout" ), opt->navGridLayoutChoice );
        pConf->Write ( _T ( "CrewGridLayout" ), opt->crewGridLayoutChoice );
        pConf->Write ( _T ( "BoatGridLayout" ), opt->boatGridLayoutChoice );
        pConf->Write ( _T ( "overviewGridLayout" ), opt->overviewGridLayoutChoice );
        pConf->Write ( _T ( "serviceGridLayout" ), opt->serviceGridLayoutChoice );
        pConf->Write ( _T ( "repairsGridLayout" ), opt->repairsGridLayoutChoice );
        pConf->Write ( _T ( "buypartsGridLayout" ), opt->buypartsGridLayoutChoice );

        pConf->Write ( _T ( "NavGridLayoutODT" ), opt->navGridLayoutChoiceODT );
        pConf->Write ( _T ( "CrewGridLayoutODT" ), opt->crewGridLayoutChoiceODT );
        pConf->Write ( _T ( "BoatGridLayoutODT" ), opt->boatGridLayoutChoiceODT );
        pConf->Write ( _T ( "overviewGridLayoutODT" ), opt->overviewGridLayoutChoiceODT );
        pConf->Write ( _T ( "serviceGridLayoutODT" ), opt->serviceGridLayoutChoiceODT );
        pConf->Write ( _T ( "repairsGridLayoutODT" ), opt->repairsGridLayoutChoiceODT );
        pConf->Write ( _T ( "buypartsGridLayoutODT" ), opt->buypartsGridLayoutChoiceODT );

        pConf->Write ( _T ( "HTMLEditor" ), opt->htmlEditor );
        pConf->Write ( _T ( "ODTEditor" ), opt->odtEditor );
        pConf->Write ( _T ( "DataManager" ), opt->dataManager );
        pConf->Write ( _T ( "MailClient" ), opt->mailClient );

        pConf->Write ( _T ( "GPSWarning" ), opt->noGPS );
        pConf->Write ( _T ( "EngineMessageSails" ), opt->engineMessageSails );
        pConf->Write ( _T ( "WriteEngineRun" ), opt->engineMessageRunning );
        pConf->Write ( _T ( "SailsDown" ), opt->engineAllwaysSailsDown );
        pConf->Write ( _T ( "StatusBar" ), opt->statusbar );
        pConf->Write ( _T ( "WindSpeeds" ), opt->windspeeds );
        pConf->Write ( _T ( "OverviewLines" ), opt->overviewlines );

        wxString str = wxEmptyString;
        for ( int i = 0; i < 7; i++ )
            str += wxString::Format( _T( "%i,%s," ),opt->filterLayout[i],opt->layoutPrefix[i].c_str() );
        str.RemoveLast();
        pConf->Write ( _T ( "PrefixLayouts" ), str );

        wxString kmlRouteTrack = wxString::Format( _T( "%i,%i" ),opt->kmlRoute,opt->kmlTrack );
        pConf->Write ( _T ( "KMLRouteTrack" ), kmlRouteTrack );
        pConf->Write ( _T ( "KMLWidth" ), opt->kmlLineWidth );
        pConf->Write ( _T ( "KMLTransp" ), opt->kmlLineTransparancy );
        pConf->Write ( _T ( "KMLRouteColor" ), opt->kmlRouteColor );
        pConf->Write ( _T ( "KMLTrackColor" ), opt->kmlTrackColor );

        pConf->Write ( _T ( "RPMIsChecked" ), opt->bRPMIsChecked );
        pConf->Write ( _T ( "Eng1RPMIsChecked" ), opt->bEng1RPMIsChecked );
        pConf->Write ( _T ( "Eng2RPMIsChecked" ), opt->bEng2RPMIsChecked );
        pConf->Write ( _T ( "GenRPMIsChecked" ), opt->bGenRPMIsChecked );

        pConf->Write ( _T ( "NMEAUseRPM" ), opt->NMEAUseERRPM );
        pConf->Write ( _T ( "Engine1" ), opt->engine1Id );
        pConf->Write ( _T ( "Engine2" ), opt->engine2Id );
        pConf->Write ( _T ( "Engine1Runs" ), opt->engine1Running );
        pConf->Write ( _T ( "Engine2Runs" ), opt->engine2Running );

        pConf->Write ( _T ( "Generator" ), opt->generator );
        pConf->Write ( _T ( "GeneratorId" ), opt->generatorId );

        pConf->Write ( _T ( "GeneratorRuns" ), opt->generatorRunning );

        pConf->Write ( _T ( "ShowLayoutP" ), opt->layoutShow );

        pConf->Write ( _T ( "toggleEngine1" ), opt->toggleEngine1 );
        pConf->Write ( _T ( "toggleEngine2" ), opt->toggleEngine2 );
        pConf->Write ( _T ( "toggleGenerator" ), opt->toggleGenerator );
        pConf->Write ( _T ( "numberofSails" ), opt->numberSails );
        
        wxString sails = wxEmptyString;
        sails = wxString::Format( _T( "%i,%i," ),opt->rowGap,opt->colGap );
        for ( int i = 0; i < opt->numberSails; i++ )
            sails += wxString::Format( _T( "%s,%s,%i," ),opt->abrSails.Item( i ).c_str(),opt->sailsName.Item( i ).c_str(),opt->bSailIsChecked[i] );
        sails.RemoveLast();
        pConf->Write ( _T ( "Sails" ), sails );

        if ( opt->dtEngine1On.IsValid() )
            pConf->Write ( _T ( "Engine1TimeStart" ), opt->dtEngine1On.FormatISODate()+_T( " " )+
                           opt->dtEngine1On.FormatISOTime() );
        else
            pConf->Write ( _T ( "Engine1TimeStart" ),wxEmptyString );

        if ( opt->dtEngine2On.IsValid() )
            pConf->Write ( _T ( "Engine2TimeStart" ), opt->dtEngine2On.FormatISODate()+_T( " " )+
                           opt->dtEngine2On.FormatISOTime() );
        else
            pConf->Write ( _T ( "Engine2TimeStart" ),wxEmptyString );

        if ( opt->dtGeneratorOn.IsValid() )
            pConf->Write ( _T ( "GeneratorTimeStart" ), opt->dtGeneratorOn.FormatISODate()+_T( " " )+
                           opt->dtGeneratorOn.FormatISOTime() );
        else
            pConf->Write ( _T ( "GeneratorTimeStart" ),wxEmptyString );

        writeCols( pConf,opt->NavColWidth,		_T( "NavGridColWidth" ) );
        writeCols( pConf,opt->WeatherColWidth,	_T( "WeatherGridColWidth" ) );
        writeCols( pConf,opt->MotorColWidth,		_T( "MotorGridColWidth" ) );
        writeCols( pConf,opt->CrewColWidth,		_T( "CrewGridColWidth" ) );
        writeCols( pConf,opt->WakeColWidth,		_T( "WakeGridColWidth" ) );
        writeCols( pConf,opt->EquipColWidth,		_T( "EquipGridColWidth" ) );
        writeCols( pConf,opt->OverviewColWidth,	_T( "OverviewGridColWidth" ) );
        writeCols( pConf,opt->ServiceColWidth,	_T( "ServiceGridColWidth" ) );
        writeCols( pConf,opt->RepairsColWidth,	_T( "RepairsGridColWidth" ) );
        writeCols( pConf,opt->BuyPartsColWidth,	_T( "BuyPartsGridColWidth" ) );
    }
}

void logbookkonni_pi::writeCols( wxFileConfig *pConf, ArrayOfGridColWidth ar, wxString entry )
{
    wxString str = wxEmptyString;
    for ( unsigned int i = 0; i < ar.Count(); i++ )
        str += wxString::Format( _T( "%i," ),ar[i] );
    str.RemoveLast();
    pConf->Write( entry,str );
}

void logbookkonni_pi::LoadConfig()
{
    wxFileConfig *pConf = ( wxFileConfig * )m_pconfig;

    if ( pConf )
    {
        pConf->SetPath ( _T( "/PlugIns/Logbook" ) );
        pConf->Read ( _T( "ShowLOGIcon" ),  &m_bLOGShowIcon, 1 );
        pConf->Read ( _T( "Traditional" ),  &opt->traditional, 1 );
        pConf->Read ( _T( "ToolTips" ),  &opt->showToolTips );
        pConf->Read ( _T( "FirstTime" ),  &opt->firstTime );
#ifdef __WXMSW__
        pConf->Read ( _T( "DlgWidth" ),  &opt->dlgWidth,1010 );
#elif defined __WXGTK__
        pConf->Read ( _T( "DlgWidth" ),  &opt->dlgWidth,1085 );
#elif defined __WXOSX__
        pConf->Read ( _T( "DlgWidth" ),  &opt->dlgWidth,1085 );
#endif
        pConf->Read ( _T( "DlgHeight" ),  &opt->dlgHeight,535 );
        pConf->Read ( _T( "Popup" ),  &opt->popup,true );
        pConf->Read ( _T( "AutoStartTimer" ),  &opt->autostarttimer, false );

        pConf->Read ( _T ( "DateFormat" ), &opt->dateformat,0 );
        pConf->Read ( _T ( "DateSepIndiv" ), &opt->dateseparatorindiv );
        pConf->Read ( _T ( "DateSepLocale" ), &opt->dateseparatorlocale );
        pConf->Read ( _T ( "Date1" ), &opt->date1,0 );
        pConf->Read ( _T ( "Date2" ), &opt->date2,1 );
        pConf->Read ( _T ( "Date3" ), &opt->date3,2 );
        pConf->Read ( _T ( "NoEngines" ), &opt->engines,0 );

        pConf->Read ( _T ( "TimeFormat" ), &opt->timeformat, -1 );
        pConf->Read ( _T ( "NoSeconds" ), &opt->noseconds );

        pConf->Read ( _T ( "GuardChange" ), &opt->guardChange );
        pConf->Read ( _T ( "GuardChangeText" ), &opt->guardChangeText );
        pConf->Read ( _T ( "WaypointArrived" ), &opt->waypointArrived );
        pConf->Read ( _T ( "WayPointText" ), &opt->waypointText );
        pConf->Read ( _T ( "CourseChange" ), &opt->courseChange );
        pConf->Read ( _T ( "CouseChangeDegrees" ), &opt->courseChangeDegrees );
        pConf->Read ( _T ( "CouseChangeAfter" ), &opt->courseTextAfterMinutes );
        pConf->Read ( _T ( "CourseChangeText" ), &opt->courseChangeText );
        pConf->Read ( _T ( "EverySM" ), &opt->everySM );
        pConf->Read ( _T ( "EverySMAmount" ), &opt->everySMAmount );
        pConf->Read ( _T ( "everySMText" ), &opt->everySMText );
        opt->courseChangeDegrees.ToDouble( &opt->dCourseChangeDegrees );
        opt->everySMAmount.ToDouble( &opt->dEverySM );

        pConf->Read ( _T ( "Timer" ), &opt->timer );
        pConf->Read ( _T ( "TimerType" ), &opt->timerType );
        pConf->Read ( _T ( "Local" ), &opt->local );
        pConf->Read ( _T ( "UTC" ),	  &opt->UTC );
        pConf->Read ( _T ( "GPSAuto" ), &opt->gpsAuto );
        pConf->Read ( _T ( "TzIndicator" ), &opt->tzIndicator );
        pConf->Read ( _T ( "TzHours" ), &opt->tzHour );
        pConf->Read ( _T ( "TimerHours" ), &opt->thour,_T( "0" ) );
        pConf->Read ( _T ( "TimerMin" ), &opt->tmin,_T( "1" ) );
        pConf->Read ( _T ( "TimerSec" ), &opt->tsec,_T( "0" ) );
        pConf->Read ( _T ( "TimerText" ), &opt->ttext );

        opt->timerSec = ( wxAtol( opt->thour )* 3600000 +
                          wxAtol( opt->tmin ) *60000 +
                          wxAtol( opt->tsec ) * 1000 );

        pConf->Read ( _T ( "NavDegrees" ), &opt->Deg );
        pConf->Read ( _T ( "NavMin" ), &opt->Min );
        pConf->Read ( _T ( "NavSec" ), &opt->Sec );
        pConf->Read ( _T ( "NavDistance" ), &opt->distance );
        pConf->Read ( _T ( "NavSpeed" ), &opt->speed );
        pConf->Read ( _T ( "Baro" ), &opt->baro );
        pConf->Read ( _T ( "Temperature" ), &opt->temperature );
        pConf->Read ( _T ( "NavMeter" ), &opt->meter );
        pConf->Read ( _T ( "NavFeet" ), &opt->feet );
        pConf->Read ( _T ( "NavFathom" ), &opt->fathom );

        pConf->Read ( _T ( "Vol" ), &opt->vol );
        pConf->Read ( _T ( "Motorhours" ), &opt->motorh );
        pConf->Read ( _T ( "Engine" ), &opt->engine,_T( "E" ) );
        pConf->Read ( _T ( "Shaft" ), &opt->shaft,_T( "S" ) );
        pConf->Read ( _T ( "RPM" ), &opt->rpm,_T( "RPM" ) );

        pConf->Read ( _T ( "Days" ), &opt->days );
        pConf->Read ( _T ( "Weeks" ), &opt->weeks );
        pConf->Read ( _T ( "Month" ), &opt->month );

        pConf->Read ( _T ( "Watermaker" ), &opt->watermaker );
        pConf->Read ( _T ( "FuelTank" ),   &opt->fuelTank );
        pConf->Read ( _T ( "WaterTank" ),  &opt->waterTank );
        pConf->Read ( _T ( "Ampere" ),     &opt->ampere );
        pConf->Read ( _T ( "Bank1" ),      &opt->bank1 );
        pConf->Read ( _T ( "Bank2" ),      &opt->bank2 );

        pConf->Read ( _T ( "ShowDepth" ), &opt->showDepth );
        pConf->Read ( _T ( "ShowWaveSwell" ), &opt->showWaveSwell );
        pConf->Read ( _T ( "ShowWindSpeedInd" ), &opt->showWindSpeedchoice );
        pConf->Read ( _T ( "ShowWindSpeed" ), &opt->showWindSpeed );
        pConf->Read ( _T ( "ShowWindDir" ), &opt->showWindDir );
        pConf->Read ( _T ( "ShowHeading" ), &opt->showHeading );
        pConf->Read ( _T ( "ShowWindHeading" ), &opt->showWindHeading );

        pConf->Read( _T ( "NavHTML" ), &opt->navHTML,1 );
        pConf->Read ( _T ( "CrewHTML" ), &opt->crewHTML,1 );
        pConf->Read ( _T ( "BoatHTML" ), &opt->boatHTML,1 );
        pConf->Read ( _T ( "overviewHTML" ), &opt->overviewHTML,1 );
        pConf->Read ( _T ( "serviceHTML" ), &opt->serviceHTML,1 );
        pConf->Read ( _T ( "repairsHTML" ), &opt->repairsHTML,1 );
        pConf->Read ( _T ( "buypartsHTML" ), &opt->buypartsHTML,1 );
        pConf->Read ( _T ( "OverViewAll" ), &opt->overviewAll,0 );

        pConf->Read ( _T ( "NavGridLayout" ), &opt->navGridLayoutChoice,0 );
        pConf->Read ( _T ( "CrewGridLayout" ), &opt->crewGridLayoutChoice,0 );
        pConf->Read ( _T ( "BoatGridLayout" ), &opt->boatGridLayoutChoice,0 );
        pConf->Read ( _T ( "overviewGridLayout" ), &opt->overviewGridLayoutChoice,0 );
        pConf->Read ( _T ( "serviceGridLayout" ), &opt->serviceGridLayoutChoice,0 );
        pConf->Read ( _T ( "repairsGridLayout" ), &opt->repairsGridLayoutChoice,0 );
        pConf->Read ( _T ( "buypartsGridLayout" ), &opt->buypartsGridLayoutChoice,0 );

        pConf->Read ( _T ( "NavGridLayoutODT" ), &opt->navGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "CrewGridLayoutODT" ), &opt->crewGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "BoatGridLayoutODT" ), &opt->boatGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "overviewGridLayoutODT" ), &opt->overviewGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "serviceGridLayoutODT" ), &opt->serviceGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "repairsGridLayoutODT" ), &opt->repairsGridLayoutChoiceODT,0 );
        pConf->Read ( _T ( "buypartsGridLayout" ), &opt->buypartsGridLayoutChoiceODT,0 );

        pConf->Read ( _T ( "HTMLEditor" ), &opt->htmlEditor );
        pConf->Read ( _T ( "ODTEditor" ), &opt->odtEditor );
        pConf->Read ( _T ( "DataManager" ), &opt->dataManager );
        pConf->Read ( _T ( "MailClient" ), &opt->mailClient );

        pConf->Read ( _T ( "GPSWarning" ), &opt->noGPS );
        pConf->Read ( _T ( "EngineMessageSails" ), &opt->engineMessageSails );
        pConf->Read ( _T ( "WriteEngineRun" ), &opt->engineMessageRunning );
        pConf->Read ( _T ( "SailsDown" ), &opt->engineAllwaysSailsDown );
        pConf->Read ( _T ( "StatusBar" ), &opt->statusbar );
        pConf->Read ( _T ( "WindSpeeds" ), &opt->windspeeds );
        pConf->Read ( _T ( "OverviewLines" ), &opt->overviewlines );

        wxString str = wxEmptyString;
        pConf->Read ( _T ( "PrefixLayouts" ), &str );
        if ( str.Contains( _T( "," ) ) )
        {
            wxStringTokenizer tkz( str,_T( "," ) );
            for ( int i = 0; i < 7; i++ )
            {
                opt->filterLayout[i] = ( wxAtoi( tkz.GetNextToken() ) )?true:false;
                opt->layoutPrefix[i] = tkz.GetNextToken();
            }
        }

        wxString kmlRouteTrack = wxEmptyString;
        pConf->Read ( _T ( "KMLRouteTrack" ), &kmlRouteTrack,_T( "1,1" ) );
        wxStringTokenizer tkz( kmlRouteTrack,_T( "," ) );
        opt->kmlRoute = wxAtoi( tkz.GetNextToken() );
        opt->kmlTrack = wxAtoi( tkz.GetNextToken() );
        pConf->Read ( _T ( "KMLWidth" ), &opt->kmlLineWidth,_T( "4" ) );
        pConf->Read ( _T ( "KMLTransp" ), &opt->kmlLineTransparancy,0 );
        pConf->Read ( _T ( "KMLRouteColor" ), &opt->kmlRouteColor,0 );
        pConf->Read ( _T ( "KMLTrackColor" ), &opt->kmlTrackColor,3 );

        pConf->Read ( _T ( "RPMIsChecked" ), &opt->bRPMIsChecked,false );
        pConf->Read ( _T ( "Eng1RPMIsChecked" ), &opt->bEng1RPMIsChecked,false );
        pConf->Read ( _T ( "Eng2RPMIsChecked" ), &opt->bEng2RPMIsChecked,false );
        pConf->Read ( _T ( "GenRPMIsChecked" ), &opt->bGenRPMIsChecked,false );

        pConf->Read ( _T ( "NMEAUseRPM" ), &opt->NMEAUseERRPM,false );
        pConf->Read ( _T ( "Engine1" ), &opt->engine1Id,_T( "" ) );
        pConf->Read ( _T ( "Engine2" ), &opt->engine2Id,_T( "" ) );
        pConf->Read ( _T ( "Engine1Runs" ), &opt->engine1Running );
        pConf->Read ( _T ( "Engine2Runs" ), &opt->engine2Running );

        pConf->Read ( _T ( "Generator" ), &opt->generator,false );
        pConf->Read ( _T ( "GeneratorId" ), &opt->generatorId,_T( "" ) );
        pConf->Read ( _T ( "GeneratorRuns" ), &opt->generatorRunning );

        pConf->Read ( _T ( "ShowLayoutP" ), &opt->layoutShow,true );

        pConf->Read ( _T ( "toggleEngine1" ), &opt->toggleEngine1 );
        pConf->Read ( _T ( "toggleEngine2" ), &opt->toggleEngine2 );
        pConf->Read ( _T ( "toggleGenerator" ), &opt->toggleGenerator );
        pConf->Read ( _T ( "numberofSails" ), &opt->numberSails,14 );

        wxString sails = wxEmptyString;
        pConf->Read ( _T ( "Sails" ), &sails );
        if ( !sails.IsEmpty() )
        {
            wxStringTokenizer tkz( sails,_T( "," ) );
            if ( wxString( sails.GetChar( 0 ) ).IsNumber() )
            {
                opt->rowGap = wxAtoi( tkz.GetNextToken() );
                opt->colGap = wxAtoi( tkz.GetNextToken() );
            }

            opt->abrSails.Empty();
            opt->sailsName.Empty();
            
            for ( int i = 0; i < opt->numberSails; i++ )
            {
                opt->abrSails.Insert( tkz.GetNextToken(), i );
                opt->sailsName.Insert( tkz.GetNextToken(), i);
                opt->bSailIsChecked[i] = ( wxAtoi( tkz.GetNextToken() )?true:false );
            }
        }

        wxString engine1 = wxEmptyString, engine2 = wxEmptyString, genny = wxEmptyString;
        pConf->Read ( _T ( "Engine1TimeStart" ), &engine1 );
        pConf->Read ( _T ( "Engine2TimeStart" ), &engine2 );
        pConf->Read ( _T ( "GeneratorTimeStart" ), &genny );

        if ( !engine1.IsEmpty() )
        {
            wxStringTokenizer tkz( engine1,_T( " " ) );
            wxString date = tkz.GetNextToken();
            wxString time = tkz.GetNextToken();

            wxDateTime dt;
            dt.ParseDate( date );
            dt.ParseTime( time );

            if ( dt.GetYear() != 1970 )
                opt->dtEngine1On = dt;
        }

        if ( !engine2.IsEmpty() )
        {
            wxStringTokenizer tkz( engine2,_T( " " ) );
            wxString date = tkz.GetNextToken();
            wxString time = tkz.GetNextToken();

            wxDateTime dt;
            dt.ParseDate( date );
            dt.ParseTime( time );

            if ( dt.GetYear() != 1970 )
                opt->dtEngine2On = dt;
        }

        if ( !genny.IsEmpty() )
        {
            wxStringTokenizer tkz( genny,_T( " " ) );
            wxString date = tkz.GetNextToken();
            wxString time = tkz.GetNextToken();

            wxDateTime dt;
            dt.ParseDate( date );
            dt.ParseTime( time );

            if ( dt.GetYear() != 1970 )
                opt->dtGeneratorOn = dt;
        }


        bool r;
        r = pConf->Read ( _T ( "NavGridColWidth" ),&str );
        if ( r )
            opt->NavColWidth = readCols( opt->NavColWidth,str );
        else
            opt->NavColWidth = readColsOld( pConf,opt->NavColWidth,_T ( "NavGridColWidth" ) );

        r = pConf->Read ( _T ( "WeatherGridColWidth" ),&str );
        if ( r )
            opt->WeatherColWidth = readCols( opt->WeatherColWidth,str );
        else
            opt->WeatherColWidth = readColsOld( pConf,opt->WeatherColWidth,_T ( "WeatherGridColWidth" ) );

        r = pConf->Read ( _T ( "MotorGridColWidth" ),&str );
        if ( r )
            opt->MotorColWidth = readCols( opt->MotorColWidth,str );
        else
            opt->MotorColWidth = readColsOld( pConf,opt->MotorColWidth,_T ( "MotorGridColWidth" ) );

        r = pConf->Read ( _T ( "CrewGridColWidth" ),&str );
        if ( r )
            opt->CrewColWidth = readCols( opt->CrewColWidth,str );
        else
            opt->CrewColWidth = readColsOld( pConf,opt->CrewColWidth,_T ( "CrewGridColWidth" ) );

        r = pConf->Read ( _T ( "WakeGridColWidth" ),&str );
        if ( r )
            opt->WakeColWidth = readCols( opt->WakeColWidth,str );
        else
            opt->WakeColWidth = readColsOld( pConf,opt->WakeColWidth,_T ( "WakeGridColWidth" ) );

        r = pConf->Read ( _T ( "EquipGridColWidth" ),&str );
        if ( r )
            opt->EquipColWidth = readCols( opt->EquipColWidth,str );
        else
            opt->EquipColWidth = readColsOld( pConf,opt->EquipColWidth,_T ( "EquipGridColWidth" ) );

        r = pConf->Read ( _T ( "OverviewGridColWidth" ),&str );
        if ( r )
            opt->OverviewColWidth = readCols( opt->OverviewColWidth,str );
        else
            opt->OverviewColWidth = readColsOld( pConf,opt->OverviewColWidth,_T ( "OverviewGridColWidth" ) );

        r = pConf->Read ( _T ( "ServiceGridColWidth" ),&str );
        if ( r )
            opt->ServiceColWidth = readCols( opt->ServiceColWidth,str );
        else
            opt->ServiceColWidth = readColsOld( pConf,opt->ServiceColWidth,_T ( "ServiceGridColWidth" ) );

        r = pConf->Read ( _T ( "RepairsGridColWidth" ),&str );
        if ( r )
            opt->RepairsColWidth = readCols( opt->RepairsColWidth,str );
        else
            opt->RepairsColWidth = readColsOld( pConf,opt->RepairsColWidth,_T ( "RepairsGridColWidth" ) );

        r = pConf->Read ( _T ( "BuyPartsGridColWidth" ),&str );
        if ( r )
            opt->BuyPartsColWidth = readCols( opt->BuyPartsColWidth,str );
        else
            opt->BuyPartsColWidth = readColsOld( pConf,opt->BuyPartsColWidth,_T ( "BuyPartsGridColWidth" ) );

        pConf->DeleteEntry ( _T ( "ShowAllLayout" ) );
        pConf->DeleteEntry ( _T ( "ShowFilteredLayout" ) );

        opt->ampereh = opt->ampere+opt->motorh;
    }

    if ( opt->timeformat == -1 )
    {
        wxString am,pm;
        wxDateTime::GetAmPmStrings( &am,&pm );
        if ( am.IsEmpty() )
            opt->timeformat = 0;
        else
            opt->timeformat = 1;
    }
}

ArrayOfGridColWidth logbookkonni_pi::readCols( ArrayOfGridColWidth ar, wxString str )
{
    wxStringTokenizer tkz( str,_T( "," ) );
    while ( tkz.HasMoreTokens() )
        ar.Add( wxAtoi( tkz.GetNextToken() ) );

    return ar;
}

ArrayOfGridColWidth logbookkonni_pi::readColsOld( wxFileConfig *pConf, ArrayOfGridColWidth ar, wxString entry )
{
    int val;
    bool r;
    int i = 0;

    while ( true )
    {
        r = pConf->Read ( wxString::Format( entry+_T ( "/%i" ),i++ ), &val );
        if ( !r ) break;
        ar.Add( val );
    }
    pConf->DeleteGroup( _T( "/PlugIns/Logbook/"+entry ) );

    return ar;
}

void logbookkonni_pi::loadLayouts( wxWindow *parent )
{
    wxString FILE = _T( "LogbookKonni*.zip" );
    std::auto_ptr<wxZipEntry> entry;
    wxString path, sep;
    sep = wxFileName::GetPathSeparator();

    wxString data = StandardPath();
    data.Append( _T( "data" ) );
    data.append( sep );
    if ( !wxDir::Exists( data ) )
        wxMkdir( data );

    wxString data1 = data;
    wxString data2 = data;
    wxString data3 = data;
    wxString help  = data;

    data.Append( _T( "HTMLLayouts" ) );
    data.append( sep );
    if ( !wxDir::Exists( data ) )
        wxMkdir( data );

    data1.Append( _T( "ODTLayouts" ) );
    data1.append( sep );
    if ( !wxDir::Exists( data1 ) )
        wxMkdir( data1 );

    data2.Append( _T( "Clouds" ) );
    data2.append( sep );
    if ( !wxDir::Exists( data2 ) )
        wxMkdir( data2 );

    data3.Append( _T( "Images" ) );
    data3.append( sep );
    if ( !wxDir::Exists( data3 ) )
        wxMkdir( data3 );
#ifdef __WXOSX__
    wxFileDialog* openFileDialog =
        new wxFileDialog( parent, _( "Select zipped Layout-Files" ), _T( "" ), _T( "" ), _T( "*.zip" ),
                          wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize );
#else
    wxFileDialog* openFileDialog =
        new wxFileDialog( parent, _( "Select zipped Layout-Files" ), _T( "" ), _T( "" ), _T( "*.zip" ),
                          wxFD_OPEN, wxDefaultPosition );
#endif
    wxString n = _T( "not " );
    int ret = true;

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        wxFFileInputStream in( openFileDialog->GetPath() );
        wxZipInputStream zip( in );

        while ( entry.reset( zip.GetNextEntry() ), entry.get() != NULL )
        {
            if ( entry->GetName().Contains( _T( "HTMLLayouts" ) ) )
                path = data;
            else if ( entry->GetName().Contains( _T( "ODTLayouts" ) ) )
                path = data1;
            else if ( entry->GetName().Contains( _T( "Clouds" ) ) )
                path = data2;
            else
                path = data3;

            wxString name = entry->GetName();

            if ( !name.Contains( _T( ".htm" ) ) && !name.Contains( _T( ".odt" ) ) &&
                    !name.Contains( _T( ".jpg" ) ) && !name.Contains( _T( ".PNG" ) ) )
                continue;
            wxString fn = name.AfterLast( wxFileName::GetPathSeparator() );

            if ( name.Contains( sep+_T( "boat" ) ) )
                path.append( _T( "boat" ) );
            else if ( name.Contains( sep+_T( "logbook" ) ) )
                path.append( _T( "logbook" ) );
            else if ( name.Contains( sep + _T( "crew" ) ) )
                path.append( _T( "crew" ) );
            else if ( name.Contains( sep + _T( "overview" ) ) )
                path.append( _T( "overview" ) );
            else if ( name.Contains( sep + _T( "service" ) ) )
                path.append( _T( "service" ) );
            else if ( name.Contains( sep + _T( "repairs" ) ) )
                path.append( _T( "repairs" ) );
            else if ( name.Contains( sep + _T( "buyparts" ) ) )
                path.append( _T( "buyparts" ) );

            if ( !name.Contains( _T( "Help" ) ) )
            {
                path.append( sep );
                if ( !wxFileName::DirExists( path ) )
                    wxMkdir( path );
            }
            else
                path = help+sep;

            path.append( fn );

            wxFileOutputStream out( path );
            if ( zip.OpenEntry( *entry ) != true )
            {
                out.Close();
                ret = false;
                break;
            }
            zip.Read( out );
            out.Close();
        }
        if ( m_plogbook_window != NULL )
        {
            m_plogbook_window->loadLayoutChoice( LogbookDialog::LOGBOOK,
                                                 m_plogbook_window->logbook->layout_locn,m_plogbook_window->logbookChoice,opt->layoutPrefix[LogbookDialog::LOGBOOK] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::CREW,
                                                 m_plogbook_window->crewList->layout_locn,m_plogbook_window->crewChoice,opt->layoutPrefix[LogbookDialog::CREW] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::BOAT,
                                                 m_plogbook_window->boat->layout_locn,m_plogbook_window->boatChoice,opt->layoutPrefix[LogbookDialog::BOAT] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::OVERVIEW,
                                                 m_plogbook_window->logbook->layout_locn,m_plogbook_window->overviewChoice,opt->layoutPrefix[LogbookDialog::OVERVIEW] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::GSERVICE,
                                                 m_plogbook_window->maintenance->layout_locnService,m_plogbook_window->m_choiceSelectLayoutService,opt->layoutPrefix[LogbookDialog::GSERVICE] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::GREPAIRS,
                                                 m_plogbook_window->maintenance->layout_locnRepairs,m_plogbook_window->m_choiceSelectLayoutRepairs,opt->layoutPrefix[LogbookDialog::GREPAIRS] );
            m_plogbook_window->loadLayoutChoice( LogbookDialog::GBUYPARTS,
                                                 m_plogbook_window->maintenance->layout_locnBuyParts,m_plogbook_window->m_choiceSelectLayoutBuyParts,opt->layoutPrefix[LogbookDialog::GBUYPARTS] );
        }
        wxString ok = wxString::Format( _( "Layouts %sinstalled at\n\n%s\n%s\n%s\n%s" ),
                                        ( !ret )?n.c_str():wxEmptyString,data.c_str(),data1.c_str(),data2.c_str(),data3.c_str() );
        wxMessageBox( ok );

		if (ret ) 
		{
			opt->navGridLayoutChoice = 0;
        	opt->crewGridLayoutChoice = 0;
        	opt->boatGridLayoutChoice = 0;
		}
    }
}

////////////////////////////////////////////////////////
void LogbookTimer::OnTimer( wxTimerEvent& ev )
{
    timerEvent();
}

void LogbookTimer::timerEvent()
{
    if ( popUp() )
        plogbook_pi->m_plogbook_window->logbook->appendRow( true, true );
}

bool LogbookTimer::popUp()
{
    if ( plogbook_pi->eventsEnabled || NULL == plogbook_pi->m_plogbook_window ) return false;

    wxFrame *frame = ( wxFrame* )plogbook_pi->m_parent_window->GetParent();
    if ( ( frame->IsIconized() || plogbook_pi->m_plogbook_window->IsIconized() ) && plogbook_pi->opt->popup )
    {
        if ( frame->IsIconized() )
            frame->Iconize( false );

        plogbook_pi->m_plogbook_window->Iconize( false );
        plogbook_pi->m_parent_window->SetFocus();
        return true;
    }

    if ( !plogbook_pi->m_plogbook_window->IsShown() && plogbook_pi->opt->popup )
    {
        plogbook_pi->m_plogbook_window->Show();
        plogbook_pi->SendLogbookMessage( _T( "LOGBOOK_WINDOW_SHOWN" ), wxEmptyString );
        plogbook_pi->dlgShow = true;
    }

    if ( plogbook_pi->opt->popup )
        plogbook_pi->m_plogbook_window->Raise();

    return true;
}
