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


wxDEFINE_EVENT(wxEVT_STEAM_STATUS_CHANGED, wxCommandEvent);

// Button Events for about Panel
BEGIN_EVENT_TABLE(MainPanel, wxPanel)
EVT_BUTTON(wxID_Hide, MainPanel::OnHide)
EVT_BUTTON(wxID_Reconnect, MainPanel::OnReconnect)
EVT_CHECKBOX(wxID_CheckBox, MainPanel::OnCheckBox)

EVT_COMMAND(wxID_ANY, wxEVT_STEAM_STATUS_CHANGED, MainPanel::OnSteamChange)

EVT_LISTBOX_DCLICK(wxID_BoxClick, MainPanel::OnBoxClick)

EVT_CLOSE(MainPanel::OnCloseWindow)
END_EVENT_TABLE()



// Create main Panel
MainPanel::MainPanel() : wxPanel(caGetNotebook(), wxID_ANY) {
	// Border and Center
	wxStaticText* text;

	// whole Body
	this->sizerBody = new wxBoxSizer(wxHORIZONTAL);

	// Box Body
	wxStaticBoxSizer* sizerBox = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Latest Calls")), wxHORIZONTAL);

	// Box for all Calls
	this->callBox = new wxListBox(this, wxID_BoxClick, wxDefaultPosition, wxSize(280, -1), 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	this->callBox->SetFont(wxFont(9, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Body
	sizerBox->Add(callBox, 0, wxEXPAND | wxALL, 5);
	this->sizerBody->Add(sizerBox, 0, wxEXPAND | wxALL, 5);

	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	// Space
	sizerTop->Add(0, 0, 0, wxBOTTOM, 15);

	// Welcome Text
	text = new wxStaticText(this, wxID_ANY, "The Admin Caller");

	text->SetFont(wxFont(30, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxBOTTOM | wxLEFT | wxRIGHT | wxALIGN_CENTER, 40);

	// Space
	sizerTop->Add(0, 0, 0, wxTOP, 20);

	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);

	// Steam Text
	this->steamText = new wxStaticText(this, wxID_ANY, "Steam is currently not running");
	this->steamText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	this->steamText->SetForegroundColour(wxColor("red"));

	// Add it
	sizerTop->Add(this->steamText, 0, wxALL | wxALIGN_CENTER, 10);

	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);

	this->eventText = new wxStaticText(this, wxID_ANY, "Starting CallAdmin Client...");

	this->eventText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	this->eventText->SetForegroundColour(wxColor("blue"));

	sizerTop->Add(this->eventText, 0, wxALL | wxALIGN_CENTER, 10);

	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);

	// Space
	sizerTop->Add(0, 0, 0, wxBOTTOM, 40);

	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const sizerChecks = new wxBoxSizer(wxHORIZONTAL);

	// With tooltip :)
	// ToolTip for first Checkbox
	wxToolTip* tipAvailable = new wxToolTip("You will receive no more calls when you uncheck this.");

	tipAvailable->SetDelay(500);
	tipAvailable->Enable(true);

	// The available Checkbox
	this->available = new wxCheckBox(this, wxID_CheckBox, "I'm available");
	this->available->SetValue(caGetConfig()->GetIsAvailable());
	this->available->SetToolTip(tipAvailable);

	sizerChecks->Add(this->available, 0, wxALL | wxALIGN_CENTER, 5);

	// ToolTip for second Checkbox
	wxToolTip* tipSound = new wxToolTip("You will hear a notification sound on an incoming call.");

	tipSound->SetDelay(500);
	tipSound->Enable(true);

	// The sound Checkbox
	this->sound = new wxCheckBox(this, wxID_CheckBox, "Sound on call");
	this->sound->SetValue(caGetConfig()->GetWantSound());
	this->sound->SetToolTip(tipSound);

	sizerChecks->Add(this->sound, 0, wxALL | wxALIGN_CENTER, 5);

	// ToolTip for third Checkbox
	wxToolTip* specAvailable = new wxToolTip("You will only receive calls but you will not be stored in the trackers database");

	specAvailable->SetDelay(500);
	specAvailable->Enable(true);

	// The store Checkbox
	this->store = new wxCheckBox(this, wxID_CheckBox, "Spectate only");
	this->store->SetValue(caGetConfig()->GetIsSpectator());
	this->store->SetToolTip(specAvailable);

	sizerChecks->Add(this->store, 0, wxALL | wxALIGN_CENTER, 5);

	// Hide, Check
	sizerBtns->Add(new wxButton(this, wxID_Hide, "Hide"), 0, wxTOP | wxLEFT | wxALIGN_CENTER, 5);

	// If max attempts reached, add a reconnect button
	this->reconnectButton = new wxButton(this, wxID_Reconnect, "Reconnect");
	this->reconnectButton->Enable(false);

	sizerBtns->Add(this->reconnectButton, 0, wxTOP | wxRIGHT | wxALIGN_CENTER, 5);

	// Add Checks to Box
	sizerTop->Add(sizerChecks, 0, wxTOP | wxRIGHT | wxALIGN_CENTER_HORIZONTAL, 5);

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_CENTER_HORIZONTAL, 5);

	// Author + Version Text
	text = new wxStaticText(this, wxID_ANY, "v" + (wxString)CALLADMIN_CLIENT_VERSION + "  (c) Popoklopsi and Impact");

	text->SetFont(wxFont(8, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_RIGHT, 10);

	// Add to Body
	sizerBody->Add(sizerTop, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_RIGHT, 10);

	// Auto Size
	SetSizerAndFit(sizerBody, true);
}


// Update Window
void MainPanel::SetEventText(wxString text) {
	this->eventText->SetLabelText(text);
	this->sizerBody->Layout();
	this->eventText->Refresh();

	SetSizerAndFit(this->sizerBody, false);

	caGetNotebook()->Fit();
	caGetMainFrame()->Fit();
}


void MainPanel::SetSteamStatus(wxString text, wxColor color) {
	this->steamText->SetLabelText(text);
	this->steamText->SetForegroundColour(color);
	this->sizerBody->Layout();

	SetSizerAndFit(this->sizerBody, false);

	caGetNotebook()->Fit();
	caGetMainFrame()->Fit();
}


// Update Call List
void MainPanel::UpdateCalls() {
	this->callBox->Clear();

	for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
		CallDialog *currentDialog = *callDialog;
		int item;

		if (currentDialog->IsHandled()) {
			item = this->callBox->Append("F - " + wxString::FromUTF8(currentDialog->GetBoxText()));
		} else {
			item = this->callBox->Append("U - " + wxString::FromUTF8(currentDialog->GetBoxText()));
		}

		this->callBox->SetSelection(item);
	}
}


void MainPanel::SetHandled(int item) {
	this->callBox->SetString(item, "F - " + wxString::FromUTF8(caGetCallDialogs()->at(item)->GetBoxText()));

	caGetCallDialogs()->at(item)->SetFinish();
	caGetCallDialogs()->at(item)->GetTakeoverButton()->Enable(false);
}


// Button Event -> Hide to Taskbar
void MainPanel::OnHide(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Hided Window");

	if (caGetTaskBarIcon()->IsAvailable()) {
		caGetMainFrame()->Show(false);
	} else {
		caGetMainFrame()->Iconize(true);
	}
}


// Check Box Event -> Write To Config
void MainPanel::OnCheckBox(wxCommandEvent& WXUNUSED(event)) {
	caGetConfig()->SetIsAvailable(this->available->IsChecked());
	caGetConfig()->SetWantSound(this->sound->IsChecked());
	caGetConfig()->SetIsSpectator(this->store->IsChecked());
}


// Button Event -> Reconnect
void MainPanel::OnReconnect(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Reconnecting...");

	SetEventText("Trying to reconnect...");
	SetReconnectButton(false);

	caGetMainFrame()->SetTitle("Call Admin Client");
	caGetMainFrame()->Show(true);
	caGetMainFrame()->Restore();

	// Start the Timer again
	caGetApp().StartTimer();
}


// Steam Changed -> Set Text
void MainPanel::OnSteamChange(wxCommandEvent &event) {
	// Get Status
	int id = event.GetInt();

	if (id == 0) {
		SetSteamStatus("Steam support is disabled", wxColour("red"));
	} else if (id == 1) {
		SetSteamStatus("Steam is not running", wxColour("red"));
	} else {
		SetSteamStatus("Steam is running", wxColour(34, 139, 34));
	}
}


// Window Event -> Open Call
void MainPanel::OnBoxClick(wxCommandEvent& WXUNUSED(event)) {
	int selection = this->callBox->GetSelection();

	caGetCallDialogs()->at(selection)->Show(true);
	caGetCallDialogs()->at(selection)->Restore();
}


void MainPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event)) {
	Destroy();
}