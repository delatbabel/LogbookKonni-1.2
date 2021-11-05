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

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

#include <wx/tokenzr.h>

#include "EzGrid.h"
#include "FastComboEditor.h"


wxSComboBox::wxSComboBox(wxWindow* parent, wxWindowID id, const wxString& value,
                         const wxPoint& pos, const wxSize& size, int n,
                         const wxString choices[], long style,
                         const wxValidator& validator, const wxString& name) {
  Create(parent, id, value, pos, size, n, choices, style, validator, name);
}

wxSComboBox::wxSComboBox() : m_pGrid(NULL) {}

BEGIN_EVENT_TABLE(wxSComboBox, wxComboBox)
EVT_COMBOBOX(-1, wxSComboBox::OnChange)
END_EVENT_TABLE()

void wxSComboBox::OnChange(wxCommandEvent& event) {
  if (m_pGrid) {
    //		m_pGrid->GetTable()->SetValue(m_nRow, m_nCol,
    //event.GetString());
    m_pGrid->SetCellValue(m_nRow, m_nCol, event.GetString());

    wxGridEvent gridEvt(m_pGrid->GetId(), wxEVT_GRID_CELL_CHANGE, m_pGrid,
                        m_nRow, m_nCol);
    gridEvt.SetString(event.GetString());
    GetEventHandler()->ProcessEvent(gridEvt);
  }
  event.Skip();
}

wxFastComboEditor::wxFastComboEditor(size_t count, const wxString choices[],
                                     bool allowOthers)
    : m_allowOthers(allowOthers) {
  SetClientData((void*)&m_pointActivate);
  if (count) {
    m_choices.Alloc(count);
    for (size_t n = 0; n < count; n++) {
      m_choices.Add(choices[n]);
    }
  }
}

wxString wxFastComboEditor::GetValue() const { return Combo()->GetValue(); }

wxGridCellEditor* wxFastComboEditor::Clone() const {
  wxFastComboEditor* editor = new wxFastComboEditor;
  editor->m_allowOthers = m_allowOthers;
  editor->m_choices = m_choices;
  return editor;
}

void wxFastComboEditor::Create(wxWindow* parent, wxWindowID id,
                               wxEvtHandler* evtHandler) {
  size_t count = m_choices.GetCount();
  wxString* choices = new wxString[count];
  for (size_t n = 0; n < count; n++) {
    choices[n] = m_choices[n];
  }
  m_control = new wxSComboBox(parent, id, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, count, choices,
                              m_allowOthers ? 0 : wxCB_READONLY);
  delete[] choices;
  wxGridCellEditor::Create(parent, id, evtHandler);
}

void wxFastComboEditor::PaintBackground(const wxRect& rectCell,
                                        wxGridCellAttr* attr) {
  wxGridCellEditor::PaintBackground(rectCell, attr);
}

void wxFastComboEditor::BeginEdit(int row, int col, wxGrid* grid) {
  wxASSERT_MSG(m_control, "The wxGridCellEditor must be Created first!");

  EzGrid* pEzGrid = (EzGrid*)grid;
  pEzGrid->RevertSel();

  m_startValue = grid->GetCellValue(row, col);

  if (m_allowOthers)
    Combo()->SetValue(m_startValue);
  else {
    // find the right position, or default to the first if not found
    int pos = Combo()->FindString(m_startValue);
    if (pos == -1) pos = 0;
    Combo()->SetSelection(pos);
  }
  Combo()->SetInsertionPointEnd();
  Combo()->SetFocus();
  Combo()->SetCellData(row, col, grid);
  if (m_pointActivate.x > -1 && m_pointActivate.y > -1) {
    m_pointActivate = Combo()->ScreenToClient(m_pointActivate);
#ifdef __WXMSW__
    SendMessage((HWND)Combo()->GetHandle(), WM_LBUTTONDOWN, 0,
                MAKELPARAM(m_pointActivate.x, m_pointActivate.y));
    SendMessage((HWND)Combo()->GetHandle(), WM_LBUTTONUP, 0,
                MAKELPARAM(m_pointActivate.x, m_pointActivate.y));
#else
    /*		GtkCombo *combo = GTK_COMBO(Combo()->m_widget);
                    combo->current_button = 0;
                    GtkWidget *button = GTK_COMBO(Combo()->m_widget)->button;
                    GdkEventButton eventb;
                    memset(&event, 0, sizeof(event));
                    gdk_window_ref (button->window);
                    // to do: only call when the click point is on the button
    //		event.x = m_pointActivate.x;
    //		event.y = m_pointActivate.y;
                    eventb.x = 0;
                    eventb.y = 0;
                    event.deviceid = GDK_CORE_POINTER;
                    event.type = GDK_BUTTON_PRESS;
                    event.source = GDK_SOURCE_MOUSE;
                    event.time = 0;
                    event.window = button->window;
                    event.send_event = TRUE;
                    event.button = 1;
                    gtk_widget_event (button, (GdkEvent *)&event);
                    gdk_window_unref (button->window);
                    while (gtk_events_pending())
                            gtk_main_iteration();
            */
#endif
  }
  //	Combo()->SetSelection(0);
}

bool wxFastComboEditor::EndEdit(int row, int col, wxGrid* grid) {
  wxString value = Combo()->GetValue();
  bool changed = value != grid->GetCellValue(row, col);

  if (changed) grid->SetCellValue(row, col, value);

  /*	m_startValue = wxEmptyString;
      if (m_allowOthers)
              Combo()->SetValue(m_startValue);
      else
              Combo()->SetSelection(0);
  */
  return changed;
}

void wxFastComboEditor::Reset() {
  Combo()->SetValue(m_startValue);
  Combo()->SetInsertionPointEnd();
}

void wxFastComboEditor::SetParameters(const wxString& params) {
  if (!params) {
    return;
  }
  m_choices.Empty();
  wxStringTokenizer tk(params, ',');
  while (tk.HasMoreTokens()) {
    m_choices.Add(tk.GetNextToken());
  }
}

void wxFastComboEditor::SetParameters(size_t count, const wxString choices[]) {
  m_choices.Empty();
  if (count) {
    m_choices.Alloc(count);
    for (size_t n = 0; n < count; n++) {
      m_choices.Add(choices[n]);
    }
  }
}
