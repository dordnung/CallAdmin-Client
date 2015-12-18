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

// Include Project
#include "notebook.h"
#include "calladmin-client.h"


// Create Notebook
Notebook::Notebook() : wxNotebook(wxGetApp().GetMainFrame(), wxID_ANY) {
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


/** Panel accessors */
MainPanel* Notebook::GetMainPanel() {
	return this->mainPanel;
}

ConfigPanel* Notebook::GetConfigPanel() {
	return this->configPanel;
}

TrackerPanel* Notebook::GetTrackersPanel() {
	return this->trackersPanel;
}

LogPanel* Notebook::GetLogPanel() {
	return this->logPanel;
}

AboutPanel* Notebook::GetAboutPanel() {
	return this->aboutPanel;
}