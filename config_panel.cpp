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
#include "config_panel.h"
#include "calladmin-client.h"

#include <wx/gbsizer.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
// Memory leak detection for debugging 
#include <wx/msw/msvcrt.h>
#endif


// Events for Config Panel
wxBEGIN_EVENT_TABLE(ConfigPanel, wxPanel)
EVT_BUTTON(XRCID("updateSettings"), ConfigPanel::OnSet)

EVT_CHECKBOX(XRCID("steamEnable"), ConfigPanel::OnSteamUpdate)
EVT_CHECKBOX(XRCID("showInTaskbar"), ConfigPanel::OnShowInTaskbarUpdate)
EVT_CHECKBOX(XRCID("hideMinimized"), ConfigPanel::OnHideUpdate)

EVT_CHOICE(XRCID("logLevel"), ConfigPanel::OnLogLevelUpdate)
wxEND_EVENT_TABLE()


// Init. Vars
ConfigPanel::ConfigPanel() {
	this->stepSlider = NULL;
	this->timeoutSlider = NULL;
	this->attemptsSlider = NULL;
	this->callsSlider = NULL;
	this->pageText = NULL;
	this->keyText = NULL;
	this->logLevel = NULL;
	this->steamEnable = NULL;
	this->showInTaskbar = NULL;
	this->hideMinimized = NULL;
}


bool ConfigPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "configPanel")) {
		wxMessageBox("Error: Couldn't find XRCID configPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Sizer
	wxSizer *sizerTop;
	FIND_OR_FAIL(sizerTop, this->GetSizer(), "configPanelSizerTop");

	// URL
	FIND_OR_FAIL(this->pageText, XRCCTRL(*this, "callAdminUrl", wxTextCtrl), "callAdminUrl");

	// Ask for Key
	FIND_OR_FAIL(this->keyText, XRCCTRL(*this, "accessKey", wxTextCtrl), "accessKey");

	// Ask for Step
	FIND_OR_FAIL(this->stepSlider, XRCCTRL(*this, "stepSlider", wxSlider), "stepSlider");

	// Ask for Timeout
	FIND_OR_FAIL(this->timeoutSlider, XRCCTRL(*this, "timeoutSlider", wxSlider), "timeoutSlider");

	// Ask for Attempts
	FIND_OR_FAIL(this->attemptsSlider, XRCCTRL(*this, "attemptsSlider", wxSlider), "attemptsSlider");

	// Ask for Last Calls
	FIND_OR_FAIL(this->callsSlider, XRCCTRL(*this, "callsSlider", wxSlider), "callsSlider");

	// Ask for LogLevel
	FIND_OR_FAIL(this->logLevel, XRCCTRL(*this, "logLevel", wxChoice), "logLevel");
	this->logLevel->SetSelection(LogLevel::LEVEL_INFO);

	// Ask for Steam
	FIND_OR_FAIL(this->steamEnable, XRCCTRL(*this, "steamEnable", wxCheckBox), "steamEnable");

	// Ask for Show in taskbar
	FIND_OR_FAIL(this->showInTaskbar, XRCCTRL(*this, "showInTaskbar", wxCheckBox), "showInTaskbar");

	// Ask for hide on mini
	FIND_OR_FAIL(this->hideMinimized, XRCCTRL(*this, "hideMinimized", wxCheckBox), "hideMinimized");

	// Auto Size
	SetSizerAndFit(sizerTop, true);

	return true;
}


// Log Level Updated -> Set Config
void ConfigPanel::OnLogLevelUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetLogLevel((LogLevel) this->logLevel->GetSelection());

	// Log Action
	caLogAction("Changed Log Level");
}


// Steam Updated -> Set Config
void ConfigPanel::OnSteamUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetSteamEnabled(this->steamEnable->GetValue());

	// Log Action
	caLogAction("Changed Steam Status");
}


// Show in Taskbar Updated -> Set Config
void ConfigPanel::OnShowInTaskbarUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetShowInTaskbar(this->showInTaskbar->GetValue());

	// Log Action
	caLogAction("Changed show in taskbar value");
}


// Hide On Minimize Updated -> Set Config
void ConfigPanel::OnHideUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetHideOnMinimize(this->hideMinimized->GetValue());

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
	caGetMainFrame()->SetTitle("CallAdmin Client");
	caGetMainPanel()->SetEventText("Enable new Settings...");

	// Log Action
	caLogAction("Saved the config");

	// Goto Main
	caGetNotebook()->GetWindow()->SetSelection(0);

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

		this->logLevel->SetSelection((int)config->GetLogLevel());
		this->steamEnable->SetValue(config->GetSteamEnabled());
		this->showInTaskbar->SetValue(config->GetShowInTaskbar());
		this->hideMinimized->SetValue(config->GetHideOnMinimize());

		// Calls are unimportant
		for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
			(*callDialog)->Destroy();
		}

		caGetCallDialogs()->clear();

		// Updated Main Interface
		caGetMainPanel()->UpdateCalls();

		// Start Steam Thread again
		caGetApp().StartSteamThread();

		// Restart the Timer
		caGetApp().StartTimer();

		// Log Action
		caLogAction("Loaded the config");
	} else {
		// Refresh main dialog
		caGetMainFrame()->SetTitle("CallAdmin Client");
		caGetMainPanel()->SetEventText("Please configurate your settings...");

		// Log Action
		caLogAction("Couldn't load/find the config");
	}
}