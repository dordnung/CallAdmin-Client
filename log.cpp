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



	// Auto Size
	SetSizerAndFit(sizerTop, true);
}




// Add Action to the logBox
void LogPanel::addLog(wxString log)
{
	if (logBox != NULL)
	{
		logBox->SetSelection(logBox->Append(wxString::FromUTF8(log)));
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