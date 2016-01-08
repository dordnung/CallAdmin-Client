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

#include "opensteam.h"


// Name Timer Class
class NameTimer : public wxTimer {
private:
	int id;
	int attempts;

	// The ID
	CSteamID client;

public:
	// Init Timer
	NameTimer(CSteamID client, int id);
	~NameTimer();

	int GetId() {
		return this->id;
	}

protected:
	virtual void Notify();
};


class TrackerPanel : public wxPanel {
private:
	int currentNameTimerId;
	wxVector<NameTimer *> nameTimers;

	wxListBox *trackerBox;

public:
	TrackerPanel() : currentNameTimerId(0), trackerBox(NULL) {};

	bool InitPanel();

	void AddTracker(wxString text) {
		this->trackerBox->Append(wxString::FromUTF8(text));
	}

	void DeleteTrackers() {
		this->trackerBox->Clear();
	}

	wxVector<NameTimer *>* GetNameTimers() {
		return &nameTimers;
	}

	int GetAndIncraseCurrentNameTimerId() {
		return currentNameTimerId++;
	}

	// Refresh the tracker list
	static void RefreshTrackers(wxString errorStr, wxString result, int extra);

protected:
	void OnUpdate(wxCommandEvent &event);
	void OnCloseWindow(wxCloseEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif