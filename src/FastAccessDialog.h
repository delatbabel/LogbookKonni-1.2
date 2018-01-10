#ifndef _FASTACCESSDIALOG_H_INCLUDED_
#define _FASTACCESSDIALOG_H_INCLUDED_

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "LogbookDialog.h"

#include <wx/string.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>

//////////////////////////////////////////////////////////////////////////////
/// Class FastAccessDialog
///////////////////////////////////////////////////////////////////////////////
class FastAccessDialog : public wxDialog
{
	private:
		wxGridSizer* container;
		static LogbookDialog* logbookDialog;

	protected:

		// Virtual event handlers, overide them in your derived class
		virtual void FastAccessDialogOnInitDialog( wxInitDialogEvent& event ) { event.Skip(); }
		void buttonCallbackHub( wxCommandEvent& event );


	public:

		FastAccessDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~FastAccessDialog();

		void AddButton(const wxString& title, bool toggleButton, void (*callback)(wxAnyButton* button, LogbookDialog* logbookDialog), bool closeAfterCallback = false);
		void AddCancelButton(const wxString& title, wxToggleButton* button, void (*callback)(wxAnyButton* button, LogbookDialog* logbookDialog));

};
LogbookDialog* FastAccessDialog::logbookDialog = 0;

#endif
