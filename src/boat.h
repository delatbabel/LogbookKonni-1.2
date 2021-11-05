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
#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/zipstrm.h>

class LogbookDialog;

class Boat
{
public:
    Boat( LogbookDialog* d, wxString data, wxString lay, wxString layoutODT );
    ~Boat( void );

    void saveData();
    void loadData();
    void toHTML( wxString path,wxString layout, bool mode );
    wxString toODT( wxString path,wxString layout, bool mode );
    void toCSV( wxString path );
    void toXML( wxString path );
    void toODS( wxString path );
    void backup( wxString path );
    void viewHTML( wxString path,wxString layout,bool mode );
    void viewODT( wxString path,wxString layout,bool mode );
    wxString readLayoutFileODT( wxString layout );
    void addEquip();
    void cellChanged( int row, int col );
    void deleteRow( int row );
    void setLayoutLocation( wxString loc );

    wxString		layout_locn;
    wxString		layout;
    wxString		ODTLayout;

    bool			modified;
private:
    LogbookDialog*	parent;
    wxTextFile*		boatFile;
    wxTextFile*		equipFile;
    wxTextFile*		boatLayoutFile;
    wxFile*			boatHTMLFile;
    wxFile*			boatODTFile;
    wxString		data_locn;
    wxString		equip_locn;
    wxString		lay;
    wxString		layoutODT;

    wxWindowList	ctrl;		// all wxTextCtrl's in Window;
    wxWindowList    ctrlStaticText; // all wxStaticText in Window


    void createFiles( wxString data, wxString lay );
    void createTextCtrlConnections();
    void createStaticTextList();
    void saveCSV( wxTextFile* file, bool mode );
    void saveXML( wxTextFile* file, bool mode );
    void saveODS( wxString path, bool mode );
    wxString repeatArea( wxString html );
    wxString repeatAreaODT( wxString odt );

//   DECLARE_EVENT_TABLE()

};
