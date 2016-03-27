/**
 * -----------------------------------------------------
 * File        main_panel.cpp
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
#include "main_panel.h"
#include "calladmin-client.h"

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Events for main panel
wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
	EVT_BUTTON(XRCID("hide"), MainPanel::OnHide)
	EVT_BUTTON(XRCID("reconnectButton"), MainPanel::OnReconnect)
	EVT_BUTTON(XRCID("exit"), MainPanel::OnExit)

	EVT_CHECKBOX(XRCID("available"), MainPanel::OnCheckBox)
	EVT_CHECKBOX(XRCID("sound"), MainPanel::OnCheckBox)
	EVT_CHECKBOX(XRCID("store"), MainPanel::OnCheckBox)
wxEND_EVENT_TABLE()


// Init. Vars
MainPanel::MainPanel() {
	this->store = NULL;
	this->available = NULL;
	this->sound = NULL;

	this->reconnectButton = NULL;

	this->eventText = NULL;
	this->steamText = NULL;
}


// Load the controls
bool MainPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "mainPanel")) {
		wxMessageBox("Error: Couldn't find XRCID mainPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Steam Text
	FIND_OR_FAIL(this->steamText, XRCCTRL(*this, "steamText", wxStaticText), "steamText");

	// Status Text
	FIND_OR_FAIL(this->eventText, XRCCTRL(*this, "eventText", wxStaticText), "eventText");

	// The available Checkbox
	FIND_OR_FAIL(this->available, XRCCTRL(*this, "available", wxCheckBox), "available");
	this->available->SetValue(caGetConfig()->GetIsAvailable());

	// The sound Checkbox
	FIND_OR_FAIL(this->sound, XRCCTRL(*this, "sound", wxCheckBox), "sound");
	this->sound->SetValue(caGetConfig()->GetWantSound());

	// The store Checkbox
	FIND_OR_FAIL(this->store, XRCCTRL(*this, "store", wxCheckBox), "store");
	this->store->SetValue(caGetConfig()->GetIsSpectator());

	// If max attempts reached, add a reconnect button
	FIND_OR_FAIL(this->reconnectButton, XRCCTRL(*this, "reconnectButton", wxButton), "reconnectButton");

	// Author + Version Text
	wxStaticText *text;

	FIND_OR_FAIL(text, XRCCTRL(*this, "copyrightTextMain", wxStaticText), "copyrightTextMain");
	text->SetLabel("v" + (wxString)CALLADMIN_CLIENT_VERSION + text->GetLabel());

	Layout();
	Fit();

	return true;
}


// Sets the status text
void MainPanel::SetStatusText(wxString text) {
	this->eventText->SetLabelText(text);

	Layout();
}


// Sets the steam text
void MainPanel::SetSteamStatus(wxString text, wxColor color) {
	this->steamText->SetLabelText(text);
	this->steamText->SetForegroundColour(color);

	Layout();
}


// Steam Changed -> Set Text
void MainPanel::OnSteamChange(int status) {
	if (caGetApp().IsRunning()) {
		if (status == 0) {
			SetSteamStatus("Steam support is disabled", wxColour("red"));
		} else if (status == 1) {
			SetSteamStatus("Steam is currently not running", wxColour("red"));
		} else {
			SetSteamStatus("Steam is running", wxColour(34, 139, 34));
		}
	}
}


// Button Event -> Hide to Taskbar
void MainPanel::OnHide(wxCommandEvent &WXUNUSED(event)) {
	caLogAction("Hided main frame through window", LogLevel::LEVEL_DEBUG);

	caGetMainFrame()->Iconize(true);
}


// Button Event -> Reconnect
void MainPanel::OnReconnect(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Trying to reconnect...", LogLevel::LEVEL_INFO);

	SetStatusText("Trying to reconnect");
	SetReconnectButton(false);

	caGetMainFrame()->SetTitle("CallAdmin Client");

	// Start the Timer again
	caGetApp().StartTimer();
}


// Check Box Event -> Write To Config
void MainPanel::OnCheckBox(wxCommandEvent &WXUNUSED(event)) {
	caGetConfig()->SetIsAvailable(this->available->IsChecked());
	caGetConfig()->SetWantSound(this->sound->IsChecked());
	caGetConfig()->SetIsSpectator(this->store->IsChecked());
}


// Button Event -> Exit CallAdmin
void MainPanel::OnExit(wxCommandEvent &WXUNUSED(event)) {
	wxGetApp().ExitProgramm();
}
