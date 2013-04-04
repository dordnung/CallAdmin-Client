/**
 * -----------------------------------------------------
 * File        about.cpp
 * Authors     David <popoklopsi> Ordnung, Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 * 
 * 
 * Copyright (C) 2013 David <popoklopsi> Ordnung, Impact
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
#include "about.h"
#include "main.h"
#include "taskbar.h"
#include "log.h"
#include "calladmin-client.h"


// WX
#include <wx/version.h>

// Curl
#include <curl/curlver.h>

// About Panel
AboutPanel* about = NULL;



// Button ID's for about Panel
enum
{
	wxID_ExitAbout = 5040,
	wxID_UpdateAbout,
	wxID_DownloadAbout,
	wxID_HideAbout,
};


// Button Events for about Panel
BEGIN_EVENT_TABLE(AboutPanel, wxPanel)
	EVT_BUTTON(wxID_ExitAbout, AboutPanel::OnExit)
	EVT_BUTTON(wxID_UpdateAbout, AboutPanel::OnUpdate)
	EVT_BUTTON(wxID_DownloadAbout, AboutPanel::OnDownload)
	EVT_BUTTON(wxID_HideAbout, AboutPanel::OnHide)
END_EVENT_TABLE()



// Create about Panel
AboutPanel::AboutPanel(wxNotebook* note) : wxPanel(note, wxID_ANY)
{
	// Set about Panel
	about = this;

	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);


	// Call Admin
	text = new wxStaticText(this, wxID_ANY, "The Call Admin Project");

	text->SetFont(wxFont(28, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxALL &~ wxBOTTOM | wxALIGN_CENTRE_HORIZONTAL, 10);



	// By
	text = new wxStaticText(this, wxID_ANY, "Copyright (C) Popoklopsi and Impact 2013");

	text->SetFont(wxFont(13, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxALL &~ wxTOP | wxALIGN_CENTRE_HORIZONTAL, 10);




	// Banner
	#if defined(__WXMSW__)
		sizerTop->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage("calladmin_banner", wxBITMAP_TYPE_RESOURCE))), flags);
	#else
		sizerTop->Add(new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage(getAppPath("resources/calladmin_banner.bmp")))), flags);
	#endif


	// Box for Current Version
	wxSizer* const sizerVersion = new wxBoxSizer(wxHORIZONTAL);


	// Your Version
	text = new wxStaticText(this, wxID_ANY, "       Your version: " + version);

	text->SetFont(wxFont(15, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerVersion->Add(text, 0, wxRIGHT | wxLEFT | wxALIGN_CENTRE_HORIZONTAL, 50);



	// Current Version
	currentText = new wxStaticText(this, wxID_ANY, "Current version: " + version);
	currentText->SetFont(wxFont(15, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	currentText->SetForegroundColour(wxColour(34, 139, 34));

	// Add it
	sizerVersion->Add(currentText, 0, wxLEFT | wxALIGN_CENTRE_HORIZONTAL, 50);



	// Update Button
	downloadButton = new wxButton(this, wxID_DownloadAbout, "Download Update");
	downloadButton->Enable(false);

	// Add it
	sizerVersion->Add(downloadButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTRE_HORIZONTAL, 5);


	// Add box
	sizerTop->Add(sizerVersion, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 5);



	// Credits, static direction oO, but it works :)
	text = new wxStaticText(this, wxID_ANY, (wxString)wxVERSION_STRING + "               CURL " + LIBCURL_VERSION + "               OSW 07.03.2013");

	text->SetFont(wxFont(12, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 5);


	
	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_HideAbout, "Hide"), flags.Border(wxALL &~ wxRIGHT, 5));
	sizerBtns->Add(new wxButton(this, wxID_UpdateAbout, "Check Update"), flags.Border(wxALL &~ wxRIGHT &~ wxLEFT, 5));
	sizerBtns->Add(new wxButton(this, wxID_ExitAbout, "Exit"), flags.Border(wxALL &~ wxLEFT, 5));



	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Auto Size
	SetSizerAndFit(sizerTop, true);
}



// Button Event -> Exit programm
void AboutPanel::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}


// Button Event -> Check for Update
void AboutPanel::OnUpdate(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Check for a new Update");

	checkUpdate();
}


// Button Event -> Download Update
void AboutPanel::OnDownload(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Download new Update");

	#if defined(__WXMSW__)
		ShellExecute(NULL, L"open", s2ws("http://popoklopsi.de/calladmin/calladmin-client.zip").c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("xdg-open http://popoklopsi.de/calladmin/calladmin-client.zip").c_str());
	#endif
}



// Button Event -> Hide to Taskbar
void AboutPanel::OnHide(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Hide to taskbar");

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

		main_dialog->Show(false);
	}
	else
	{
		main_dialog->Iconize(true);
	}
}