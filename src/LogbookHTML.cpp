//#pragma once
#include "LogbookHTML.h"
#include "LogbookDialog.h"
#include "Logbook.h"
#include "logbook_pi.h"
#include "Options.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/mimetype.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/grid.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>
#include "wx/stdpaths.h"

#include <memory>
#include <map>
using namespace std;

LogbookHTML::LogbookHTML( Logbook *l, LogbookDialog *d, wxString data, wxString layout )
{
    parent = d;
    logbook = l;
    data_locn = data;
    layout_locn = layout;
}

LogbookHTML::~LogbookHTML( void )
{

}

void LogbookHTML::setPlaceholders()
{
    gridc gridcols = { 0,LogbookHTML::ROUTE};
    placeholders[_T( "ROUTE" )]=gridcols;
    placeholders[_T( "LROUTE" )]=gridcols;
    gridc gridcols1 = { 0,LogbookHTML::RDATE};
    placeholders[_T( "DATE" )]=gridcols1;
    placeholders[_T( "LDATE" )]=gridcols1;
    gridc gridcols2 = { 0,LogbookHTML::RTIME};
    placeholders[_T( "TIME" )]=gridcols2;
    placeholders[_T( "LTIME" )]=gridcols2;
    gridc gridcols3 = { 0,LogbookHTML::STATUS};
    placeholders[_T( "STATUS" )]=gridcols3;
    placeholders[_T( "LSTATUS" )]=gridcols3;
    gridc gridcols4 = { 0,LogbookHTML::WAKE};
    placeholders[_T( "WAKE" )]=gridcols4;
    placeholders[_T( "LWAKE" )]=gridcols4;
    gridc gridcols5 = { 0,LogbookHTML::DISTANCE};
    placeholders[_T( "DISTANCE" )]=gridcols5;
    placeholders[_T( "LDISTANCE" )]=gridcols5;
    gridc gridcols6 = { 0,LogbookHTML::DTOTAL};
    placeholders[_T( "DTOTAL" )]=gridcols6;
    placeholders[_T( "LDTOTAL" )]=gridcols6;
    gridc gridcols7 = { 0,LogbookHTML::POSITION};
    placeholders[_T( "POSITION" )]=gridcols7;
    placeholders[_T( "LPOSITION" )]=gridcols7;
    gridc gridcols8 = { 0,LogbookHTML::COG};
    placeholders[_T( "COG" )]=gridcols8;
    placeholders[_T( "LCOG" )]=gridcols8;
    gridc gridcols9 = { 0,LogbookHTML::RTIME};
    placeholders[_T( "COW" )]=gridcols9;
    placeholders[_T( "LCOW" )]=gridcols9;
    gridc gridcols10 = { 0,LogbookHTML::COW};
    placeholders[_T( "SOG" )]=gridcols10;
    placeholders[_T( "LSOG" )]=gridcols10;
    gridc gridcols11 = { 0,LogbookHTML::SOW};
    placeholders[_T( "SOW" )]=gridcols11;
    placeholders[_T( "LSOW" )]=gridcols11;
    gridc gridcols12 = { 0,LogbookHTML::DEPTH};
    placeholders[_T( "DEPTH" )]=gridcols12;
    placeholders[_T( "LDEPTH" )]=gridcols12;
    gridc gridcols13 = { 0,LogbookHTML::REMARKS};
    placeholders[_T( "REMARKS" )]=gridcols13;
    placeholders[_T( "LREMARKS" )]=gridcols13;

    gridc gridcols14 = { 1,LogbookHTML::BARO};
    placeholders[_T( "BARO" )]=gridcols14;
    placeholders[_T( "LBARO" )]=gridcols14;
    gridc gridcols15 = { 1,LogbookHTML::HYDRO };
    placeholders[_T( "HYDRO" )]=gridcols15;
    placeholders[_T( "LHYDRO" )]=gridcols15;
    gridc gridcols16 = { 1,LogbookHTML::AIRTE};
    placeholders[_T( "AIRTE" )]=gridcols16;
    placeholders[_T( "LAIRTE" )]=gridcols16;
    gridc gridcols17 = { 1,LogbookHTML::WATERTE};
    placeholders[_T( "WTE" )]=gridcols17;
    placeholders[_T( "LWTE" )]=gridcols17;
    gridc gridcols18 = { 1,LogbookHTML::WIND};
    placeholders[_T( "WIND" )]=gridcols18;
    placeholders[_T( "LWIND" )]=gridcols18;
    gridc gridcols19 = { 1,LogbookHTML::WSPD};
    placeholders[_T( "WSPD" )]=gridcols19;
    placeholders[_T( "LWSPD" )]=gridcols19;
    gridc gridcols20 = { 1,LogbookHTML::WINDR};
    placeholders[_T( "WIND" )]=gridcols20;
    placeholders[_T( "LWIND" )]=gridcols20;
    gridc gridcols21 = { 1,LogbookHTML::WSPDR};
    placeholders[_T( "WSPD" )]=gridcols21;
    placeholders[_T( "LWSPD" )]=gridcols21;
    gridc gridcols22 = { 1,LogbookHTML::CURRENT};
    placeholders[_T( "CUR" )]=gridcols22;
    placeholders[_T( "LCUR" )]=gridcols22;
    gridc gridcols23 = { 1,LogbookHTML::CSPD};
    placeholders[_T( "CSPD" )]=gridcols23;
    placeholders[_T( "LCSPD" )]=gridcols23;
    gridc gridcols24 = { 1,LogbookHTML::WAVE };
    placeholders[_T( "WAVE" )]=gridcols24;
    placeholders[_T( "LWAVE" )]=gridcols24;
    gridc gridcols25 = { 1,LogbookHTML::SWELL };
    placeholders[_T( "SWELL" )]=gridcols25;
    placeholders[_T( "LSWELL" )]=gridcols25;
    gridc gridcols26 = { 1,LogbookHTML::WEATHER };
    placeholders[_T( "WEATHER" )]=gridcols26;
    placeholders[_T( "LWEATHER" )]=gridcols26;
    gridc gridcols27 = { 1,LogbookHTML::CLOUDS };
    placeholders[_T( "CLOUDS" )]=gridcols27;
    placeholders[_T( "LCLOUDS" )]=gridcols27;
    gridc gridcols28 = { 1,LogbookHTML::VISIBILITY };
    placeholders[_T( "VISIBILITY" )]=gridcols28;
    placeholders[_T( "LVISIBILITY" )]=gridcols28;

    gridc gridcols29 = { 2,LogbookHTML::MOTOR};
    placeholders[_T( "MOTOR" )]=gridcols29;
    placeholders[_T( "LMOTOR" )]=gridcols29;
    gridc gridcols30 = { 2,LogbookHTML::MOTORT };
    placeholders[_T( "MOTORT" )]=gridcols30;
    placeholders[_T( "LMOTORT" )]=gridcols30;
    gridc gridcols31 = { 2,LogbookHTML::MOTOR1};
    placeholders[_T( "MOTOR1" )]=gridcols31;
    placeholders[_T( "LMOTOR1" )]=gridcols31;
    gridc gridcols32 = { 2,LogbookHTML::MOTOR1T};
    placeholders[_T( "MOTOR1T" )]=gridcols32;
    placeholders[_T( "LMOTOR1T" )]=gridcols32;
    gridc gridcols33 = { 2,LogbookHTML::FUEL};
    placeholders[_T( "FUEL" )]=gridcols33;
    placeholders[_T( "LFUEL" )]=gridcols33;
    gridc gridcols34 = { 2,LogbookHTML::FUELT};
    placeholders[_T( "FUELT" )]=gridcols34;
    placeholders[_T( "LFUELT" )]=gridcols34;
    gridc gridcols35 = { 2,LogbookHTML::GENE};
    placeholders[_T( "GENE" )]=gridcols35;
    placeholders[_T( "LGENE" )]=gridcols35;
    gridc gridcols36 = { 2,LogbookHTML::GENET};
    placeholders[_T( "GENET" )]=gridcols36;
    placeholders[_T( "LGENET" )]=gridcols36;
    gridc gridcols37 = { 2,LogbookHTML::BANK1 };
    placeholders[_T( "BANK1" )]=gridcols37;
    placeholders[_T( "LBANK1" )]=gridcols37;
    gridc gridcols38 = { 2,LogbookHTML::BANK1T };
    placeholders[_T( "BANK1T" )]=gridcols38;
    placeholders[_T( "LBANK1T" )]=gridcols38;
    gridc gridcols39 = { 2,LogbookHTML::BANK2 };
    placeholders[_T( "BANK2" )]=gridcols39;
    placeholders[_T( "LBANK2" )]=gridcols39;
    gridc gridcols40 = { 2,LogbookHTML::BANK2T };
    placeholders[_T( "BANK2T" )]=gridcols40;
    placeholders[_T( "LBANK2T" )]=gridcols40;
    gridc gridcols41 = { 2,LogbookHTML::SAILS };
    placeholders[_T( "SAILS" )]=gridcols41;
    placeholders[_T( "LSAILS" )]=gridcols41;
    gridc gridcols42 = { 2,LogbookHTML::REEF };
    placeholders[_T( "REEF" )]=gridcols42;
    placeholders[_T( "LREEF" )]=gridcols42;
    gridc gridcols43 = { 2,LogbookHTML::WATERM };
    placeholders[_T( "WATERM" )]=gridcols43;
    placeholders[_T( "LWATERM" )]=gridcols43;
    gridc gridcols44 = { 2,LogbookHTML::WATERMT };
    placeholders[_T( "WATERMT" )]=gridcols44;
    placeholders[_T( "LWATERMT" )]=gridcols44;
    gridc gridcols45 = { 2,LogbookHTML::WATERMO };
    placeholders[_T( "WATERMO" )]=gridcols45;
    placeholders[_T( "LWATERMO" )]=gridcols45;
    gridc gridcols46 = { 2,LogbookHTML::WATER };
    placeholders[_T( "WATER" )]=gridcols46;
    placeholders[_T( "LWATER" )]=gridcols46;
    gridc gridcols47 = { 2,LogbookHTML::WATERT };
    placeholders[_T( "WATERT" )]=gridcols47;
    placeholders[_T( "LWATERT" )]=gridcols47;
    gridc gridcols48 = { 2,LogbookHTML::MREMARKS };
    placeholders[_T( "MREMARKS" )]=gridcols48;
    placeholders[_T( "LMREMARKS" )]=gridcols48;

    placeholdersboat[_T( "LLOGBOOK" )]     = parent->m_logbook->GetPageText( 0 );
    placeholdersboat[_T( "LFROM" )]        = _( "from" );
    placeholdersboat[_T( "LTO" )]          = _( "to" );
    if ( parent->logGrids[0]->GetNumberRows() > 0 )
    {
        placeholdersboat[_T( "SDATE" )]        = parent->m_gridGlobal->GetCellValue( 0,1 );
        placeholdersboat[_T( "EDATE" )]        = parent->m_gridGlobal->GetCellValue( parent->m_gridGlobal->GetNumberRows()-1,1 );
    }
    placeholdersboat[_T( "TYPE" )]         = parent->boatType->GetValue();
    placeholdersboat[_T( "BOATNAME" )]     = parent->boatName->GetValue();
    placeholdersboat[_T( "HOMEPORT" )]     = parent->homeport->GetValue();
    placeholdersboat[_T( "CALLSIGN" )]     = parent->callsign->GetValue();
    placeholdersboat[_T( "REGISTRATION" )] = parent->registration->GetValue();
//	placeholdersboat[_T("LOCATION")]     = layout_locn + layout + _T(".html");
}

void LogbookHTML::viewHTML( wxString path, wxString layout, bool mode )
{
    if ( layout.Contains( _T( "Help" ) ) )
    {
        path = layout_locn+layout+_T( ".html" );
        parent->startBrowser( path );
        return;
    }

    setSelection();

    wxString prefix = logbook->opt->engineStr[logbook->opt->engines]+logbook->opt->layoutPrefix[LogbookDialog::LOGBOOK];
    if ( logbook->opt->filterLayout[LogbookDialog::LOGBOOK] )
        layout.Prepend( prefix );
    wxString file = toHTML( path, layout, mode );
    if ( file != wxT( "" ) )
        parent->startBrowser( file );
}

void LogbookHTML::setSelection()
{
    int row = parent->selGridRow;
    int z = 0 , r = 1, topRow, bottomRow, first, last;

    first = last = 0;

    if ( parent->logGrids[0]->GetNumberRows() == 0 ) return;

    if ( parent->logGrids[parent->m_notebook8->GetSelection()]->IsSelection() )
    {
        wxGridCellCoordsArray top =
            parent->logGrids[parent->m_notebook8->GetSelection()]->GetSelectionBlockTopLeft();
        wxGridCellCoordsArray bottom =
            parent->logGrids[parent->m_notebook8->GetSelection()]->GetSelectionBlockBottomRight();
        if ( top.Count() != 0 )
        {
            topRow = top[0].GetRow();
            bottomRow = bottom[0].GetRow();
            r = bottomRow-topRow+1;
        }
    }

    z = parent->m_gridGlobal->GetNumberRows();
    if ( r == z )
    {
        parent->m_gridGlobal->SelectAll();
        parent->m_gridWeather->SelectAll();
        parent->m_gridMotorSails->SelectAll();
    }
    else
    {
        parent->m_gridGlobal->ClearSelection();
        parent->m_gridWeather->ClearSelection();
        parent->m_gridMotorSails->ClearSelection();
    }

    wxString selRoute = parent->m_gridGlobal->GetCellValue( row,0 );

    while ( row > -1 && parent->m_gridGlobal->GetCellValue( row,0 ) == selRoute )
        first = row--;
    row = parent->selGridRow;
    while ( row < parent->m_gridGlobal->GetNumberRows() && parent->m_gridGlobal->GetCellValue( row,0 ) == selRoute )
        last = row++;

    for ( row = first; row <= last; row++ )
    {
        parent->m_gridGlobal->SelectRow( row,true );
        parent->m_gridWeather->SelectRow( row,true );
        parent->m_gridMotorSails->SelectRow( row,true );
    }
}

wxString LogbookHTML::toHTML( wxString path, wxString layout, bool mode )
{
    wxArrayInt arrayRows;
    int count = 0, selCount = 0;
    bool selection = false;
    route = wxEmptyString;

    selCount = parent->m_gridGlobal->GetSelectedRows().Count() ;

    if ( selCount > 0 )
    {
        selection = true;
        arrayRows = parent->m_gridGlobal->GetSelectedRows();
    }

    if ( layout == _T( "" ) )
    {
        wxMessageBox( _( "Sorry, no Layout installed" ),_( "Information" ),wxOK );
        return _T( "" );
    }

    wxString html = readLayoutFile( layout );

    wxString topHTML;
    wxString bottomHTML;
    wxString headerHTML;
    wxString middleHTML;

    wxString seperatorTop = _T( "<!--Repeat -->" );
    wxString seperatorBottom = _T( "<!--Repeat End -->" );
    wxString seperatorHeaderTop = _T( "<!--Header -->" );
    wxString seperatorHeaderBottom = _T( "<!--Header end -->" );

    int indexTop = html.Find( seperatorTop );
    if ( !checkLayoutError( indexTop,seperatorTop,layout ) ) return _T( "" );
    indexTop += seperatorTop.Len();
    int indexBottom = html.Find( seperatorBottom );
    if ( !checkLayoutError( indexBottom,seperatorBottom,layout ) ) return _T( "" );
    indexBottom += seperatorBottom.Len();
    int indexHeaderTop = html.Find( seperatorHeaderTop );
    if ( !checkLayoutError( indexHeaderTop,seperatorHeaderTop,layout ) ) return _T( "" );
    indexHeaderTop += seperatorHeaderTop.Len();
    int indexHeaderBottom = html.Find( seperatorHeaderBottom );
    if ( !checkLayoutError( indexHeaderBottom,seperatorHeaderBottom,layout ) ) return _T( "" );
    indexHeaderBottom += seperatorHeaderBottom.Len();

    topHTML = html.substr( 0,indexHeaderTop );
    bottomHTML = html.substr( indexBottom,html.Len()-1 );
    middleHTML = html.substr( indexTop,indexBottom-indexTop );
    headerHTML = html.substr( indexHeaderTop,indexHeaderBottom-indexHeaderTop );

    wxString filename = this->fileName;

    if ( mode == false )
    {
        filename.Replace( wxT( "txt" ),wxT( "html" ) );
    }
    else
        filename = path;

    if ( ::wxFileExists( filename ) )
        ::wxRemoveFile( filename );

    wxFileOutputStream output( filename );
    wxTextOutputStream htmlFile( output );

    wxString newMiddleHTML;

    topHTML.Replace( wxT( "#LLOGBOOK#" ),parent->m_logbook->GetPageText( 0 ),false );
    topHTML.Replace( wxT( "#LFROM#" ),_( "from" ),false );
    topHTML.Replace( wxT( "#LTO#" ),_( "to" ),false );
    topHTML.Replace( wxT( "#SDATE#" ),parent->m_gridGlobal->GetCellValue( 0,1 ),false );
    topHTML.Replace( wxT( "#EDATE#" ),parent->m_gridGlobal->GetCellValue(
                         parent->m_gridGlobal->GetNumberRows()-1,1 ),false );
    topHTML.Replace( wxT( "#TYPE#" ),parent->boatType->GetValue(),false );
    topHTML.Replace( wxT( "#BOATNAME#" ),parent->boatName->GetValue(),false );
    topHTML.Replace( wxT( "#HOMEPORT#" ),parent->homeport->GetValue(),false );
    topHTML.Replace( wxT( "#CALLSIGN#" ),parent->callsign->GetValue(),false );
    topHTML.Replace( wxT( "#REGISTRATION#" ),parent->registration->GetValue(),false );
    topHTML.Replace( wxT( "#LOCATION#" ),layout_locn + layout + _T( ".html" ),false );
    htmlFile << topHTML;

    int rowsMax = parent->m_gridGlobal->GetNumberRows();
    for ( int row = 0; row < rowsMax; row++ )
    {
        count++;
        if ( selection && arrayRows[0]+1 > count ) continue;
        if ( selection && arrayRows[selCount-1]+1 < count ) break;

        newMiddleHTML = middleHTML;

#ifdef __WXMSW__
        unsigned int first = 0, ofirst = 0;
#endif
        for ( int grid = 0; grid < 3; grid++ )
        {
            wxGrid* g = parent->logGrids[grid];
            for ( int col = 0; col < g->GetNumberCols(); col++ )
            {
                if ( grid == 0 && col == 0 )
                {
                    replacePlaceholder( newMiddleHTML,headerHTML,grid,row,0,0,htmlFile );
                    continue;
                }
#ifdef __WXMSW__
                first = newMiddleHTML.find_first_of( '#' )-1;
                if ( first != ofirst )
                {
                    htmlFile << newMiddleHTML.SubString( 0,first );
                    newMiddleHTML.Remove( 0,first+1 );
                    ofirst = first;
                }
#endif
                newMiddleHTML = replacePlaceholder( newMiddleHTML,headerHTML,grid,row,col,0,htmlFile );
            }
        }
        htmlFile << newMiddleHTML;
    }
    htmlFile << bottomHTML;

    if ( count <= 0 )
    {
        wxMessageBox( _( "Sorry, Logbook has no lines" ),_( "Information" ),wxOK );
        return _T( "" );
    }

    output.Close();

    return filename;
}

wxString LogbookHTML::replacePlaceholder( wxString html,wxString htmlHeader,int grid, int row, int col, bool mode, wxTextOutputStream &htmlFile )
{

    wxString s;
    wxGrid* g = parent->logGrids[grid];

    switch ( grid )
    {
    case 0:
        switch ( col )
        {
        case ROUTE:
            if ( route != Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ) )
            {
                htmlHeader.Replace( wxT( "#ROUTE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ) );
                htmlHeader.Replace( wxT( "#LROUTE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
                htmlFile << htmlHeader;
            }
            route = Export::replaceNewLine( mode,g->GetCellValue( row,col ),false );

            break;
        case RDATE:
            html.Replace( wxT( "#DATE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LDATE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            html.Replace( wxT( "#NO.#" ),wxString::Format( _T( "%i" ),row+1 ) );
            break;
        case RTIME:
            html.Replace( wxT( "#TIME#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LTIME#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case STATUS:
            html.Replace( wxT( "#STATUS#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LSTATUS#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WAKE:
            html.Replace( wxT( "#WAKE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWAKE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case DISTANCE:
            html.Replace( wxT( "#DISTANCE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LDISTANCE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case DTOTAL:
            html.Replace( wxT( "#DTOTAL#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LDTOTAL#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case POSITION:
            html.Replace( wxT( "#POSITION#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LPOSITION#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case COG:
            html.Replace( wxT( "#COG#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LCOG#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case COW:
            html.Replace( wxT( "#COW#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LCOW#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case SOG:
            html.Replace( wxT( "#SOG#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LSOG#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case SOW:
            html.Replace( wxT( "#SOW#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LSOW#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case DEPTH:
            html.Replace( wxT( "#DEPTH#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LDEPTH#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case REMARKS:
            html.Replace( wxT( "#REMARKS#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LREMARKS#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        }
        break;
    case 1:
        switch ( col )
        {
        case BARO:
            html.Replace( wxT( "#BARO#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LBARO#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case HYDRO:
            html.Replace( wxT( "#HYDRO#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LHYDRO#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case AIRTE:
            html.Replace( wxT( "#AIRTE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LAIRTE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATERTE:
            html.Replace( wxT( "#WTE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWTE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WIND:
            html.Replace( wxT( "#WIND#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWIND#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WSPD:
            html.Replace( wxT( "#WSPD#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWSPD#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WINDR:
            html.Replace( wxT( "#WINDR#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWINDR#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WSPDR:
            html.Replace( wxT( "#WSPDR#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWSPDR#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case CURRENT:
            html.Replace( wxT( "#CUR#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LCUR#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case CSPD:
            html.Replace( wxT( "#CSPD#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LCSPD#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WAVE:
            html.Replace( wxT( "#WAVE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWAVE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case SWELL:
            html.Replace( wxT( "#SWELL#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LSWELL#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WEATHER:
            html.Replace( wxT( "#WEATHER#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWEATHER#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case CLOUDS:
            html.Replace( wxT( "#CLOUDS#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ) );
            html.Replace( wxT( "#LCLOUDS#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ) );
            {
                wxString cl = Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ).Lower();
                if ( cl.IsEmpty() )
                    cl = _T( "nocloud" );
                if ( cl.Contains( _T( " " ) ) )
                    cl = cl.SubString( 0,cl.find_first_of( ' ' )-1 );
                if ( html.Contains( wxT( "#PCLOUDS%23" ) ) )
                {
                    wxString s = parent->data;
                    s.Replace( _T( "\\" ),_T( "/" ) );
                    html.Replace( wxT( "Clouds/#PCLOUDS%23" ),s+_T( "Clouds/" )+cl,false );
                }
                else
                    html.Replace( wxT( "#PCLOUDS#" ),cl,false );
            }
            break;
        case VISIBILITY:
            html.Replace( wxT( "#VISIBILITY#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LVISIBILITY#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        }
        break;
    case 2:
        switch ( col )
        {
        case MOTOR:
            html.Replace( wxT( "#MOTOR#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LMOTOR#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case MOTORT:
            html.Replace( wxT( "#MOTORT#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LMOTORT#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case RPM1:
            html.Replace( wxT( "#RPM1#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LRPM1#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case MOTOR1:
            html.Replace( wxT( "#MOTOR1#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LMOTOR1#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case MOTOR1T:
            html.Replace( wxT( "#MOTOR1T#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LMOTOR1T#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case RPM2:
            html.Replace( wxT( "#RPM2#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LRPM2#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case FUEL:
            html.Replace( wxT( "#FUEL#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LFUEL#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case FUELT:
            html.Replace( wxT( "#FUELT#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LFUELT#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case GENE:
            html.Replace( wxT( "#GENE#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LGENE#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case GENET:
            html.Replace( wxT( "#GENET#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LGENET#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case BANK1:
            html.Replace( wxT( "#BANK1#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LBANK1#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case BANK1T:
            html.Replace( wxT( "#BANK1T#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LBANK1T#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case BANK2:
            html.Replace( wxT( "#BANK2#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LBANK2#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case BANK2T:
            html.Replace( wxT( "#BANK2T#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LBANK2T#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case SAILS:
            html.Replace( wxT( "#SAILS#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LSAILS#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case REEF:
            html.Replace( wxT( "#REEF#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LREEF#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATERM:
            html.Replace( wxT( "#WATERM#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWATERM#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATERMT:
            html.Replace( wxT( "#WATERMT#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWATERMT#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATERMO:
            html.Replace( wxT( "#WATERMO#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWATERMO#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATER:
            html.Replace( wxT( "#WATER#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWATER#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case WATERT:
            html.Replace( wxT( "#WATERT#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LWATERT#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        case MREMARKS:
            html.Replace( wxT( "#MREMARKS#" ),Export::replaceNewLine( mode,g->GetCellValue( row,col ),false ),false );
            html.Replace( wxT( "#LMREMARKS#" ),Export::replaceNewLine( mode,g->GetColLabelValue( col ),true ),false );
            break;
        }
        break;
    }

    if ( mode == 0 )
        return html;
    else
    {
#ifdef __WXOSX__
        wxString str( html.wx_str(), wxConvUTF8 );
#else
        wxString str( html.wx_str(),wxConvUTF8 );
#endif
        return str;
    }
}

bool LogbookHTML::checkLayoutError( int result, wxString html, wxString layout )
{
    if ( result == wxNOT_FOUND )
    {
        wxMessageBox( html+_( "\nnot found in layoutfile " )+layout+_( "!\n\nDid you forget to add this line in your layout ?" ),_( "Information" ) );
        return false;
    }
    return true;
}

void LogbookHTML::setFileName( wxString s, wxString l )
{
    fileName = s;
    layout_locn = l;
}

wxString LogbookHTML::readLayoutFile( wxString layout )
{
    wxString html;

    wxString filename = layout_locn + layout + _T( ".html" );
    wxTextFile *layoutfile = new wxTextFile( filename );

    if ( wxFileExists( filename ) )
    {
        layoutfile->Open();

        for ( unsigned row = 0; row < layoutfile->GetLineCount(); row++ )
        {
            html += layoutfile->GetLine( row )+wxT( "\n" );
        }
    }
    layoutfile->Close();
    return html;
}

wxString LogbookHTML::readLayoutFileODT( wxString layout )
{
    auto_ptr<wxZipEntry> entry;
    wxString odt = _T( "" );
    wxString filename = layout_locn + layout + _T( ".odt" );
    if ( wxFileExists( filename ) )
    {
//#ifdef __WXOSX__

        static const wxString fn = _T( "content.xml" );
        wxString name = wxZipEntry::GetInternalName( fn );
        wxFFileInputStream in( filename );
        wxZipInputStream zip( in );
        do
        {
            entry.reset( zip.GetNextEntry() );
        }
        while ( entry.get() != NULL && entry->GetInternalName() != name );
        if ( entry.get() != NULL )
        {
            wxTextInputStream txt( zip,_T( "\n" ),wxConvUTF8 );
            while ( !zip.Eof() )
                odt += txt.ReadLine();
        }
//#else
        /*        static const wxString fn = _T("content.xml");
        		wxFileInputStream in(filename);
                wxZipInputStream zip(in);
        		wxTextInputStream txt(zip);
        		while(!zip.Eof())
        			odt += txt.ReadLine();
        //#endif*/
    }
    return odt;
}

void LogbookHTML::viewODT( wxString path, wxString layout, bool mode )
{
    wxString prefix = logbook->opt->engineStr[logbook->opt->engines]+logbook->opt->layoutPrefix[LogbookDialog::LOGBOOK];
    if ( logbook->opt->filterLayout[LogbookDialog::LOGBOOK] )
        layout.Prepend( prefix );

    setSelection();

    wxString file = toODT( path, layout, mode );
    if ( file != wxT( "" ) )
        parent->startApplication( file,_T( ".odt" ) );
}

wxString LogbookHTML::toODT( wxString path,wxString layout, bool mode )
{
    wxArrayInt arrayRows;
    int count = 0, selCount = 0;
    bool selection = false;
    route = wxEmptyString;

    selCount = parent->m_gridGlobal->GetSelectedRows().Count() ;

    if ( selCount > 0 )
    {
        selection = true;
        arrayRows = parent->m_gridGlobal->GetSelectedRows();
    }

    if ( layout == _T( "" ) )
    {
        wxMessageBox( _( "Sorry, no Layout installed" ),_( "Information" ),wxOK );
        return _T( "" );
    }

    wxString odt = readLayoutFileODT( layout );

    if ( !odt.Contains( _T( "[[" ) ) && !odt.Contains( _T( "{{" ) ) )
    {
        wxMessageBox( _( "Have You forgotten to enclose the Header with [[ and ]]\n or Data with {{ and }} ?" ) );
        return _T( "" );
    }

    wxString topODT;
    wxString bottomODT;
    wxString headerODT;
    wxString middleODT;

    wxString seperatorTop = _T( "{{" );
    wxString seperatorBottom = _T( "}}" );
    wxString seperatorHeaderTop = _T( "[[" );
    wxString seperatorHeaderBottom = _T( "]]" );
    wxString textbegin = _T( "<text:p" );
    wxString textend = _T( "</text:p" );

    odt.Replace( wxT( "#LLOGBOOK#" ),parent->m_logbook->GetPageText( 0 ),false );
    odt.Replace( wxT( "#LFROM#" ),_( "from" ),false );
    odt.Replace( wxT( "#LTO#" ),_( "to" ),false );
    odt.Replace( wxT( "#SDATE#" ),parent->m_gridGlobal->GetCellValue( 0,1 ),false );
    odt.Replace( wxT( "#EDATE#" ),parent->m_gridGlobal->GetCellValue( parent->m_gridGlobal->GetNumberRows()-1,1 ),false );
    odt.Replace( wxT( "#TYPE#" ),parent->boatType->GetValue(),false );
    odt.Replace( wxT( "#BOATNAME#" ),parent->boatName->GetValue(),false );
    odt.Replace( wxT( "#HOMEPORT#" ),parent->homeport->GetValue(),false );
    odt.Replace( wxT( "#CALLSIGN#" ),parent->callsign->GetValue(),false );
    odt.Replace( wxT( "#REGISTRATION#" ),parent->registration->GetValue(),false );

    int indexTopODT = odt.Find( seperatorHeaderTop );
    int top = indexTopODT;
    topODT = odt.substr( 0,indexTopODT );
    indexTopODT = topODT.find_last_of( '<' );
    topODT = topODT.substr( 0,indexTopODT );
    odt = odt.substr( top );

    int headerStart = odt.find_first_of( '>' )+1;
    odt = odt.substr( headerStart );

    int indexBottomODT = odt.Find( seperatorHeaderBottom );
    headerODT = odt.substr( 0,indexBottomODT );
    odt = odt.substr( indexBottomODT );
    int headerEnd = odt.find_first_of( '>' )+1;
    odt = odt.substr( headerEnd );
    indexBottomODT = headerODT.find_first_of( '<' );
    headerODT = headerODT.substr( indexBottomODT );
    headerODT = headerODT.substr( 0,headerODT.find_last_of( '<' ) );

    indexBottomODT = odt.Find( seperatorTop );
    middleODT = odt.substr( indexBottomODT );
    odt = odt.substr( indexBottomODT );

    int middleStart = middleODT.find_first_of( '>' )+1;
    middleODT = middleODT.substr( middleStart );
    odt = odt.substr( odt.find_first_of( '>' )+1 );

    indexBottomODT = odt.Find( seperatorBottom );
    middleODT = odt.substr( 0,indexBottomODT );
    odt = odt.substr( indexBottomODT );

    int middleEnd = odt.find_first_of( '>' )+1;
    odt = odt.substr( middleEnd );
    middleODT = middleODT.substr( 0,middleODT.find_last_of( '<' ) );

    wxString filename = this->fileName;

    if ( mode == false )
    {
        filename.Replace( wxT( "txt" ),wxT( "odt" ) );
    }
    else
        filename = path;

    if ( ::wxFileExists( filename ) )
        ::wxRemoveFile( filename );

    auto_ptr<wxFFileInputStream> in( new wxFFileInputStream( layout_locn + layout + _T( ".odt" ) ) );
    wxTempFileOutputStream out( filename );

    wxZipInputStream inzip( *in );
    wxZipOutputStream outzip( out );
    wxTextOutputStream odtFile( outzip );
    auto_ptr<wxZipEntry> entry;

    outzip.CopyArchiveMetaData( inzip );

    while ( entry.reset( inzip.GetNextEntry() ), entry.get() != NULL )
        if ( !entry->GetName().Matches( _T( "content.xml" ) ) )
            if ( !outzip.CopyEntry( entry.release(), inzip ) )
                break;

    in.reset();

    outzip.PutNextEntry( _T( "content.xml" ) );

    odtFile << topODT;

    wxString newMiddleODT;
    int rowsMax = parent->m_gridGlobal->GetNumberRows();
    for ( int row = 0; row < rowsMax; row++ )
    {
        count++;
        if ( selection && arrayRows[0]+1 > count ) continue;
        if ( selection && arrayRows[selCount-1]+1 < count ) break;

        newMiddleODT = middleODT;
        for ( int grid = 0; grid < 3; grid++ )
        {
            wxGrid* g = parent->logGrids[grid];
            for ( int col = 0; col < g->GetNumberCols(); col++ )
                newMiddleODT = replacePlaceholder( newMiddleODT,headerODT,grid,row,col,1,odtFile );
        }
        odtFile << newMiddleODT;
    }

    odtFile << odt;

    inzip.Eof() && outzip.Close() && out.Commit();
    return filename;

}

void LogbookHTML::toCSV( wxString path )
{
    wxString s, temp;

    if ( ::wxFileExists( path ) )
        ::wxRemoveFile( path );

    wxFileOutputStream output( path );
    wxTextOutputStream csvFile( output );

    for ( int n = 0; n < parent->numPages; n++ )
    {
        for ( int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++ )
        {
            wxString str = _T( "\"" )+parent->logGrids[n]->GetColLabelValue( i )+_T( "\"," );
            if ( i == LogbookHTML::POSITION && n == 0 )
            {
                csvFile << str;
                csvFile << str;
            }
            else
                csvFile << str;
        }
    }
    csvFile << _T( "\n" );
    for ( int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++ )
    {
        for ( int grid = 0; grid < parent->numPages; grid++ )
        {
            for ( int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++ )
            {
                temp = parent->logGrids[grid]->GetCellValue( row,col );
                if ( col == LogbookHTML::POSITION && grid == 0 )
                {
                    wxStringTokenizer p( temp,_T( "\n" ) );
                    wxString lat = p.GetNextToken();
                    wxString lon = p.GetNextToken();
                    temp =  lat+_T( "\",\"" )+lon;
                }
                s +=  _T( "\"" )+temp+_T( "\"," );
            }
        }
        s.RemoveLast();
        csvFile << s + _T( "\n" );
        s = wxEmptyString;
    }
    output.Close();
}

void LogbookHTML::toXML( wxString path )
{
    wxString s = _T( "" );
    wxString temp;

    if ( ::wxFileExists( path ) )
        ::wxRemoveFile( path );

    wxFileOutputStream output( path );
    wxTextOutputStream xmlFile( output );

    xmlFile << parent->xmlHead;

    s = wxString::Format( _T( "<Row>" ) );
    for ( int n = 0; n < parent->numPages; n++ )
    {
        for ( int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++ )
        {
            s += _T( "<Cell>\n" );
            s += _T( "<Data ss:Type=\"String\">#DATA#</Data>\n" );
            s.Replace( _T( "#DATA#" ),parent->logGrids[n]->GetColLabelValue( i ) );
            s += _T( "</Cell>" );
        }
    }
    s += _T( "</Row>>" );
    xmlFile << s;
    for ( int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++ )
    {
        xmlFile << wxString::Format( _T( "<Row ss:Height=\"%u\">" ),parent->m_gridGlobal->GetRowHeight( row ) );
        for ( int grid = 0; grid < parent->numPages; grid++ )
        {
            for ( int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++ )
            {
                s = _T( "<Cell>\n" );
                s += _T( "<Data ss:Type=\"String\">#DATA#</Data>\n" );
                temp = parent->logGrids[grid]->GetCellValue( row,col );
                temp.Replace( _T( "&" ),_T( "&amp;" ) );
                temp.Replace( _T( "\\n" ),_T( "&#xA;" ) );
                temp.Replace( _T( "\"" ),_T( "&quot;" ) );
                temp.Replace( _T( "<" ),_T( "&lt;" ) );
                temp.Replace( _T( ">" ),_T( "&gt;" ) );
                temp.Replace( _T( "'" ),_T( "&apos;" ) );
                s.Replace( _T( "#DATA#" ),temp );
                s += _T( "</Cell>" );
                xmlFile << s;
            }
        }
        xmlFile << _T( "</Row>>" );;
    }
    xmlFile << parent->xmlEnd;
    output.Close();
}


void LogbookHTML::toODS( wxString path )
{
    wxString s = _T( "" );
    wxString line;
    wxString temp;

    wxFFileOutputStream out( path );
    wxZipOutputStream zip( out );
    wxTextOutputStream txt( zip );
    wxString sep( wxFileName::GetPathSeparator() );

    zip.PutNextEntry( wxT( "content.xml" ) );
    txt << parent->content;

    txt << _T( "<table:table-row table:style-name=\"ro2\">" );

    for ( int n = 0; n < parent->numPages; n++ )
    {
        for ( int i = 0; i < parent->logGrids[n]->GetNumberCols(); i++ )
        {
            txt << _T( "<table:table-cell office:value-type=\"string\">" );
            txt << _T( "<text:p>" );
            txt << parent->logGrids[n]->GetColLabelValue( i );
            txt << _T( "</text:p>" );
            txt << _T( "</table:table-cell>" );
        }
    }
    txt << _T( "</table:table-row>" );
    for ( int row = 0; row < parent->m_gridGlobal->GetNumberRows(); row++ )
    {
        txt << _T( "<table:table-row table:style-name=\"ro2\">" );
        for ( int grid = 0; grid < parent->numPages; grid++ )
        {
            for ( int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++ )
            {
                wxString s = parent->logGrids[grid]->GetCellValue( row,col );
                s.Replace( _T( "&" ),_T( "&amp;" ) );
                s.Replace( _T( "\"" ),_T( "&quot;" ) );
                s.Replace( _T( "<" ),_T( "&lt;" ) );
                s.Replace( _T( ">" ),_T( "&gt;" ) );
                s.Replace( _T( "'" ),_T( "&apos;" ) );

                txt << _T( "<table:table-cell office:value-type=\"string\">" );
                txt << _T( "<text:p>" );
                txt << s;
                txt << _T( "</text:p>" );
                txt << _T( "</table:table-cell>" );
            }
        }
        txt << _T( "</table:table-row>" );
    }
    txt << parent->contentEnd;

    zip.PutNextEntry( wxT( "mimetype" ) );
    txt << wxT( "application/vnd.oasis.opendocument.spreadsheet" );

    zip.PutNextEntry( wxT( "styles.xml" ) );
    txt << parent->styles;

    zip.PutNextEntry( wxT( "meta.xml" ) );
    txt << parent->meta;

    zip.PutNextEntry( wxT( "META-INF" ) + sep + wxT( "manifest.xml" ) );
    txt << parent->manifest;

    zip.PutNextEntry( wxT( "Thumbnails" ) + sep );

    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "floater" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "menubar" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "popupmenu" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "progressbar" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "statusbar" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "toolbar" ) );
    zip.PutNextEntry( wxT( "Configurations2" ) + sep + wxT( "images" ) + sep + wxT( "Bitmaps" ) );


    zip.Close();
    out.Close();
}

void LogbookHTML::backup( wxString path )
{
    logbook->update();
    wxCopyFile( data_locn+parent->backupFile+_T( ".txt" ),path );
}

void LogbookHTML::toKML( wxString path )
{
    wxString datetime, position, description, temp, folder, t, header, logpointName,
             route = _T( "nil" ) , oldroute, remarks, fRemarks, label, pathXML;
    wxString snil = _T( "---" );
    wxString trackID = wxEmptyString, trackOldID = wxEmptyString;
    wxString routeID = wxEmptyString, routeOldID = wxEmptyString;

    bool error = false, first = true, rfirst = true;
    wxDateTime dt;
    int maxRow = parent->m_gridGlobal->GetNumberRows(), row = 0;

    if ( ::wxFileExists( path ) )
        ::wxRemoveFile( path );

    wxFileOutputStream output( path );
    wxTextOutputStream kmlFile1( output );

    kmlFile = &kmlFile1 ;

    wxString h = parent->kmlHead;
    h.Replace( _T( "#TITLE#" ),logbook->title );
    *kmlFile << h;
    h = parent->kmlLine;
    h.Replace( _T( "#LWIDTH#" ),logbook->opt->kmlLineWidth );
    h.Replace( _T( "#LTRANS#" ),logbook->opt->kmlTrans.Item( logbook->opt->kmlLineTransparancy ) );
    h.Replace( _T( "#LCOLORR#" ),logbook->opt->kmlColor.Item( logbook->opt->kmlRouteColor ) );
    h.Replace( _T( "#LCOLORT#" ),logbook->opt->kmlColor.Item( logbook->opt->kmlTrackColor ) );
    *kmlFile << h;


    for ( ; row < parent->m_gridGlobal->GetNumberRows(); row++ )
    {
        temp = parent->kmlBody;
        folder = parent->kmlFolder;
        error = false;
        remarks = wxEmptyString;
        for ( int grid = 0; grid < parent->numPages; grid++ )
        {
            for ( int col = 0; col < parent->logGrids[grid]->GetNumberCols(); col++ )
            {
                wxString e = parent->logGrids[grid]->GetCellValue( row,col );
                if ( grid == 0 )
                {
                    switch ( col )
                    {
                    case LogbookHTML::ROUTE:
                        //temp.Replace(_T("#ROW#"),wxString::Format(_("Row: %i"),row));
                        if ( e != route )
                        {
                            if ( !first )
                                ( *kmlFile ) << parent->kmlEndFolder;
                            first = false;

                            e = replaceKMLCharacters( e );
                            folder.Replace( _T( "#NAME#" ),e );
                            ( *kmlFile ) << folder;

                            route = e;
                            rfirst = true;
                            routeID = wxEmptyString;

                        }
                        else
                        {
                            first = false;
                            rfirst = false;
                        }
                        break;
                    case LogbookHTML::RDATE:
                        logpointName = e +_T( " " );
                        break;
                    case LogbookHTML::RTIME:
                        logpointName += e;
                        break;
                    case LogbookHTML::POSITION:
                        if ( e.IsEmpty() )
                        {
                            error = true;
                            break;
                        }
                        position = e;
                        e = replaceKMLCharacters( e );
                        description += position+_T( "\n" ) ;
                        position = convertPositionToDecimalDegrees( position );
                        temp.Replace( _T( "#POSITION#" ),position,false );
                        break;
                    case LogbookHTML::COG:
                        label = parent->m_gridGlobal->GetColLabelValue( LogbookHTML::COG );
                        label.Replace( _T( "\n" ),_T( " " ) );
                        if ( e.IsEmpty() ) e = snil;
                        e = replaceKMLCharacters( e );
                        description += label + _T( " " ) + e +_T( "  " ) ;
                        break;
                    case LogbookHTML::SOG:
                        label = parent->m_gridGlobal->GetColLabelValue( LogbookHTML::SOG );
                        label.Replace( _T( "\n" ),_T( " " ) );
                        if ( e.IsEmpty() ) e = snil;
                        e = replaceKMLCharacters( e );
                        description += label + _T( " " ) + e +_T( "\n" ) ;
                        break;
                    case LogbookHTML::DEPTH:
                        label = parent->m_gridGlobal->GetColLabelValue( LogbookHTML::DEPTH );
                        label.Replace( _T( "\n" ),_T( " " ) );
                        if ( e.IsEmpty() ) e = snil;
                        e = replaceKMLCharacters( e );
                        description += label + _T( " " ) + e +_T( "\n" ) ;
                        break;
                    case LogbookHTML::REMARKS:
                        e = replaceKMLCharacters( e );
                        fRemarks = e;
                        if ( rfirst )
                        {
                            remarks = wxEmptyString; //e.SubString(0,50)+_T("...");
                            folder.Replace( _T( "#CREATED#" ),remarks,false );
                            rfirst = false;
                        }
                        break;
                    }
                }
                else if ( grid == 1 )
                {
                    switch ( col )
                    {
                    case WIND:
                        label = parent->m_gridWeather->GetColLabelValue( WIND );
                        label.Replace( _T( "\n" ),_T( " " ) );
                        if ( e.IsEmpty() ) e = snil;
                        e = replaceKMLCharacters( e );
                        description += label + _T( " " ) + e +_T( "  " ) ;
                        break;
                    case WSPD:
                        label = parent->m_gridWeather->GetColLabelValue( WSPD );
                        label.Replace( _T( "\n" ),_T( " " ) );
                        if ( e.IsEmpty() ) e = snil;
                        e = replaceKMLCharacters( e );
                        description += label + _T( " " ) + e +_T( "\n" ) ;
                        break;
                    }
                }
                else if ( grid == 2 )
                {
                    switch ( col )
                    {
                    case MREMARKS:
                        e = replaceKMLCharacters( e );
                        temp.Replace( _T( "#NAME#" ),logpointName );
                        temp.Replace( _T( "#DESCRIPTION#" ),description+( ( fRemarks.IsEmpty() )?_T( "" ):_T( "\n" )+fRemarks )+_T( "\n" )+e,false );
                        break;
                    case ROUTEID:
                        routeOldID = routeID;
                        routeID = e;
                        if ( ( logbook->opt->kmlRoute && !routeID.IsEmpty() ) && ( routeID != routeOldID ) )
                        {
                            wxJSONWriter w;
                            wxString out;
                            wxJSONValue v;
                            v[_T( "Route_ID" )] =   parent->logGrids[2]->GetCellValue( row,ROUTEID );
                            w.Write( v, out );
                            SendPluginMessage( wxString( _T( "OCPN_ROUTE_REQUEST" ) ),out );

                            ::wxSafeYield();
                        }
                        break;
                    case TRACKID:
                        trackOldID = trackID;
                        trackID = e;
                        if ( ( logbook->opt->kmlTrack && !trackID.IsEmpty() ) && ( trackID != trackOldID ) )
                        {
                            wxJSONWriter w;
                            wxString out;
                            wxJSONValue v;
                            v[_T( "Track_ID" )] =   parent->logGrids[2]->GetCellValue( row,TRACKID );
                            w.Write( v, out );
                            SendPluginMessage( wxString( _T( "OCPN_TRACK_REQUEST" ) ),out );

                            ::wxSafeYield();
                        }
                        break;
                    }
                }
            }
        }
        if ( !error )
        {
            temp.Replace( _T( "#icon#" ),_T( "http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png" ) );
            ( *kmlFile ) << temp;
        }
        description = wxEmptyString;
    }
    if ( row == maxRow-1 ) *kmlFile << parent->kmlEndFolder;
    ( *kmlFile ) << parent->kmlEnd;
    output.Close();

    wxFileType *filetype = wxTheMimeTypesManager->GetFileTypeFromExtension( _T( "kml" ) );
    wxString cmd = filetype->GetOpenCommand( path );
    if ( !cmd.IsEmpty() )
        wxExecute( cmd );
}

void LogbookHTML::writeTrackToKML( wxJSONValue data )
{
    wxString trkLine = parent->kmlPathHeader;
    trkLine.Replace( _T( "#NAME#" ),_T( "Trackline" ) );

    *kmlFile << trkLine;
    for ( int i = 0; i < data.Size(); i++ )
        ( *kmlFile ) << wxString::Format( _T( "%.13f,%.13f\n" ),data[i][1].AsDouble(),data[i][0].AsDouble() );

    ( *kmlFile ) << parent->kmlPathFooter;
}

void LogbookHTML::writeRouteToKML( wxJSONValue data )
{
    wxString routeLine = parent->kmlPathHeader;
    routeLine.Replace( _T( "#NAME#" ),_T( "Routeline" ) );
    routeLine.Replace( _T( "#LINE#" ),_T( "#LineRoute" ) );
    *kmlFile << routeLine;

    for ( int i = 0; i < data.Size(); i++ )
        ( *kmlFile ) << wxString::Format( _T( "%f,%f\n" ),data[i][_T( "lon" )].AsDouble(),data[i][_T( "lat" )].AsDouble() );

    ( *kmlFile ) << parent->kmlPathFooter;

    wxString n = parent->kmlFolder;
    n.Replace( _T( "#NAME#" ),_( "Routepoints" ) );
    *kmlFile << n;

    for ( int i = 0; i < data.Size(); i++ )
    {
        wxString routeWP = parent->kmlBody;
        routeWP.Replace( _T( "#icon#" ),_T( "http://maps.google.com/mapfiles/kml/pal4/icon48.png" ) );

        routeWP.Replace( _T( "#NAME#" ),data[i][_T( "WPName" )].AsString() );
        wxString description = data[i][_T( "WPDescription" )].AsString()+_T( "<br>" );

        int li = 1;
        wxString links = wxEmptyString;
        wxString desc = wxEmptyString;

        while ( true )
        {
            wxString count = wxString::Format( _T( "%d" ),li );

            if ( data[i].HasMember( _T( "WPLink" )+count ) )
                links = data[i][_T( "WPLink" )+count].AsString();
            else
                break;

            if ( data[i].HasMember( _T( "WPLinkDesciption" )+count ) )
            {
                desc = data[i][_T( "WPLinkDesciption" )+count].AsString();
                description += _T( "<a href=\"" )+links+_T( "\">" )+desc+_T( "</a><br>" );
            }
            else

                break;
            li++;
        }
        routeWP.Replace( _T( "#DESCRIPTION#" ),_T( " <![CDATA[\n" )+description+_T( "\n]]>" ) );
        routeWP.Replace( _T( "#POSITION#" ),wxString::Format( _T( "%f,%f\n" ),data[i][_T( "lon" )].AsDouble(),data[i][_T( "lat" )].AsDouble() ) );
        ( *kmlFile ) << routeWP;

    }
    *kmlFile << parent->kmlEndFolder;
}
/*
          +void LogbookHTML::createJumpTable()
          +{
          +	wxString path  = parent->basePath+_T("navobj.xml");
          +	wxString patho = parent->basePath+_T("navobj.xml.changes");
          +
          +	offsetChanges.clear();
          +	offsetNavobj.clear();
          +
          +	if(wxFile::Exists(patho))
          +		insertTracks(patho,&offsetChanges,&offsetChangesGuid);
          +	else
          +		insertTracks(path,&offsetNavobj,&offsetNavobjGuid);
          +
          +}
          +
          +void LogbookHTML::insertTracks(wxString file, std::map<wxString,long> *navobj, std::map<wxString,long> *navobjgui )
          +{
          +	wxString temp;
          +
          +	wxFileInputStream in(file);
          +	wxTextInputStream xml(in);
          +
          +	long i = -1, n = 0;
          +	while(!in.Eof())
          +	{
          +		temp = xml.ReadLine();
          +		i++;
          +		if(temp.Contains(_T("<trk>")))
          +		{
          +			n = i;
          +			temp = xml.ReadLine();
          +			i++;
          +			if(temp.Contains(_T("<name>")))
          +			{
          +				temp.Trim(false);
          +				temp.Replace(_T("<name>"),_T(""));
          +				temp.Replace(_T("</name>"),_T(""));
          +				navobj->insert(pair(temp,n));
          +			}
          +
          +			do{
          +				temp = xml.ReadLine();
          +				i++;
          +			}while(!temp.Contains(_T("<opencpn:guid>")));
          +
          +			temp.Trim(false);
          +			temp = temp.AfterFirst('>');
          +			temp = temp.BeforeFirst('<');
          +			navobjgui->insert(pair(temp,n));
          +		}
          +	}
          +}
          +*/
wxString LogbookHTML::replaceKMLCharacters( wxString e )
{
    e.Replace( _T( "\"" ),_T( "&quot;" ) );
    e.Replace( _T( "<" ),_T( "&lt;" ) );
    e.Replace( _T( ">" ),_T( "&gt;" ) );
    e.Replace( _T( "'" ),_T( "&apos;" ) );
    e.Replace( _T( "&" ),_T( "&amp;" ) );

    return e;
}
/*
          +wxString LogbookHTML::findTrackInXML(wxDateTime dt, wxString file, wxString *name, wxString route, wxString trackguid, long offset, bool f, bool mode)
          +{
          +	wxString temp,lat,lon,track = wxEmptyString, trkguid = wxEmptyString;
          +	bool first = true;
          +
          +	wxString path = parent->basePath + file;
          +	if(f ==	0)
          +			if(!wxFile::Exists(path)) return wxEmptyString;
          +
          +		wxTextFile in(path);
          +		in.Open();
          +		in.GoToLine(offset);
          +			while(!in.Eof())
          +			{
          +				temp = in.GetLine(offset);
          +				if(temp.Contains(_T("<trk>")))
          +				{
          +					temp = in.GetNextLine();
          +					if(mode)
          +					{
          +					if(temp.Contains(_T("<name>")))
          +						{
          +							temp.Trim(false);
          +							temp = temp.AfterFirst('>');
          +							temp = temp.BeforeFirst('<');
          +							*name = temp;
          +						}
          +					}
          +					else
          +					{
          +						do{
          +							temp = in.GetNextLine();
          +						}while(!temp.Contains(_T("<opencpn:guid>")));
          +						temp.Trim(false);
          +						temp = temp.AfterFirst('>');
          +						trkguid = temp.BeforeFirst('<');
          +					}
          +
          +					do
          +					{
          +						temp = in.GetNextLine();
          +						if(temp.Contains(_T("</trk>"))) return track;
          +
          +						if(temp.Contains(_T("<trkpt")))
          +						{
          +							temp.Trim(false);
          +							temp.Replace(_T("<trkpt lat="),_T(""));
          +							temp.Replace(_T("lon="),_T(""));
          +							temp.Replace(_T("\""),_T(""));
          +							wxStringTokenizer tkz(temp,_T(" "));
          +							lon = tkz.GetNextToken();
          +							lat = tkz.GetNextToken().RemoveLast();
          +						}
          +
          +						if(temp.Contains(_T("<time>")))
          +						{
          +							temp.Trim(false);
          +							temp = temp.AfterFirst('>');
          +							temp = temp.BeforeFirst('<');
          +							temp.Replace(_T("T"),_T(" "));
          +
          +							wxDateTime dtt;
          +							dtt.ParseDateTime(temp.RemoveLast());
          +							//wxMessageBox(dtt.FormatDate()+_T(" ")+dtt.FormatTime());
          +						}
          +
          +							if(*name == route || trkguid == trackguid)
          +								track += lat+_T(",")+lon+_T("\n");
          +					}while(!in.Eof());
          +				}
          +			}
          +	in.Close();
          +	return track;
          +}
          +
          +wxString LogbookHTML::getPathFromTrack(wxDateTime dt, wxString route, wxString trackguid, long offset, bool ind, bool mode)
          +{
          +	wxString name = wxEmptyString;
          +	wxString kmlData = wxEmptyString;
          +	wxString header = parent->kmlPathHeader;
          +
          +	if(!ind)
          +		kmlData = findTrackInXML(dt, _T("navobj.xml.changes"), &name, route, trackguid, offset, ind, mode);
          +	else
          +		kmlData = findTrackInXML(dt, _T("navobj.xml"), &name, route, trackguid, offset, ind, mode);
          +
          +	name = route;
          +	if(!kmlData.IsEmpty())
          +	{
          +		header.Replace(_T("#NAME#"),name);
          +		return header+kmlData+parent->kmlPathFooter;
          +	}
          +	else
          +		return wxEmptyString;
          +}
          +*/
wxString LogbookHTML::convertPositionToDecimalDegrees( wxString str )
{
    wxString pos;

    wxStringTokenizer tkz( str,_T( "\n" ) );
    pos = positionToDecimalDegrees( tkz.GetNextToken() );
    pos.Prepend( positionToDecimalDegrees( tkz.GetNextToken() )+_T( "," ) );

    return pos;
}
wxString LogbookHTML::positionToDecimalDegrees( wxString str )
{
    double deg, min, sec = 0;
    wxString ind;

    wxStringTokenizer tkz( str,_T( " " ) );

    if ( tkz.CountTokens() == 4 )
    {
        deg = wxAtof( tkz.GetNextToken() );
        min = wxAtof( tkz.GetNextToken() );
        wxString t = tkz.GetNextToken();
        t.Replace( _T( "," ),_T( "." ) );
        sec = wxAtof( t );

        sec = min*60+sec;
        deg = deg+( sec/3600 );
    }
    else
    {
        deg = wxAtof( tkz.GetNextToken() );
        wxString t = tkz.GetNextToken();
        t.Replace( _T( "," ),_T( "." ) );
        min = wxAtof( t );
        min = min / 60;
        deg += min;
    }

    ind = tkz.GetNextToken();
    if ( ind == 'W' || ind == 'S' )
        deg = -deg;

    return wxString::Format( _T( "%f" ),deg );
}
