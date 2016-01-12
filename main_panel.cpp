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


// Button Events for about Panel
wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
	EVT_BUTTON(XRCID("hide"), MainPanel::OnHide)
	EVT_BUTTON(XRCID("reconnectButton"), MainPanel::OnReconnect)

	EVT_CHECKBOX(XRCID("available"), MainPanel::OnCheckBox)
	EVT_CHECKBOX(XRCID("sound"), MainPanel::OnCheckBox)
	EVT_CHECKBOX(XRCID("store"), MainPanel::OnCheckBox)

	EVT_LISTBOX_DCLICK(XRCID("callBox"), MainPanel::OnBoxClick)
wxEND_EVENT_TABLE()


// Init. Vars
MainPanel::MainPanel() {
	this->store = NULL;
	this->available = NULL;
	this->sound = NULL;

	this->reconnectButton = NULL;

	this->callBox = NULL;
	this->sizerBody = NULL;

	this->eventText = NULL;
	this->steamText = NULL;
}


// Load the controls
bool MainPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "mainPanel")) {
		wxMessageBox("Error: Couldn't find XRCID mainPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	FIND_OR_FAIL(this->sizerBody, this->GetSizer(), "mainPanelSizerBody");

	// Box for all Calls
	FIND_OR_FAIL(this->callBox, XRCCTRL(*this, "callBox", wxListBox), "callBox");

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

	FIND_OR_FAIL(text, XRCCTRL(*this, "copyrightText", wxStaticText), "copyrightText");
	text->SetLabel("v" + (wxString)CALLADMIN_CLIENT_VERSION + text->GetLabel());

	// Auto Size
	this->sizerBody->Layout();
	this->sizerBody->Fit(this);

	return true;
}


// Update Window
void MainPanel::SetStatusText(wxString text) {
	this->eventText->SetLabelText(text);

	// TODO: Benötigt?
	//this->eventText->Refresh();
	this->sizerBody->Layout();
	this->sizerBody->Fit(this);

	// TODO: Benötigt?
	//caGetNotebook()->GetWindow()->Fit();
	//caGetMainFrame()->Fit();
}


void MainPanel::SetSteamStatus(wxString text, wxColor color) {
	this->steamText->SetLabelText(text);
	this->steamText->SetForegroundColour(color);

	this->sizerBody->Layout();
	this->sizerBody->Fit(this);

	// TODO: Benötigt?
	//caGetNotebook()->GetWindow()->Fit();
	//caGetMainFrame()->Fit();
}


// Update Call List
void MainPanel::UpdateCalls() {
	int item = -1;

	// First clear calls
	this->callBox->Clear();

	for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
		CallDialog *currentDialog = *callDialog;

		if (currentDialog->IsHandled()) {
			item = this->callBox->Append("F - " + wxString::FromUTF8(currentDialog->GetBoxText()));
		} else {
			item = this->callBox->Append("U - " + wxString::FromUTF8(currentDialog->GetBoxText()));
		}
	}

	if (item > -1) {
		// Select newest call
		this->callBox->SetSelection(item);
	}
}


// The call is now handled
void MainPanel::SetHandled(int item) {
	this->callBox->SetString(item, "F - " + wxString::FromUTF8(caGetCallDialogs()->at(item)->GetBoxText()));

	caGetCallDialogs()->at(item)->SetFinish();
	caGetCallDialogs()->at(item)->GetTakeoverButton()->Enable(false);
}


// Button Event -> Hide to Taskbar
void MainPanel::OnHide(wxCommandEvent &WXUNUSED(event)) {
	caLogAction("Hided main frame through window", LogLevel::LEVEL_DEBUG);

	if (caGetConfig()->GetHideOnMinimize()) {
		if (caGetTaskBarIcon()->IsAvailable()) {
			// Log Action
			caLogAction("Hided main frame to taskbar", LogLevel::LEVEL_DEBUG);

			caGetMainFrame()->Show(false);
		} else {
			caLogAction("Hided main frame (Taskbar not available)", LogLevel::LEVEL_DEBUG);

			caGetMainFrame()->Iconize(true);
		}
	}
}


// Check Box Event -> Write To Config
void MainPanel::OnCheckBox(wxCommandEvent &WXUNUSED(event)) {
	caGetConfig()->SetIsAvailable(this->available->IsChecked());
	caGetConfig()->SetWantSound(this->sound->IsChecked());
	caGetConfig()->SetIsSpectator(this->store->IsChecked());
}


// Button Event -> Reconnect
void MainPanel::OnReconnect(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Trying to reconnect...");

	SetStatusText("Trying to reconnect...");
	SetReconnectButton(false);

	caGetMainFrame()->SetTitle("CallAdmin Client");

	// Start the Timer again
	caGetApp().StartTimer();
}


// Steam Changed -> Set Text
void MainPanel::OnSteamChange(int status) {
	if (status == 0) {
		SetSteamStatus("Steam support is disabled", wxColour("red"));
	} else if (status == 1) {
		SetSteamStatus("Steam is currently not running", wxColour("red"));
	} else {
		SetSteamStatus("Steam is running", wxColour(34, 139, 34));
	}
}


// Window Event -> Open Call
void MainPanel::OnBoxClick(wxCommandEvent &WXUNUSED(event)) {
	int selection = this->callBox->GetSelection();

	caGetCallDialogs()->at(selection)->Show(true);
}
