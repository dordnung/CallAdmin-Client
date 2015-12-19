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


// Events for Log Panel
BEGIN_EVENT_TABLE(LogPanel, wxPanel)
EVT_CLOSE(LogPanel::OnCloseWindow)
END_EVENT_TABLE()

// Create Log Panel
LogPanel::LogPanel() : wxPanel(caGetNotebook(), wxID_ANY) {
	// Border and Center
	wxSizerFlags flags;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();

	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	this->logBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	this->logBox->SetFont(wxFont(12, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add Log Box
	sizerTop->Add(this->logBox, 1, wxEXPAND);

	// Auto Size
	SetSizerAndFit(sizerTop, true);
}

void LogPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event)) {
	Destroy();
}