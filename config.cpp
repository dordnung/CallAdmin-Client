/**
 * -----------------------------------------------------
 * File        config.cpp
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Header File
 * Copyright (C) 2013 Impact, David <popoklopsi> Ordnung
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
#include "simpleini/SimpleIni.h"
#include "config.h"
#include "main.h"
#include "calladmin-client.h"

// Wx
#include <wx/statline.h>
#include <wx/stdpaths.h>



// Settings
int step = 0;
int timeout = 0;
int maxAttempts = 0;

std::string page = "";
std::string key = "";


ConfigDialog *config_dialog = NULL;



// Button ID's for Config Dialog
enum
{
	wxID_SetConfig = 5011,
	wxID_ExitConfig,
};


// Button Events for Error Dialog
BEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
	EVT_BUTTON(wxID_ExitConfig, ConfigDialog::OnExit)
	EVT_BUTTON(wxID_SetConfig, ConfigDialog::OnSet)
	EVT_CLOSE(ConfigDialog::OnCloseWindow)
END_EVENT_TABLE()



// Create Error Window
ConfigDialog::ConfigDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);


	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();



	// Config
	text = new wxStaticText(this, wxID_ANY, "Set up your Config");

	text->SetFont(wxFont(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("blue"));

	// Add it
	sizerTop->Add(text, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	// Ask for Page
	wxSizer* const sizerPage = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Your page to the notice.php: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerPage->Add(text, flags);

	pageText = new wxTextCtrl(this, wxID_ANY, "http://yourpage.com/notice.php", wxDefaultPosition, wxSize(220, 20));
	pageText->SetMaxLength(256);

	sizerPage->Add(pageText, flags);

	// Add text and box
	sizerTop->Add(sizerPage, flags.Align(wxALIGN_CENTER_HORIZONTAL));





	// Ask for Key
	wxSizer* const sizerKey = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Your private access Key: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerKey->Add(text, flags);

	keyText = new wxTextCtrl(this, wxID_ANY, "accesskey", wxDefaultPosition, wxSize(120, 20));
	keyText->SetMaxLength(64);

	sizerKey->Add(keyText, flags);

	// Add text and box
	sizerTop->Add(sizerKey, flags.Align(wxALIGN_CENTER_HORIZONTAL));






	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);





	// Ask for Step
	wxSizer* const sizerStep = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Time Intervall to search for new Calls: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerStep->Add(text, flags);

	stepSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 5, 20, 5, "Time Inverall");
	sizerStep->Add(stepSlider, flags);

	// Add text and box
	sizerTop->Add(sizerStep, flags.Align(wxALIGN_CENTER_HORIZONTAL));





	// Ask for Timeout
	wxSizer* const sizerTimeout = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Timeout for connection: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTimeout->Add(text, flags);

	timeoutSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 10, 3, "Timout");

	sizerTimeout->Add(timeoutSlider, flags);

	// Add text and box
	sizerTop->Add(sizerTimeout, flags.Align(wxALIGN_CENTER_HORIZONTAL));





	// Ask for Attempts
	wxSizer* const sizerAttempts = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Max. auto. reconnect attempts: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerAttempts->Add(text, flags);

	attemptsSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 3, 10, 3, "Max Attempts");

	sizerAttempts->Add(attemptsSlider, flags);

	// Add text and box
	sizerTop->Add(sizerAttempts, flags.Align(wxALIGN_CENTER_HORIZONTAL));




	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);




	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_SetConfig, "Create Config"), flags);
	sizerBtns->Add(new wxButton(this, wxID_ExitConfig, "Exit"), flags);

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Auto Size
	SetSizerAndFit(sizerTop, true);

	// Centre to Screen
	Centre();

	// Show
	Show(true);

	// Set the Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
}



// Button Event -> Try to set new config
void ConfigDialog::OnSet(wxCommandEvent& WXUNUSED(event))
{
	// Read config values
	timeout = timeoutSlider->GetValue();
	step = stepSlider->GetValue();
	maxAttempts = attemptsSlider->GetValue();

	page = pageText->GetValue();
	key = keyText->GetValue();


	CSimpleIniA config;

	// Read config file
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	size_t start = path.find_last_of("\\");

	if (start < 0)
	{
		start = path.find_last_of("/");
	}


	// Write to new config file
	path = path.replace(start, path.size(), "/calladmin-client_settings.ini");

	config.SetValue("settings", "step", (wxString() << step));
	config.SetValue("settings", "timeout", (wxString() << timeout));
	config.SetValue("settings", "attempts", (wxString() << maxAttempts));
	config.SetValue("settings", "page", page.c_str());
	config.SetValue("settings", "key", key.c_str());

	config.SaveFile(((std::string)path).c_str());


	// Close Config dialog
	config_dialog = NULL;

	Destroy();


	// Parse again
	parseConfig();
}




// Button Event -> Exit programm
void ConfigDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}



// Window Event -> Close Window
void ConfigDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	exitProgramm();
}



// parse Config
bool parseConfig()
{
	bool foundConfigError = false;

	// Read Config file ;)
	CSimpleIniA config;

	// Read config file
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	size_t start = path.find_last_of("\\");

	if (start < 0)
	{
		start = path.find_last_of("/");
	}

	path = path.replace(start, path.size(), "/calladmin-client_settings.ini");

	SI_Error rc = config.LoadFile(((std::string)path).c_str());

	// Was parsing good?
	if (rc != SI_OK)
	{
		foundConfigError = true;
	}
	else
	{
		// Get files out of config
		try
		{
			std::string stepS = config.GetValue("settings", "step", "5");
			std::stringstream s(stepS);

			s >> step;

			std::string timeoutS = config.GetValue("settings", "timeout", "3");
			std::stringstream t(timeoutS);

			t >> timeout;

			std::string maxAttemptsS = config.GetValue("settings", "attempts", "5");
			std::stringstream m(maxAttemptsS);

			m >> maxAttempts;

			page = config.GetValue("settings", "page", "");
			key = config.GetValue("settings", "key", "");
		}
		catch(...) {foundConfigError = true;}
		
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

			if (timeout < 1)
			{
				timeout = 1;
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

			if (timeout >= step)
			{
				timeout = step - 1;
			}

			// Create main Dialog
			main_dialog = new MainDialog("CallAdmin Welcome");
	
			main_dialog->createWindow(CallAdmin::start_taskbar);

			// Start Timer
			timer = new Timer();
			timer->run(step*1000);

			timer->setAttempts(0);
		}
	}

	return !foundConfigError;
}
