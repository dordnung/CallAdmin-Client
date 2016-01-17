/**
 * -----------------------------------------------------
 * File        log_panel.cpp
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
#include "log_panel.h"
#include "calladmin-client.h"

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Load controls
bool LogPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "logPanel")) {
		wxMessageBox("Error: Couldn't find XRCID logPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Box
	FIND_OR_FAIL(this->logBox, XRCCTRL(*this, "logBox", wxListBox), "logBox");

	return true;
}

// Add Action to the logBox
void LogPanel::AddLog(wxString log, LogLevel logLevel) {
	if (caGetApp().IsRunning() && this->logBox) {
		this->logBox->SetSelection(this->logBox->Append("[" + LogLevelNames[logLevel] + "] " + wxString::FromUTF8(wxNow() + " - " + log)));
	}
}