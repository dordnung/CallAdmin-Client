/**
 * -----------------------------------------------------
 * File        main_frame.cpp
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
#include "main_frame.h"
#include "calladmin-client.h"
#include "curl_util.h"


// Window events for Main Frame
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_COMMAND(wxID_ThreadHandled, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnThread)

EVT_CLOSE(MainFrame::OnCloseWindow)
EVT_ICONIZE(MainFrame::OnMinimizeWindow)
END_EVENT_TABLE()


MainFrame::MainFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMINIMIZE_BOX) {
	this->notebook = NULL;
}


// Create Main Frame (Window)
void MainFrame::CreateWindow(bool createInTaskbar) {
	// Create the notebook
	this->notebook = new Notebook();
	this->notebook->CreateAndAddPages();

	// Start in taskbar
	if (createInTaskbar && caGetTaskBarIcon()->IsAvailable()) {
		Show(false);
		caGetTaskBarIcon()->ShowMessage("Call Admin Started", "Call Admin started in taskbar", this);
	} else {
		// Show
		Show(true);
	}

	// Set the Icon
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
#else
	wxLogNull nolog;

	SetIcon(wxIcon(caGetApp().GetAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
#endif

	// Fit Notebook
	this->notebook->Fit();

	// Fit Main
	Fit();

	// Centre to Screen
	Centre();
}


// Notebook accessor
Notebook* MainFrame::GetNotebook() {
	return this->notebook;
}


// Thread Handled -> Call Function
void MainFrame::OnThread(wxCommandEvent& event) {
	// Get Content
	CurlThreadData *data = static_cast<CurlThreadData *>(event.GetClientObject());

	// Get Function
	CurlCallback function = data->GetCallbackFunction();

	// Call it
	function(data->GetError(), data->GetContent(), data->GetExtra());

	// Delete data
	delete data;
}


// Window Event -> exit programm
void MainFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	caGetApp().ExitProgramm();
}


// Window Event -> Hide Window
void MainFrame::OnMinimizeWindow(wxIconizeEvent& WXUNUSED(event)) {
	if (caGetConfig()->GetHideOnMinimize()) {
		// Log Action
		caLogAction("Hided Window");
		caGetTaskBarIcon()->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

		if (caGetTaskBarIcon()->IsAvailable()) {
			Show(false);
		} else {
			Iconize(true);
		}
	}
}