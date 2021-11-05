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

#include "GridCellChoiceRenderer.h"

void wxGridCellChoiceRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr,
                                    wxDC& dc, const wxRect& rectCell, int row,
                                    int col, bool isSelected) {
  wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);
  // first calculate button size
  // don't draw outside the cell
  int nButtonWidth = 17;
  if (rectCell.height < 2) return;
  wxRect rectButton;
  rectButton.x = rectCell.x + rectCell.width - nButtonWidth;
  rectButton.y = rectCell.y + 1;
  int cell_rows, cell_cols;
  attr.GetSize(&cell_rows, &cell_cols);
  rectButton.width = nButtonWidth;
  if (cell_rows == 1)
    rectButton.height = rectCell.height - 2;
  else
    rectButton.height = nButtonWidth;

  SetTextColoursAndFont(grid, attr, dc, isSelected);
  int hAlign, vAlign;
  attr.GetAlignment(&hAlign, &vAlign);
  // leave room for button
  wxRect rect = rectCell;
  rect.SetWidth(rectCell.GetWidth() - rectButton.GetWidth() - 2);
  rect.Inflate(-1);
  grid.DrawTextRectangle(dc, grid.GetCellValue(row, col), rect, hAlign, vAlign);

  // don't bother drawing if the cell is too small
  if (rectButton.height < 4 || rectButton.width < 4) return;
  // draw 3-d button
  wxColour colourBackGround = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
  dc.SetBrush(wxBrush(colourBackGround, wxSOLID));
  dc.SetPen(wxPen(colourBackGround, 1, wxSOLID));
  dc.DrawRectangle(rectButton);
  dc.SetPen(
      wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
  dc.DrawLine(rectButton.GetLeft(), rectButton.GetBottom(),
              rectButton.GetRight(), rectButton.GetBottom());
  dc.DrawLine(rectButton.GetRight(), rectButton.GetBottom(),
              rectButton.GetRight(), rectButton.GetTop() - 1);
  dc.SetPen(
      wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 1, wxSOLID));
  dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetBottom() - 1,
              rectButton.GetRight() - 1, rectButton.GetBottom() - 1);
  dc.DrawLine(rectButton.GetRight() - 1, rectButton.GetBottom() - 1,
              rectButton.GetRight() - 1, rectButton.GetTop());
  dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT), 1,
                  wxSOLID));
  dc.DrawLine(rectButton.GetRight() - 2, rectButton.GetTop() + 1,
              rectButton.GetLeft() + 1, rectButton.GetTop() + 1);
  dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetTop() + 1,
              rectButton.GetLeft() + 1, rectButton.GetBottom() - 1);
  // Draw little triangle
  int nTriWidth = 7;
  int nTriHeight = 4;
  wxPoint point[3];
  point[0] =
      wxPoint(rectButton.GetLeft() + (rectButton.GetWidth() - nTriWidth) / 2,
              rectButton.GetTop() + (rectButton.GetHeight() - nTriHeight) / 2);
  point[1] = wxPoint(point[0].x + nTriWidth - 1, point[0].y);
  point[2] = wxPoint(point[0].x + 3, point[0].y + nTriHeight - 1);
  dc.SetBrush(
      wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), wxSOLID));
  dc.SetPen(
      wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
  dc.DrawPolygon(3, point);
  if (m_border == wxLAYOUT_TOP) {
    dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
    dc.DrawLine(rectCell.GetRight(), rectCell.GetTop(), rectCell.GetLeft(),
                rectCell.GetTop());
  }
}
