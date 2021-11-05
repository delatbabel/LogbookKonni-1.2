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

#ifndef __logbookoptions__
#define __logbookoptions__

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#ifdef __WXOSX__
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/generic/grid.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/scrolwin.h>
#include <wx/dialog.h>
#include <wx/tglbtn.h>
#else
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/generic/grid.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/tglbtn.h>
#endif

#include "LogbookDialog.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LogbookOptions
///////////////////////////////////////////////////////////////////////////////
class Options;
class logbook_pi;

class LogbookOptions : public wxDialog
{
public:
    wxScrolledWindow* m_scrolledWindow1;
    wxNotebook* m_notebook4;
    wxPanel* m_panel15;
    wxStaticText* m_staticText132;
    wxChoice* m_choiceDateFormat;
    wxChoice* m_choiceDate1;
    wxChoice* m_choiceDate2;
    wxChoice* m_choiceDate3;
    wxStaticText* m_staticText128;
    wxTextCtrl* m_textCtrlDateSeparator;
    wxStaticText* m_staticTextDate;
    wxStaticText* m_staticText133;
    wxChoice* m_choiceTimeFormat;
    wxCheckBox* m_checkBoxNoSeconds;
    wxStaticText* m_staticText59;
    wxStaticText* m_staticTextTime;
    wxRadioButton* m_radioBtnUTC;
    wxStaticText* m_staticText71;
    wxStaticLine* m_staticline44;
    wxCheckBox* m_checkBoxShowLogbook;
    wxStaticText* m_staticText76;
    wxStaticText* m_staticText1361;
    wxCheckBox* m_checkBoxShowOnlySelectedLayouts;
    wxStaticText* m_staticText741;
    wxStaticLine* m_staticline25;
    wxStaticLine* m_staticline22;
    wxStaticText* m_staticText123;
    wxStaticText* m_staticText56;
    wxTextCtrl* m_timerText;
    wxStaticText* m_staticText121;
    wxStaticText* m_staticText122;
    wxStaticText* m_staticText1231;
    wxStaticText* m_staticText124;
    wxCheckBox* m_checkBoxPopUp;
    wxCheckBox* m_checkBoxAutoStartTimer;
    wxStaticLine* m_staticline10;
    wxStaticText* m_staticText731;
    wxStaticText* m_staticText751;
    wxButton* m_buttonInstallHTMLFiles;
    wxButton* m_buttonUninstall;
    wxStaticLine* m_staticline261;
    wxPanel* m_panel20;
    wxStaticText* m_staticText1041;
    wxStaticLine* m_staticline34;
    wxStaticText* m_staticText99;
    wxStaticText* m_staticText100;
    wxStaticText* m_staticText102;
    wxStaticLine* m_staticline33;
    wxStaticText* m_staticText96;
    wxStaticText* m_staticText97;
    wxStaticLine* m_staticline32;
    wxStaticText* m_staticText104;
    wxStaticText* m_staticText105;
    wxStaticText* m_staticText107;
    wxPanel* m_panel16;
    wxStaticText* m_staticText34;
    wxStaticText* m_staticText31;
    wxTextCtrl* m_sDeg;
    wxStaticText* m_staticText35;
    wxTextCtrl* m_sMin;
    wxStaticText* m_staticText36;
    wxTextCtrl* m_sSec;
    wxStaticText* m_staticText38;
    wxStaticText* m_staticText39;
    wxTextCtrl* m_sSpeed;
    wxStaticText* m_staticText40;
    wxStaticText* m_staticText41;
    wxTextCtrl* m_smeter;
    wxStaticText* m_staticText63;
    wxStaticText* m_staticText48;
    wxChoice* m_choiceWindSpeed;
    wxTextCtrl* m_sFeet;
    wxStaticText* m_staticText632;
    wxStaticText* m_staticText49;
    wxTextCtrl* m_sFathom;
    wxStaticText* m_staticText42;
    wxStaticText* m_staticText43;
    wxTextCtrl* m_sBaro;
    wxStaticText* m_staticText44;
    wxStaticText* m_staticText791;
    wxStaticText* m_staticText46;
    wxStaticText* m_staticText47;
    wxTextCtrl* m_sLiter;
    wxStaticText* m_staticText50;
    wxTextCtrl* m_sMotorh;
    wxStaticText* m_staticText106;
    wxStaticText* m_staticText135;
    wxStaticText* m_staticText137;
    wxStaticText* m_staticText136;
    wxStaticText* m_staticText70;
    wxStaticText* m_staticText712;
    wxStaticText* m_staticText72;
    wxStaticText* m_staticText73;
    wxStaticLine* m_staticline251;
    wxStaticText* m_staticText75;
    wxStaticLine* m_staticline26;
    wxStaticText* m_staticText51;
    wxChoice* m_choiceWind;
    wxStaticText* m_staticText45;
    wxChoice* m_choiceDir;
    wxPanel* m_panel17;
    wxStaticText* m_staticText761;
    wxBitmapButton* m_bpButtonODT;
    wxStaticText* m_staticText77;
    wxBitmapButton* m_bpButtonDatamanager;
    wxStaticText* m_staticText78;
    wxBitmapButton* m_bpButtonMail;
    wxStaticText* m_staticText79;
    wxButton* m_buttonResetPath;
    wxPanel* m_panel24;
    wxNotebook* m_notebook7;
    wxPanel* m_panel27;
    wxStaticText* m_staticText1381;
    wxCheckBox* m_checkBoxKMLRoute;
    wxCheckBox* m_checkBoxKMLTrack;
    wxStaticText* m_staticText1281;
    wxStaticText* m_staticText131;
    wxStaticText* m_staticText129;
    wxStaticText* m_staticText1291;
    wxPanel* m_panel28;
    wxStaticText* m_staticTextUseRPM;
    wxStaticText* m_staticTextRPMSentence;
    wxToggleButton* m_toggleBtnRPMCheck;
    wxCheckBox* m_checkBoxEng1RPM;
    wxTextCtrl* m_textCtrlEngine1;
    wxCheckBox* m_checkBoxEng2RPM;
    wxTextCtrl* m_textCtrlEngine2;
    wxCheckBox* m_checkBoxGenRPM;
    wxTextCtrl* m_textCtrlGenerator;
    wxButton* m_buttonToSailsSpace;
    wxStaticText* m_staticText1351;
    wxButton* m_buttonBackToSails;
    wxButton* m_button60;
    wxButton* m_button61;
    wxButton* m_button62;
    wxButton* m_button63;
    wxButton* m_buttonSpaceRPlus;
    wxButton* m_buttonSpaceRMinus;
    wxButton* m_buttonSpaceCPlus;
    wxButton* m_buttonSpaceCMinus;
    wxButton* m_buttonSpaceReset;
    wxStdDialogButtonSizer* m_sdbSizer1;
    wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;
    wxCheckBox* m_checkBoxSailsDown;
    wxCheckBox* m_checkBoxStatusBar;
    wxCheckBox* m_checkBoxWindspeeds;
    wxCheckBox* m_checkBoxOverview;

    wxDateTime textCtrlDate;

    // Virtual event handlers, overide them in your derived class
    virtual void OnClose( wxCloseEvent& event );
    virtual void OnChoiceDateFormat( wxCommandEvent& event );
    virtual void OnChoiceDate1( wxCommandEvent& event );
    virtual void OnChoiceDate2( wxCommandEvent& event );
    virtual void OnChoiceDate3( wxCommandEvent& event );
    virtual void OnTextDateSeparator( wxCommandEvent& event );
    virtual void OnChoiceTimeFormat( wxCommandEvent& event );
    virtual void OnCheckboxNoSeconds( wxCommandEvent& event );
    virtual void onRadioBtnUTC( wxCommandEvent& event );
    virtual void onRadioBtnLocal( wxCommandEvent& event );
    virtual void onRadioBtnGPSAuto( wxCommandEvent& event );
    virtual void onChoicePositionFormat( wxCommandEvent& event );
    virtual void onCheckBoxToolTips( wxCommandEvent& event );
    virtual void OnChoiceNoEngines( wxCommandEvent& event );
    virtual void OnCheckBoxEngineMessage( wxCommandEvent& event );
    virtual void OnCheckBoxGeneratorMessage( wxCommandEvent& event );
    virtual void OnCheckBoxEngineRunning( wxCommandEvent& event );
    virtual void OnCheckBoxOverview( wxCommandEvent& event );
    virtual void onCheckBoNoGPS( wxCommandEvent& event );
    virtual void onButtonClickInstallHTMLFiles( wxCommandEvent& event );
    virtual void OnButtonClickUninstall( wxCommandEvent& event );
    virtual void OnTextEnterWaterTank( wxCommandEvent& event );
    virtual void OnTextEnterm_textCtrlWatermaker( wxCommandEvent& event );
    virtual void OnTextEnterFuelTank( wxCommandEvent& event );
    virtual void OnTextEnterBank1( wxCommandEvent& event );
    virtual void onTextEnterBank2( wxCommandEvent& event );
    virtual void onTextm_sLiter( wxCommandEvent& event );
    virtual void onButtonClickODT( wxCommandEvent& event );
    virtual void onButtonClickDataManager( wxCommandEvent& event );
    virtual void onButtonClickMail( wxCommandEvent& event );
    virtual void onButtonHTMLEditor( wxCommandEvent& event );
    virtual void OnButtonResetPaths( wxCommandEvent& event );
    virtual void OnCheckBoxEng1RPM( wxCommandEvent& event );
    virtual void OnCheckBoxEng2RPM( wxCommandEvent& event );
    virtual void OnCheckBoxGenRPM( wxCommandEvent& event );
    virtual void OnToggleButtonRPMCheck( wxCommandEvent& event );
    virtual void OnButtonToSailsSpace( wxCommandEvent& event );
    virtual void onButtonBackToSails( wxCommandEvent& event );
    virtual void OnButtonSpaceRPlus( wxCommandEvent& event );
    virtual void OnButtonSpaceRMinus( wxCommandEvent& event );
    virtual void OnButtonSpaceCPlus( wxCommandEvent& event );
    virtual void OnButtonSpaceCMinus( wxCommandEvent& event );
    virtual void OnButtonSpaceReset( wxCommandEvent& event );
    virtual void OnCancel( wxCommandEvent& event );
    virtual void OnButtonOKClick( wxCommandEvent& event );
    virtual void OnTextEng1IdEntry( wxCommandEvent& event );
    virtual void OnTextEng2IdEntry( wxCommandEvent& event );
    virtual void OnTextGenIdEntry( wxCommandEvent& event );

public:
    wxRadioButton* m_radioBtnLocal;
    wxChoice* m_choiceTzIndicator;
    wxChoice* m_choiceTzHours;
    wxRadioButton* m_radioBtnGPSAuto;
    wxChoice* m_choicePositionFormat;
    wxCheckBox* m_checkBoxToolTips;
    wxChoice* m_choiceNoEngines;
    wxCheckBox* m_checkBoxShowAllLayouts;
    wxTextCtrl* m_textCtrlLayoutPrefix;
    wxCheckBox* m_checkBoxNoGPS;
    wxChoice* m_choiceWindTo;
    wxStaticText* m_staticText54;
    wxTextCtrl* m_textCtrlWayPoint;
    wxTextCtrl* m_textCtrlGuradChanged;
    wxTextCtrl* m_textCtrlEverySMText;
    wxTextCtrl* m_textCtrlChancedCourse;
    wxTextCtrl* m_textCtrlTankWater;
    wxTextCtrl* m_textCtrlWatermaker;
    wxTextCtrl* m_textCtrlTankFuel;
    wxTextCtrl* m_textCtrlBank1;
    wxTextCtrl* m_textCtrlBank2;
	wxChoice* m_choiceDistance;
	wxChoice* m_choiceBoatSpeed;
    wxChoice* m_choiceDepth;
    wxChoice* m_choiceWaveSwell;
    wxChoice* m_choiceHeading;
    wxTextCtrl* m_textCtrlTemperature;
    wxTextCtrl* m_textCtrlAmpere;
    wxTextCtrl* m_textCtrlEngine;
    wxTextCtrl* m_textCtrlPropShaft;
    wxTextCtrl* m_textCtrRPM;
    wxTextCtrl* m_Days;
    wxTextCtrl* m_Weeks;
    wxTextCtrl* m_textMonth;
    wxTextCtrl* m_textCtrlODTEditor;
    wxTextCtrl* m_textCtrlDataManager;
    wxTextCtrl* m_textCtrlMailClient;
    wxTextCtrl* m_textCtrlHTMLEditorPath;
    wxBitmapButton* m_bpButtonHTMLEditor;
    wxTextCtrl* m_textCtrlKMLLineWidt;
    wxChoice* m_choiceKMLLineTranparency;
    wxChoice* m_choiceKMLRouteLineColo;
    wxChoice* m_choiceKMLTrackLineColor;
    wxCheckBox* m_checkBoxNMEAUseRPM;
    wxPanel* m_panelSails;
    wxPanel* m_panelSailNames;
    wxGrid* m_gridSailNames;
    wxPanel* m_panelSailsCheckbox;
    wxFlexGridSizer* fgSizerSailsCheckboxes;
    wxCheckBox* m_checkBoxEngine;
    wxCheckBox* m_checkBoxEngineRunning;
    wxCheckBox* m_checkBoxGenerator;

#ifdef __WXOSX__
    LogbookOptions( wxWindow* parent, Options* opt, logbookkonni_pi* log_pi, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 680,700 ), long style = wxDEFAULT_DIALOG_STYLE|wxVSCROLL );
#else
    LogbookOptions( wxWindow* parent, Options* opt, logbookkonni_pi* log_pi, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 654,422 ), long style = wxDEFAULT_DIALOG_STYLE|wxVSCROLL );
#endif
    ~LogbookOptions();

    void getValues();
    void setTimeFormat( int i );
    void setDateFormat();
    void setRPMSentence( wxString sentence );

private:
    Options			 *opt;
    logbookkonni_pi* log_pi;
    wxDateTime		 sample;
    wxString		 path;
    bool			 modified;
    wxString		oldPattern;
    wxString		oldDateFormat;
    int				iOldDateFormat;
    int				ioldDate1;
    int				ioldDate2;
    int				ioldDate3;
    int				ioldTimeFormat;
    bool			boldNoSeconds;

    wxString choicesWind[3];

    void init();
    void setValues();
    void updateWindChoice();
    void updateChoiceBoxes();
    void setDateEnabled( int i );
    void resetToOldDateTimeFormat();
    void setUseRPM();
};
#endif
