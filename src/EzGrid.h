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

#ifndef _EZGRID_H_INCLUDED_
#define _EZGRID_H_INCLUDED_

#include "wx/grid.h"


// EzGrid Provides extra functionality to wxGrid
class EzGrid : public wxGrid
{
public:
    EzGrid( wxWindow *parent,
            wxWindowID id,	const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
            const wxString& name = wxPanelNameStr );
    ~EzGrid();
    void PreviousState();
    void SetNumberRows( int nRows );
    void RecalcRowLabel();
    void UpdateNumRows();
    void ClearSelection();
    void RevertSel()
    {
        if ( m_selTemp )
        {
            wxASSERT( m_selection == NULL );
            m_selection = m_selTemp;
            m_selTemp = NULL;
        }
    }
protected:
    wxSize DoGetBestSize() const;
    DECLARE_EVENT_TABLE()
    void OnCellLeftClick( wxGridEvent& event );
#ifdef __WXGTK__
    void OnMouseWheel( wxMouseEvent& event );
#endif
    wxGridSelection *m_selTemp;
};



#endif // _EZGRID_H_INCLUDED_
