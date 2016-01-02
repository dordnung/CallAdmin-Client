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
#include "main_frame.h"
#include "calladmin-client.h"
#include "curl_util.h"


// Events for Main Frame
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnCloseWindow)
EVT_ICONIZE(MainFrame::OnMinimizeWindow)
END_EVENT_TABLE()


// Create Main Frame (Window)
void MainFrame::CreateWindow(bool createInTaskbar) {
	// Create the notebook
	this->notebook = new Notebook();
	this->notebook->CreateAndAddPages();

	// Start in taskbar?
	Show(!createInTaskbar || !caGetTaskBarIcon()->IsAvailable());

	// Set the Icon
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
#else
	SetIcon(wxIcon(caGetApp().GetAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
#endif

	// Fit Notebook
	this->notebook->Fit();

	// Fit Main
	Fit();

	// Centre to Screen
	Centre();
}


// Window Event -> exit programm
void MainFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	caLogAction("Closed main frame");

	caGetApp().ExitProgramm();
}


// Window Event -> Hide Window
void MainFrame::OnMinimizeWindow(wxIconizeEvent& WXUNUSED(event)) {
	if (caGetConfig()->GetHideOnMinimize()) {
		// Log Action
		caLogAction("Hided main frame");

		if (caGetTaskBarIcon()->IsAvailable()) {
			Show(false);
		} else {
			Iconize(true);
		}
	}
}