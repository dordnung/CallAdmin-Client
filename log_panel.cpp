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


// Events for Log Panel
wxBEGIN_EVENT_TABLE(LogPanel, wxScrolledWindow)
	EVT_CHOICE(XRCID("logLevel"), LogPanel::OnLogLevelUpdate)
wxEND_EVENT_TABLE()


// Load controls
bool LogPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadObject(this, caGetNotebook()->GetWindow(), "logPanel", "wxScrolledWindow")) {
		wxMessageBox("Error: Couldn't find XRCID logPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Ask for LogLevel
	FIND_OR_FAIL(this->logLevel, XRCCTRL(*this, "logLevel", wxChoice), "logLevel");
	this->logLevel->SetSelection((int)caGetConfig()->GetLogLevel());

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
		for (int i = 0; i < this->logBox->GetColumnCount(); i++) {
			this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE);

			// Only on Windows wxLIST_AUTOSIZE_USEHEADER gives the real header width
			#if defined (__WXMSW__)
				// Get the width if autosize with content size
				int contentSize = this->logBox->GetColumnWidth(0);

				// Get the width if autosize with header size
				this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
				int headerSize = this->logBox->GetColumnWidth(0);

				// Use content width if it is higher then the header size
				if (contentSize > headerSize) {
					this->logBox->SetColumnWidth(i, wxLIST_AUTOSIZE);
				}
			#endif
		}
	}
}


// Log Level Updated -> Set Config
void LogPanel::OnLogLevelUpdate(wxCommandEvent &WXUNUSED(event)) {
	// Write to config file
	caGetConfig()->SetLogLevel((LogLevel) this->logLevel->GetSelection());

	// Log Action
	caLogAction("Changed Log Level to " + LogLevelNames[this->logLevel->GetSelection()], LogLevel::LEVEL_INFO);
}
