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

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
// Memory leak detection for debugging 
#include <wx/msw/msvcrt.h>
#endif


// Loads the pages of the notebook
bool Notebook::CreatePages() {
	this->mainPanel = new MainPanel();
	this->configPanel = new ConfigPanel();
	this->trackerPanel = new TrackerPanel();
	this->logPanel = new LogPanel();
	this->aboutPanel = new AboutPanel();

	// Init panels
	if (!(this->mainPanel->InitPanel() && this->configPanel->InitPanel() &&
		this->trackerPanel->InitPanel() && this->logPanel->InitPanel() && this->aboutPanel->InitPanel())) {
		return false;
	}

	this->window->AddPage(this->mainPanel, "Main", true);
	this->window->AddPage(this->configPanel, "Settings");
	this->window->AddPage(this->trackerPanel, "Trackers");
	this->window->AddPage(this->logPanel, "Logging");
	this->window->AddPage(this->aboutPanel, "About");

	return true;
}

Notebook::~Notebook() {
	this->mainPanel->Destroy();
	this->configPanel->Destroy();
	this->trackerPanel->Destroy();
	this->logPanel->Destroy();
	this->aboutPanel->Destroy();

	this->window->Destroy();
}