/**
 * -----------------------------------------------------
 * File        main.cpp
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


// c++ lib
#include <sstream>


// Include Project
#include "main.h"
#include "call.h"
#include "taskbar.h"
#include "config.h"
#include "calladmin-client.h"


// Wx
#include <wx/statline.h>
#include <wx/listbox.h>
#include <wx/tooltip.h>


// Main Dialog
MainDialog *main_dialog = NULL;


// Button ID's for Main Dialog
enum
{
    wxID_Exit = 4999,
    wxID_Hide,
    wxID_Reconnect,
	wxID_BoxClick,
};


// Button Events for Main Dialog
BEGIN_EVENT_TABLE(MainDialog, wxDialog)
    EVT_BUTTON(wxID_Exit, MainDialog::OnExit)
    EVT_BUTTON(wxID_Hide, MainDialog::OnHide)
    EVT_BUTTON(wxID_Reconnect, MainDialog::OnReconnect)
    EVT_CLOSE(MainDialog::OnCloseWindow)
	EVT_LISTBOX_DCLICK(wxID_BoxClick, MainDialog::OnBoxClick)
END_EVENT_TABLE()



// Create Main Window
void MainDialog::createWindow(bool taskbar)
{
	// Border and Center
    wxSizerFlags flags;
	wxStaticText* text;

	// Hole Body
	sizerBody = new wxBoxSizer(wxHORIZONTAL);


	// Box for all Calls
	callBox = new wxListBox(this, wxID_BoxClick, wxDefaultPosition, wxSize(250, -1), 0, NULL, wxLB_HSCROLL);
	callBox->SetFont(wxFont(11, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add to Body
	sizerBody->Add(callBox, 0, wxEXPAND | wxALL, 5);
	


	// Create Box
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);



	// Border and Centre
    flags.Border(wxALL, 10);
	flags.Centre();



	// Space
	sizerTop->Add(0, 0, 0, wxBOTTOM, 15);



	// Welcome Text
	text = new wxStaticText(this, wxID_ANY, "The Admin Caller");

	text->SetFont(wxFont(30, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, flags.Border(wxALL &~ wxTOP, 40));


	// Restore Border
	flags.Border(wxALL, 10);



	// Space
	sizerTop->Add(0, 0, 0, wxTOP, 20);

	

	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	// Steam Text
	steamText = new wxStaticText(this, wxID_ANY, "Steam is currently not running");
	steamText->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	steamText->SetForegroundColour(wxColor("red"));

	// Add it
	sizerTop->Add(steamText, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);






	eventText = new wxStaticText(this, wxID_ANY,  "Waiting for a new Event...");

	eventText->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	eventText->SetForegroundColour(wxColor("blue"));

	sizerTop->Add(eventText, flags);




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
	available = new wxCheckBox(this, wxID_ANY, "I'm available");
	available->SetValue(true);
	available->SetToolTip(tipAvailable);

	sizerChecks->Add(available , flags.Border(wxALL, 5));

	// ToolTip for second Checkbox
	wxToolTip* tipSound = new wxToolTip("You will hear a notification sound on an incoming call.");

	tipSound->SetDelay(500);
	tipSound->Enable(true);




	// The sound Checkbox
	sound = new wxCheckBox(this, wxID_ANY, "Sound on Call");
	sound->SetValue(true);
	sound->SetToolTip(tipSound);

	sizerChecks->Add(sound, flags.Border(wxALL, 5));

	// ToolTip for third Checkbox
	wxToolTip* specAvailable = new wxToolTip("You will only receive calls but you will not be stored in the database");

	specAvailable->SetDelay(500);
	specAvailable->Enable(true);





	// The store Checkbox
	store = new wxCheckBox(this, wxID_ANY, "Spectate Only");
	store->SetValue(false);
	store->SetToolTip(specAvailable);

	sizerChecks->Add(store, flags.Border(wxALL, 5));



	// Hide, Check and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_Hide, "Hide"), flags.Border(wxALL &~ wxBottom, 5));


	// Exit
    sizerBtns->Add(new wxButton(this, wxID_Exit, "Exit"), flags.Border(wxALL &~ wxBottom, 5));


	// If max attempts reached, add a reconnect button
	reconnectButton = new wxButton(this, wxID_Reconnect, "Reconnect");
	reconnectButton->Enable(false);

	sizerBtns->Add(reconnectButton, flags.Border(wxALL &~ wxBottom, 5));



	// Add Checks to Box
    sizerTop->Add(sizerChecks, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL &~ wxBottom, 5));



	// Author + Version Text
	text = new wxStaticText(this, wxID_ANY, "v" + version + "  (c) Popoklopsi and Impact");

	text->SetFont(wxFont(8, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxALIGN_RIGHT, 0);



	// Add to Body
	sizerBody->Add(sizerTop, flags.Right());




	// Auto Size
    SetSizerAndFit(sizerBody, true);

	// Centre to Screen
    Centre();



	// Start in taskbar
	if (taskbar && m_taskBarIcon != NULL)
	{
		main_dialog->Show(false);
				
		m_taskBarIcon->ShowBalloon("CallAdmin Started", "CallAdmin started in taskbar");
	}
	else
	{
		// Show
		Show(true);
	}


	// Set the Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));

	// Update Call List
	updateCall();
}



// Button Event -> Hide to Taskbar
void MainDialog::OnHide(wxCommandEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");

		Show(false);
	}
	else
	{
		Iconize(true);
	}
}


// Button Event -> Exit hole programm
void MainDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}


// Button Event -> Reconnect
void MainDialog::OnReconnect(wxCommandEvent& WXUNUSED(event))
{
	// Reset attempts
	timer->setAttempts(0);

	if (main_dialog != NULL)
	{
		main_dialog->SetTitle("CallAdmin Welcome");
		main_dialog->setEventText("Waiting for a new Event...");
		main_dialog->setReconnectButton(false);

		main_dialog->Show(true);
		main_dialog->Restore();
	}

    // Start the Timer again
	timer->Start(step*1000);
}


// Window Event -> Open Call
void MainDialog::OnBoxClick(wxCommandEvent& WXUNUSED(event))
{
	int selection = callBox->GetCount() - callBox->GetSelection() - 1;

	if (call_dialogs[selection] != NULL)
	{
		call_dialogs[selection]->Show(true);

	}
}


// Window Event -> Hide Window
void MainDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		Show(false);

		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");
	}
	else
	{
		Iconize(true);
	}
}


// Update Call List
void MainDialog::updateCall()
{
	callBox->Clear();

	for (int i=99; i >= 0; i--)
	{
		if (call_dialogs[i] != NULL)
		{
			callBox->Append(call_dialogs[i]->getBoxText());
		}
	}
}
