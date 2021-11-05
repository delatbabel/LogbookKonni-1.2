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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LOGBOOKDIALOG__
#define __LOGBOOKDIALOG__

#include "CrewList.h"
#include "Maintenance.h"
#include "boat.h"
#include "OverView.h"
#include "tinyxml.h"

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/grid.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/radiobox.h>
#include <wx/calctrl.h>
#include <wx/tglbtn.h>
#include <wx/mimetype.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/dnd.h>
#include <wx/jsonreader.h>

///////////////////////////////////////////////////////////////////////////

#define LOGGRIDS 3

#define DELETE_ROW				500
#define SELECT_ROUTE			501
#define MENUSAILS				502
#define SHOWHIDDENCOL			503
#define HIDDENCOLSEP			504
#define	MENUTIMER				505
#define HIDECOLUMN				506
#define ID_GPSTIMER				507
#define ID_SAILSTIMER			508
#define ID_LOGTIMER				510
#define COLDFINGER				511
#define MENUCREWALL				512
#define MENUCREWONBOARD			513
#define MENUWAKESPLIT			514
#define MENUWAKEMERGE			515
#define MENUWAKEDELETE			516
#define MENUDELETEEVENTS		517
#define MENUDELROWS				518
#define MENUFLIP				519
#define MENUWAKECHANGE			520
#define SELECT_TRACK			521

#define GPSTIMEOUT 5000
#define LOGSAVETIME 900000

///////////////////////////////////////////////////////////////////////////////
/// Class LogbookDialog
///////////////////////////////////////////////////////////////////////////////
class boat;
class Logbook;
class logbookkonni_pi;
class ColdFinger;
class LogbookTimer;
class myBitmapButton;
class wxJSONReader;

class LogbookDialog : public wxDialog
{
private:
    wxString columns[92];
    wxString			clouds[10];
    wxString			statusText[3];
    ColdFinger*         coldfinger;

    wxString			saveDialogFilter;
    int					lastRowSelectedService;
    int					lastRowSelectedRepairs;
    int					lastRowSelectedBuyParts;
    int					previousColumn;
    bool				noOpenPositionDlg;

    void				declareExportHeader();
    bool				checkHiddenColumns( wxGrid* grid,int i,bool use );
    void				navigationHideColumn( wxCommandEvent& ev );
    void				sortGrid( wxGrid* grid, int col, bool ascending );
    void				clearDataDir();
    void				addColdFingerDialog( wxMenu* m_menu1 );
    void				addColdFingerTextBlocks( wxMenu* m_menu1 );
    wxTreeItemId		FindMenuItem( int grid, int col, wxString name );
    void				labelLeftClick( wxGridEvent& event, wxGrid* grid );

public:
    enum fields { ROWHIGHT,ROUTE,RDATE,RTIME,WAKE,DISTANCE,POSITION,COG,SOG,REMARKS,
                  BARO,WIND,WSPD,CURRENT,CSPD,WEATHER,CLOUDS,VISIBILITY,
                  MOTOR,FUEL,SAILS,REEF,MREMARKS
                };
    enum status {STOP,SUSPEND,RUN};
    enum grids  {LOGBOOK,OVERVIEW,CREW,BOAT,GSERVICE,GREPAIRS,GBUYPARTS};

    Boat*			boat;
    CrewList*		crewList;
    Maintenance*	maintenance;
    wxString		backupFile;

    wxImageList *imageList;

    wxNotebook* m_logbook;
    wxButton* m_button4;
    wxButton* logSave;
    wxStaticLine* m_staticline8;
    wxBitmapButton* m_bpButtonShowHideLayout;
    wxStaticText* m_staticText32;
    wxChoice* logbookChoice;
    wxButton* logView;
    wxStaticLine* m_staticline7;
    wxButton* newLogbook;
    wxButton* selLogbook;
    wxStaticLine* m_staticline411;
    wxBitmapButton* m_bpButtonShowHideStatusGlobal;
    wxBitmapButton* m_bpButtonHelpGlobal;
    wxStaticLine* m_staticline401;
    wxNotebook* m_notebook8;
    wxPanel* m_panel6;
    wxMenu* m_menu10;
    wxPanel* m_panel7;
    wxGrid* m_gridWeather;
    wxPanel* m_panel71;
    wxGrid* m_gridMotorSails;
    wxButton* m_buttonSetTimer;
    wxStaticText* m_staticText1241;
    myBitmapButton* m_bpButtonTimer;
    wxStaticLine* m_staticline40;
    wxStaticText* m_staticText12411;
    myBitmapButton* m_bpButtonWatch;
    wxStaticLine* m_staticline41;
    wxStaticText* m_staticText124111;
    myBitmapButton* m_bpButton8Waypoint;
    wxStaticLine* m_staticline42;
    wxStaticText* m_staticText1241111;
    myBitmapButton* m_bpButtonDistance;
    wxTextCtrl* m_textCtrlStatusDistance;
    wxStaticLine* m_staticline43;
    wxStaticText* m_staticText12411111;
    myBitmapButton* m_bpButtonCourse;
    wxTextCtrl* m_textCtrlStatusCourseDeg;
    wxTextCtrl* m_textCtrlStatusCourseMin;
    wxStaticLine* m_staticline44;
    wxButton* m_buttonStartStop;
    wxStaticLine* m_staticline46;
    wxStaticText* m_staticTextStatusText;
    wxPanel* m_panel142;
    wxButton* logSaveOverview;
    wxStaticLine* m_staticline81;
    wxStaticText* m_staticText323;
    wxButton* logViewOverview;
    wxStaticLine* m_staticline71;
    wxStaticLine* m_staticline11;
    wxGrid* m_gridOverview;
    wxMenu* m_menuOverView;
    wxPanel* m_panel21;
    wxButton* crewAdd;
    wxButton* crewSave;
    wxStaticLine* m_staticline5;
    wxStaticText* m_staticText321;
    wxChoice* crewChoice;
    wxButton* crewView;
    wxStaticLine* m_staticline36;
    wxStaticText* m_staticText1171;
    wxStaticLine* m_staticline2;
    wxSplitterWindow* m_splitterWatch;
    wxPanel* m_panel211;
    wxGrid* m_gridCrew;
    wxMenu* m_menu2;
    wxStaticBoxSizer* sbSizer61;
    wxStaticText* m_staticText1141;
    wxTextCtrl* m_textCtrlWatchStartDate;
    wxTextCtrl* m_textCtrlWatchStartTime;
    wxStaticText* m_staticText1161;
    wxButton* m_buttonCalculate;
    wxButton* m_buttonReset;
    wxButton* m_buttonClear;
    wxStaticLine* m_staticline35;
    wxButton* m_buttonDayMinus;
    wxButton* m_buttonNow;
    wxButton* m_buttonDayPlus;
    wxStaticText* m_staticTextWakeDay;
    wxStaticText* m_staticText1181;
    wxGrid* m_gridCrewWake;
    wxMenu* m_menu21;
    wxPanel* m_panel24;
    wxStaticText* m_staticTextStatusWatch;
    wxPanel* m_panel3;
    wxButton* addEquipmentButton;
    wxToggleButton* m_toggleBtnShowEquipment;
    wxStaticLine* m_staticline27;
    wxButton* boatSave;
    wxStaticLine* m_staticline4;
    wxStaticText* m_staticText322;
    wxChoice* boatChoice;
    wxButton* boatView;
    wxStaticLine* m_staticline3;
    wxSplitterWindow* m_splitter1;
    wxPanel* m_panel72;
    wxStaticText* bname;
    wxTextCtrl* boatName;
    wxStaticText* m_staticText114;
    wxTextCtrl* homeport;
    wxStaticText* m_staticText115;
    wxTextCtrl* callsign;
    wxStaticText* m_staticText116;
    wxTextCtrl* hin;
    wxStaticText* m_staticText117;
    wxTextCtrl* sailno;
    wxStaticText* m_staticText119;
    wxTextCtrl* insurance;
    wxStaticText* m_staticText118;
    wxTextCtrl* registration;
    wxStaticText* m_staticText120;
    wxTextCtrl* policy;
    wxStaticText* m_staticText53;
    wxTextCtrl* mmsi;
    wxStaticText* m_staticText90;
    wxTextCtrl* ownerName;
    wxStaticText* m_staticText91;
    wxTextCtrl* ownerFirstname;
    wxStaticText* m_staticText95;
    wxTextCtrl* ownerPhone;
    wxStaticText* m_staticText92;
    wxTextCtrl* ownerStreet;
    wxStaticText* m_staticText93;
    wxTextCtrl* owenerZip;
    wxStaticText* m_staticText94;
    wxTextCtrl* ownerTown;
    wxStaticText* m_staticText128;
    wxTextCtrl* boatType;
    wxStaticText* m_staticText125;
    wxTextCtrl* builder;
    wxStaticText* m_staticText124;
    wxTextCtrl* hull;
    wxStaticText* m_staticText126;
    wxTextCtrl* dateLaunched;
    wxStaticText* m_staticText127;
    wxTextCtrl* yardNr;
    wxStaticText* m_staticText123;
    wxTextCtrl* designer;
    wxStaticText* m_staticText129;
    wxTextCtrl* construction;
    wxStaticText* m_staticText106;
    wxTextCtrl* LOA;
    wxStaticText* m_staticText107;
    wxTextCtrl* LOD;
    wxStaticText* m_staticText108;
    wxTextCtrl* LWL;
    wxStaticText* m_staticText109;
    wxTextCtrl* beam;
    wxStaticText* m_staticText110;
    wxTextCtrl* draft;
    wxTextCtrl* UserLabel1;
    wxTextCtrl* Userfield2;
    wxTextCtrl* Userfield3;
    wxStaticText* m_staticText122;
    wxTextCtrl* displacement;
    wxPanel* m_panel8;
    wxGrid* m_gridEquipment;
    wxMenu* m_menu3;
    wxPanel* m_panel14;
    wxStaticLine* m_staticline151;
    wxStaticText* m_staticText621;
    wxStaticLine* m_staticline141;
    wxMenu* m_menu7;
    wxPanel* m_panel141;
    wxStaticLine* m_staticline1513;
    wxStaticText* m_staticText6213;
    wxStaticLine* m_staticline1413;
    wxMenu* m_menu71;
    wxPanel* m_panel16;
    wxStaticLine* m_staticline1511;
    wxStaticText* m_staticText6211;
    wxStaticLine* m_staticline1411;
    wxMenu* m_menu711;
//        wxStaticText* m_staticText140;
    wxTextCtrl* m_textCtrl95;
    wxStaticText* m_staticText141;
    wxTextCtrl* m_textCtrl96;
    wxStaticText* m_staticText142;
    wxTextCtrl* m_textCtrl100;
    wxStaticText* m_staticText143;
    wxTextCtrl* m_textCtrl98;
    wxStaticLine* m_staticline421;
    wxButton* m_button61;
    wxStaticLine* m_staticline431;
    wxChoice* m_choice32;
    wxButton* m_buttonSavePolar;
    wxTextCtrl* m_textCtrl99;
    wxPanel* m_panel22;
    wxButton* m_buttonSailsReset;
    wxBoxSizer* bSizerPolar;
    wxStaticText* m_staticText144;
    wxChoice* m_choiceDegreesPolar;
    wxStaticLine* m_staticline441;
    wxStaticText* m_staticText145;
    wxStaticText* m_staticText146;
    wxChoice* m_choiceSourcePolar;
    wxStaticText* m_staticText148;
    wxChoice* m_choiceWindPolar;
    wxChoice* m_choiceWavefromPolar;
    wxStaticText* m_staticText147;
    wxChoice* m_choice37;
    wxChoice* m_choiceCurrentPolar;
    wxButton* m_button62;
    wxStaticLine* m_staticline45;
    wxButton* m_buttonFilterPolar;
    wxChoice* m_choiceRingsPolar;

    // Virtual event handlers, overide them in your derived class
    void LogbookDialogOnClose( wxCloseEvent& event ) ;
    void LogbookDialogOnInitDialog( wxInitDialogEvent& event ) ;
    void OnNoteBookPageChangedLogbook( wxNotebookEvent& event );
    void m_button4OnButtonClick( wxCommandEvent& event ) ;
    void logSaveOnButtonClick( wxCommandEvent& event ) ;
    void OnGridLabelLeftDClickGlobal( wxGridEvent& event );
    void OnGridLabelLeftDClickWeather( wxGridEvent& event );
    void OnGridLabelLeftDClickMotorSails( wxGridEvent& event );
    void OnGridLabelLeftDClickOverview( wxGridEvent& event );
    void OnGridLabelLeftDClickCrew( wxGridEvent& event );
    void OnGridLabelLeftDClickCrewWake( wxGridEvent& event );
    void OnMenuSelectionHiddenCrew( wxCommandEvent& event );
    void onButtonReloadLayoutLogbook( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutLogbook( wxCommandEvent& event ) ;
    void onRadioButtonHTML( wxCommandEvent& event ) ;
    void onRadioButtonODT( wxCommandEvent& event ) ;
    void logViewOnButtonClick( wxCommandEvent& event ) ;
    void newLogbookOnButtonClick( wxCommandEvent& event ) ;
    void selLogbookOnButtonClick( wxCommandEvent& event ) ;
    void m_gridGlobalOnGridCellRightClick( wxGridEvent& event ) ;
    void m_gridGlobalOnGridCmdCellChange( wxGridEvent& event ) ;
    void m_gridGlobalOnGridSelectCell( wxGridEvent& event ) ;
    void OnMenuSelectionHideColumn( wxCommandEvent& event );
    void OnMenuSelectionHideColumnOverView( wxCommandEvent& event );
    void m_gridGlobalOnKeyDown( wxKeyEvent& event ) ;
    void m_menuItem1OnMenuSelection( wxCommandEvent& event ) ;
    void m_gridWeatherOnGridCellRightClick( wxGridEvent& event ) ;
    void m_gridWeatherOnGridCmdCellChange( wxGridEvent& event ) ;
    void m_gridWeatherOnGridSelectCell( wxGridEvent& event ) ;
    void m_gridWeatherOnKeyDown( wxKeyEvent& event ) ;
    void m_gridMotorSailsOnGridCellRightClick( wxGridEvent& event ) ;
    void m_gridMotorSailsOnGridCmdCellChange( wxGridEvent& event ) ;
    void m_gridMotorSailsOnGridSelectCell( wxGridEvent& event ) ;
    void m_gridMotorSailsOnKeyDown( wxKeyEvent& event ) ;
    void crewAddOnButtonClick( wxCommandEvent& event ) ;
    void crewSaveOnButtonClick( wxCommandEvent& event ) ;
    void OnMenuSelectionOnboardCrew( wxCommandEvent& event );
    void OnMenuSelectionAllEntriesCrew( wxCommandEvent& event );
    void onButtonClickReloadLayoutsCrew( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutCrew( wxCommandEvent& event ) ;
    void onRadioButtonHTMLCrew( wxCommandEvent& event ) ;
    void onRadioButtonODTCrew( wxCommandEvent& event ) ;
    void crewViewOnButtonClick( wxCommandEvent& event ) ;
    void m_gridCrewOnGridCellChange( wxGridEvent& event ) ;
    void m_gridCrewOnGridCellRightClick( wxGridEvent& event ) ;
    void m_menuItem2OnMenuSelection( wxCommandEvent& event ) ;
    void m_gridCrewWakeOnGridCellChange( wxGridEvent& event ) ;
    void boataddEquipmentButtonOnButtonClick( wxCommandEvent& event ) ;
    void boatSaveOnButtonClick( wxCommandEvent& event ) ;
    void onButtonClickReloadLayoutsBoat( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutBoat( wxCommandEvent& event ) ;
    void onRadioButtonHTMLBoat( wxCommandEvent& event ) ;
    void onRadioButtonODTBoat( wxCommandEvent& event ) ;
    void boatViewOnButtonClick( wxCommandEvent& event ) ;
    void boatNameOnTextEnter( wxCommandEvent& event ) ;
    void m_gridEquipmentOnGridCellChange( wxGridEvent& event ) ;
    void m_gridEquipmentOnGridCellRightClick( wxGridEvent& event ) ;
    void m_menuItem3OnMenuSelection( wxCommandEvent& event ) ;
    void OnMenuSelectionShowHiddenCols( wxCommandEvent& event );
    void onButtobClickAddLineService( wxCommandEvent& event ) ;
    void onButtobClickSaveService( wxCommandEvent& event ) ;
    void onButtonClickReloadLayoutsServiceHTML( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutODTService( wxCommandEvent& event ) ;
    void onRadioButtonHTMLService( wxCommandEvent& event ) ;
    void onRadioButtonODTService( wxCommandEvent& event ) ;
    void onButtonClickViewService( wxCommandEvent& event ) ;
    void onGridCellServiceChange( wxGridEvent& event ) ;
    void onGridCellRightClickService( wxGridEvent& event ) ;
    void onGridCellServiceSelected( wxGridEvent& event ) ;
    void OnKeyDownService( wxKeyEvent& event ) ;
    void onMenuSelectionServiceOK( wxCommandEvent& event ) ;
    void onMenuSelectionServiceBuyParts( wxCommandEvent& event ) ;
    void onMenuSelectionServiceDelete( wxCommandEvent& event ) ;
    void onButtobClickAddLineRepairs( wxCommandEvent& event ) ;
    void onButtobClickSaveRepairs( wxCommandEvent& event ) ;
    void onButtonClickReloadLayoutsRepairsHTML( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutODTRepairs( wxCommandEvent& event ) ;
    void onRadioButtonHTMLRepairs( wxCommandEvent& event ) ;
    void onRadioButtonODTRepairs( wxCommandEvent& event ) ;
    void onButtonClickViewRepairs( wxCommandEvent& event ) ;
    void onGridCellRepairsChange( wxGridEvent& event ) ;
    void onGridCellRightClickRepairs( wxGridEvent& event ) ;
    void onGridCellRepairsSelected( wxGridEvent& event ) ;
    void OnKeyDownRepairs( wxKeyEvent& event ) ;
    void onMenuSelectionRepairsOK( wxCommandEvent& event ) ;
    void onMenuSelectionRepairsBuyParts( wxCommandEvent& event ) ;
    void onMenuSelectionRepairsDelete( wxCommandEvent& event ) ;
    void onButtobClickAddLineBuyParts( wxCommandEvent& event ) ;
    void onButtobClickSaveBuyParts( wxCommandEvent& event ) ;
    void onButtonClickReloadLayoutsBuyPartsHTML( wxCommandEvent& event ) ;
    void onButtonClickEditLayoutODTBuyParts( wxCommandEvent& event ) ;
    void onRadioButtonHTMLBuyParts( wxCommandEvent& event ) ;
    void onRadioButtonODTBuyParts( wxCommandEvent& event ) ;
    void onButtonClickViewBuyParts( wxCommandEvent& event ) ;
    void onGridCellChangeBuyParts( wxGridEvent& event ) ;
    void onGridCellRightClickBuyParts( wxGridEvent& event ) ;
    void onGridEditorHidden( wxGridEvent& event ) ;
    void onGridEditorShow( wxGridEvent& event ) ;
    void onGridCellSelectedBuyParts( wxGridEvent& event ) ;
    void OnKeyDownBuyParts( wxKeyEvent& event ) ;
    void onMenuSelectionBuyPartsDelete( wxCommandEvent& event ) ;
    void OnButtonClickOverviewSave( wxCommandEvent& event );
    void onButtonReloadLayoutOverView( wxCommandEvent& event );
    void onButtonClickEditLayoutOverview( wxCommandEvent& event );
    void onRadioButtonHTMLOverview( wxCommandEvent& event );
    void onRadioButtonODTOverView( wxCommandEvent& event );
    void OnButtonClickOverView( wxCommandEvent& event );
    void OnGridCellRightClickOverview( wxGridEvent& event );
    void OnGridLabelLeftClickOverview( wxGridEvent& event );
    void OnMenuSelectionGotoRoute( wxCommandEvent& event );
    void OnMenuSelectionShowHiddenColsOverview( wxCommandEvent& event );
    void onButtonClickSelectLogbook( wxCommandEvent& event );
    void OnRadioButtonActualLogbook( wxCommandEvent& event );
    void OnRadioButtonAllLogbooks( wxCommandEvent& event );
    void onGridCellLeftClickService( wxGridEvent& event );
    void onGridCellLeftClickBuyParts( wxGridEvent& event );
    void OnToggleButtonShowEquip( wxCommandEvent& event );
    void OnKeyDownOverview( wxKeyEvent& event );
    void OnMenuSelectionSearch( wxCommandEvent& event );
    void OnMenuSelectionNewWatchWake( wxCommandEvent& event );
    void OnGridCellRightClickWake( wxGridEvent& event );
    void OnMenuSelectionAddWatch( wxCommandEvent& event );
    void OnMenuSelectionSameWatch( wxCommandEvent& event );
    void OnGridLabelLeftClickService( wxGridEvent& event );
    void OnGridLabelLeftClickRepairs( wxGridEvent& event );
    void OnGridLabelLeftClickBuyParts( wxGridEvent& event );
    void OnGridCellLeftClickGlobal( wxGridEvent& event );
    void OnMenuSelectionAsc( wxCommandEvent& event );
    void OnMenuSelectionDesc( wxCommandEvent& event );
    void OnKeyDownCrew( wxKeyEvent& event );
    void OnKeyDownWatch( wxKeyEvent& event );
    void OnButtonClickWakeClear( wxCommandEvent& event );
    void OnMenuSelectionShutdown( wxCommandEvent& event );
    void OnGridLabelRightClickGlobal( wxGridEvent& event );
    void OnGridLabelRightClickWeather( wxGridEvent& event );
    void OnGridLabelRightClickMotorSails( wxGridEvent& event );
    void OnLeftUpDistance( wxMouseEvent& event );
    void OnLeftUpCourse( wxMouseEvent& event );
    void OnLeftUpCourseMin( wxMouseEvent& event );
    void OnLeftUpStartTime( wxMouseEvent& event );
    void OnLeftUpDays( wxMouseEvent& event );
    void OnLeftUpDay( wxMouseEvent& event );
    void OnNotebookPageChangedLoggrids( wxNotebookEvent& event );
    void OnButtomClickStatusbarGlobal( wxCommandEvent& event );
    void OnClickButtonHelpGlobal( wxCommandEvent& event );
    void OnChoiceGlobal( wxCommandEvent& event );
    void OnChoiceCrew( wxCommandEvent& event );
    void OnChoiceBoat( wxCommandEvent& event );
    void OnChoiceBoatOverview( wxCommandEvent& event );
    void OnChoiceService( wxCommandEvent& event );
    void OnChoiceBuyParts( wxCommandEvent& event );
    void OnChoiceRepairs( wxCommandEvent& event );
    void OnGridBeginDragCrew( wxGridEvent& event );
    void OnGridBeginDragWatch( wxGridEvent& event );
    void m_gridCrewOnGridCellLeftClick( wxGridEvent& event );
    void OnButtonClickCalculate( wxCommandEvent& event );
    void OnButtonClickReset( wxCommandEvent& event );
    void OnButtonClickDayPlus( wxCommandEvent& event );
    void OnButtonClickNow( wxCommandEvent& event );
    void OnButtonClickDayMinus( wxCommandEvent& event );
    void OnLeftDownWatchStartDate( wxMouseEvent& event );
    void OnTextEnterWatchTime( wxCommandEvent& event );
    void OnGridCellLeftClickWatch( wxGridEvent& event );
    void OnGridEditorShownWatch( wxGridEvent& event );
    void OnGridEditorHiddenWatch( wxGridEvent& event );
    void OnGridEditorCreatedWatch( wxGridEditorCreatedEvent& event );
    void OnMotion( wxMouseEvent& event );
    void OnGridEditorShownCrew( wxGridEvent& event );
    void OnMenuSelectionSplitWatch( wxCommandEvent& event );
    void OnMenuSelectionMergeWatch( wxCommandEvent& event );
    void OnTextEnterDayWake( wxCommandEvent& event );
    void OnMenuSelectionDeleteMembers( wxCommandEvent& event );
    void OnSplitterSashPositionChangedWake( wxSplitterEvent& event );
    void OnGridLabelRightClickWake( wxGridEvent& event );
    void OnGridLabelLeftClickWake( wxGridEvent& event );
    void OnIdleMainDialog( wxIdleEvent& event );
    void OnButtonClickStatusTimer( wxCommandEvent& event );
    void OnStatusBulletTimer( wxCommandEvent& event );
    void OnStatusBulletWatch( wxCommandEvent& event );
    void OnStatusBulletWaypoint( wxCommandEvent& event );
    void OnStatusBulletDistance( wxCommandEvent& event );
    void OnStatusBulletCourse( wxCommandEvent& event );
    void OnButtonClickStatusStartStop( wxCommandEvent& event );
    void OnTextEnterStatusDistance( wxCommandEvent& event );
    void OnTextEnterStatusCourseDeg( wxCommandEvent& event );
    void OnTextEnterStatusMinutes( wxCommandEvent& event );
    void OnGridLabelLeftClickGlobal( wxGridEvent& event );
    void OnGridLabelLeftClickWeather( wxGridEvent& event );
    void OnGridLabelLeftClickMotorSails( wxGridEvent& event );
    void onMenuSelectionDeleteRows( wxCommandEvent& event );
    void OnMenuSelectionFlip( wxCommandEvent& event );
    void OnMenuSelectionFlipWatches( wxCommandEvent& event );
    void OnButtomClickShowHideLayout( wxCommandEvent& event );
    void OnToggleButtonEngine1( wxCommandEvent& event );
    void OnToggleButtonEngine2( wxCommandEvent& event );
    void OnToggleButtonGenerator( wxCommandEvent& event );
    void OnButtonClickResetSails( wxCommandEvent& event );
    void OnCheckboxSails( wxCommandEvent& event );
    void AutoStatusStartStop();

    void OnComboUpService( wxCommandEvent& event );
    void OnComboUpRepairs( wxCommandEvent& event );
    void OnComboUpBuyParts( wxCommandEvent& event );

    void m_menu1Highlighted( wxMenuEvent& event );

    void gridGlobalScrolled( wxScrollWinEvent& event );
    void gridWeatherScrolled( wxScrollWinEvent& event );
    void gridMotorSailsScrolled( wxScrollWinEvent& event );

    void appendOSDirSlash( wxString* pString );

    void getIniValues();
    void setIniValues();
    void flipMonthDay();
    void deleteRows();
    void setBulletColorAllStates( myBitmapButton* button, wxBitmap bmp );

public:
    enum maintenanceTab {SERVICE,REPAIRS,BUYPARTS};
    enum FORMAT {HTML,ODT};

    wxPanel* m_panel2;
    wxBoxSizer* bSizer6;
    wxPanel* m_panelLayout;
    wxButton* m_buttonReloadLayout;
    wxButton* m_buttonEditLayout;
    wxPanel* m_panelEngine;
    wxToggleButton* m_toggleBtnEngine1;
    wxToggleButton* m_toggleBtnEngine2;
    wxToggleButton* m_toggleBtnGenerator;
    wxFlexGridSizer* fgSizerSails;
    wxRadioButton* m_radioBtnHTML;
    wxRadioButton* m_radioBtnODT;
    wxGrid* m_gridGlobal;
    wxMenu* m_menu1;
    wxPanel* Statusbar;
    wxChoice* overviewChoice;
    wxButton* m_buttonReloadLayoutOview;
    wxButton* m_buttonEditLayoutOview;
    wxRadioButton* m_radioBtnHTMLOverview;
    wxRadioButton* m_radioBtnODTOverview;
    wxRadioButton* m_radioBtnActualLogbook;
    wxRadioButton* m_radioBtnAllLogbooks;
    wxRadioButton* m_radioBtnSelectLogbook;
    wxButton* m_buttonSelectLogbook;
    wxButton* m_buttonReloadCrew;
    wxButton* m_buttonEditLayoutCrew;
    wxRadioButton* m_radioBtnHTMLCrew;
    wxRadioButton* m_radioBtnODTCrew;
    wxChoice* m_choiceCrewNames;
    wxTextCtrl* m_textCtrlWakeTrip;
    wxTextCtrl* m_textCtrlWakeDay;
    wxChoice* m_choiceWakeDisplay;
    wxButton* m_buttonReloadLayoutsBoat;
    wxButton* m_buttonEditLayoutBoat;
    wxRadioButton* m_radioBtnHTMLBoat;
    wxRadioButton* m_radioBtnODTBoat;
    wxStaticBoxSizer* sbSizer6;
    wxTextCtrl* Userfield1;
    wxTextCtrl* UserLabel2;
    wxTextCtrl* UserLabel3;
    wxStaticBoxSizer* sbSizer12;
    wxPanel* m_panel13;
    wxNotebook* m_notebook6;
    wxButton* m_buttonAddLineService;
    wxButton* m_buttonSaveService;
    wxChoice* m_choiceSelectLayoutService;
    wxButton* m_buttonReloadLayoutsServiceHTML;
    wxButton* m_buttonEditLayoutODTService;
    wxRadioButton* m_radioBtnHTMLService;
    wxRadioButton* m_radioBtnODTService;
    wxButton* m_buttonViewService;
    wxGrid* m_gridMaintanence;
    wxButton* m_buttonAddLineRepairs;
    wxButton* m_buttonSaveRepairs;
    wxChoice* m_choiceSelectLayoutRepairs;
    wxButton* m_buttonReloadLayoutsRepairsHTML;
    wxButton* m_buttonEditLayoutODTRepairs;
    wxRadioButton* m_radioBtnHTMLRepairs;
    wxRadioButton* m_radioBtnODTRepairs;
    wxButton* m_buttonViewRepairs;
    wxGrid* m_gridMaintanenceRepairs;
    wxButton* m_buttonAddLineBuyParts;
    wxButton* m_buttonSaveBuyParts;
    wxChoice* m_choiceSelectLayoutBuyParts;
    wxButton* m_buttonReloadLayoutsBuyPartsHTML;
    wxButton* m_buttonEditLayoutODTBuyParts;
    wxRadioButton* m_radioBtnHTMLBuyParts;
    wxRadioButton* m_radioBtnODTBuyParts;
    wxButton* m_buttonViewBuyParts;
    wxGrid* m_gridMaintenanceBuyParts;
    wxPanel* m_panelPolar;

    LogbookDialog( logbookkonni_pi* d, wxTimer* t, LogbookTimer* lt, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _( "Active Logbook" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1010,535 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
    ~LogbookDialog();

    void m_gridGlobalOnContextMenu( wxMouseEvent &event )
    {
        m_gridGlobal->PopupMenu( m_menu1, event.GetPosition() );
    }

    void m_gridServiceOnContextMenu( wxMouseEvent &event )
    {
        m_gridMaintanence->PopupMenu( m_menu7, event.GetPosition() );
    }

    void m_splitter1OnIdle( wxIdleEvent& )
    {
        m_splitter1->SetSashPosition( 310 );
        m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( LogbookDialog::m_splitter1OnIdle ), NULL, this );
    }

    void m_splitterWatchOnIdle( wxIdleEvent& )
    {
        m_splitterWatch->UpdateSize();
        m_splitterWatch->SetSashPosition( 0 );
        m_splitterWatch->Disconnect( wxEVT_IDLE, wxIdleEventHandler( LogbookDialog::m_splitterWatchOnIdle ), NULL, this );
    }


    static bool myParseDate( wxString s,wxDateTime  &dt );
    static bool myParseTime( wxString s, wxDateTime &dt );
    static void setDatePattern();

    void setTitleExt();
    wxString replaceDangerChar( wxString s );
    wxString restoreDangerChar( wxString s );
    void startBrowser( wxString filename );
    void startApplication( wxString filename, wxString ext );
    void loadLayoutChoice( int grid, wxString path, wxChoice* choice, wxString filter );
    void setEqualRowHeight( int row );
    void init();
    void OnTimerGPS( wxTimerEvent& ev );
    void OnLogTimer( wxTimerEvent& ev );
    void OnTimerSails( wxTimerEvent& ev );
    int  showLayoutDialog( int grid, wxChoice *choice, wxString location, int format );
    bool isInArrayString( wxArrayString ar, wxString s );
    wxDateTime getDateTo( wxString filename );
    void loadTimerEx();
    int setBulletColor( myBitmapButton* button, bool* option, int on );
    int  checkBitmaps();
    void refreshBullets();
    void startNormalTimer();
    void resetBullets();
    void deselectAllLogbookGrids();
    void writeToRouteDlg( wxJSONValue data );
    void setToNumberEngine();
    void setShowGenerator();
    void stopEngine1( bool enable, bool print );
    void stopEngine2( bool enable, bool show, bool print );
    void stopGenerator( bool enabled, bool show, bool print );
    void startEngine1( bool enabled, bool active, bool print );
    void startEngine2( bool enabled, bool active, bool print );
    void startGenerator( bool enabled, bool active, bool print );
    void resetSails();
    void setCheckboxLabels();
    void setSailsGap();
    void setAbbreviations();
    void setCheckboxSails();
    void stateSails();

    logbookkonni_pi*	  logbookPlugIn;
    wxString			  Home_Locn;
    wxString			  basePath;
    wxString              data;
    wxString              image_locn;
    wxString              help_locn;
    wxString              clouds_locn;
    Logbook*			  logbook;
    OverView*			  overview;
    wxString			  onOff[2];
    wxString			  panelOnOff[2];
    wxCheckBox*			  checkboxSails[14];

    static wxString		datePattern;
    static wxChar		dateSeparator;
    wxString			decimalPoint;

    wxMenuItem*			menuItemTimer;
    wxString			layoutHTML;
    wxString			layoutODT;
    LogbookTimer*		logbookTimerWindow;
    wxTimer*            timer;
    wxTimer*			GPSTimer;
    wxTimer*			SailsTimer;
    bool				statusGPS;
    wxTimer*			logbookTimer;
    int					fullHourPlus;
    bool				fullHourModified;
    int					sashPos;

    wxColour			defaultBackground;
    int					gridGlobalCol;
    int					gridWeatherCol;
    int					gridMotorSailsCol;

    int					selGridRow;
    int					selGridCol;
    
    int                 sc;
    int                 cntsails;

    wxGrid				*logGrids[LOGGRIDS];
    int					numPages;
    int					totalColumns;
    wxFont				font;

    // ODT-Strings for Export
    wxString			content;
    wxString			contentEnd;
    wxString			manifest;
    wxString			styles;
    wxString			meta;

    // XML-String for Export Excel
    wxString			xmlHead;
    wxString			xmlEnd;

    // KML-String for Export
    wxString			kmlHead;
    wxString			kmlFolder;
    wxString			kmlLine;
    wxString			kmlEndFolder;
    wxString			kmlBody;
    wxString			kmlEnd;
    wxString			kmlPathHeader;
    wxString			kmlPathFooter;

    wxString			titleExt;
};

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RouteDialog
///////////////////////////////////////////////////////////////////////////////
class RouteDialog : public wxDialog
{
private:

protected:

public:
    wxListCtrl* m_listCtrlRoute;
    wxStdDialogButtonSizer* m_sdbSizer6;
    wxButton* m_sdbSizer6OK;
    wxButton* m_sdbSizer6Cancel;

    RouteDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _( "Select Route" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 269,318 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~RouteDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class LayoutDialog
///////////////////////////////////////////////////////////////////////////////
class LayoutDialog : public wxDialog
{
private:

protected:
    wxBitmapButton* m_bpButtonLoadLayout;
    wxStdDialogButtonSizer* m_sdbSizer3;
    wxButton* m_sdbSizer3OK;
    wxButton* m_sdbSizer3Cancel;

    // Virtual event handlers, overide them in your derived class
    void OnButtonClickLoadLayout( wxCommandEvent& event );
    void OnChoice( wxCommandEvent& event );
    void OnText( wxCommandEvent& event );


public:
    wxRadioButton* m_radioBtnEdit;
    wxRadioButton* m_radioBtnFilter;
    wxRadioButton* m_radioBtnRename;
    wxTextCtrl* m_textCtrlRename;
    wxRadioButton* m_radioBtnDelete;
    wxRadioButton* m_radioBtnEmail;
    wxRadioButton* m_radioBtnInstall;
    wxChoice* m_choice15;
    wxString layoutPath;
    wxString layoutFileName;
    LogbookDialog *dialog;
#ifdef __WXMSW__
    LayoutDialog( wxWindow* parent, wxString location, wxChoice* choice,wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 307,205 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#else
    LayoutDialog( wxWindow* parent, wxString location, wxChoice* choice,wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 307,230 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

#endif
    ~LayoutDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SelectLogbookDialog
///////////////////////////////////////////////////////////////////////////////
class SelectLogbook : public wxDialog
{
private:
    wxString path;
    LogbookDialog* parent;
    wxDateTime getDateTo( wxString filename );
protected:
    wxStdDialogButtonSizer* m_sdbSizer4;
    wxButton* m_sdbSizer4OK;
    wxButton* m_sdbSizer4Cancel;

    void OnInit( wxInitDialogEvent& event );
    void OnCellSelecttion( wxGridEvent& event );
    void OnGridCellChange( wxGridEvent& event );
    void OnKeyDown( wxKeyEvent& event );

public:
    wxGrid* m_grid13;
    wxArrayString files;
    int selRow;

#ifdef __WXMSW__
    SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = _( "Select Logbook" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 700,252 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
#else
    SelectLogbook( wxWindow* parent, wxString path, wxWindowID id = wxID_ANY, const wxString& title = _( "Select Logbook" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 610,350 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER  );
#endif
    ~SelectLogbook();

};

///////////////////////////////////////////////////////////////////////////
WX_DECLARE_OBJARRAY( wxArrayString, myGridStringArray ); //(,
//class WXDLLIMPEXP_ADV);

class myGridStringTable : public wxGridTableBase
{
public:
    myGridStringTable();
    myGridStringTable( int numRows, int numCols );
    ~myGridStringTable();

    // these are pure virtual in wxGridTableBase
    //
    int GetNumberRows();
    int GetNumberCols();
    wxString GetValue( int row, int col );
    void SetValue( int row, int col, const wxString& s );
    bool IsEmptyCell( int row, int col );

    // overridden functions from wxGridTableBase
    //
    void Clear();
    bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool AppendRows( size_t numRows = 1 );
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool AppendCols( size_t numCols = 1 );
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    void SetRowLabelValue( int row, const wxString& );
    void SetColLabelValue( int col, const wxString& );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );

    wxGridStringArray m_data;

private:


    // These only get used if you set your own labels, otherwise the
    // GetRow/ColLabelValue functions return wxGridTableBase defaults
    //
    wxArrayString     m_rowLabels;
    wxArrayString     m_colLabels;

//    DECLARE_DYNAMIC_CLASS_NO_COPY( wxGridStringTable )
};

class myTreeItem;
//////////////////// Dialog ColdFinger //////////////////////////////////
class ColdFinger : public wxDialog
{
private:
    LogbookDialog	*dialog;
    bool			modified;
    wxString		dataPath;
    wxImageList*    imageList;

    void            init();
    void			addElement( TiXmlElement* root, const char* key, const char* label );
    wxTreeItemId	recursiveWrite( wxTreeItemId id, TiXmlElement *elem );
    void			fillTree( wxTreeItemId id, TiXmlNode *elem );

protected:
    wxSplitterWindow* m_splitter2;
    wxPanel* m_panel18;
    wxMenu* m_menu9;
    wxPanel* m_panel19;
    wxStaticText* m_staticText94;
    wxStaticText* m_staticText95;
    wxChoice* m_choice23;
    wxStaticText* m_staticText96;
    wxChoice* m_choice24;
    wxStaticLine* m_staticline32;
    wxStaticText* m_staticText97;
    wxStdDialogButtonSizer* m_sdbSizer8;
    wxButton* m_sdbSizer8OK;
    wxButton* m_sdbSizer8Cancel;

    // Virtual event handlers, overide them in your derived class
    void OnCloseCold( wxCloseEvent& event );
    void OnInitDialog( wxInitDialogEvent& event );
    void OnTreeBeginDragCold( wxTreeEvent& event );
    void OnTreeEndDragCold( wxTreeEvent& event );
    void OnTreeItemRightClickCold( wxTreeEvent& event );
    void OnTreeSelChanged( wxTreeEvent& event );
    void OnMenuSelectionAddCold( wxCommandEvent& event );
    void OnMenuSelectionDeleteCold( wxCommandEvent& event );
    void OnMenuSelectionRenameCold( wxCommandEvent& event );
    void OnMenuSelectionaddNodeCold( wxCommandEvent& event );
    void OnMenuTreeSelectionDeleteNodeCold( wxCommandEvent& event );
    void OnTextCold( wxCommandEvent& event );
    void OnCancelButtonClickCold( wxCommandEvent& event );
    void OnOKButtonClickCold( wxCommandEvent& event );


public:
    enum treenodes { NODE,ITEM };
    wxTreeCtrl* m_treeCtrl3;
    wxTextCtrl* m_textCtrl73;
    myTreeItem* retItem;
    wxTreeItemId selectedItem;
    int				fo;
    int				it;

    ColdFinger( LogbookDialog* parent,  wxWindowID id = wxID_ANY, const wxString& title = _( "Textblocks Dialog" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 524,392 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
    ~ColdFinger();

    void	writeTextblocks();
    void	loadTextBlocks();

    void m_splitter2OnIdle( wxIdleEvent& )
    {
        m_splitter2->SetSashPosition( 0 );
        m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ColdFinger::m_splitter2OnIdle ), NULL, this );
    }
};

class myTreeItem : public wxTreeItemData
{
public:
    myTreeItem( int type, wxString name, wxString text, wxString guid, wxString route, wxString guidWP, wxString WP, int grid, int gridcol, bool deleteable, bool add, bool menu );
    myTreeItem( const myTreeItem* item );
    ~myTreeItem() {};

    int		 type;
    wxString name;
    wxString text;
    wxString guid;
    wxString route;
    wxString guidWP;
    wxString WP;
    int      grid;
    int      gridcol;
    bool	 deleteable;
    bool     add;
    bool     menu;

};

class DnD : public wxTextDropTarget
{
public:
    DnD( ColdFinger* d, wxTreeCtrl *pOwner, wxTreeCtrl* pSender )
    {
        m_pOwner = pOwner;
        m_pSender = pSender;
        dialog = d;
    }
    ~DnD();

    bool OnDropText( wxCoord x, wxCoord y, const wxString& str );

private:
    wxTreeCtrl *m_pOwner;
    wxTreeCtrl* m_pSender;
    ColdFinger* dialog;
};

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/gdicmn.h>
////////////// Timer-Interval Dialog ///////////
class TimerInterval : public wxDialog
{
    WX_DEFINE_ARRAY_INT( unsigned int, ArrayTimerFull );
    WX_DEFINE_ARRAY_INT( unsigned int, ArrayTimerIndividualH );
    WX_DEFINE_ARRAY_INT( unsigned int, ArrayTimerIndividualM );

    ArrayTimerFull oldTimerFull;
    ArrayTimerIndividualH oldTimerIndividualH;
    ArrayTimerIndividualM oldTimerIndividualM;
    wxArrayString oldTimerIndidividualAMPM;

private:
    wxSpinCtrl* m_spinCtrlH;
    wxStaticText* m_staticTextH;
    wxSpinCtrl* m_spinCtrlM;
    wxStaticText* m_staticTextM;
    wxSpinCtrl* m_spinCtrlS;
    wxStaticText* m_staticTextS;

    Options* opt;
    LogbookDialog* dialog;
    bool timerruns;
    int oldFullHour;
    bool ok;
    int oldTimerType;
    wxString oldSpinH;
    wxString oldSpinM;
    wxString oldSpinS;
    int oldHrDelay;

    void init( Options* opt, LogbookDialog* dialog );
    void setCellValue( wxGrid* grid, int row, int col );
    void appendRow( wxGrid* grid, int row, int col );
    void saveData();
    bool loadData();
    void bubbleSort();
    void setTimerFullArray();
    void setTimerIndividualArray();
    void deleteRow();
    void clearAll();

protected:
    wxStaticText* m_staticText120;
    wxStaticLine* m_staticline37;
    wxStaticText* m_staticText125;
    wxSpinCtrl* m_spinCtrl4;
    wxStaticText* m_staticText124;
    wxGrid* m_gridFull;
    wxStaticText* m_staticText1201;
    wxStaticLine* m_staticline38;
    wxRadioButton* m_radioBtnIndividualTimer;
    wxGrid* m_gridIndividual;
    wxStaticText* m_staticText12011;
    wxStaticLine* m_staticline39;
    wxStdDialogButtonSizer* m_sdbSizer9;
    wxButton* m_sdbSizer9OK;
    wxButton* m_sdbSizer9Cancel;
    wxMenu* m_menu11;
    wxMenu* m_menu12;
    wxGrid* selGrid;
    int selRow;

    // Virtual event handlers, overide them in your derived class
    void OnRadioButtonNormal( wxCommandEvent& event );
    void OnRadioButtonFull( wxCommandEvent& event );
    void OnSpinCtrlFullh( wxSpinEvent& event );
    void OnGridCellChangeFull( wxGridEvent& event );
    void OnRadioButtonIndividual( wxCommandEvent& event );
    void OnGridCellChangeIndividual( wxGridEvent& event );
    void OnButtonOKClick( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnGridCellRightClickFull( wxGridEvent& event );
    void OnGridCellRightClickIndividual( wxGridEvent& event );
    void OnMenuDeleteRow( wxCommandEvent& event );
    void OnMenuClearAll( wxCommandEvent& event );


public:
    wxRadioButton* m_radioBtnNormalTimer;
    wxRadioButton* m_radioBtnFullHour;

    TimerInterval( wxWindow* parent, Options* options, wxWindowID id = wxID_ANY, const wxString& title = _( "Set Timer Interval" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 274,452 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
    ~TimerInterval();
};

///////////////////////////////////////////////////////////////////////////////
/// Class PositionDlg
///////////////////////////////////////////////////////////////////////////////
class PositionDlg : public wxDialog
{
private:
    wxTextCtrl* m_textCtrlDeg1;
    wxStaticText* m_staticTextDeg1;
    wxTextCtrl* m_textCtrlmin1;
    wxStaticText* m_staticTextmin1;
    wxTextCtrl* m_textCtrlsec1;
    wxStaticText* m_staticTextsec1;
    wxTextCtrl* m_textCtrlNS;
    wxFlexGridSizer* fgSizer441;
    wxTextCtrl* m_textCtrlDeg2;
    wxStaticText* m_staticTextDeg2;
    wxTextCtrl* m_textCtrlmin2;
    wxStaticText* m_staticTextmin2;
    wxTextCtrl* m_textCtrlsec2;
    wxStaticText* m_staticTextsec2;
    wxTextCtrl* m_textCtrlWE;
    wxChoice* m_choiceFormat;

    void           init( LogbookDialog* dlg );
    void           setFormat( int fmt );
    wxString       replaceComma( wxString s );
    LogbookDialog* dlg;
    int            oldSel;

protected:
    wxStaticText* m_staticTextWE;
    wxStdDialogButtonSizer* m_sdbSizer10;
    wxButton* m_sdbSizer10OK;
    wxButton* m_sdbSizer10Cancel;

    // Virtual event handlers, overide them in your derived class
    void OnCancelButtonClick( wxCommandEvent& event )
    {
        event.Skip();
    }
    void OnOKButtonClick( wxCommandEvent& event );
    void OnChoice( wxCommandEvent& event );


public:

    PositionDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _( "Position" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,127 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
    ~PositionDlg();

    wxString retstr;
};

#ifdef __WXGTK__
class myBitmapButton : public wxBitmapButton
{
public:
    myBitmapButton( wxWindow* Statusbar, wxWindowID id = wxID_ANY, const wxBitmap bm = wxBitmap(), const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize( 10,10 ), long style = wxBU_AUTODRAW, int state = 0 ) : wxBitmapButton( Statusbar, id, bm, pos, size, style )
    {
        this->state = state;
    }
    ~myBitmapButton() {}

    int state;
};
#else
class myBitmapButton : public wxButton
{
public:
    myBitmapButton( wxWindow* Statusbar, wxWindowID id = wxID_ANY, wxString text = wxEmptyString, const wxBitmap bm = wxBitmap(), const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize( -1,20 ), long style = wxBU_AUTODRAW, int state = 0 ) : wxButton( Statusbar, id, text, pos, size, style )
    {
        this->state = state;
        this->SetBitmapLabel( bm );
        this->SetBitmapHover( bm );
        this->SetBitmapPosition( wxRIGHT );
    }
    ~myBitmapButton() {}

    int state;
};
#endif

#endif
