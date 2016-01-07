/**
 * -----------------------------------------------------
 * File        about_panel.cpp
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
#include "about_panel.h"
#include "calladmin-client.h"

#include <wx/version.h>
#include <wx/xrc/xmlres.h>

#include <curl/curlver.h>


// Button Events for about Panel
BEGIN_EVENT_TABLE(AboutPanel, wxPanel)
EVT_BUTTON(XRCID("checkForUpdate"), AboutPanel::OnUpdate)
EVT_BUTTON(XRCID("downloadUpdate"), AboutPanel::OnDownload)
END_EVENT_TABLE()


// Create about Panel
bool AboutPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadPanel(this, caGetNotebook()->GetWindow(), "aboutPanel")) {
		wxMessageBox("Error: Couldn't find XRCID mainPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	this->currentVersion = wxString(CALLADMIN_CLIENT_VERSION);

	// Sizer
	FIND_OR_FAIL(this->sizerTop, this->GetSizer(), "aboutPanelSizerTop");

	// Help value for static texts
	wxStaticText *text;

	// Your Version
	FIND_OR_FAIL(text, XRCCTRL(*this, "yourVersion", wxStaticText), "yourVersion");
	text->SetLabel(text->GetLabel() + (wxString)CALLADMIN_CLIENT_VERSION);

	// Current Version
	FIND_OR_FAIL(this->currentText, XRCCTRL(*this, "currentVersion", wxStaticText), "currentVersion");
	this->currentText->SetLabel(this->currentText->GetLabel() + (wxString)CALLADMIN_CLIENT_VERSION);

	// WX Version
	FIND_OR_FAIL(text, XRCCTRL(*this, "wxVersion", wxStaticText), "wxVersion");
	text->SetLabel(text->GetLabel() + (wxString)wxVERSION_STRING);

	// Curl Version
	FIND_OR_FAIL(text, XRCCTRL(*this, "curlVersion", wxStaticText), "curlVersion");
	text->SetLabel(text->GetLabel() + (wxString)LIBCURL_VERSION);

	// OSW Version
	FIND_OR_FAIL(text, XRCCTRL(*this, "oswVersion", wxStaticText), "oswVersion");
	text->SetLabel(text->GetLabel() + "2015/12/11");

	// Download Button
	FIND_OR_FAIL(this->downloadButton, XRCCTRL(*this, "downloadUpdate", wxButton), "downloadUpdate");

	// Auto Size
	SetSizerAndFit(this->sizerTop, true);

	return true;
}


void AboutPanel::UpdateVersionText(wxString currentVersion, wxColor color) {
	wxString oldText = this->currentText->GetLabel();
	oldText.Replace(this->currentVersion, currentVersion);

	this->currentText->SetLabel(oldText);
	this->currentText->SetForegroundColour(color);

	this->currentVersion = currentVersion;

	this->sizerTop->Layout();
}


// Button Event -> Check for Update
void AboutPanel::OnUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Check for a new Update");
	caGetApp().CheckUpdate();
}


// Button Event -> Download Update
void AboutPanel::OnDownload(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Download new Update");
	caGetApp().StartUpdate();
}