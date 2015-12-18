#ifndef TRACKERS_PANEL_H
#define TRACKERS_PANEL_H

/**
 * -----------------------------------------------------
 * File        trackers_panel.h
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


// Tracker Panel Class
class TrackerPanel : public wxPanel {
private:
	wxListBox *trackerBox;

public:
	TrackerPanel();

	void AddTracker(wxString text);
	void DeleteTrackers();

	// Refresh the tracker list
	static void RefreshTrackers(char *error, wxString result, int extra);

protected:
	void OnExit(wxCommandEvent &event);
	void OnUpdate(wxCommandEvent &event);
	void OnHide(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};


#endif