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
#ifndef _OVERVIEW_H_
#define _OVERVIEW_H_

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "wx/generic/gridctrl.h"
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include "Export.h"

#include <map>

class LogbookDialog;
class Options;
class Logbook;


using namespace std;

struct total
{
    wxArrayString		sails;

    wxString			logbookStart;
    wxString			logbookEnd;
    wxString			logbookTimeStart;
    wxString			logbookTimeEnd;
    double				distance;
    double				bestetmal;
    double				speed;
    double				speedpeak;
    double				speedSTW;
    double				speedpeakSTW;
    int					enginehours;
    int					enginemin;
    int					enginehours2;
    int					enginemin2;
    int					watermhours;
    int					watermmin;
    double              watermo;
    int					generatorhours;
    int					generatormin;
    double				bank1u;
    double				bank2u;
    double				bank1g;
    double				bank2g;
    double				fuel;
    double				water;
    double				winddir;
    double				wind;
    double				wave;
    double				swell;
    double				current;
    double				currentdir;
    double				windpeak;
    double				wavepeak;
    double				swellpeak;
    double				currentpeak;

    int					windcount;
    int					currentcount;
    int					wavecount;
    int					swellcount;
    int					etmalcount;
    int					speedcount;
    int					speedcountSTW;
};

class OverView : public Export
{
public:
    enum gridfields {FLOG, FROUTE, FSTART, FEND, FJOURNEY, FDISTANCE, FETMAL, FBESTETMAL, FSPEED, FBSPEED,/* FSPEEDSTW, FBSPEEDSTW,*/ FENGINE1, FENGINE2,FFUEL,
                     FGENERATOR,FBANK1G,FBANK1U,FBANK2G,FBANK2U,FWATERM,FWATERMO,FWATER, FWINDDIR, FWIND, FWINDPEAK,
                     FCURRENTDIR, FCURRENT, FCURRENTPEAK, FWAVE, FWAVEPEAK, FSWELL, FSWELLPEAK, FSAILS, FPATH
                    };
    OverView( LogbookDialog* d, wxString data, wxString lay, wxString layoutODT );
    ~OverView( void );

    void refresh();
    void actualLogbook();
    void allLogbooks();
    void selectLogbook();
    void setSelectedRow( int row );
    void gotoRoute();
    void setLayoutLocation();
    void viewODT( wxString path,wxString layout,int mode );
    void viewHTML( wxString path,wxString layout,int mode );
    wxString toODT( wxString path,wxString layout,int mode );
    wxString toHTML( wxString path,wxString layout,int mode );

    wxString			layout_locn;
    struct total		oneLogbookTotal;

private:
    enum logfields {ROUTE,DATEM, DATED, DATEY,TIMEH, TIMEM, TIMES, STATUS, WATCH, DISTANCE, DISTANCETOTAL, POSITION, COG, HEADING, SOG, STW, DEPTH, REMARKS,
                    BAROMETER, WIND, WINDFORCE, CURRENT, CURRENTFORCE, WAVE, SWELL, WEATHER,	CLOUDS, VISIBILITY,
                    ENGINE1, ENGINE1T,FUEL, FUELTOTAL,  SAILS, REEF, WATER, WATERTOTAL, MREMARKS,
                    HYDRO, TEMPAIR, TEMPWATER, ENGINE2, ENGINE2T, GENERATOR, GENERATORT, BANK1, BANK1T, BANK2, BANK2T,
                    WATERM, WATERMT, WATERMO
                   };

    void loadLogbookData( wxString filename, bool colour );
    void loadAllLogbooks();
    void writeSumColumn( int row, wxString logbook, wxString path, bool colour );
    void writeSumColumnLogbook( total data, int row, wxString logbook, bool colour );
    void resetValues();
    void clearGrid();
    wxString setPlaceHolders( int mode, wxGrid *grid, int row, wxString middle );
    void oneLogbookTotalReset();

    LogbookDialog*		parent;
    wxGrid*				grid;
    Options*			opt;
    Logbook*			logbook;
    wxString			selectedLogbook;

    int					row;
    int					selectedRow;

    wxString			ODTLayout_locn;
    wxString			HTMLLayout_locn;
    wxString			data_locn;
    wxString			data_file;
    wxArrayString		logbooks;
    wxArrayString		sails;

    wxString			startdate;
    wxString			enddate;
    wxString			starttime;
    wxString			endtime;
    wxString			date;
    wxString			etmaldate;
    double				distance;
    double				etmal;
    double				bestetmal;
    double				bestetmaltemp;
    double				speed;
    double				speedpeak;
    double				speedSTW;
    double				speedpeakSTW;
    int					enginehours;
    int					enginemin;
    int					enginehours2;
    int					enginemin2;
    int					watermhours;
    int					watermmin;
    double              watermo;
    int					generatorhours;
    int					generatormin;
    double				bank1u;
    double				bank2u;
    double				bank1g;
    double				bank2g;
    double				fuel;
    double				water;
    double				winddir;
    double				wind;
    double				wave;
    double				swell;
    double				current;
    double				currentdir;
    double				windpeak;
    double				wavepeak;
    double				swellpeak;
    double				currentpeak;

    int					windcount;
    int					currentcount;
    int					wavecount;
    int					swellcount;
    int					etmalcount;
    int					speedcount;
    int					speedcountSTW;
    wxString			sign;
    bool				showAllLogbooks;

    typedef std::map<wxString, int> collection;
    typedef std::pair<wxString, int> pair;
    collection t_coll;
};
#endif
