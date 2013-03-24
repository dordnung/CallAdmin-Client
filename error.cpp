/**
 * -----------------------------------------------------
 * File        error.cpp
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


// Project includes
#include "error.h"
#include "taskbar.h"
#include "config.h"
#include "calladmin-client.h"

// wx
#include <wx/statline.h>


// Error Dialog
ErrorDialog *error_dialog = NULL;



// Button ID's for Error Dialog
enum
{
	wxID_ExitError = 5003,
	wxID_HideError,
	wxID_CloseError,
};



// Button Events for Error Dialog
BEGIN_EVENT_TABLE(ErrorDialog, wxDialog)
	EVT_BUTTON(wxID_ExitError, ErrorDialog::OnExit)
	EVT_BUTTON(wxID_CloseError, ErrorDialog::OnClose)
	EVT_BUTTON(wxID_HideError, ErrorDialog::OnHide)
	EVT_CLOSE(ErrorDialog::OnCloseWindow)
END_EVENT_TABLE()



// Create Error Window
ErrorDialog::ErrorDialog(const wxString& title, int error, wxString errorMessage, wxString type) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);

	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();



	// Error Text
	text = new wxStaticText(this, wxID_ANY, "An error occured while connecting/reading the Page");

	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("red"));

	// Add it
	sizerTop->Add(text, flags);




	// Error Code and Message
	textError = new wxStaticText(this, wxID_ANY, type + " Error " + (wxString() << error) + ": " + errorMessage);

	textError->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(textError, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);




	// Try again
	textTray = new wxStaticText(this, wxID_ANY,  "Try again... " + (wxString() << timer->getAttempts()) + "/" + (wxString() << maxAttempts));

	textTray->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	textTray->SetForegroundColour(wxColor("blue"));

	sizerTop->Add(textTray, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);




	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_HideError, "Hide"), flags);
	sizerBtns->Add(new wxButton(this, wxID_ExitError, "Exit"), flags);
	sizerBtns->Add(new wxButton(this, wxID_CloseError, "Close"), flags);

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



// Button Event -> Hide to Taskbar
void ErrorDialog::OnHide(wxCommandEvent& WXUNUSED(event))
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


// Button Event -> Exit only Error
void ErrorDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
	error_dialog = NULL;

	Destroy();
}


// Button Event -> Exit only Error
void ErrorDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}


// Window Event -> Hide Window
void ErrorDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
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
