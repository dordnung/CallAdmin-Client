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

#include <curl/curlver.h>


// Event ID's
enum {
	wxID_UpdateAbout = wxID_HIGHEST + 40,
	wxID_DownloadAbout,
};


// Button Events for about Panel
BEGIN_EVENT_TABLE(AboutPanel, wxPanel)
EVT_BUTTON(wxID_UpdateAbout, AboutPanel::OnUpdate)
EVT_BUTTON(wxID_DownloadAbout, AboutPanel::OnDownload)
EVT_CLOSE(AboutPanel::OnCloseWindow)
END_EVENT_TABLE()


// Create about Panel
AboutPanel::AboutPanel() : wxPanel(caGetNotebook(), wxID_ANY) {
	// Border and Center
	wxStaticText *text;

	// Create Box
	this->sizerTop = new wxBoxSizer(wxVERTICAL);

	// Call Admin
	text = new wxStaticText(this, wxID_ANY, "The Call Admin Project");

	text->SetFont(wxFont(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	this->sizerTop->Add(text, 0, wxALL &~wxBOTTOM | wxALIGN_CENTRE_HORIZONTAL, 10);

	// By
	text = new wxStaticText(this, wxID_ANY, "Copyright (C) Popoklopsi and Impact 2013-2016");

	text->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	this->sizerTop->Add(text, 0, wxALL &~wxTOP | wxALIGN_CENTRE_HORIZONTAL, 10);


	// Banner
#if defined(__WXMSW__)
	this->sizerTop->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage("calladmin_banner", wxBITMAP_TYPE_RESOURCE))), 0, wxALL | wxALIGN_CENTRE, 10);
#else
	this->sizerTop->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage(caGetApp().GetAppPath("resources/calladmin_banner.bmp")))), flags);
#endif

	// Box for Current Version
	wxSizer* const sizerVersion = new wxBoxSizer(wxHORIZONTAL);

	// Your Version
	text = new wxStaticText(this, wxID_ANY, "Your version: " + (wxString)CALLADMIN_CLIENT_VERSION);

	text->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerVersion->Add(text, 0, wxRIGHT | wxLEFT | wxALIGN_CENTRE_HORIZONTAL, 20);

	// Current Version
	this->currentText = new wxStaticText(this, wxID_ANY, "Current version: " + (wxString)CALLADMIN_CLIENT_VERSION);
	this->currentText->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	this->currentText->SetForegroundColour(wxColour(34, 139, 34));

	// Add it
	sizerVersion->Add(this->currentText, 0, wxLEFT | wxALIGN_CENTRE_HORIZONTAL, 20);


	// Add box
	this->sizerTop->Add(sizerVersion, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 5);


	// Box for Credits
	wxSizer* const sizerCredits = new wxBoxSizer(wxHORIZONTAL);


	// WX
	text = new wxStaticText(this, wxID_ANY, (wxString)wxVERSION_STRING);
	text->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerCredits->Add(text, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 10);


	// Curl
	text = new wxStaticText(this, wxID_ANY, "CURL " + (wxString)LIBCURL_VERSION);
	text->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerCredits->Add(text, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 10);


	// OSW
	text = new wxStaticText(this, wxID_ANY, "OpenSteamWorks 2015/12/11");
	text->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerCredits->Add(text, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 10);


	// Add box
	this->sizerTop->Add(sizerCredits, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 5);


	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Buttons
	sizerBtns->Add(new wxButton(this, wxID_UpdateAbout, "Check For Update"), 0, wxTOP | wxBOTTOM | wxLEFT | wxALIGN_CENTRE, 5);

	this->downloadButton = new wxButton(this, wxID_DownloadAbout, "Download Update");
	this->downloadButton->Enable(false);

	sizerBtns->Add(this->downloadButton, 0, wxTOP | wxBOTTOM | wxRIGHT | wxALIGN_CENTRE, 5);


	// Add Buttons to Box
	this->sizerTop->Add(sizerBtns, 0, wxTOP | wxBOTTOM | wxRIGHT | wxALIGN_CENTRE_HORIZONTAL, 5);


	// Auto Size
	SetSizerAndFit(this->sizerTop, true);
}


void AboutPanel::UpdateVersionText(wxString currentVersion, wxColor color) {
	this->currentText->SetLabelText("Current version: " + currentVersion);
	this->currentText->SetForegroundColour(color);

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

void AboutPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event)) {
	Destroy();
}