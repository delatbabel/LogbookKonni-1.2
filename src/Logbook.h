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

#ifndef _LOGBOOK_H_
#define _LOGBOOK_H_

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers


#include <wx/button.h>
#include <wx/colour.h>
#include <wx/datectrl.h>
#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textfile.h>
#include "ocpn_plugin.h"
#include "LogbookHTML.h"
#include "nmea0183/nmea0183.h"

//#define PBVE_DEBUG 1
#define DEVICE_TIMEOUT 4 // NMEA-Device doesn't send for 4 sec. sets the strings to empty in appendRow()

class Options;
class LogbookDialog;
class PBVEDialog;
class ActualWatch;

class Logbook : public LogbookHTML
{
private:
    struct Pos
    {
        double posLat;
        double latitude;
        double latmin;
        char   NSflag;
        double posLon;
        double longitude;
        double lonmin;
        char   WEflag;
    } oldPosition,newPosition;

    NMEA0183L      	m_NMEA0183;
    RMB		tempRMB;
    wxString	sLat;
    wxString	sLon;
    wxDateTime	mUTCDateTime;
    wxString	sSOG;
    wxString	sSOW;
    wxDateTime	dtSOW;
    bool		bSOW;
    wxString	sCOG;
    wxString	sCOW;
    wxDateTime	dtCOW;
    wxString	sDistance;
    wxString	sTemperatureWater;
    wxDateTime	dtTemperatureWater;
    bool		bTemperatureWater;
    wxString	sTemperatureAir;
    bool		bTemperatureAir;
    wxDateTime	dtWimda;
    wxString	sPressure;
    wxString	sHumidity;
    wxString    sVolume;
    wxString	sWindA;
    wxString	sWindT;
    wxDateTime	dtWindA;
    wxDateTime	dtWindT;
    bool		bWindA;
    bool		bWindT;
    wxString	sWindSpeedA;
    wxString	sWindSpeedT;
    wxString	sDepth;
    wxDateTime	dtDepth;
    bool		bDepth;
    wxString	sLogText;
    wxString	sLinesReminder;
    wxString	sRPM1;
    wxString	sRPM1Shaft;
    wxString	sRPM1Source;
    wxString	sRPM2Shaft;
    wxString	sRPM2Source;
    wxString	sRPM2;
    wxDateTime	dtRPM2;
    long		engine;

    bool		noSentence;
    bool		bCOW;
    double		dCOW;
    double		dCOG;
    bool		mode;
    bool		courseChange;
    bool		everySM;
    bool		waypointArrived;
    bool		oldLogbook;
    bool		wimdaSentence;
    double		minwindA;
    double		avgwindA;
    double		maxwindA;
    double		minwindT;
    double		avgwindT;
    double		maxwindT;
    double      dVolume;
    wxString	swindspeedsA;
    wxString	swindspeedsT;

    wxString	toSDMM ( int NEflag, double a, bool mode );
    wxString	toSDMMOpenCPN ( int NEflag, double a, bool hi_precision );
    void		setPositionString( double lat,int north, double lon, int east );
    void		setDateTimeString( wxDateTime s );
    wxString	computeCell( int grid,int row, int col, wxString s, bool mode );
    wxString	calculateDistance( wxString fromstr, wxString tostr );
    wxDouble	positionStringToDezimal( wxString pos );
    wxDouble	positionStringToDezimalModern( wxString pos );
    void		checkCourseChanged();
    void		checkGuardChanged();
    void		checkDistance();
    wxString	positionTraditional( int NEflag, double a, bool mode );
    wxString	positionGPSLike( int NEflag, double a, bool mode );
    void		setOldPosition();
    void		setWayPointArrivedText();

#ifdef PBVE_DEBUG
    int pbvecount;
#endif

public:
    enum fields { ROUTE,RDATE,RTIME,STATUS,WAKE,DISTANCE,DTOTAL,POSITION,COG,COW,SOG,SOW,DEPTH,REMARKS,
                  BARO,HYDRO,TEMPAIR,TEMPWATER,WIND,WSPD,WINDR,WSPDR,CURRENT,CSPD,WAVE,SWELL,WEATHER,CLOUDS,VISIBILITY,
                  MOTOR,MOTORT,RPM1,MOTOR1,MOTOR1T,RPM2,FUEL,FUELT,SAILS,REEF,GENE,GENET,BANK1,BANK1T,BANK2,BANK2T,WATERM,WATERMT,WATERMO,WATER,WATERT,MREMARKS,ROUTEID,TRACKID
                };

    Options		*opt;
    wxArrayString	mergeList;
    LogbookDialog*	dialog;
    LogbookHTML*	logbookHTML;
    wxString	layout_locn;
    wxString	layoutODT;
    wxString	data_locn;
    bool		modified;
    wxDateTime	mCorrectedDateTime;
    long		dLastMinute;
    PBVEDialog*	pvbe;
    bool		WP_skipped;
    wxString	lastWayPoint;
    bool		OCPN_Message;
    bool		routeIsActive;
    wxString	activeRoute;
    wxString	activeRouteGUID;
    bool		trackIsActive;
    wxString	activeMOB;

    wxString	MOB_GUID;
    bool		MOBIsActive;
    wxString	activeTrack;
    wxString	activeTrackGUID;

    wxString	sDate;
    wxString	sTime;
    bool		guardChange;
    bool		rpmSentence;
    wxDateTime	dtRPM;
    bool		engine1Manual;
    bool		engine2Manual;
    bool        generatorManual;
    wxTimeSpan	dtEngine1Off;
    wxTimeSpan	dtEngine2Off;
    wxTimeSpan  dtGeneratorOff;
    bool		bRPM2;
    bool		bRPM1;
    bool        bGEN;
    bool		sailsMessage;
    int		sailsState;
    int		oldSailsState;

public:
    Logbook( LogbookDialog* parent, wxString data, wxString layout, wxString layoutODT );
    ~Logbook( void );

    void SetSentence( wxString &sentence );
    void SetPosition( PlugIn_Position_Fix &pfix );
    void loadData();
    void loadSelectedData( wxString path );
    void loadDatanew();
    void deleteRow( int row );
    void appendRow( bool showlastline, bool autoline );
    void update();
    void clearNMEAData();
    void newLogbook();
    void switchToActualLogbook();
    void selectLogbook();
    void changeCellValue( int row, int col, int offset );
    void setLayoutLocation( wxString loc );
    void SetGPSStatus( bool status );
    bool checkGPS( bool appendClick );
    void checkWayPoint( RMB rmb );
    void showSearchDlg( int row, int col );
    void getModifiedCellValue( int grid, int row, int selcol, int col );
    void clearAllGrids();
    void recalculateLogbook( int row );
    void deleteRows();
    void setTrackToNewID( wxString target );
    void checkNMEADeviceIsOn();
    void resetEngineManualMode( int enginenumber );

    static wxString makeDateFromFile( wxString date, wxString dateformat );
    static wxString makeWatchtimeFromFile( wxString time, wxString timeformat );

    wxTextFile* logbookFile;
    wxString	title;
    bool		gpsStatus;

private:
    void     setCellAlign( int i );
    wxString decimalToHours( double res, bool b );
    void     convertTo_1_2();

    wxString	logbookData_actual;
    bool		noAppend; // Old Logbook; append Rows not allowed
    wxString	logbookDescription;
};

//////////////////////////////////////////////////////////////////////////////
/// Class NoAppendDialog
///////////////////////////////////////////////////////////////////////////////
class NoAppendDialog : public wxDialog
{
private:

protected:
    wxStaticText* m_staticText73;
    wxStdDialogButtonSizer* m_sdbSizer5;
    wxButton* m_sdbSizer5OK;

public:

    NoAppendDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _( "Information" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 255,130 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
    ~NoAppendDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class LinesReminderDlg
///////////////////////////////////////////////////////////////////////////////
class LinesReminderDlg : public wxDialog
{
private:

protected:
    wxStaticText* m_staticTextreminder;
    wxStdDialogButtonSizer* m_sdbSizer9;
    wxButton* m_sdbSizer9OK;

public:

    LinesReminderDlg( wxString str, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _( "Reminder" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 263,84 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
    ~LinesReminderDlg();

};

////////////////////////////
// PVBE Dialog
///////////////////////////
class PBVEDialog : public wxFrame
{
private:
    LogbookDialog* dialog;
protected:


public:
    virtual void PBVEDialogOnClose( wxCloseEvent& event );
    void OnCloseWindow( wxCloseEvent& ev );
    wxTextCtrl* m_textCtrlPVBE;
    PBVEDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    ~PBVEDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class LogbookSearch
///////////////////////////////////////////////////////////////////////////////
class LogbookSearch : public wxDialog
{
private:
    LogbookDialog* parent;
    int row, col;
    int searchrow;
    bool direction;

protected:
    wxStaticText* m_staticText96;
    wxStaticLine* m_staticline32;
    wxStaticText* m_staticText108;
    wxStaticText* m_staticText110;
    wxStaticText* m_staticText97;
    wxStaticLine* m_staticline39;
    wxButton* m_buttonBack;
    wxButton* m_buttonForward;

    // Virtual event handlers, overide them in your derived class
    virtual void OnInitDialog( wxInitDialogEvent& event );
    virtual void OnButtonClickBack( wxCommandEvent& event );
    virtual void OnButtonClickForward( wxCommandEvent& event );
    virtual void OnButtonClickSelectDate( wxCommandEvent& event );


public:
    wxRadioButton* m_radioBtnActual;
    wxRadioButton* m_radioBtnAll;
    wxTextCtrl* m_textCtrl72;
    wxChoice* m_choice23;
    wxChoice* m_choiceGreaterEqual;
    wxDatePickerCtrl* m_datePicker;
    wxButton* m_buttonSelectDate;

    LogbookSearch( wxWindow* parent, int row, int col, wxWindowID id = wxID_ANY, const wxString& title = _( "Search in Logbook" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 353,219 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~LogbookSearch();

};
#endif
