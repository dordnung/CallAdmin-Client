/**
 * -----------------------------------------------------
 * File        log.cpp
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
#include <ctime>

// Include Project
#include "log.h"
#include "main.h"
#include "taskbar.h"
#include "calladmin-client.h"


// Log Panel
LogPanel* logPanel = NULL;



// Button ID's for Log Panel
enum
{
	wxID_ExitLog = wxID_HIGHEST+500,
	wxID_HideLog,
};


// Button Events for Log Panel
BEGIN_EVENT_TABLE(LogPanel, wxPanel)
	EVT_BUTTON(wxID_ExitLog, LogPanel::OnExit)
	EVT_BUTTON(wxID_HideLog, LogPanel::OnHide)
END_EVENT_TABLE()



// Create Log Panel
LogPanel::LogPanel(wxNotebook* note) : wxPanel(note, wxID_ANY)
{
	// Set Log Panel
	logPanel = this;


	// Border and Center
	wxSizerFlags flags;


	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);


	logBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	logBox->SetFont(wxFont(12, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	// Add Log Box
	sizerTop->Add(logBox, 1, wxEXPAND);

	
	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_HideLog, "Hide"), flags.Border(wxALL &~ wxRIGHT, 5));
	sizerBtns->Add(new wxButton(this, wxID_ExitLog, "Exit"), flags.Border(wxALL &~ wxLEFT, 5));



	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Auto Size
	SetSizerAndFit(sizerTop, true);
}




// Add Action to the logBox
void LogPanel::addLog(wxString log)
{
	logBox->SetSelection(logBox->Append(wxString::FromUTF8(log)));
}




// Button Event -> Exit programm
void LogPanel::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}


// Button Event -> Hide to Taskbar
void LogPanel::OnHide(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Hide to taskbar");

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

		main_dialog->Show(false);
	}
	else
	{
		main_dialog->Iconize(true);
	}
}


void LogAction(wxString action)
{
	// Add the new Log to the Log box
	char buffer[80];

	time_t t = time(0);

	struct tm* now = localtime(&t);

	// But first we need a Time
	strftime(buffer, sizeof(buffer), "%c", now);

	// Add the log
	if (logPanel != NULL)
	{
		logPanel->addLog((wxString)buffer + " - " + action);		
	}
}