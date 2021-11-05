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

#pragma once
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/jsonreader.h>

class LogbookDialog;
class Logbook;
#include <map>
using namespace std;
class LogbookHTML
{
public:
    LogbookHTML( Logbook* l, LogbookDialog* d, wxString data, wxString layout );
    ~LogbookHTML( void );

    void viewHTML( wxString path,wxString layout, bool mode );
    void viewODT( wxString path, wxString layout, bool mode );
    wxString toHTML( wxString path,wxString layout, bool mode );
    wxString toODT( wxString path,wxString layout, bool mode );
    void toKML( wxString path );
    void writeTrackToKML( wxJSONValue data );
    void writeRouteToKML( wxJSONValue data );
    void toCSV( wxString path );
    void toXML( wxString path );
    void toODS( wxString path );
    void backup( wxString path );
    void setFileName( wxString s, wxString l );
    void setPlaceholders();

public:
    enum fieldsGlobal { ROUTE,RDATE,RTIME,STATUS,WAKE,DISTANCE,DTOTAL,POSITION,COG,COW,SOG,SOW,DEPTH,REMARKS};
    enum fieldsWeather { BARO,HYDRO, AIRTE,WATERTE,WIND,WSPD,WINDR,WSPDR,CURRENT,CSPD,WAVE,SWELL,WEATHER,CLOUDS,VISIBILITY};
    enum fieldsMotor { MOTOR,MOTORT,RPM1,MOTOR1,MOTOR1T,RPM2,FUEL,FUELT,SAILS,REEF,GENE,GENET,BANK1,BANK1T,BANK2,BANK2T,WATERM,WATERMT,WATERMO,WATER,WATERT,MREMARKS,ROUTEID,TRACKID};

    struct gridc
    {
        int grid;
        int col;
    };

    map<wxString,gridc> placeholders;
    map<wxString,wxString> placeholdersboat;
    map<wxString,gridc>::iterator its;
    map<wxString,wxString>::iterator it;

    // KML export
    map<wxString,long> offsetChanges;
    map<wxString,long> offsetNavobj;
    map<wxString,long> offsetChangesGuid;
    map<wxString,long> offsetNavobjGuid;
    map<wxString,long>::iterator itc;
    map<wxString,long>::iterator itn;
    map<wxString,long>::iterator itcg;
    map<wxString,long>::iterator itng;
    typedef std::pair<wxString, long> pair;

    wxString	readLayoutFile( wxString layout );
    wxString	readLayoutFileODT( wxString layout );
    void		setSelection();
    bool		checkLayoutError( int result, wxString html, wxString layout );
    wxString	replacePlaceholder( wxString html, wxString htmlHeader, int grid, int row, int col, bool mode,wxTextOutputStream &htmlFile );
    wxString	replaceNewLine( wxString s, bool mode, bool label );
    int			setTableValues( bool mode, int index, int row, wxString html, wxTextOutputStream &d );
    wxString	convertPositionToDecimalDegrees( wxString str );
    wxString	positionToDecimalDegrees( wxString );
    wxString	replaceKMLCharacters( wxString );
    //	wxString	getPathFromTrack(wxDateTime datetime, wxString route, wxString trackguid, long offset, bool file, bool mode);
    //	wxString	findTrackInXML(wxDateTime dt, wxString file, wxString *name, wxString route, wxString track, long offset, bool ind, bool mode);
    //	void		createJumpTable();
    //	void		insertTracks(wxString file, std::map<wxString,long> *navobj, std::map<wxString,long> *navobjgui );

    LogbookDialog *parent;
    Logbook		  *logbook;
    wxString data_locn;
    wxString layout_locn;
    wxString fileName;

    wxString route;

    wxTextOutputStream *kmlFile;
};
