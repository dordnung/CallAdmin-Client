#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

/**
 * -----------------------------------------------------
 * File        main_panel.h
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

#include <wx/listbox.h>
#include <wx/statline.h>


wxDECLARE_EVENT(wxEVT_STEAM_STATUS_CHANGED, wxCommandEvent);


// Main Panel Class
class MainPanel : public wxPanel {
private:
	wxCheckBox *store;
	wxCheckBox *available;
	wxCheckBox *sound;

	wxButton *reconnectButton;

	wxListBox *callBox;
	wxSizer *sizerBody;

	wxStaticText *eventText;
	wxStaticText *steamText;

	void SetSteamStatus(wxString text, wxColor color);

public:
	MainPanel();

	bool InitPanel();

	// Sets the event text
	void SetEventText(wxString text);

	void SetReconnectButton(bool enable = false) {
		this->reconnectButton->Enable(enable);
	}

	// Update Call list
	void UpdateCalls();

	// TODO: Nicht int sondern iterator item
	void SetHandled(int item);

protected:
	// Events
	void OnCloseWindow(wxCloseEvent &event);

	void OnHide(wxCommandEvent &event);
	void OnReconnect(wxCommandEvent &event);

	void OnBoxClick(wxCommandEvent &event);

	void OnCheckBox(wxCommandEvent &event);
	void OnSteamChange(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif
