//#pragma once
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "OverView.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Options.h"
#include "Logbook.h"

#include <wx/filename.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>
#include <wx/generic/gridctrl.h>
#include <wx/dir.h>

#include <map>
using namespace std;
OverView::OverView( LogbookDialog* d, wxString data, wxString lay, wxString layoutODT )
    : Export( d )
{
    parent = d;

    ODTLayout_locn = layoutODT;
    HTMLLayout_locn = lay;
    data_locn = data;
    data_file = data + _T( "overview.html" );
    grid = d->m_gridOverview;
    opt = d->logbookPlugIn->opt;
    selectedRow = 0;
    logbook = d->logbook;
    collection t_coll;
    setLayoutLocation();
    loadAllLogbooks();
}

OverView::~OverView( void )
{
}

void OverView::refresh()
{
    grid->DeleteRows( 0,grid->GetNumberRows() );
    row = -1;
    if ( parent->m_radioBtnActualLogbook->GetValue() )
        actualLogbook();
    else if ( parent->m_radioBtnAllLogbooks->GetValue() )
        allLogbooks();
    else if ( parent->m_radioBtnSelectLogbook->GetValue() )
        if ( !selectedLogbook.IsEmpty() )
            loadLogbookData( selectedLogbook,false );
}

void OverView::viewODT( wxString path,wxString layout,int mode )
{
    wxString fn;

    if ( opt->filterLayout[LogbookDialog::OVERVIEW] )
        layout.Prepend( opt->layoutPrefix[LogbookDialog::OVERVIEW] );

    fn = toODT( path, layout, mode );

    if ( layout != _T( "" ) )
    {
        fn.Replace( _T( "txt" ),_T( "odt" ) );
        parent->startApplication( fn,_T( ".odt" ) );
    }
}

void OverView::viewHTML( wxString path,wxString layout,int mode )
{
    wxString fn;

    if ( opt->filterLayout[LogbookDialog::OVERVIEW] )
        layout.Prepend( opt->layoutPrefix[LogbookDialog::OVERVIEW] );

    fn = toHTML( path, layout, mode );

    if ( layout != _T( "" ) )
    {
        fn.Replace( _T( "txt" ),_T( "html" ) );
        parent->startBrowser( fn );
    }
}

wxString OverView::toODT( wxString path,wxString layout,int mode )
{
    wxString top;
    wxString header;
    wxString middle;
    wxString bottom;
    wxString tempPath = data_file;

    wxString odt = readLayoutODT( layout_locn,layout );
    if ( !cutInPartsODT( odt, &top, &header,	&middle, &bottom ) )
        return _T( "" );

    if ( mode == 1 )
        tempPath.Replace( _T( "html" ),_T( "txt" ) );
    wxTextFile* text = setFiles( path, &tempPath, mode );
    writeToODT( text,parent->m_gridOverview,tempPath,layout_locn+layout+_T( ".odt" ), top,header,middle,bottom,mode );

    return tempPath;
}

wxString OverView::toHTML( wxString path,wxString layout,int mode )
{
    wxString top;
    wxString header;
    wxString middle;
    wxString bottom;
    wxString tempPath = data_file;

    wxString html = readLayoutHTML( layout_locn,layout );
    if ( !cutInPartsHTML( html, &top, &header, &middle, &bottom ) )
        return _T( "" );

    wxTextFile* text = setFiles( path, &tempPath, mode );
    writeToHTML( text,parent->m_gridOverview,tempPath,layout_locn+layout+_T( ".html" ), top,header,middle,bottom,mode );

    return tempPath;
}

void OverView::loadAllLogbooks()
{
    wxArrayString		files;

    logbooks.clear();
    int i = wxDir::GetAllFiles( data_locn,&files,_T( "*logbook*.txt" ),wxDIR_FILES );

    for ( int f = 0; f < i; f++ )
    {
        //wxFileName name(files.Item(f));
        //if(name.GetName().Contains(_T("logbook")))
        logbooks.Add( files[f] );
    }
}

void OverView::selectLogbook()
{
    SelectLogbook selLogbook( parent,data_locn );

    if ( selLogbook.ShowModal() == wxID_CANCEL )
        return;

    if ( selLogbook.selRow == -1 ) return;

    parent->m_radioBtnSelectLogbook->SetValue( true );
    grid->DeleteRows( 0,grid->GetNumberRows() );
    row = -1;

    selectedLogbook = selLogbook.files[selLogbook.selRow];
    showAllLogbooks = false;
    loadLogbookData( selectedLogbook,false );
    opt->overviewAll = 2;
}

void OverView::actualLogbook()
{
    showAllLogbooks = false;
    clearGrid();
    for ( unsigned int i=0; i < logbooks.size(); i++ )
        if ( !logbooks[i].Contains( _T( "until" ) ) )
        {
            loadLogbookData( logbooks[i],false );
            break;
        }
    opt->overviewAll = 0;
}

void OverView::allLogbooks()
{
    showAllLogbooks = true;
    loadAllLogbooks();
    clearGrid();
    for ( unsigned int i = 0; i < logbooks.Count(); i++ )
    {
        if ( i % 2 )
            loadLogbookData( logbooks[i],true );
        else
            loadLogbookData( logbooks[i],false );
    }
    opt->overviewAll = 1;
}

void OverView::clearGrid()
{
    if ( row != -1 )
        grid->DeleteRows( 0,grid->GetNumberRows() );
    row = -1;
}

void OverView::loadLogbookData( wxString logbook, bool colour )
{
    wxString t,s,temp;
    bool test = true;
    bool overviewflag = false;
    //	bool write = true;
    double x = 0;
    wxStringTokenizer tkz1;
    wxTimeSpan span;
    wxDateSpan oneday(0,0,0,1);
    wxDateTime enddt;
    
    collection::iterator it;

    resetValues();

    wxFileInputStream input( logbook );
    wxTextInputStream* stream = new wxTextInputStream ( input );
    wxString path = logbook;
    wxFileName fn( logbook );
    logbook = fn.GetName();
    if ( logbook == _T( "logbook" ) )
        logbook = _( "Active Logbook" );
    else
    {
        wxDateTime dt = parent->getDateTo( logbook );
        logbook = _( "Logbook until " )+dt.FormatDate();
    }

    int lastrow = 0;
    oneLogbookTotalReset();

    wxString route = _T( "xxx" );
    int rowNewLogbook = -1;

    stream->ReadLine(); // skip line with #1.2#
    int month = 0,day = 0,year = 0,hour = 0,min = 0,sec = 0;

    while ( ( !( t = stream->ReadLine() ).IsEmpty() ) )
    {
        sign = wxEmptyString;
        rowNewLogbook++;
        wxStringTokenizer tkz( t, _T( "\t" ),wxTOKEN_RET_EMPTY );
        int c = 0;
        while ( tkz.HasMoreTokens() )
        {
            s = parent->restoreDangerChar( tkz.GetNextToken() );
            s.RemoveLast();
            s.Replace( _T( "," ),_T( "." ) );

            switch ( c )
            {
            case ROUTE:
                if ( route != s )
                {
                    //write = true;
                    resetValues();
                    grid->AppendRows();
                    route = s;
                    row++;
                    lastrow = row;
                    test = true;
                    grid->SetCellValue( row,FROUTE,s );
                }
                else ;
                //write = false;

                break;
            case DATEM:
                if ( s.IsEmpty() )
                    month = -1;
                else
                    month = wxAtoi( s );
                break;
            case DATED:
                if ( s.IsEmpty() )
                    day = -1;
                else
                    day = wxAtoi( s );
                break;
            case DATEY:
            {
                year = wxAtoi( s );
                if ( day == -1 || month == -1 ) continue;

                wxDateTime tmp;
                tmp.Set( day,( wxDateTime::Month ) month,year );
                s = tmp.Format( opt->sdateformat );

                if ( s != startdate && !test && !showAllLogbooks && opt->overviewlines )
                {
                    temp = route;
                    endtime = "00:00";
                    parent->myParseDate( startdate,enddt );
                    enddt.Add( oneday );
                    enddate = enddt.Format(opt->sdateformat);
                    //write = true;
                    writeSumColumn( lastrow, logbook, path, colour );
                    resetValues();
                    grid->AppendRows();
                    route = temp;
                    overviewflag = true;
                    row++;
                    lastrow = row;
                    test = true;
                    grid->SetCellValue( row,FROUTE,route );
                }

                if ( test )
                {
                    startdate = s;
                    enddate = s;
                    if ( rowNewLogbook == 0 )
                        oneLogbookTotal.logbookStart = oneLogbookTotal.logbookEnd = s;
                }
                else
                    enddate = s;

                if ( etmaldate != s )
                {
                    etmaldate = s;
                    etmalcount++;
                    bestetmaltemp = 0;
                }

                date = s;
            }
            break;
            case TIMEH:
                if ( s.IsEmpty() )
                    hour = -1;
                else
                    hour = wxAtoi( s );
                break;
            case TIMEM:
                if ( s.IsEmpty() )
                    min = -1;
                else
                    min = wxAtoi( s );
                break;
            case TIMES:
            {
                if ( hour == -1 || min == -1 ) continue;
                sec = wxAtoi( s );
                wxDateTime tmp;
                tmp.Set( hour,min,sec );
                s = tmp.Format( opt->stimeformat );

                if ( test )
                {
                    if ( overviewflag )
                        starttime = "00:00";
                    else
                        starttime = s;
                    endtime = s;
                    if ( rowNewLogbook == 0 )
                        oneLogbookTotal.logbookTimeStart = oneLogbookTotal.logbookTimeEnd = s;
                }
                else
                    endtime = s;
            }
            break;
            case STATUS:
                sign = s;
                break;
            case WATCH:
                break;
            case DISTANCE:
                if ( !s.IsEmpty() && sign != 'S' ) break;
                s.ToDouble( &x );

                distance += x;
                etmal = distance/etmalcount;

                if ( date == etmaldate )
                {
                    bestetmaltemp += x;
                    if ( bestetmaltemp > bestetmal )
                        bestetmal = bestetmaltemp;
                    if ( bestetmaltemp > oneLogbookTotal.bestetmal )
                        oneLogbookTotal.bestetmal = bestetmaltemp;
                }
                oneLogbookTotal.distance += x;
                //allLogbooksTotal.distance += x;
                break;
            case DISTANCETOTAL:
            case POSITION:
            case COG:
            case HEADING:
                break;
            case SOG:
                if ( !s.IsEmpty() && sign == _T( "S" ) )
                {
                    s.ToDouble( &x );
                    speed += x;
                    oneLogbookTotal.speed += x;
                    speedcount++;
                    oneLogbookTotal.speedcount++;
                    if ( x > speedpeak ) speedpeak = x;
                    if ( x > oneLogbookTotal.speedpeak ) oneLogbookTotal.speedpeak = x;
                }
                break;
            case STW:
                if ( !s.IsEmpty() && sign == _T( "S" ) )
                {
                    s.ToDouble( &x );
                    speedSTW += x;
                    oneLogbookTotal.speedSTW += x;
                    speedcountSTW++;
                    oneLogbookTotal.speedcountSTW++;
                    if ( x > speedpeakSTW ) speedpeakSTW = x;
                    if ( x > oneLogbookTotal.speedpeakSTW ) oneLogbookTotal.speedpeakSTW = x;
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
                if ( !s.IsEmpty() )
                {
                    s.ToDouble( &x );
                    winddir += x;
                    oneLogbookTotal.winddir += x;
                    windcount++;
                    oneLogbookTotal.windcount++;
                }
                break;
            case WINDFORCE:
                if ( !s.IsEmpty() )
                {
                    if ( s.Length() < 11 )
                    {
                        s.ToDouble( &x );
                        wind += x;
                        oneLogbookTotal.wind += x;
                        if ( x > windpeak ) windpeak = x;
                        if ( x > oneLogbookTotal.windpeak ) oneLogbookTotal.windpeak = x;
                    }
                    else
                    {
                        wxArrayString WS;
                        WS = wxStringTokenize( s,_T( "|" ) );
                        WS[1].ToDouble( &x );
                        wind += x;
                        oneLogbookTotal.wind += x;
                        WS[2].ToDouble( &x );
                        if ( x > windpeak ) windpeak = x;
                        if ( x > oneLogbookTotal.windpeak ) oneLogbookTotal.windpeak = x;
                    }
                }
                break;
            case CURRENT:
                if ( !s.IsEmpty() )
                {
                    s.ToDouble( &x );
                    currentdir += x;
                    oneLogbookTotal.currentdir += x;
                    currentcount++;
                    oneLogbookTotal.currentcount++;
                }
                break;
            case CURRENTFORCE:
                if ( !s.IsEmpty() )
                {
                    s.ToDouble( &x );
                    current += x;
                    oneLogbookTotal.current += x;
                    if ( x > currentpeak ) currentpeak = x;
                    if ( x > oneLogbookTotal.currentpeak ) oneLogbookTotal.currentpeak = x;
                }
                break;
            case WAVE:
                if ( !s.IsEmpty() )
                {
                    s.ToDouble( &x );
                    wave += x;
                    oneLogbookTotal.wave += x;
                    wavecount++;
                    oneLogbookTotal.wavecount++;
                    if ( x > wavepeak ) wavepeak = x;
                    if ( x > oneLogbookTotal.wavepeak ) oneLogbookTotal.wavepeak = x;
                }
                break;
            case SWELL:
                if ( !s.IsEmpty() )
                {
                    s.ToDouble( &x );
                    swell += x;
                    oneLogbookTotal.swell += x;
                    swellcount++;
                    oneLogbookTotal.swellcount++;
                    if ( x > swellpeak ) swellpeak = x;
                    if ( x > oneLogbookTotal.swellpeak ) oneLogbookTotal.swellpeak = x;
                }
                break;
            case WEATHER:
            case CLOUDS:
            case VISIBILITY:
                break;
            case ENGINE1:
                if ( s.IsEmpty() ) continue;

                tkz1.SetString( s, _T( ":" ),wxTOKEN_RET_EMPTY );
                long hours, minutes;
                tkz1.GetNextToken().ToLong( &hours );
                tkz1.GetNextToken().ToLong( &minutes );
                enginehours += hours;
                enginemin += minutes;
                if ( enginemin >= 60 )
                {
                    enginehours++;
                    enginemin -= 60;
                }
                oneLogbookTotal.enginehours += hours;
                oneLogbookTotal.enginemin += minutes;
                if ( oneLogbookTotal.enginemin >= 60 )
                {
                    oneLogbookTotal.enginehours++;
                    oneLogbookTotal.enginemin -= 60;
                }
                break;
            case ENGINE2:
                if ( s.IsEmpty() ) continue;

                {
                    tkz1.SetString( s, _T( ":" ),wxTOKEN_RET_EMPTY );
                    long hours, minutes;
                    tkz1.GetNextToken().ToLong( &hours );
                    tkz1.GetNextToken().ToLong( &minutes );
                    enginehours2 += hours;
                    enginemin2 += minutes;
                    if ( enginemin2 >= 60 )
                    {
                        enginehours2++;
                        enginemin2 -= 60;
                    }
                    oneLogbookTotal.enginehours2 += hours;
                    oneLogbookTotal.enginemin2 += minutes;
                    if ( oneLogbookTotal.enginemin2 >= 60 )
                    {
                        oneLogbookTotal.enginehours2++;
                        oneLogbookTotal.enginemin2 -= 60;
                    }
                }
                break;
            case GENERATOR:
                if ( s.IsEmpty() ) continue;

                {
                    tkz1.SetString( s, _T( ":" ),wxTOKEN_RET_EMPTY );
                    long hours, minutes;
                    tkz1.GetNextToken().ToLong( &hours );
                    tkz1.GetNextToken().ToLong( &minutes );
                    generatorhours += hours;
                    generatormin += minutes;
                    if ( generatormin >= 60 )
                    {
                        generatorhours++;
                        generatormin -= 60;
                    }
                    oneLogbookTotal.generatorhours += hours;
                    oneLogbookTotal.generatormin += minutes;
                    if ( oneLogbookTotal.generatormin >= 60 )
                    {
                        oneLogbookTotal.generatorhours++;
                        oneLogbookTotal.generatormin -= 60;
                    }
                }
                break;
            case WATERM:
                if ( s.IsEmpty() ) continue;

                {
                    tkz1.SetString( s, _T( ":" ),wxTOKEN_RET_EMPTY );
                    long hours, minutes;
                    tkz1.GetNextToken().ToLong( &hours );
                    tkz1.GetNextToken().ToLong( &minutes );
                    watermhours += hours;
                    watermmin += minutes;
                    if ( watermmin >= 60 )
                    {
                        watermhours++;
                        watermmin -= 60;
                    }
                    oneLogbookTotal.watermhours += hours;
                    oneLogbookTotal.watermmin += minutes;
                    if ( oneLogbookTotal.watermmin >= 60 )
                    {
                        oneLogbookTotal.watermhours++;
                        oneLogbookTotal.watermmin -= 60;
                    }
                }
                break;
            case ENGINE1T:
                break;
            case FUEL:
                if ( s.IsEmpty() ) continue;

                if ( s.GetChar( 0 ) == '+' ) break;
                s.ToDouble( &x );
                if ( s.GetChar( 0 ) != '-' ) s.Prepend( _T( "-" ) ); // version 0.910 has no minus sign

                //	if(x < 0)
                //	{
                fuel += x;
                oneLogbookTotal.fuel += x;
                //	}
                break;
            case WATERMO:
                if ( s.IsEmpty() ) continue;
                s.ToDouble( &x );
                //	if(x < 0)
                //	{
                watermo += x;
                oneLogbookTotal.watermo += x;
                //	}
                break;
            case FUELTOTAL:
                break;
            case SAILS:
                if ( !s.empty() )
                {
                    bool found = false;
                    wxString result;
                    for ( it = t_coll.begin(); it != t_coll.end(); ++it )
                    {
                        if ( s == it->first )
                        {
                            int a = it->second;
                            it->second = ++a;
                            result += it->first+_T( "\n" );
                            found = true;
                        }
                    }
                    if ( !found )
                    {
                        t_coll.insert( pair( s,1 ) );
                        wxString result;
                        for ( it = t_coll.begin(); it != t_coll.end(); ++it )
                        {
                            result = it->first;
                        }
                    }
                }
                break;
            case REEF:
                break;
            case WATER:
                if ( s.IsEmpty() ) continue;
                if ( s.GetChar( 0 ) == '+' ) break;				 // add water used only
                if ( s.GetChar( 0 ) != '-' ) s.Prepend( _T( "-" ) ); // version 0.910 has no minus sign
                s.ToDouble( &x );

                x = fabs( x );
                //	if(x < 0)
                //	{
                water += x;
                oneLogbookTotal.water += x;
                //	}
                break;
            case BANK1:
                if ( s.IsEmpty() ) continue;
                s.ToDouble( &x );
                if ( x >= 0 )
                {
                    bank1g += x;
                    oneLogbookTotal.bank1g += x;
                }
                else
                {
                    bank1u += fabs( x );
                    oneLogbookTotal.bank1u += bank1u;
                }
                break;
            case BANK2:
                if ( s.IsEmpty() ) continue;
                s.ToDouble( &x );
                if ( x >= 0 )
                {
                    bank2g += x;
                    oneLogbookTotal.bank2g += x;
                }
                else
                {
                    bank2u += fabs( x );
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
        //if(test)
        writeSumColumn( lastrow, logbook, path, colour );
        test = false;
        overviewflag = false;
    }
    if ( !showAllLogbooks )
        writeSumColumnLogbook( oneLogbookTotal,lastrow, logbook, colour );
}

void OverView::resetValues()
{
    startdate =_T( "" );
    enddate = _T( "" );
    starttime =_T( "" );
    endtime = _T( "" );
    etmaldate = _T( "" );
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
    currentdir=0;
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

void OverView::oneLogbookTotalReset()
{
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

void OverView::writeSumColumn( int row, wxString logbook, wxString path, bool colour )
{
    wxString d, sail;
    wxString nothing = _T( "-----" );

    switch ( opt->showWaveSwell )
    {
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

    grid->SetCellAlignment( row,FLOG,wxALIGN_LEFT, wxALIGN_TOP );
    grid->SetCellAlignment( row,FROUTE,wxALIGN_LEFT, wxALIGN_TOP );
    grid->SetCellAlignment( row,FSAILS,wxALIGN_LEFT, wxALIGN_TOP );

    grid->SetCellValue( row,FLOG,logbook );
    grid->SetCellValue( row,FSTART,startdate );
    grid->SetCellValue( row,FEND,enddate );

    wxString temp = wxString::Format( _T( "%6.2f %s" ),distance,opt->distance.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FDISTANCE,temp );
    temp = wxString::Format( _T( "%6.2f %s" ),etmal,opt->distance.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FETMAL,temp );
    temp = wxString::Format( _T( "%6.2f %s" ),bestetmal,opt->distance.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBESTETMAL,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),fabs( fuel ),opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FFUEL,temp );
    temp = wxString::Format( _T( "%6.2f %s" ),fabs( water ),opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATER,temp );

    temp = wxString::Format( _T( "%3.2f %s" ),bank1u,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK1U,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),bank1g,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK1G,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),bank2u,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK2U,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),bank2g,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK2G,temp );

    if ( windcount )
        temp = wxString::Format( _T( "%6.2f %s" ),wind/windcount,_T( "kts" ) );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWIND,temp );

    if ( windcount )
        temp = wxString::Format( _T( "%6.2f %s" ),winddir/windcount,opt->Deg.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWINDDIR,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),windpeak,_T( "kts" ) );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWINDPEAK,temp );

    if ( wavecount )
        temp = wxString::Format( _T( "%6.2f %s" ),wave/wavecount,d.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWAVE,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),wavepeak,d.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWAVEPEAK,temp );

    if ( swellcount )
        temp = wxString::Format( _T( "%6.2f %s" ),swell/swellcount,d.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSWELL,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),swellpeak,d.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSWELLPEAK,temp );

    if ( currentcount )
        temp = wxString::Format( _T( "%6.2f %s" ),currentdir/currentcount,opt->Deg.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENTDIR,temp );

    if ( currentcount )
        temp = wxString::Format( _T( "%6.2f %s" ),current/currentcount,opt->speed.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENT,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),currentpeak,opt->speed.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENTPEAK,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),enginehours,enginemin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FENGINE1,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),enginehours2,enginemin2,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FENGINE2,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),generatorhours,generatormin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FGENERATOR,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),watermhours,watermmin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATERM,temp );

    temp = wxString::Format( _T( "%3.2f %s" ),watermo,opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATERMO,temp );


    if ( speedcount )
        temp = wxString::Format( _T( "%6.2f %s" ),speed/speedcount,opt->speed.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSPEED,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),speedpeak,opt->speed.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBSPEED,temp );

    //	if(speedcountSTW)
    //		temp = wxString::Format(_T("%6.2f %s"),speedSTW/speedcountSTW,opt->speed.c_str());
    //	else
    //		temp = nothing;
    //	temp.Replace(_T("."),parent->decimalPoint);
    //	grid->SetCellValue(row,FSPEEDSTW,temp);

    //	temp = wxString::Format(_T("%6.2f %s"),speedpeakSTW,opt->speed.c_str());
    //	temp.Replace(_T("."),parent->decimalPoint);
    //	grid->SetCellValue(row,FBSPEEDSTW,temp);

    grid->SetCellValue( row,FPATH,path );

    wxDateTime startdt, enddt, timet;

    if ( ( startdate.IsEmpty() || enddate.IsEmpty() ) ||
            ( starttime.IsEmpty() || endtime.IsEmpty() ) )
        return;

    parent->myParseTime( starttime,timet );
    wxTimeSpan timesp( timet.GetHour(),timet.GetMinute() );
    parent->myParseDate( startdate,startdt );
    startdt.Add( timesp );

    parent->myParseTime( endtime,timet );
    wxTimeSpan timespe( timet.GetHour(),timet.GetMinute() );
    parent->myParseDate( enddate,enddt );
    enddt.Add( timespe );

    wxTimeSpan journey = enddt.Subtract( startdt );
    wxTimeSpan time;
    if ( journey.GetDays() > 0 )
    {
        journey.GetMinutes();
        wxTimeSpan t( 0,journey.GetMinutes()-( journey.GetDays()*24*60 ) );
        time = t;
    }
    else
    {
        wxTimeSpan t( 0,journey.GetMinutes() );
        time = t;
    }
    grid->SetCellValue( row,FJOURNEY,wxString::Format( _T( "%s %s %s %s" ),
                        journey.Format( _T( "%D" ) ).c_str(),opt->days.c_str(),time.Format( _T( "%H:%M" ) ).c_str(),opt->motorh.c_str() ) );

    int max = 0;
    wxString result;
    collection::iterator it;

    for ( it = t_coll.begin(); it != t_coll.end(); ++it )
        if ( it->second >= max )
        {
            sail = it->first;
            max = it->second;
        }
    grid->SetCellValue( row,FSAILS,sail );

    if ( colour )
        for ( int i = 0; i < grid->GetNumberCols(); i++ )
            grid->SetCellBackgroundColour( row,i,wxColour( 230,230,230 ) );
}

void OverView::writeSumColumnLogbook( total data, int row, wxString logbook, bool colour )
{
    wxString nothing = _T( "-----" );

    parent->m_gridOverview->AppendRows();
    row = parent->m_gridOverview->GetNumberRows()-1;
    for ( int i= 0; i < parent->m_gridOverview->GetNumberCols(); i++ )
        grid->SetCellBackgroundColour( row,i,wxColour( 156,156,156 ) );

    wxString d, sail;
    switch ( opt->showWaveSwell )
    {
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

    grid->SetCellAlignment( row,FLOG,wxALIGN_LEFT, wxALIGN_TOP );
    grid->SetCellAlignment( row,FROUTE,wxALIGN_LEFT, wxALIGN_TOP );
    grid->SetCellAlignment( row,FSAILS,wxALIGN_LEFT, wxALIGN_TOP );

    grid->SetCellValue( row,FLOG,_( "Logbook Total" ) );
    grid->SetCellValue( row,FSTART,data.logbookStart );
    grid->SetCellValue( row,FEND,enddate );

    wxString temp = wxString::Format( _T( "%6.2f %s" ),data.distance,opt->distance.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FDISTANCE,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.bestetmal,opt->distance.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBESTETMAL,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),fabs( data.fuel ),opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FFUEL,temp );
    temp = wxString::Format( _T( "%6.2f %s" ),fabs( data.water ),opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATER,temp );

    temp = wxString::Format( _T( "%3.2f %s" ),data.bank1u,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK1U,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),data.bank1g,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK1G,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),data.bank2u,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK2U,temp );
    temp = wxString::Format( _T( "%3.2f %s" ),data.bank2g,opt->ampereh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBANK2G,temp );

    if ( data.windcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.wind/data.windcount,_T( "kts" ) );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWIND,temp );

    if ( data.windcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.winddir/data.windcount,opt->Deg.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWINDDIR,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.windpeak,_T( "kts" ) );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWINDPEAK,temp );

    if ( data.wavecount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.wave/data.wavecount,d.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWAVE,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.wavepeak,d.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWAVEPEAK,temp );

    if ( data.swellcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.swell/data.swellcount,d.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSWELL,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.swellpeak,d.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSWELLPEAK,temp );

    if ( data.currentcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.currentdir/data.currentcount,opt->Deg.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENTDIR,temp );

    if ( data.currentcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.current/data.currentcount,opt->speed.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENT,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.currentpeak,opt->speed.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FCURRENTPEAK,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),data.enginehours,data.enginemin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FENGINE1,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),data.enginehours2,data.enginemin2,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FENGINE2,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),data.generatorhours,data.generatormin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FGENERATOR,temp );

    temp = wxString::Format( _T( "%0002i:%02i %s" ),data.watermhours,data.watermmin,opt->motorh.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATERM,temp );

    temp = wxString::Format( _T( "%3.2f %s" ),data.watermo,opt->vol.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FWATERMO,temp );

    if ( data.speedcount )
        temp = wxString::Format( _T( "%6.2f %s" ),data.speed/data.speedcount,opt->speed.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FSPEED,temp );

    temp = wxString::Format( _T( "%6.2f %s" ),data.speedpeak,opt->speed.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );
    grid->SetCellValue( row,FBSPEED,temp );

    if ( data.speedcountSTW )
        temp = wxString::Format( _T( "%6.2f %s" ),data.speedSTW/data.speedcountSTW,opt->speed.c_str() );
    else
        temp = nothing;
    temp.Replace( _T( "." ),parent->decimalPoint );

    temp = wxString::Format( _T( "%6.2f %s" ),data.speedpeakSTW,opt->speed.c_str() );
    temp.Replace( _T( "." ),parent->decimalPoint );

    wxDateTime startdt, enddt, startdtd, enddtd, time;

    if ( ( data.logbookStart.IsEmpty() || enddate.IsEmpty() ) ||
            ( data.logbookTimeStart.IsEmpty() || endtime.IsEmpty() ) )
        return;

    parent->myParseTime( data.logbookTimeStart,time );
    wxTimeSpan timesp( time.GetHour(),time.GetMinute() );
    parent->myParseDate( data.logbookStart,startdt );
    startdt.Add( timesp );

    parent->myParseTime( endtime,time );
    wxTimeSpan timespe( time.GetHour(),time.GetMinute() );
    parent->myParseDate( enddate,enddt );
    enddt.Add( timespe );

    wxTimeSpan journey = enddt.Subtract( startdt );
    if ( journey.GetWeeks() > 3 )
    {
        int years = 0;
        int month;
        int days;

        //years = journey.GetDays() / 365 ;


        years = enddt.GetYear()-startdt.GetYear();

        int bd = startdt.GetDay(), bm = startdt.GetMonth() ,by = startdt.GetYear(),
            cd = enddt.GetDay(),cm = enddt.GetMonth(),cy = enddt.GetYear(),ad,am,ay;
        if ( cd<bd )
        {
            cm=cm-1;
            cd=cd+startdt.GetLastMonthDay().GetDay();
        }
        if ( cm<bm )
        {
            cy=cy-1;
            cm=cm+12;
        }
        ad=cd-bd;
        am=cm-bm;
        ay=cy-by;

        month = startdt.GetMonth()-enddt.GetMonth();
        if ( month < 0 && startdt.GetYear() != enddt.GetYear() )
        {
            month = 12 + month;
            years--;
        }
        else
            month = enddt.GetMonth()-startdt.GetMonth();

        days = enddt.GetDay() - startdt.GetDay();
        if ( days < 0 )
        {
            days = startdt.GetDay() - days;
            month --;
        }
        /*		else
        {
        month--;
        days = enddt.GetLastMonthDay((wxDateTime::Month)month).GetDay() - (startdt.GetDay() - enddt.GetDay());
        }
        */
        grid->SetCellValue( row,FJOURNEY,wxString::Format( _( "%i Year(s) %i Month(s) %i Day(s)" ),ay,am,ad ) );
    }
    else
        grid->SetCellValue( row,FJOURNEY,journey.Format( _( "%E Week(s) %D Day(s) %H:%M " ) )+opt->motorh );

    int max = 0;
    wxString result;
    collection::iterator it;

    for ( it = t_coll.begin(); it != t_coll.end(); ++it )
        if ( it->second >= max )
        {
            sail = it->first;
            max = it->second;
        }
    grid->SetCellValue( row,FSAILS,sail );

    if ( colour )
        for ( int i = 0; i < grid->GetNumberCols(); i++ )
            grid->SetCellBackgroundColour( row,i,wxColour( 230,230,230 ) );
}

void OverView::setLayoutLocation()
{
    bool radio = parent->m_radioBtnHTMLOverview->GetValue();
    if ( radio )
        layout_locn = HTMLLayout_locn;
    else
        layout_locn = ODTLayout_locn;
    this->layout_locn = layout_locn;

    this->layout_locn.Append( _T( "overview" ) );
    parent->appendOSDirSlash( &layout_locn );

    parent->loadLayoutChoice( LogbookDialog::OVERVIEW,layout_locn,parent->overviewChoice,opt->layoutPrefix[LogbookDialog::OVERVIEW] );
    if ( radio )
        parent->overviewChoice->SetSelection( opt->overviewGridLayoutChoice );
    else
        parent->overviewChoice->SetSelection( opt->overviewGridLayoutChoiceODT );
}

void OverView::setSelectedRow( int row )
{
    selectedRow = row;
}

void OverView::gotoRoute()
{
    wxString file = grid->GetCellValue( selectedRow,FLOG );
    wxString route =  grid->GetCellValue( selectedRow,FROUTE );
    wxString date =  grid->GetCellValue( selectedRow,FSTART );
    wxString path  =  grid->GetCellValue( selectedRow,FPATH );

    if ( logbook->data_locn != path )
    {
        logbook->data_locn = path;
        logbook->loadSelectedData( path );
    }

    int i;
    for ( i = 0; i < parent->m_gridGlobal->GetNumberRows(); i++ )
    {
        if ( parent->m_gridGlobal->GetCellValue( i,0 ) == route &&
                parent->m_gridGlobal->GetCellValue( i,1 ) == date )
            break;
    }

    parent->m_gridGlobal->MakeCellVisible( i,0 );
    parent->m_gridGlobal->SelectRow( i );
    parent->m_gridWeather->MakeCellVisible( i,0 );
    parent->m_gridWeather->SelectRow( i );
    parent->m_gridMotorSails->MakeCellVisible( i,0 );
    parent->m_gridMotorSails->SelectRow( i );

    parent->m_logbook->SetSelection( 0 );

}

wxString OverView::setPlaceHolders( int mode, wxGrid *grid, int row, wxString middle )
{
    wxString newMiddleODT = middle;

    newMiddleODT.Replace( wxT( "#FLOG#" ),replaceNewLine( mode,grid->GetCellValue( row,FLOG ),false ) );
    newMiddleODT.Replace( wxT( "#LLOG#" ),grid->GetTable()->GetColLabelValue( FLOG ) );
    newMiddleODT.Replace( wxT( "#FROUTE#" ),replaceNewLine( mode,grid->GetCellValue( row,FROUTE ),false ) );
    newMiddleODT.Replace( wxT( "#LROUTE#" ),grid->GetTable()->GetColLabelValue( FROUTE ) );
    newMiddleODT.Replace( wxT( "#FSTART#" ),replaceNewLine( mode,grid->GetCellValue( row,FSTART ),false ) );
    newMiddleODT.Replace( wxT( "#LSTART#" ),grid->GetTable()->GetColLabelValue( FSTART ) );
    newMiddleODT.Replace( wxT( "#FEND#" ),replaceNewLine( mode,grid->GetCellValue( row,FEND ),false ) );
    newMiddleODT.Replace( wxT( "#LEND#" ),grid->GetTable()->GetColLabelValue( FEND ) );
    newMiddleODT.Replace( wxT( "#FJOURNEY#" ),replaceNewLine( mode,grid->GetCellValue( row,FJOURNEY ),false ) );
    newMiddleODT.Replace( wxT( "#LJOURNEY#" ),grid->GetTable()->GetColLabelValue( FJOURNEY ) );
    newMiddleODT.Replace( wxT( "#FDISTANCE#" ),replaceNewLine( mode,grid->GetCellValue( row,FDISTANCE ),false ) );
    newMiddleODT.Replace( wxT( "#LDISTANCE#" ),grid->GetTable()->GetColLabelValue( FDISTANCE ) );
    newMiddleODT.Replace( wxT( "#FETMAL#" ),replaceNewLine( mode,grid->GetCellValue( row,FETMAL ),false ) );
    newMiddleODT.Replace( wxT( "#LETMAL#" ),grid->GetTable()->GetColLabelValue( FETMAL ) );
    newMiddleODT.Replace( wxT( "#FBESTETMAL#" ),replaceNewLine( mode,grid->GetCellValue( row,FBESTETMAL ),false ) );
    newMiddleODT.Replace( wxT( "#LBESTETMAL#" ),grid->GetTable()->GetColLabelValue( FBESTETMAL ) );
    newMiddleODT.Replace( wxT( "#FSPEED#" ),replaceNewLine( mode,grid->GetCellValue( row,FSPEED ),false ) );
    newMiddleODT.Replace( wxT( "#LSPEED#" ),grid->GetTable()->GetColLabelValue( FSPEED ) );
    newMiddleODT.Replace( wxT( "#FBSPEED#" ),replaceNewLine( mode,grid->GetCellValue( row,FBSPEED ),false ) );
    newMiddleODT.Replace( wxT( "#LBSPEED#" ),grid->GetTable()->GetColLabelValue( FBSPEED ) );
    newMiddleODT.Replace( wxT( "#FENGINE1#" ),replaceNewLine( mode,grid->GetCellValue( row,FENGINE2 ),false ) );
    newMiddleODT.Replace( wxT( "#LENGINE1#" ),grid->GetTable()->GetColLabelValue( FENGINE2 ) );
    newMiddleODT.Replace( wxT( "#FGENERATOR#" ),replaceNewLine( mode,grid->GetCellValue( row,FGENERATOR ),false ) );
    newMiddleODT.Replace( wxT( "#LGENERATOR#" ),grid->GetTable()->GetColLabelValue( FGENERATOR ) );
    newMiddleODT.Replace( wxT( "#FWATERM#" ),replaceNewLine( mode,grid->GetCellValue( row,FWATERM ),false ) );
    newMiddleODT.Replace( wxT( "#LWATERM#" ),grid->GetTable()->GetColLabelValue( FWATERM ) );
    newMiddleODT.Replace( wxT( "#FWATERMO#" ),replaceNewLine( mode,grid->GetCellValue( row,FWATERMO ),false ) );
    newMiddleODT.Replace( wxT( "#LWATERMO#" ),grid->GetTable()->GetColLabelValue( FWATERMO ) );
    newMiddleODT.Replace( wxT( "#FBANK1G#" ),replaceNewLine( mode,grid->GetCellValue( row,FBANK1G ),false ) );
    newMiddleODT.Replace( wxT( "#LBANK1G#" ),grid->GetTable()->GetColLabelValue( FBANK1G ) );
    newMiddleODT.Replace( wxT( "#FBANK1U#" ),replaceNewLine( mode,grid->GetCellValue( row,FBANK1U ),false ) );
    newMiddleODT.Replace( wxT( "#LBANK1U#" ),grid->GetTable()->GetColLabelValue( FBANK1U ) );
    newMiddleODT.Replace( wxT( "#FBANK2G#" ),replaceNewLine( mode,grid->GetCellValue( row,FBANK2G ),false ) );
    newMiddleODT.Replace( wxT( "#LBANK2G#" ),grid->GetTable()->GetColLabelValue( FBANK2G ) );
    newMiddleODT.Replace( wxT( "#FBANK2U#" ),replaceNewLine( mode,grid->GetCellValue( row,FBANK2U ),false ) );
    newMiddleODT.Replace( wxT( "#LBANK2U#" ),grid->GetTable()->GetColLabelValue( FBANK2U ) );

    newMiddleODT.Replace( wxT( "#FENGINE#" ),replaceNewLine( mode,grid->GetCellValue( row,FENGINE1 ),false ) );
    newMiddleODT.Replace( wxT( "#LENGINE#" ),grid->GetTable()->GetColLabelValue( FENGINE1 ) );
    newMiddleODT.Replace( wxT( "#FFUEL#" ),replaceNewLine( mode,grid->GetCellValue( row,FFUEL ),false ) );
    newMiddleODT.Replace( wxT( "#LFUEL#" ),grid->GetTable()->GetColLabelValue( FFUEL ) );
    newMiddleODT.Replace( wxT( "#FWATER#" ),replaceNewLine( mode,grid->GetCellValue( row,FWATER ),false ) );
    newMiddleODT.Replace( wxT( "#LWATER#" ),grid->GetTable()->GetColLabelValue( FWATER ) );
    newMiddleODT.Replace( wxT( "#FWINDDIR#" ),replaceNewLine( mode,grid->GetCellValue( row,FWINDDIR ),false ) );
    newMiddleODT.Replace( wxT( "#LWINDDIR#" ),grid->GetTable()->GetColLabelValue( FWINDDIR ) );
    newMiddleODT.Replace( wxT( "#FWIND#" ),replaceNewLine( mode,grid->GetCellValue( row,FWIND ),false ) );
    newMiddleODT.Replace( wxT( "#LWIND#" ),grid->GetTable()->GetColLabelValue( FWIND ) );
    newMiddleODT.Replace( wxT( "#FWINDPEAK#" ),replaceNewLine( mode,grid->GetCellValue( row,FWINDPEAK ),false ) );
    newMiddleODT.Replace( wxT( "#LWINDPEAK#" ),grid->GetTable()->GetColLabelValue( FWINDPEAK ) );
    newMiddleODT.Replace( wxT( "#FCURRENTDIR#" ),replaceNewLine( mode,grid->GetCellValue( row,FCURRENTDIR ),false ) );
    newMiddleODT.Replace( wxT( "#LCURRENTDIR#" ),grid->GetTable()->GetColLabelValue( FCURRENTDIR ) );

    newMiddleODT.Replace( wxT( "#FCURRENT#" ),replaceNewLine( mode,grid->GetCellValue( row,FCURRENT ),false ) );
    newMiddleODT.Replace( wxT( "#LCURRENT#" ),grid->GetTable()->GetColLabelValue( FCURRENT ) );
    newMiddleODT.Replace( wxT( "#FCURRENTPEAK#" ),replaceNewLine( mode,grid->GetCellValue( row,FCURRENTPEAK ),false ) );
    newMiddleODT.Replace( wxT( "#LCURRENTPEAK#" ),grid->GetTable()->GetColLabelValue( FCURRENTPEAK ) );
    newMiddleODT.Replace( wxT( "#FWAVE#" ),replaceNewLine( mode,grid->GetCellValue( row,FWAVE ),false ) );
    newMiddleODT.Replace( wxT( "#LWAVE#" ),grid->GetTable()->GetColLabelValue( FWAVE ) );
    newMiddleODT.Replace( wxT( "#FWAVEPEAK#" ),replaceNewLine( mode,grid->GetCellValue( row,FWAVEPEAK ),false ) );
    newMiddleODT.Replace( wxT( "#LWAVEPEAK#" ),grid->GetTable()->GetColLabelValue( FWAVEPEAK ) );
    newMiddleODT.Replace( wxT( "#FSWELL#" ),replaceNewLine( mode,grid->GetCellValue( row,FSWELL ),false ) );
    newMiddleODT.Replace( wxT( "#LSWELL#" ),grid->GetTable()->GetColLabelValue( FSWELL ) );
    newMiddleODT.Replace( wxT( "#FSWELLPEAK#" ),replaceNewLine( mode,grid->GetCellValue( row,FSWELLPEAK ),false ) );
    newMiddleODT.Replace( wxT( "#LSWELLPEAK#" ),grid->GetTable()->GetColLabelValue( FSWELLPEAK ) );
    newMiddleODT.Replace( wxT( "#FSAILS#" ),replaceNewLine( mode,grid->GetCellValue( row,FSAILS ),false ) );
    newMiddleODT.Replace( wxT( "#LSAILS#" ),grid->GetTable()->GetColLabelValue( FSAILS ) );

    return newMiddleODT;
}
