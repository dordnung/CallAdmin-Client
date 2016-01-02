/**
 * -----------------------------------------------------
 * File        notebook.cpp
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
#include "notebook.h"
#include "calladmin-client.h"

// Events
BEGIN_EVENT_TABLE(Notebook, wxNotebook)
EVT_CLOSE(Notebook::OnCloseWindow)
END_EVENT_TABLE()

Notebook::Notebook() : wxNotebook(caGetMainFrame(), wxID_ANY) {
	this->mainPanel = NULL;
	this->configPanel = NULL;
	this->trackersPanel = NULL;
	this->logPanel = NULL;
	this->aboutPanel = NULL;
}

// Create Notebook pages
void Notebook::CreateAndAddPages() {
	this->mainPanel = new MainPanel();
	this->configPanel = new ConfigPanel();
	this->trackersPanel = new TrackerPanel();
	this->logPanel = new LogPanel();
	this->aboutPanel = new AboutPanel();

	// Add main Panel
	AddPage(this->mainPanel, ("Main"));

	// Add config Panel
	AddPage(this->configPanel, ("Settings"));

	// Add trackers Panel
	AddPage(this->trackersPanel, ("Trackers"));

	// Add log Panel
	AddPage(this->logPanel, ("Logging"));

	// Add about Panel
	AddPage(this->aboutPanel, ("About"));
}


void Notebook::OnCloseWindow(wxCloseEvent &WXUNUSED(event)) {
	this->mainPanel->Close();
	this->configPanel->Close();
	this->trackersPanel->Close();
	this->logPanel->Close();
	this->aboutPanel->Close();

	Destroy();
}