/**
 * -----------------------------------------------------
 * File        main.cpp
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


// c++ lib
#include <sstream>


// Include Project
#include "mainframe.h"
#include "log.h"
#include "about.h"
#include "trackers.h"
#include "taskbar.h"
#include "config.h"
#include "calladmin-client.h"


// Wx
#include <wx/statline.h>
#include <wx/listbox.h>
#include <wx/tooltip.h>

// Main Dialog
MainFrame *mainFrame = NULL;


// Notebook
wxNotebook* notebook = NULL;




// Button Events for Main Dialog
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_BUTTON(wxID_Hide, MainFrame::OnHide)
	EVT_BUTTON(wxID_Reconnect, MainFrame::OnReconnect)

	EVT_CHECKBOX(wxID_CheckBox, MainFrame::OnCheckBox)

	EVT_COMMAND(wxID_ThreadHandled, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnThread)
	EVT_COMMAND(wxID_SteamChanged, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnSteamChange)

	EVT_CLOSE(MainFrame::OnCloseWindow)
	EVT_ICONIZE(MainFrame::OnMinimizeWindow)

	EVT_LISTBOX_DCLICK(wxID_BoxClick, MainFrame::OnBoxClick)
END_EVENT_TABLE()



// Create Main Window
void MainFrame::createWindow(bool taskbar)
{
	// Create Notebook and panel
	notebook = new wxNotebook(this, wxID_ANY);


	// Valid?
	if (notebook == NULL)
	{
		return;
	}


	panel = new wxPanel(notebook, wxID_ANY);


	// Valid?
	if (panel == NULL)
	{
		return;
	}


	// Add Main Page
	notebook->AddPage(panel, ("Main"));

	// Border and Center
	wxStaticText* text;

	// Hole Body
	sizerBody = new wxBoxSizer(wxHORIZONTAL);

	// Box Body
	wxStaticBoxSizer* sizerBox = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, wxT("Latest Calls")), wxHORIZONTAL);


	// Box for all Calls
	callBox = new wxListBox(panel, wxID_BoxClick, wxDefaultPosition, wxSize(280, -1), 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	callBox->SetFont(wxFont(9, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	// Add to Body
	sizerBox->Add(callBox, 0, wxEXPAND | wxALL, 5);
	sizerBody->Add(sizerBox, 0, wxEXPAND | wxALL, 5);

	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	// Space
	sizerTop->Add(0, 0, 0, wxBOTTOM, 15);



	// Welcome Text
	text = new wxStaticText(panel, wxID_ANY, "The Admin Caller");

	text->SetFont(wxFont(30, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxBOTTOM | wxLEFT | wxRIGHT | wxALIGN_CENTER, 40);


	// Space
	sizerTop->Add(0, 0, 0, wxTOP, 20);

	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	// Steam Text
	steamText = new wxStaticText(panel, wxID_ANY, "Steam is currently not running");
	steamText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	steamText->SetForegroundColour(wxColor("red"));

	// Add it
	sizerTop->Add(steamText, 0, wxALL | wxALIGN_CENTER, 10);




	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);






	eventText = new wxStaticText(panel, wxID_ANY,  "Waiting for a new report...");

	eventText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	eventText->SetForegroundColour(wxColor("blue"));

	sizerTop->Add(eventText, 0, wxALL | wxALIGN_CENTER, 10);




	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);


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
	available = new wxCheckBox(panel, wxID_CheckBox, "I'm available");
	available->SetValue(g_config->ReadBool("available", true));
	available->SetToolTip(tipAvailable);

	sizerChecks->Add(available, 0, wxALL | wxALIGN_CENTER, 5);

	// ToolTip for second Checkbox
	wxToolTip* tipSound = new wxToolTip("You will hear a notification sound on an incoming call.");

	tipSound->SetDelay(500);
	tipSound->Enable(true);




	// The sound Checkbox
	sound = new wxCheckBox(panel, wxID_CheckBox, "Sound on call");
	sound->SetValue(g_config->ReadBool("sound", true));
	sound->SetToolTip(tipSound);

	sizerChecks->Add(sound, 0, wxALL | wxALIGN_CENTER, 5);

	// ToolTip for third Checkbox
	wxToolTip* specAvailable = new wxToolTip("You will only receive calls but you will not be stored in the trackers database");

	specAvailable->SetDelay(500);
	specAvailable->Enable(true);





	// The store Checkbox
	store = new wxCheckBox(panel, wxID_CheckBox, "Spectate only");
	store->SetValue(g_config->ReadBool("spectate", false));
	store->SetToolTip(specAvailable);

	sizerChecks->Add(store, 0, wxALL | wxALIGN_CENTER, 5);



	// Hide, Check
	sizerBtns->Add(new wxButton(panel, wxID_Hide, "Hide"), 0, wxTOP | wxLEFT | wxALIGN_CENTER, 5);



	// If max attempts reached, add a reconnect button
	reconnectButton = new wxButton(panel, wxID_Reconnect, "Reconnect");
	reconnectButton->Enable(false);

	sizerBtns->Add(reconnectButton, 0, wxTOP | wxRIGHT | wxALIGN_CENTER, 5);


	// Add Checks to Box
	sizerTop->Add(sizerChecks, 0, wxTOP | wxRIGHT | wxALIGN_CENTER_HORIZONTAL, 5);

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_CENTER_HORIZONTAL, 5);



	// Author + Version Text
	text = new wxStaticText(panel, wxID_ANY, "v" + version + "  (c) Popoklopsi and Impact");

	text->SetFont(wxFont(8, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_RIGHT, 10);



	// Add to Body
	sizerBody->Add(sizerTop, 0, wxTOP | wxRIGHT | wxLEFT | wxALIGN_RIGHT, 10);


	// Auto Size
	panel->SetSizerAndFit(sizerBody, true);




	// Add Config Page
	notebook->AddPage(new ConfigPanel(notebook), ("Settings"));


	// Add trackers Page
	notebook->AddPage(new TrackerPanel(notebook), ("Trackers"));

	// Add Log Page
	notebook->AddPage(new LogPanel(notebook), ("Logging"));


	// Add about Page
	notebook->AddPage(new AboutPanel(notebook), ("About"));


	// Log Action
	LogAction("Start the main Window");


	// Start in taskbar
	if (taskbar && m_taskBarIcon != NULL)
	{
		mainFrame->Show(false);
				
		m_taskBarIcon->ShowMessage("Call Admin Started", "Call Admin started in taskbar", panel);
	}
	else
	{
		// Show
		Show(true);
	}


	// Set the Icon
	#if defined(__WXMSW__)
		SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
	#else
		wxLogNull nolog;

		SetIcon(wxIcon(getAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
	#endif

	// Fit Notebook
	notebook->Fit();
	
	// Fit Main
	this->Fit();


	// Centre to Screen
	Centre();


	// Update Call List
	updateCall();
}





// Button Event -> Hide to Taskbar
void MainFrame::OnHide(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Hided Window");

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

		Show(false);
	}
	else
	{
		Iconize(true);
	}
}




// Check Box Event -> Write To Config
void MainFrame::OnCheckBox(wxCommandEvent& WXUNUSED(event))
{
	g_config->Write("available", available->IsChecked());
	g_config->Write("sound", sound->IsChecked());
	g_config->Write("spectate", store->IsChecked());
}



// Button Event -> Reconnect
void MainFrame::OnReconnect(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Reconnecting...");

	// Reset attempts
	attempts = 0;

	if (mainFrame != NULL)
	{
		mainFrame->SetTitle("Call Admin Client");
		mainFrame->setEventText("Trying to reconnect...");
		mainFrame->setReconnectButton(false);

		mainFrame->Show(true);
		mainFrame->Restore();
	}

	// Start the Timer again
	timer->Start(step*1000);
}



// Thread Handled -> Call Function
void MainFrame::OnThread(wxCommandEvent& event)
{
	// Get Content
	ThreadData* data = static_cast<ThreadData *>(event.GetClientObject());

	// Get Function
	callback function = data->getCallback();

	// Call it
	function(data->getError(), data->getContent(), data->getExtra());

	// Delete data
	delete data;
}



// Steam Changed -> Set Text
void MainFrame::OnSteamChange(wxCommandEvent& event)
{
	// Get Status
	int id = event.GetInt();

	if (id == 0)
	{
		mainFrame->setSteamStatus("Steam support is disabled", wxColour("red"));
	}

	else if (id == 1)
	{
		mainFrame->setSteamStatus("Steam is currently not running", wxColour("red"));
	}

	else
	{
		mainFrame->setSteamStatus("Steam is currently running", wxColour(34, 139, 34));
	}
}



// Window Event -> Open Call
void MainFrame::OnBoxClick(wxCommandEvent& WXUNUSED(event))
{
	int selection = callBox->GetSelection();

	if (call_dialogs != NULL && call_dialogs[selection] != NULL)
	{
		call_dialogs[selection]->Show(true);
		call_dialogs[selection]->Restore();
	}
}


// Window Event -> exit programm
void MainFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	exitProgramm();
}



// Window Event -> Hide Window
void MainFrame::OnMinimizeWindow(wxIconizeEvent& WXUNUSED(event))
{
	if (hideOnMinimize)
	{
		// Log Action
		LogAction("Hided Window");

		if (m_taskBarIcon != NULL)
		{
			m_taskBarIcon->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

			Show(false);
		}
		else
		{
			Iconize(true);
		}
	}
}



// Update Call List
void MainFrame::updateCall()
{
	callBox->Clear();

	for (int i=0; i < MAXCALLS; i++)
	{
		if (call_dialogs != NULL && call_dialogs[i] != NULL)
		{
			int item;

			
			if (call_dialogs[i]->getHandled())
			{
				item = callBox->Append("F - " + wxString::FromUTF8(call_dialogs[i]->getBoxText()));
			}
			else
			{
				item = callBox->Append("U - " + wxString::FromUTF8(call_dialogs[i]->getBoxText()));
			}
			

			callBox->SetSelection(item);
		}
	}
}
