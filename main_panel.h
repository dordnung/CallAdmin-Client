#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

/**
 * -----------------------------------------------------
 * File        main_panel.h
 * Authors     David Ordnung, Impact
 * License     GPLv3
 * Web         http://dordnung.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung, Impact
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
#include "panel.h"

#define MAIN_PANEL_PAGE 0
#define MAIN_PANEL_LABEL "Main"


enum MainPanelColumns {
	MainPanelColumn_Time,
	MainPanelColumn_Server,
	MainPanelColumn_Finished
};

// Main Panel Class
class MainPanel : public Panel, public wxPanel {
private:
	wxListCtrl *callBox;

	wxCheckBox *store;
	wxCheckBox *available;
	wxCheckBox *sound;

	wxButton *reconnectButton;

	wxStaticText *eventText;
	wxStaticText *steamText;

public:
	MainPanel();

	// Load the controls
	bool InitPanel();

	int GetPage() {
		return MAIN_PANEL_PAGE;
	}

	wxString GetLabel() {
		return MAIN_PANEL_LABEL;
	}

	wxPanel *GetPanel() {
		return this;
	}

	// Update Call list
	void UpdateCalls();

	// Append call to the callbox
	void AppendCall(bool finished, wxString time, wxString server);

	void SetHandled(int item);

	// Set the event text
	void SetStatusText(wxString text);
	void SetSteamStatus(wxString text, wxColor color);

	void OnSteamChange(int status);

	void SetReconnectButton(bool enable = false) {
		this->reconnectButton->Enable(enable);
	}

protected:
	// Events
	void OnBoxClick(wxListEvent &event);
	void OnReconnect(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnCheckBox(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif
