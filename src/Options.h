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

#ifndef _LOGOPTIONS_H_
#define _LOGOPTIONS_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>

WX_DEFINE_ARRAY_INT( int, ArrayOfGridColWidth );

class Options
{
public:
    Options( void );
    ~Options( void );

    bool checkStateOfEvents();
    void stopAllEvents();
    void setTimeFormat( int i );
    void setOptionsTimeFormat();
    void setDateFormat();

    // Global Options
    bool		firstTime;
    int			dlgWidth;
    int			dlgHeight;
    bool		popup;
    bool        autostarttimer;
    bool		hourFormat;
    bool		statusbarGlobal;
    bool		layoutShow;
    bool		statusbarWatch;
    int			engines;
    bool		toggleEngine1;
    bool		toggleEngine2;
    bool        toggleGenerator;
    wxDateTime	dtEngine1On;
    wxDateTime	dtEngine2On;
    wxDateTime  dtGeneratorOn;
    wxString	engineStr[2];
    bool		engineMessageSails;
    bool		engineMessageRunning;
    bool		engineAllwaysSailsDown;
    bool        statusbar;
    bool		windspeeds;
    bool        overviewlines;

    wxString    layoutPrefix[7];
    bool		filterLayout[7];
    bool		noGPS;
    bool		colouredMaintenance;
    bool		guardChange;
    wxString	guardChangeText;
    bool		waypointArrived;
    wxString	waypointText;
    bool		courseChange;
    wxString	courseChangeDegrees;
    double		dCourseChangeDegrees;
    wxString	courseTextAfterMinutes;
    wxString    courseChangeText;
    bool		everySM;
    wxString	everySMAmount;
    double		dEverySM;
    wxString    everySMText;

    bool		traditional;
    bool		modern;
    bool		showToolTips;

    // Timer
    bool		timer;
    int         timerType;
    wxString	thour;
    wxString	tmin;
    wxString	tsec;
    wxString	ttext;
    long		timerSec;

    // Grid Navigation
    wxString	Deg;
    wxString	Min;
    wxString	Sec;

    
    

    wxString	meter;
    wxString	feet;
    wxString	fathom;

    // Grid Weather
    wxString	baro;
    wxString    temperature;

    // Grid MotorSails
    wxString	vol;
    wxString	motorh;
    wxString    engine;
    wxString	shaft;
    wxString	rpm;

    //Maintenance
    wxString	days;
    wxString	weeks;
    wxString	month;

    //Paths
    wxString    htmlEditor;
    wxString   	odtEditor;
    wxString    mailClient;
    wxString   	dataManager;

    wxString    htmlEditorReset;
    wxString    odtEditorReset;
    wxString    mailClientReset;
    wxString    dataManagerReset;

    //Capacity
    wxString	fuelTank;
    wxString	waterTank;
    wxString	watermaker;
    wxString	bank1;
    wxString    bank2;
    wxString    ampere;
    wxString    ampereh;

    // KML
    int			kmlRoute;
    int			kmlTrack;
    wxString	kmlLineWidth;
    int			kmlLineTransparancy;
    int			kmlRouteColor;
    int			kmlTrackColor;
    wxArrayString kmlColor;
    wxArrayString kmlTrans;

    //NMEA
    bool		bRPMIsChecked;
    bool		bEng1RPMIsChecked;
    bool		bEng2RPMIsChecked;
    bool		bGenRPMIsChecked;

    wxString	sentence;
    wxString	engine1Id;
    wxString	engine2Id;
    wxString    generatorId;
    bool        generator;
    bool	engine1Running;
    bool	engine2Running;
    bool        generatorRunning;
    bool	bRPMCheck;
    bool	NMEAUseERRPM;

    // Sails
    wxArrayString	abrSails;
    wxArrayString	sailsName;
    bool			bSailIsChecked[14];
    int				rowGap;
    int				colGap;
    int             numberSails;

    //wxChoice
	int	    
		Choice;
	int	    showDistanceChoice;
	int	    showBoatSpeedchoice;
    int		showDepth;
    int		showWaveSwell;
    int     showWindSpeedchoice;
    int		showWindDir;
    int		showWindHeading;
    int		showHeading;

    bool 		navHTML;
    bool		crewHTML;
    bool		boatHTML;
    bool		overviewHTML;
    bool		serviceHTML;
    bool		repairsHTML;
    bool		buypartsHTML;
    int         overviewAll;

    int		navGridLayoutChoice;
    int		crewGridLayoutChoice;
    int		boatGridLayoutChoice;
    int		overviewGridLayoutChoice;
    int		serviceGridLayoutChoice;
    int		repairsGridLayoutChoice;
    int		buypartsGridLayoutChoice;

    int		navGridLayoutChoiceODT;
    int		crewGridLayoutChoiceODT;
    int		boatGridLayoutChoiceODT;
    int		overviewGridLayoutChoiceODT;
    int		serviceGridLayoutChoiceODT;
    int		repairsGridLayoutChoiceODT;
    int		buypartsGridLayoutChoiceODT;

    bool	local;
    bool	UTC;
    bool	gpsAuto;
    int		tzIndicator;
    int		tzHour;

    int 	  timeformat;
    bool	  noseconds;
    int       dateformat;
	wxString  showDistance;
	wxString  showBoatSpeed;
    wxString  showWindSpeed;
    wxString  dateseparatorlocale;
    wxString  dateseparatorindiv;
    wxString  sdateformat;
    wxString  stimeformat;
    wxString  stimeformatw;
    int		  date1;
    int       date2;
    int       date3;

    ArrayOfGridColWidth		NavColWidth;
    ArrayOfGridColWidth		WeatherColWidth;
    ArrayOfGridColWidth		MotorColWidth;
    ArrayOfGridColWidth		CrewColWidth;
    ArrayOfGridColWidth		WakeColWidth;
    ArrayOfGridColWidth		EquipColWidth;
    ArrayOfGridColWidth		OverviewColWidth;
    ArrayOfGridColWidth		ServiceColWidth;
    ArrayOfGridColWidth		RepairsColWidth;
    ArrayOfGridColWidth		BuyPartsColWidth;

    int stateEvents;
};
#endif
