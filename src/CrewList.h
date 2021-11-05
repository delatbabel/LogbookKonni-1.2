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
#include <wx/grid.h>
#include <wx/object.h>
#include <wx/arrstr.h>
#include "wx/dnd.h"
#include <wx/tokenzr.h>
#include "Options.h"

#define CREWFIELDS 13

class LogbookDialog;

/////////////////////////////// holds actual Watch ///////////////
class ActualWatch
{
public:
    static bool          active;
    static unsigned int  day;
    static int			 col;
    static wxTimeSpan	 time;
    static wxDateTime	 start, end;
    static wxString		 member;
    static wxArrayString menuMembers;
};

class CrewList
{
public:
    enum fields {ONBOARD,NAME,BIRTHNAME,FIRSTNAME,TITLE,BIRTHDATE,BIRTHPLACE,NATIONALITY,
                 PASSPORT,EST_IN,EST_ON,ZIP,COUNTRY,TOWN,STREET
                };

    enum status {DEFAULTWATCH,ALTERWATCH,HITCALCULATE,ALTERDAY};

    CrewList( LogbookDialog* d, wxString data, wxString lay, wxString layoutODT );
    ~CrewList( void );

    void loadData();
    void addCrew( wxGrid* grid, wxGrid* wake );
    void changeCrew( wxGrid* grid, int row, int col, int offset );
    void changeCrewWake( wxGrid* grid, int row, int col, bool* toggle );
    void saveCSV( wxString path );
    void saveHTML( wxString path,wxString layout, bool mode );
    void saveODT( wxString path,wxString layout, bool mode );
    void saveXML( wxString path );
    void saveODS( wxString path );
    void backup( wxString path );
    void viewHTML( wxString path,wxString layout );
    void viewODT( wxString path,wxString layout );
    void deleteRow( int row );
    void saveData();
    void setLayoutLocation( wxString loc );
    void filterCrewMembers();
    void showAllCrewMembers();
    void clearWake();
    void insertDefaultCols( bool* insertCols );
    wxString updateWatchTime( unsigned int day, int col, bool* insertCols );
    void Calculate();
    void Reset();
    void readRecord( int nr );
    void dayPlus();
    void dayMinus();
    void dayNow( bool mode );
    void dateTextCtrlClicked();
    void timeTextCtrlTextEntered( wxCommandEvent& event );
    void wakeMemberDrag( int row, int col );
    void watchEditorCreated( int row, int col );
    void watchEditorShown( int row, int col );
    void watchEditorHidden( int row, int col );
    void watchEditorHighlight( wxMouseEvent& event );
    void updateLine();
    void OnGridBeginDragCrew( wxGridEvent& event );
    void setMembersInMenu();
    void mergeWatches();
    void splitWatch();
    void setAsNewWatchbase();
    void enterDay();
    void deleteMembers();
    void checkMemberIsInMenu( wxString member );
    void rightClickMenu( int row, int col );
    int	 getDayOne( int day );
    void firstColumn();
    void statusText( int i );
    void flipWatches();

    LogbookDialog*	dialog;
    wxGrid*			gridCrew;
    wxGrid*			gridWake;

    wxString		layout_locn;
    wxString		layout;
    wxString		ODTLayout;

    wxString        lastSelectedName;
    wxString        lastSelectedFirstName;
    int				selRow,selCol,selRowWake,selColWake;
    wxString        selstr;

    bool			modified;
    unsigned int	day;

private:
    wxString	readLayout( wxString layoutFileName );
    wxString	readLayoutODT( wxString layoutFileName );
    wxString	replacePlaceholder( wxString html, wxString s, int ngrid, int row, int col, bool mode, wxArrayString watch, int offset );
    bool		checkHourFormat( wxString s, int row, int col, wxDateTime *dt );
    void		insertWatchColumn( int col, wxString time, wxDateTime wtime, wxDateTime dt, wxDateTime dtend, bool* insertCols );
    wxDateTime	stringToDateTime( wxString date, wxString time,bool mode );
    void		gridWakeInit();
    void		writeWatchTimeForDay( int i );
    wxDateTime	getStartTimeWatch( wxString s );
    wxDateTime	getEndTimeWatch( wxString s );
    wxDateTime	getAbsEndTimeWatch( wxDateTime );
    void		getStartEndDate( wxString date, wxDateTime &dtstart, wxDateTime &dtend );
    wxTimeSpan	createDefaultDateTime( wxDateTime &dt, wxDateTime &dtend, wxDateTime &time );
    void		setDayButtons( bool shift );
    wxString	deleteODTCols( wxString odt );

    wxTextFile* crewListFile;
    wxTextFile* watchListFile;
    int			linenoStart,linenoEnd;

    int			rowHeight;
    wxString	statustext[4];

private:
    Options*		opt;
    wxArrayString	crewFields;
    wxString		data_locn;
    wxString 		html_locn;
    wxString		ODT_locn;
};


/////////// Drag n Drop for Watchlist //////////

class DnDWatch : public wxTextDropTarget
{
public:
    DnDWatch( wxGrid* pOwner, CrewList* pcrewList )
    {
        m_pOwner = pOwner;
        crewList = pcrewList;
        row = -1, col = -1;
    }

    bool OnDropText( wxCoord x, wxCoord y, const wxString &text );
    wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def );

    wxGrid* source;
    int		row,col;

private:
    wxGrid*   m_pOwner;
    CrewList* crewList;
};

/////////// Drag n Drop for Crewlist //////////

class DnDCrew : public wxTextDropTarget
{
public:
    DnDCrew( wxGrid* pOwner, CrewList* pcrewList )
    {
        m_pOwner = pOwner;
        crewList = pcrewList;
        row = -1, col = -1;
    }

    bool OnDropText( wxCoord x, wxCoord y, const wxString& text );
    wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def );

    wxGrid*  source;
    wxString moveStr;
    int		 row,col;

private:
    wxGrid* m_pOwner;
    CrewList* crewList;
};

///// Immediate change in Grid when bool value changes //////////
class myGridCellBoolEditor : public wxGridCellBoolEditor
{
public:
    /*	void BeginEdit (int row, int col, wxGrid* grid)
    	{
    		wxGridCellBoolEditor::BeginEdit(row, col, grid);

    		wxFocusEvent event (wxEVT_KILL_FOCUS);
    		if (m_control)
    		{
    			m_control->GetEventHandler()->AddPendingEvent(event);
    		}
    	}
    */
    void StartingClick()
    {
        this->CBox()->SetValue( !this->CBox()->GetValue() );
        wxFocusEvent event ( wxEVT_KILL_FOCUS );
        if ( m_control )
        {
            m_control->GetEventHandler()->AddPendingEvent( event );
        }
    }
};

