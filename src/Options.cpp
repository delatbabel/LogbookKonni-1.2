#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers


#include "Options.h"
#include <wx/msgdlg.h>
#include <wx/mimetype.h>
#include <wx/tokenzr.h>
#include "LogbookDialog.h"


Options::Options( void )
{
    //Date/Time
    timeformat  = -1;
    noseconds   = true;
    dateformat  = 0;
    dateseparatorlocale = _T( "/" );
    dateseparatorindiv  = _T( "/" );
    sdateformat = _T( "" );
    stimeformat = _T( "" );
    date1 = date2 = date3 = 0;
    statusbarGlobal = true;
    layoutShow = false;
    statusbarWatch = true;
    kmlTrans.Add( _T( "50" ) );
    kmlTrans.Add( _T( "FF" ) );
    kmlColor.Add( _T( "FFFFFF" ) );
    kmlColor.Add( _T( "000000" ) );
    kmlColor.Add( _T( "0000FF" ) );
    kmlColor.Add( _T( "14F0FF" ) );
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
    engineStr[0] = _T( "1" );
    engineStr[1] = _T( "2" );
    engineMessageSails = false;
    engineMessageRunning = false;
    engineAllwaysSailsDown = false;
    statusbar = true;
    windspeeds = false;
    numberSails = 14;

    abrSails.Add( _( "Ma" ) );
    abrSails.Add( _( "Tr" ) );
    abrSails.Add( _( "Ge1" ) );
    abrSails.Add( _( "Ge2" ) );
    abrSails.Add( _( "Ge3" ) );
    abrSails.Add( _( "Fo1" ) );
    abrSails.Add( _( "Fo2" ) );
    abrSails.Add( _( "Fo3" ) );
    abrSails.Add( _( "Ji" ) );
    abrSails.Add( _( "Mi" ) );
    abrSails.Add( _( "Mt" ) );
    abrSails.Add( _( "St" ) );
    abrSails.Add( _( "Sp" ) );
    abrSails.Add( _( "?" ) );

    sailsName.Add( _( "Main" ) );
    sailsName.Add( _( "Try" ) );
    sailsName.Add( _( "Genoa 1" ) );
    sailsName.Add( _( "Genoa 2" ) );
    sailsName.Add( _( "Genoa 3" ) );
    sailsName.Add( _( "Fock 1" ) );
    sailsName.Add( _( "Fock 2" ) );
    sailsName.Add( _( "Fock 3" ) );
    sailsName.Add( _( "Jib" ) );
    sailsName.Add( _( "Mizzen" ) );
    sailsName.Add( _( "Mizzen Storm" ) );
    sailsName.Add( _( "Staysail" ) );
    sailsName.Add( _( "Spinnacker" ) );
    sailsName.Add( _( "You can change the abreviations and the names of the sails\nin Options/Miscellaneous/Sails" ) );

    rowGap = 0;
    colGap = 0;

    for ( int i = 0; i < numberSails; i++ )
        bSailIsChecked[i] = false;

    // Global Options
    traditional = true;
    modern      = false;
    showToolTips= true;

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
    for ( int i = 0; i < 7; i++ )
    {
        layoutPrefix[i] =  _T( "Label_" );
        filterLayout[i] =  true;
    }
    noGPS = true;
    colouredMaintenance = true;
    waypointArrived = false;
    waypointText = _( "automatic line Waypoint arrived" );
    guardChange = false;
    guardChangeText = _( "automatic line\nchange of guards" );
    courseChange = false;
    courseChangeDegrees = _T( "90" );
    courseChangeText = _( "automatic line\nchange of course >" );
    courseTextAfterMinutes = _T( "5" );
    everySM = false;
    everySMAmount = _T( "10" );
    everySMText = _( "automatic line\nDistance > " );
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
    thour = _T( "0" );
    tmin = _T( "1" );;
    tsec = _T( "0" );;
    ttext = _( "Automatic line by timer\n\n\
You can set/unset a timer-interval\n\
or change this text in Toolbox/Plugins/Logbook\n\nShift+Enter appends a new line" );
    timerSec = 5;

    // Grid Navigation
    Deg = _T( "\xB0" ); // Apple \xA1 ??
    Min = _( "'" );
    Sec = _( "\"" );

    distance = _( "NM" );
    speed = _( "kts" );

    meter = _( "m" );
    feet = _( "ft" );
    fathom = _( "fth" );

    // Grid Weather
    baro = _( "mb" );
    temperature = _( "F" );

    // Grid MotorSails
    vol = _( "gal" );
    motorh = _( "h" );
    engine = _T( "E" );
    shaft = _T( "S" );
    rpm = _T( "RPM" );

    //Maintenance
    days = _( "day(s)" );
    weeks = _( "week(s)" );
    month = _( "month" );

    //Capacity
    fuelTank   = _T( "0" );
    waterTank  = _T( "0" );
    watermaker = _T( "0" );
    bank1      = _T( "0" );
    bank2      = _T( "0" );
    ampere     = _T( "A" );

    //wxChoice
    showDepth = 0;
    showWaveSwell = 0;
    showWindSpeedchoice = 0;
    showWindSpeed = _T( "kts" );
    showWindHeading = 0;
    showWindDir = 0;
    showHeading = 0;


    overviewAll = 0;

    crewNamingStyle = 0;

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
    htmlEditor  = _T( "" );
    htmlEditorReset = htmlEditor;

    // KML
    kmlRoute = 1;
    kmlTrack = 1;
    kmlLineWidth = _T( "4" );;
    kmlLineTransparancy = 0;
    kmlRouteColor = 0;
    kmlTrackColor = 3;

    //NMEA
    NMEAUseERRPM = false;

#ifdef __WXMSW__
    wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension( _T( "odt" ) );
    wxString command;

    if ( filetype1 )
    {
        if ( ( command = filetype1->GetOpenCommand( _T( "x.odt" ) ) ) != wxEmptyString )
        {
            command = command.Remove( command.find_last_of( _T( " " ) ) );

            odtEditor   = command;
            odtEditorReset = odtEditor;
        }
    }
    else
    {
        odtEditor   = _T( "" );
        odtEditorReset = _T( "" );
    }
#endif
#ifdef __WXGTK__
    odtEditor   = _T( "libreoffice --writer" );
    odtEditorReset = odtEditor;
#endif
#ifdef __WXOSX__
//	wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension(_T("odt"));
    wxFileType *filetype1=wxTheMimeTypesManager->GetFileTypeFromMimeType( _T( "application/vnd.oasis.opendocument.text-template" ) );

    wxString command;

    if ( filetype1 )
    {
        if ( ( command = filetype1->GetOpenCommand( _T( "x.odt" ) ) )!= wxEmptyString )
        {
            command = command.Remove( command.find_last_of( _T( " " ) ) );
            odtEditor   = command;
            odtEditorReset = odtEditor;
        }
    }
#endif

#ifdef __WXMSW__
#include <windows.h>

    filetype1=wxTheMimeTypesManager->GetFileTypeFromExtension( _T( "RSS" ) );

    if ( filetype1 )
    {
        command=filetype1->GetOpenCommand( _T( " " ) );
        command = command.Remove( command.find_first_of( _T( '/' ) )-1 );
        mailClient = command;
        mailClientReset = mailClient;
    }
    else
    {
        OSVERSIONINFO osvi;

        ZeroMemory( &osvi, sizeof( OSVERSIONINFO ) );
        osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

        GetVersionEx( &osvi );

        if ( ( osvi.dwMajorVersion > 5 ) ||
                ( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion >= 1 ) ) )
        {
            mailClient = _T( "C:\\Programme\\Outlook Express\\msimn.exe" );
            mailClientReset = mailClient;
        }
        else
        {
            mailClient = _T( "" );
            mailClientReset = _T( "" );
        }
    }

#endif
#ifdef __WXGTK__
    mailClient = _T( "kmail --composer" );
    mailClientReset = mailClient;
#endif
#ifdef __WXOSX__
    mailClient = _T( "/Applications/Mail.app " );
    mailClientReset = mailClient;
#endif

#ifdef __WXMSW__
    dataManager = _T( "explorer.exe /select," );
    dataManagerReset = dataManager;
#endif
#ifdef __WXGTK__
    dataManager = _T( "dolphin --select" );
    dataManagerReset = dataManager;

    htmlEditor = _T( "libreoffice" );
    htmlEditorReset = htmlEditor;
#endif
#ifdef __WXOSW__
    dataManager = _T( "" );
    dataManagerReset = dataManagerReset;
#endif

    stateEvents = 0;

    for ( int i = 0; i < numberSails; i++ )
        bSailIsChecked[i] = false;
}

Options::~Options( void )
{
}

bool Options::checkStateOfEvents()
{
    if ( timer || guardChange || courseChange || waypointArrived || everySM )
        return true;
    else
        return false;
}

void Options::stopAllEvents()
{
    timer           = false;
    guardChange     = false;
    courseChange    = false;
    waypointArrived = false;
    everySM         = false;
}

void Options::setTimeFormat( int i )
{
    wxString stimeh = _T( "%H:" );
    wxString stimei = _T( "%I:" );
    wxString stimem = _T( "%M" );
    wxString stimes = _T( ":%S" );
    wxString sam    = _T( " %p" );

    if ( i )
    {
        if ( timeformat == 0 )
        {
            stimeformat = stimeh+stimem;
            stimeformatw = stimeformat;
        }
        else
        {
            stimeformat = stimei+stimem+sam;
            stimeformatw = stimeformat;
        }
    }
    else
    {
        if ( timeformat == 0 )
        {
            stimeformat = stimeh+stimem+stimes;
            stimeformatw = stimeh+stimem;
        }
        else
        {
            stimeformat = stimei+stimem+stimes+sam;
            stimeformatw = stimei+stimem+sam;
        }
    }
}

void Options::setOptionsTimeFormat()
{
    wxDateTime sample = wxDateTime::Now();
    wxString am,pm;
    sample.GetAmPmStrings( &am,&pm );

    if ( am.IsEmpty() )
    {
        timeformat = 0;
        hourFormat = true;
    }
    else
    {
        //timeformat = 1;
        hourFormat = false;
    }
}

void Options::setDateFormat()
{
    LogbookDialog::setDatePattern();

    wxArrayString sadate;
    sadate.Add( _T( "%m" ) );
    sadate.Add( _T( "%d" ) );
    sadate.Add( _T( "%Y" ) );

    wxArrayString spattern;
    spattern.Add( _T( "mm" ) );
    spattern.Add( _T( "dd" ) );
    spattern.Add( _T( "yyyy" ) );

    wxArrayInt ar;

    if ( dateformat == 0 )
    {
        wxStringTokenizer tkz( LogbookDialog::datePattern,LogbookDialog::dateSeparator );

        wxString tmp = tkz.GetNextToken();
        if ( tmp.Contains( _T( "m" ) ) )
        {
            sdateformat = sadate[0] + dateseparatorlocale;
            ar.Add( 0 );
        }
        else if ( tmp.Contains( _T( "d" ) ) )
        {
            sdateformat = sadate[1] + dateseparatorlocale;
            ar.Add( 1 );
        }
        else if ( tmp.Contains( _T( "y" ) ) )
        {
            sdateformat = sadate[2] + dateseparatorlocale;
            ar.Add( 2 );
        }

        tmp = tkz.GetNextToken();
        if ( tmp.Contains( _T( "m" ) ) )
        {
            sdateformat += sadate[0] + dateseparatorlocale;
            ar.Add( 0 );
        }
        else if ( tmp.Contains( _T( "d" ) ) )
        {
            sdateformat += sadate[1] + dateseparatorlocale;
            ar.Add( 1 );
        }
        else if ( tmp.Contains( _T( "y" ) ) )
        {
            sdateformat += sadate[2] + dateseparatorlocale;
            ar.Add( 2 );
        }

        tmp = tkz.GetNextToken();
        if ( tmp.Contains( _T( "m" ) ) )
        {
            sdateformat += sadate[0];
            ar.Add( 0 );
        }
        else if ( tmp.Contains( _T( "d" ) ) )
        {
            sdateformat += sadate[1];
            ar.Add( 1 );
        }
        else if ( tmp.Contains( _T( "y" ) ) )
        {
            sdateformat += sadate[2];
            ar.Add( 2 );
        }

        wxString sep = dateseparatorlocale;
        LogbookDialog::datePattern = spattern[ar[0]]+sep+spattern[ar[1]]+sep+spattern[ar[2]];
    }
    else
    {
        wxString sep = dateseparatorindiv;
        sdateformat = sadate[date1]+sep+sadate[date2]+sep+sadate[date3];
        LogbookDialog::datePattern = spattern[date1]+sep+spattern[date2]+sep+spattern[date3];
    }
}
