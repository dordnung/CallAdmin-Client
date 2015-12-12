/**
 * -----------------------------------------------------
 * File        config.cpp
 * Authors     David <popoklopsi> Ordnung, Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 * 
 * 
 * Copyright (C) 2013 David <popoklopsi> Ordnung, Impact
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


// c++
#include <sstream>

// Include Project
#include "config.h"
#include "call.h"
#include "log.h"
#include "mainframe.h"
#include "opensteam.h"
#include "taskbar.h"
#include "calladmin-client.h"

// Wx
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <wx/gbsizer.h>


// Settings
int step = 5;
int timeout = 3;
int maxAttempts = 3;
int lastCalls = 25;

wxString page = "";
wxString key = "";


// Steam Enabled?
bool steamEnabled = true;
bool hideOnMinimize = false;


// The config
wxConfig *g_config = NULL;


// Button ID's for Config Panel
enum
{
	wxID_SetConfig = wxID_HIGHEST+400,
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
ConfigPanel::ConfigPanel(wxNotebook* note) : wxPanel(note, wxID_ANY)
{
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
	flags.Border(wxALL &~ wxBOTTOM, 10);
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
	gridSizer->Add(new wxStaticLine(this, wxID_ANY), wxGBPosition(currentPos++, 0), wxGBSpan(1, 2), wxEXPAND | (wxALL &~ wxLEFT &~ wxRIGHT), 10);





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
	gridSizer->Add(new wxStaticLine(this, wxID_ANY), wxGBPosition(currentPos++, 0), wxGBSpan(1, 2), wxEXPAND | (wxALL &~ wxLEFT &~ wxRIGHT), 10);



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


	// Parse Config
	parseConfig();
}



// Steam Updated -> Set Config
void ConfigPanel::OnCheckBox(wxCommandEvent& WXUNUSED(event))
{
	// Read config value
	if (steamEnable != NULL && g_config != NULL)
	{
		steamEnabled = steamEnable->GetValue();

		// Write to config file
		g_config->Write("steam", steamEnabled);

		// Log Action
		LogAction("Changed Steam Status");
	}
}





// Hide On Minimize Updated -> Set Config
void ConfigPanel::OnCheckBox2(wxCommandEvent& WXUNUSED(event))
{
	// Read config value
	if (hideMini != NULL && g_config != NULL)
	{
		hideOnMinimize = hideMini->GetValue();

		// Write to config file
		g_config->Write("hideonminimize", hideOnMinimize);

		// Log Action
		LogAction("Changed Hide on Minimize Status");
	}
}




// Button Event -> Try to set new config
void ConfigPanel::OnSet(wxCommandEvent& WXUNUSED(event))
{
	// Valid?
	if (hideMini == NULL || timeoutSlider == NULL || stepSlider == NULL || attemptsSlider == NULL || lastCalls == NULL || pageText == NULL || keyText == NULL || g_config == NULL || mainFrame == NULL || notebook == NULL)
	{
		return;
	}

	// Read config values
	timeout = timeoutSlider->GetValue();
	step = stepSlider->GetValue();
	maxAttempts = attemptsSlider->GetValue();
	lastCalls = callsSlider->GetValue();

	page = pageText->GetValue();
	key = keyText->GetValue();


	// Write to new config file
	g_config->Write("step", step);
	g_config->Write("timeout", timeout);
	g_config->Write("attempts", maxAttempts);
	g_config->Write("lastcalls", lastCalls);
	g_config->Write("page", page);
	g_config->Write("key", key);



	// Refresh main dialog
	mainFrame->SetTitle("Call Admin Client");
	mainFrame->setEventText("Enable new Settings...");

	// Log Action
	LogAction("Saved the config");


	// Goto Main
	notebook->SetSelection(0);

	// Parse again
	parseConfig();
}





// parse Config
void ConfigPanel::parseConfig()
{
	// Valid?
	if (hideMini == NULL || timeoutSlider == NULL || stepSlider == NULL || attemptsSlider == NULL || lastCalls == NULL || pageText == NULL || keyText == NULL || g_config == NULL || mainFrame == NULL)
	{
		return;
	}

	bool foundConfigError = false;

	// Log Action
	LogAction("Parse the config");


	// Was parsing good?
	if (!g_config->Exists("page") || !g_config->Exists("key"))
	{
		foundConfigError = true;
	}
	else
	{
		// Get files out of config
		try
		{
			step = g_config->ReadLong("step", 5l);
			timeout = g_config->ReadLong("timeout", 3l);
			maxAttempts = g_config->ReadLong("attempts", 5l);
			lastCalls = g_config->ReadLong("lastcalls", 25l);

			steamEnabled = g_config->ReadBool("steam", true);
			hideOnMinimize = g_config->ReadBool("hideonminimize", false);

			g_config->Read("page", &page, "");

			// Strip last /
			int lastSlash = page.find_last_of("/");
			int length = page.length() - 1;

			if (lastSlash == length)
			{
				// Delete it
				page.RemoveLast();
			}

			g_config->Read("key", &key, "");
		}
		catch(...) {foundConfigError = true;}
		
	}

	// Everything good?
	if (!foundConfigError)
	{
		// Check invalid values
		if (step < 5)
		{
			step = 5;
		}

		if (step > 20)
		{
			step = 20;
		}

		if (timeout < 3)
		{
			timeout = 3;
		}

		if (timeout > 10)
		{
			timeout = 10;
		}

		if (maxAttempts < 3)
		{
			maxAttempts = 3;
		}

		if (maxAttempts > 10)
		{
			maxAttempts = 10;
		}

		if (lastCalls < 0)
		{
			lastCalls = 0;
		}

		if (lastCalls > 50)
		{
			lastCalls = 50;
		}

		if (timeout >= step)
		{
			timeout = step - 1;
		}


		// Set Config Values
		timeoutSlider->SetValue(timeout);
		stepSlider->SetValue(step);
		attemptsSlider->SetValue(maxAttempts);
		callsSlider->SetValue(lastCalls);

		pageText->SetValue(page);
		keyText->SetValue(key);

		steamEnable->SetValue(steamEnabled);
		hideMini->SetValue(hideOnMinimize);

		// Calls are unimportant
		for (int i=0; i < MAXCALLS; i++)
		{
			if (call_dialogs[i] != NULL)
			{
				// Stop Avatar Timer
				if (call_dialogs[i]->avatarTimer != NULL && call_dialogs[i]->avatarTimer->IsRunning())
				{
					call_dialogs[i]->avatarTimer->Stop();
				}

				call_dialogs[i]->Destroy();
				call_dialogs[i] = NULL;
			}
		}


		// Timer... STOP!
		if (timer != NULL)
		{
			timer->Stop();
			timer = NULL;
		}

		// Log Action
		LogAction("Loaded the config");


		// Updated Main Interface
		mainFrame->resetCalls();

		// First Start again ;D
		timerStarted = false;

		// Start Timer
		timer = new Timer();
		timer->run(step*1000);

		// Reset Attempts
		attempts = 0;


		// Start Steam Thread
		steamThreader = new steamThread();
	}
	else
	{
		// Refresh main dialog
		mainFrame->SetTitle("Call Admin Client");
		mainFrame->setEventText("Please configurate your settings...");

		// Log Action
		LogAction("Couldn't load/find the config");
	}
}
