#ifndef ABOUT_PANEL_H
#define ABOUT_PANEL_H

/**
 * -----------------------------------------------------
 * File        about_panel.h
 * Authors     David O., Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2016 David O., Impact
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#pragma once

// Precomp Header
#include <wx/wxprec.h>

// We need WX
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


// About Panel Class
class AboutPanel : public wxPanel {
private:
	wxButton *downloadButton;
	wxStaticText *currentText;
	wxSizer *sizerTop;

public:
	AboutPanel();

	void EnableDownload(bool enable);
	void UpdateVersion(wxString currentBersion, wxColor color);

protected:
	void OnUpdate(wxCommandEvent &event);
	void OnDownload(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
