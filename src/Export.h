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
#ifndef _EXPORT_H_
#define _EXPORT_H_

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h>
#include <wx/generic/gridctrl.h>

class LogbookDialog;
class Boat;

class Export
{
public:
    Export( LogbookDialog *dialog );
    ~Export( void );

    wxString readLayoutODT( wxString path,wxString layout );
    wxString readLayoutHTML( wxString path,wxString layoutFileName );

    bool cutInPartsODT( wxString odt, wxString* top, wxString* header, wxString* middle, wxString* bottom );
    bool cutInPartsHTML( wxString html, wxString* top, wxString* header, wxString* middle, wxString* bottom );

    bool writeToODT( wxTextFile* logFile, wxGrid* grid, wxString filenameOut,wxString filenameIn,
                     wxString top,wxString header,wxString middle,wxString bottom, int mode );
    bool writeToHTML( wxTextFile* logFile, wxGrid* grid, wxString filenameOut,wxString filenameIn,
                      wxString top,wxString header,wxString middle,wxString bottom, int mode );

    wxTextFile* setFiles( wxString savePath, wxString* path, int mode );
    virtual wxString setPlaceHolders( int mode, wxGrid *grid, int row, wxString middle )
    {
        return "";
    }
    static wxString replaceNewLine( int mode, wxString str, bool label );

private:
    LogbookDialog* dialog;

};
#endif
