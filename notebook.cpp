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
	// Init panels
	for (wxVector<Panel *>::iterator panel = this->panels.begin(); panel != this->panels.end(); ++panel) {
		if (!(*panel)->InitPanel()) {
			return false;
		}
	}

	// Add pages
	for (wxVector<Panel *>::iterator panelIt = this->panels.begin(); panelIt != this->panels.end(); ++panelIt) {
		Panel *panel = *panelIt;
		this->window->InsertPage(panel->GetPage(), panel->GetPanel(), panel->GetLabel());
	}

	// Go to first page
	this->window->ChangeSelection(0);

	return true;
}

Notebook::~Notebook() {
	this->window->Destroy();
}
