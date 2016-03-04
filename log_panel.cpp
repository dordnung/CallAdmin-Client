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
	if (!wxXmlResource::Get()->LoadObject(this, caGetNotebook()->GetWindow(), "logPanel", "wxScrolledWindow")) {
		wxMessageBox("Error: Couldn't find XRCID logPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// The log box
	FIND_OR_FAIL(this->logBox, XRCCTRL(*this, "logBox", wxListCtrl), "logBox");

	this->logBox->InsertColumn(0, "Level");
	this->logBox->InsertColumn(1, "Time");
	this->logBox->InsertColumn(2, "Log");

	return true;
}

// Add Action to the logBox
void LogPanel::AddLog(wxString log, LogLevel logLevel) {
	if (logLevel >= caGetConfig()->GetLogLevel()) {
		long item = this->logBox->InsertItem(0, "log");

		this->logBox->SetItem(item, 0, LogLevelNames[logLevel]);
		this->logBox->SetItem(item, 1, wxNow());
		this->logBox->SetItem(item, 2, wxString::FromUTF8(log));

		// Hacky, autosize columns
		for (int i = 0; i < 3; i++) {
			// Get the width if autosize with content size
			this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE);
			int contentSize = this->logBox->GetColumnWidth(0);

			// Get the width if autosize with header size
			this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
			int headerSize = this->logBox->GetColumnWidth(0);

			// Use content width if it is higher then the header size
			if (contentSize > headerSize) {
				this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE);
			}
		}
	}
}
