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

	EVT_SLIDER(XRCID("stepSlider"), ConfigPanel::OnSliderChanged)
	EVT_SLIDER(XRCID("timeoutSlider"), ConfigPanel::OnSliderChanged)
	EVT_SLIDER(XRCID("attemptsSlider"), ConfigPanel::OnSliderChanged)
	EVT_SLIDER(XRCID("callsSlider"), ConfigPanel::OnSliderChanged)

	EVT_FILEPICKER_CHANGED(XRCID("soundFile"), ConfigPanel::OnSoundFileChanged)
	EVT_BUTTON(XRCID("soundFileDefault"), ConfigPanel::OnSoundFileDefault)

	EVT_CHECKBOX(XRCID("steamEnable"), ConfigPanel::OnSteamUpdate)
	EVT_CHECKBOX(XRCID("showAsNotification"), ConfigPanel::OnShowAsNotificationUpdate)
	EVT_CHECKBOX(XRCID("hideMinimize"), ConfigPanel::OnHideUpdate)
	EVT_CHECKBOX(XRCID("minimizeExit"), ConfigPanel::OnHideUpdate)
wxEND_EVENT_TABLE()


// Init. Vars
ConfigPanel::ConfigPanel() {
	this->pageText = NULL;
	this->keyText = NULL;
	this->stepSlider = NULL;
	this->stepSliderValue = NULL;
	this->timeoutSlider = NULL;
	this->timeoutSliderValue = NULL;
	this->attemptsSlider = NULL;
	this->attemptsSliderValue = NULL;
	this->callsSlider = NULL;
	this->callsSliderValue = NULL;
	this->steamEnable = NULL;
	this->showAsNotification = NULL;
	this->hideMinimize = NULL;
	this->minimizeExit = NULL;
}


// Load controls
bool ConfigPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "configPanel")) {
		wxMessageBox("Error: Couldn't find XRCID configPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// URL
	FIND_OR_FAIL(this->pageText, XRCCTRL(*this, "callAdminUrl", wxTextCtrl), "callAdminUrl");

	// Ask for Key
	FIND_OR_FAIL(this->keyText, XRCCTRL(*this, "accessKey", wxTextCtrl), "accessKey");

	// Ask for 
	FIND_OR_FAIL(this->stepSlider, XRCCTRL(*this, "stepSlider", wxSlider), "stepSlider");
	FIND_OR_FAIL(this->stepSliderValue, XRCCTRL(*this, "stepSliderValue", wxStaticText), "stepSliderValue");

	// Ask for Timeout
	FIND_OR_FAIL(this->timeoutSlider, XRCCTRL(*this, "timeoutSlider", wxSlider), "timeoutSlider");
	FIND_OR_FAIL(this->timeoutSliderValue, XRCCTRL(*this, "timeoutSliderValue", wxStaticText), "timeoutSliderValue");

	// Ask for Attempts
	FIND_OR_FAIL(this->attemptsSlider, XRCCTRL(*this, "attemptsSlider", wxSlider), "attemptsSlider");
	FIND_OR_FAIL(this->attemptsSliderValue, XRCCTRL(*this, "attemptsSliderValue", wxStaticText), "attemptsSliderValue");

	// Ask for Last Calls
	FIND_OR_FAIL(this->callsSlider, XRCCTRL(*this, "callsSlider", wxSlider), "callsSlider");
	FIND_OR_FAIL(this->callsSliderValue, XRCCTRL(*this, "callsSliderValue", wxStaticText), "callsSliderValue");

	// Ask for sound file
	FIND_OR_FAIL(this->soundFilePicker, XRCCTRL(*this, "soundFile", wxFilePickerCtrl), "soundFile");

	// Ask for Steam
	FIND_OR_FAIL(this->steamEnable, XRCCTRL(*this, "steamEnable", wxCheckBox), "steamEnable");

	// Ask for Show in taskbar
	FIND_OR_FAIL(this->showAsNotification, XRCCTRL(*this, "showAsNotification", wxCheckBox), "showAsNotification");

	// Ask for hide on minimize
	FIND_OR_FAIL(this->hideMinimize, XRCCTRL(*this, "hideMinimize", wxCheckBox), "hideMinimize");

	// Ask for hide on exit
	FIND_OR_FAIL(this->minimizeExit, XRCCTRL(*this, "minimizeExit", wxCheckBox), "minimizeExit");

	// On Windows the config panel is to small, so increase the vertical gap to stretch it
	#if defined(__WXMSW__)
		if (wxGridBagSizer* gridBagSizer = dynamic_cast<wxGridBagSizer*>(this->pageText->GetContainingSizer())) {
			gridBagSizer->SetVGap(gridBagSizer->GetVGap() * 1.5);
		}
	#endif

	return true;
}


// A slider changed, set values
void ConfigPanel::OnSliderChanged(wxCommandEvent &WXUNUSED(event)) {
	this->stepSliderValue->SetLabel(wxString::Format("%02d", this->stepSlider->GetValue()));
	this->timeoutSliderValue->SetLabel(wxString::Format("%02d", this->timeoutSlider->GetValue()));
	this->attemptsSliderValue->SetLabel(wxString::Format("%02d", this->attemptsSlider->GetValue()));
	this->callsSliderValue->SetLabel(wxString::Format("%02d", this->callsSlider->GetValue()));
}


// The path changed
void ConfigPanel::OnSoundFileChanged(wxFileDirPickerEvent &event) {
	// Log Action
	caLogAction("Set sound file to " + event.GetPath(), LogLevel::LEVEL_INFO);

	if (!caGetConfig()->SetSoundFile(event.GetPath())) {
		this->soundFilePicker->SetPath("");
	}

	caGetConfig()->Flush();
}


// Using default sound file
void ConfigPanel::OnSoundFileDefault(wxCommandEvent &WXUNUSED(event)) {
	caGetConfig()->SetSoundFile("");
	caGetConfig()->Flush();

	this->soundFilePicker->SetPath("");

	// Log Action
	caLogAction("Changed sound file to the default sound", LogLevel::LEVEL_INFO);
}


// Steam Updated -> Set Config
void ConfigPanel::OnSteamUpdate(wxCommandEvent &WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetSteamEnabled(this->steamEnable->GetValue());
	caGetConfig()->Flush();

	// Log Action
	if (this->steamEnable->GetValue()) {
		caLogAction("Enabled Steam support", LogLevel::LEVEL_INFO);
	} else {
		caLogAction("Disabled Steam support", LogLevel::LEVEL_INFO);
	}
}


// Show in Taskbar Updated -> Set Config
void ConfigPanel::OnShowAsNotificationUpdate(wxCommandEvent &WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetShowAsNotification(this->showAsNotification->GetValue());
	caGetConfig()->Flush();

	// Set icon if user want it
	if (this->hideMinimize->GetValue() || this->showAsNotification->GetValue()) {
		caGetTaskBarIcon()->AddIcon();
	} else {
		caGetTaskBarIcon()->RemoveIcon();
	}

	// Log Action
	if (this->showAsNotification->GetValue()) {
		caLogAction("Enabled show as notification", LogLevel::LEVEL_INFO);
	} else {
		caLogAction("Disabled show as notification", LogLevel::LEVEL_INFO);
	}
}


// Hide On Minimize / Exit Updated -> Set Config
void ConfigPanel::OnHideUpdate(wxCommandEvent &WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetHideOnMinimize(this->hideMinimize->GetValue());
	caGetConfig()->SetMinimizeOnExit(this->minimizeExit->GetValue());
	caGetConfig()->Flush();

	// Set icon if user want it
	if (this->hideMinimize->GetValue() || this->showAsNotification->GetValue()) {
		caGetTaskBarIcon()->AddIcon();
	} else {
		caGetTaskBarIcon()->RemoveIcon();
	}

	// Log Action
	if (this->hideMinimize->GetValue() || this->minimizeExit->GetValue()) {
		caLogAction("Enabled hide on minimize / minimize on exit", LogLevel::LEVEL_INFO);
	} else {
		caLogAction("Disabled hide on minimize / minimize on exit", LogLevel::LEVEL_INFO);
	}
}


// Button Event -> Try to set new config
void ConfigPanel::OnSet(wxCommandEvent &WXUNUSED(event)) {
	// Write to new config file
	caGetConfig()->SetStep(this->stepSlider->GetValue());
	caGetConfig()->SetTimeout(this->timeoutSlider->GetValue());
	caGetConfig()->SetMaxAttempts(this->attemptsSlider->GetValue());
	caGetConfig()->SetNumLastCalls(this->callsSlider->GetValue());
	caGetConfig()->SetPage(this->pageText->GetValue());
	caGetConfig()->SetKey(this->keyText->GetValue());
	caGetConfig()->Flush();

	// Refresh main dialog
	caGetMainFrame()->SetTitle("CallAdmin Client");
	caGetMainPanel()->SetStatusText("Enable new Settings");

	// Log Action
	caLogAction("Saved the config", LogLevel::LEVEL_INFO);

	// Goto Main
	caGetNotebook()->GetWindow()->ChangeSelection(MAIN_PANEL_PAGE);

	// Parse again
	ParseConfig();
}


// Parse Config
void ConfigPanel::ParseConfig() {
	Config *config = caGetConfig();

	// Log Action
	caLogAction("Parsing the config", LogLevel::LEVEL_INFO);

	bool isConfigSet = config->ParseConfig();

	// Everything good?
	if (isConfigSet) {
		// Set Config Sliders
		this->timeoutSlider->SetValue(config->GetTimeout());
		this->stepSlider->SetValue(config->GetStep());
		this->attemptsSlider->SetValue(config->GetMaxAttempts());
		this->callsSlider->SetValue(config->GetNumLastCalls());

		// Notice changed values to slider values
		wxCommandEvent dummyEvent;
		OnSliderChanged(dummyEvent);

		this->pageText->SetValue(config->GetPage());
		this->keyText->SetValue(config->GetKey());
		this->soundFilePicker->SetPath(config->GetSoundFile());

		this->steamEnable->SetValue(config->GetSteamEnabled());
		this->showAsNotification->SetValue(config->GetShowAsNotification());
		this->hideMinimize->SetValue(config->GetHideOnMinimize());
		this->minimizeExit->SetValue(config->GetMinimizeOnExit());

		// Calls are unimportant
		for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
			(*callDialog)->Destroy();
		}

		caGetCallDialogs()->clear();

		// Update the calls
		caGetMainPanel()->UpdateCalls();

		// Start Steam thread if not started yet
		caGetApp().StartSteamThread();

		// Restart the timer
		caGetApp().StartTimer();

		// Set icon if user want it
		if (config->GetHideOnMinimize() || config->GetShowAsNotification()) {
			caGetTaskBarIcon()->AddIcon();
		} else {
			caGetTaskBarIcon()->RemoveIcon();
		}

		// Log Action
		caLogAction("Loaded the config successfully", LogLevel::LEVEL_INFO);
	} else {
		// Refresh main dialog
		caGetMainFrame()->SetTitle("CallAdmin Client");
		caGetMainPanel()->SetStatusText("Please configurate your settings");

		// Log Action
		caLogAction("Couldn't load/find the config", LogLevel::LEVEL_WARNING);
	}
}
