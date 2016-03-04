#ifndef TRACKER_PANEL_H
#define TRACKER_PANEL_H

/**
 * -----------------------------------------------------
 * File        tracker_panel.h
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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include "opensteam.h"


class NameTimer;

class TrackerPanel : public wxScrolledWindow {
private:
	// The running NameTimers
	wxVector<NameTimer *> nameTimers;

	// All current trackers
	wxVector<wxString> currentTrackers;

	wxListCtrl *trackerBox;

public:
	TrackerPanel() : trackerBox(NULL) {};
	~TrackerPanel();

	// Init Panel with controls
	bool InitPanel();

	// Update the tracker list
	void UpdateTrackerList();

	// Refresh the tracker list
	static void RefreshTrackers(wxString errorStr, wxString result, int extra);

	void AddTracker(wxString steamId, wxString name = "", bool isFriend = false, bool isOnline = false);

	wxVector<NameTimer *> *GetNameTimers() {
		return &nameTimers;
	}

	wxVector<wxString> *GetCurrentTrackers() {
		return &currentTrackers;
	}

protected:
	void OnUpdate(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};


// Name Timer to load names from Steam Lib
class NameTimer : public wxTimer {
private:
	int attempts;

	// The SteamId of the name to load
	CSteamID client;

public:
	// Init Timer
	explicit NameTimer(CSteamID client);
	~NameTimer();

protected:
	// Timer executed
	virtual void Notify();
};

#endif
