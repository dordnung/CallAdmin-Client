/**
 * -----------------------------------------------------
 * File        config_panel.cpp
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

// Include Project
#include "config_panel.h"
#include "calladmin-client.h"

// Wx
#include <wx/gbsizer.h>


// Button ID's for Config Panel
enum {
	wxID_SetConfig = wxID_HIGHEST + 30,
	wxID_SteamUpdate,
	wxID_HideUpdate,
};


// Button Events for Config Panel
BEGIN_EVENT_TABLE(ConfigPanel, wxPanel)
EVT_BUTTON(wxID_SetConfig, ConfigPanel::OnSet)

EVT_CHECKBOX(wxID_SteamUpdate, ConfigPanel::OnCheckBox)
EVT_CHECKBOX(wxID_HideUpdate, ConfigPanel::OnCheckBox2)
END_EVENT_TABLE()


// Create Config Panel
ConfigPanel::ConfigPanel() : wxPanel(caGetNotebook(), wxID_ANY) {
	// Current Position
	int currentPos = 1;

	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	// Grid
	wxGridBagSizer *gridSizer = new wxGridBagSizer(10, 2);
	gridSizer->AddGrowableCol(0);


	// Flags and Text
	wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
	flags.Border(wxALL &~wxBOTTOM, 10);
	flags.Centre();


	// Add Grid
	sizerTop->Add(gridSizer, 1, wxALL | wxEXPAND, 10);

	// Ask for Page
	text = new wxStaticText(this, wxID_ANY, "Your url to the call admin directory: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	pageText = new wxTextCtrl(this, wxID_ANY, "http://yourpage.com/calladmin/", wxDefaultPosition, wxSize(300, -1));
	pageText->SetMaxLength(256);
	pageText->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(pageText, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);

	// Ask for Key
	text = new wxStaticText(this, wxID_ANY, "Your private access key: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	keyText = new wxTextCtrl(this, wxID_ANY, "accesskey");
	keyText->SetMaxLength(64);
	keyText->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	// Add to Grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(keyText, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);


	// Static line
	gridSizer->Add(new wxStaticLine(this, wxID_ANY), wxGBPosition(currentPos++, 0), wxGBSpan(1, 2), wxEXPAND | (wxALL &~wxLEFT &~wxRIGHT), 10);


	// Ask for Step
	text = new wxStaticText(this, wxID_ANY, "Time intervall to search for calls (in seconds): ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	stepSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 5, 20, 10, "Time Inverall");
	stepSlider->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(stepSlider, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);


	// Ask for Timeout
	text = new wxStaticText(this, wxID_ANY, "Timeout for connection (in seconds): ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	timeoutSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 3, 10, 5, "Timeout");
	timeoutSlider->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(timeoutSlider, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);


	// Ask for Attempts
	text = new wxStaticText(this, wxID_ANY, "Maximum auto. reconnect attempts: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	attemptsSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 3, 10, 3, "Max Attempts");
	attemptsSlider->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(attemptsSlider, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);


	// Ask for Last Calls
	text = new wxStaticText(this, wxID_ANY, "Number of last calls to load at start: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	callsSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 0, 50, 25, "Load Calls");
	callsSlider->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Grid
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(callsSlider, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxEXPAND);


	// Static line
	gridSizer->Add(new wxStaticLine(this, wxID_ANY), wxGBPosition(currentPos++, 0), wxGBSpan(1, 2), wxEXPAND | (wxALL &~wxLEFT &~wxRIGHT), 10);

	// Ask for Steam
	text = new wxStaticText(this, wxID_ANY, "Steam support to write messages and load Avatars: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Steam support
	steamEnable = new wxCheckBox(this, wxID_SteamUpdate, "Enable Steam Support");
	steamEnable->SetValue(true);

	// Add Steam
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(steamEnable, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxRIGHT);

	// Ask for hide on mini
	text = new wxStaticText(this, wxID_ANY, "Hide windows on minimize: ");
	text->SetFont(wxFont(11, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add
	hideMini = new wxCheckBox(this, wxID_HideUpdate, "Hide on Minimize");
	hideMini->SetValue(false);


	// Add Hide on minimize
	gridSizer->Add(text, wxGBPosition(currentPos, 0), wxDefaultSpan, 0, 10);
	gridSizer->Add(hideMini, wxGBPosition(currentPos++, 1), wxDefaultSpan, wxRIGHT);


	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_SetConfig, "Update Settings"), flags.Border(wxALL, 5));

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Auto Size
	SetSizerAndFit(sizerTop, true);
}


// Steam Updated -> Set Config
void ConfigPanel::OnCheckBox(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetSteamEnabled(this->steamEnable->GetValue());

	// Log Action
	caLogAction("Changed Steam Status");
}


// Hide On Minimize Updated -> Set Config
void ConfigPanel::OnCheckBox2(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetHideOnMinimize(this->hideMini->GetValue());

	// Log Action
	caLogAction("Changed Hide on Minimize Status");
}


// Button Event -> Try to set new config
void ConfigPanel::OnSet(wxCommandEvent& WXUNUSED(event)) {
	// Write to new config file
	caGetConfig()->Write("step", this->stepSlider->GetValue());
	caGetConfig()->Write("timeout", this->timeoutSlider->GetValue());
	caGetConfig()->Write("attempts", this->attemptsSlider->GetValue());
	caGetConfig()->Write("lastcalls", this->callsSlider->GetValue());
	caGetConfig()->Write("page", this->pageText->GetValue());
	caGetConfig()->Write("key", this->keyText->GetValue());

	// Refresh main dialog
	caGetMainFrame()->SetTitle("Call Admin Client");
	caGetMainFrame()->GetNotebook()->GetMainPanel()->SetEventText("Enable new Settings...");

	// Log Action
	caLogAction("Saved the config");

	// Goto Main
	caGetNotebook()->SetSelection(0);

	// Parse again
	ParseConfig();
}


// Parse Config
void ConfigPanel::ParseConfig() {
	Config *config = caGetConfig();

	// Log Action
	caLogAction("Parse the config");

	bool foundConfigError = config->ParseConfig();

	// Everything good?
	if (!foundConfigError) {
		// Set Config Sliders
		this->timeoutSlider->SetValue(config->GetTimeout());
		this->stepSlider->SetValue(config->GetStep());
		this->attemptsSlider->SetValue(config->GetMaxAttempts());
		this->callsSlider->SetValue(config->GetNumLastCalls());

		this->pageText->SetValue(config->GetPage());
		this->keyText->SetValue(config->GetKey());

		this->steamEnable->SetValue(config->GetSteamEnabled());
		this->hideMini->SetValue(config->GetHideOnMinimize());

		// Calls are unimportant
		for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
			CallDialog *dialog = *callDialog;

			// Stop Avatar Timer
			if (dialog->GetAvatarTimer() != NULL && dialog->GetAvatarTimer()->IsRunning()) {
				dialog->GetAvatarTimer()->Stop();
				delete dialog->GetAvatarTimer();
			}

			dialog->Destroy();
		}

		caGetCallDialogs()->clear();

		// Updated Main Interface
		caGetMainPanel()->ResetCalls();

		// Restart the Timer
		caGetApp().StartTimer();

		// Log Action
		caLogAction("Loaded the config");

		// Start Steam Thread again
		caGetApp().StartSteamThread();
	} else {
		// Refresh main dialog
		caGetMainFrame()->SetTitle("Call Admin Client");
		caGetMainPanel()->SetEventText("Please configurate your settings...");

		// Log Action
		caLogAction("Couldn't load/find the config");
	}
}
