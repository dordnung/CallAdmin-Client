/**
* -----------------------------------------------------
* File        update_dialog.cpp
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

// Includes Project
#include "calladmin-client.h"
#include <wx/stdpaths.h>


// Button ID's for Update Dialog
enum {
	wxID_UpdateUpdated = wxID_HIGHEST + 60,
	wxID_UpdateFinish
};


wxDEFINE_EVENT(wxEVT_THREAD_UPDATE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_THREAD_FINISHED, wxCommandEvent);

// Events for Update Dialog
BEGIN_EVENT_TABLE(UpdateDialog, wxDialog)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_UPDATE, UpdateDialog::OnUpdate)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_FINISHED, UpdateDialog::OnFinish)

EVT_CLOSE(UpdateDialog::OnCloseWindow)
END_EVENT_TABLE()


// Open Update Dialog
UpdateDialog::UpdateDialog() : wxDialog(NULL, wxID_ANY, "Update CallAdmin", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMINIMIZE_BOX) {
	// Action
	caLogAction("Start downloading Update");

	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);

	// Panel
	panel = new wxPanel(this, wxID_ANY);

	// Border and Center
	wxSizerFlags flags;


	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// Create Progress Bar
	progressBar = new wxGauge(panel, wxID_ANY, 100);

	// Add it
	sizerTop->Add(progressBar, 0, wxALL | wxEXPAND, 10);

	// Download Info
	dlinfo = new wxStaticText(panel, wxID_ANY, "0000kB of 0000KB (0000 kB/s). Time: 0,00 Seconds");

	dlinfo->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	sizerTop->Add(dlinfo, flags.Border(wxALL, 10));


	// Download Status
	dlstatus = new wxStaticText(panel, wxID_ANY, "Status: Downloading...");

	dlstatus->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	// Add it
	sizerTop->Add(dlstatus, flags.Border(wxALL, 10));


	// Auto Size
	panel->SetSizerAndFit(sizerTop, true);

	// Fit
	Fit();

	// Centre to Screen
	Centre();

	// Set Icon
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
#else
	wxLogNull nolog;

	SetIcon(wxIcon(caGetApp().GetAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
#endif

	Show(true);

	// Now start the Update
	CreateThread(wxTHREAD_DETACHED);
	GetThread()->Run();
}


UpdateDialog::~UpdateDialog() {
	// Notice app that update is finished
	wxCommandEvent event(wxEVT_UPDATE_DIALOG_CLOSED);

	caGetApp().AddPendingEvent(event);
}


// Progress Update
void UpdateDialog::OnUpdate(wxCommandEvent &event) {
	// Set new Text
	this->dlinfo->SetLabelText(event.GetString());
	this->sizerTop->Layout();

	// Update Progress Bar
	this->progressBar->SetValue(event.GetInt());

	this->panel->SetSizerAndFit(this->sizerTop, false);

	Fit();
}


// Update Finished
void UpdateDialog::OnFinish(wxCommandEvent &event) {
	// Update Progress Bar
	this->progressBar->SetValue(100);

	// Path
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	// Notice finish
	if (event.GetString() == "") {
		// Renaming Files
		// TODO: Activate again
		//rename(path, path + ".old");
		//rename(path + ".new", path);

		// Refresh Status
		this->dlstatus->SetLabelText("Status: Finished!");
		this->dlstatus->SetForegroundColour(wxColour(34, 139, 34));
		this->sizerTop->Layout();

		caGetTaskBarIcon()->ShowMessage("Update Finished", "Downloading update is finished\nPlease restart your Call Admin Client", this);
	} else {
		// Refresh Status
		this->dlstatus->SetLabelText("Status: Error!");
		this->dlstatus->SetForegroundColour(wxColour("red"));
		this->sizerTop->Layout();

		caGetTaskBarIcon()->ShowMessage("Error on Update", "Couldn't download update\n" + event.GetString(), this);
	}

	// Fit Window
	this->panel->SetSizerAndFit(sizerTop, false);

	Fit();
}


// Window Event -> Disable Window
void UpdateDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	// Thread still there?
	if (GetThread()) {
		// And also running?
		if (GetThread()->IsRunning()) {
			// Update is still running
			caGetTaskBarIcon()->ShowMessage("Please Wait", "Please wait until download is finished", this);

			return;
		}

		GetThread()->Delete();
	}

	// Wait until thread is finished
	while (1) {
		if (!this->GetThread()) {
			break;
		}

		wxThread::This()->Sleep(1);
	}

	Destroy();
}


// Curl Thread started
wxThread::ExitCode UpdateDialog::Entry() {
	if (!GetThread()->TestDestroy()) {
		// Event
		wxCommandEvent event(wxEVT_THREAD_FINISHED);

		// dl Progress
		struct dlProgress progress;

		// Init Curl
		CURL *curl = curl_easy_init();

		if (curl != NULL) {
			FILE *filePointer;

			// Error
			char errorBuffer[CURL_ERROR_SIZE + 1];

			// Path
			wxString path = wxStandardPaths::Get().GetExecutablePath();

			// Open File
			filePointer = fopen(path + ".new", "wb");

			if (filePointer == NULL) {
				event.SetString("Couldn't create file " + path + ".new");
			} else {
				// Reset Prog time
				progress.curl = curl;

				// Configurate Curl
				curl_easy_setopt(curl, CURLOPT_URL, CALLADMIN_UPDATE_EXE);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
				curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
				curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, caGetConfig()->GetTimeout());
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
				curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataToFile);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, filePointer);
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressUpdated);
				curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);

				// Perform Curl
				CURLcode res = curl_easy_perform(curl);

				// Everything good :)
				if (res == CURLE_OK) {
					event.SetString(wxString());
				} else {
					// Error ):
					event.SetString(errorBuffer);
				}

				// Clean Curl
				curl_easy_cleanup(curl);

				// Close File
				fclose(filePointer);
			}

			// Add Event Handler
			caGetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);

			return (wxThread::ExitCode)0;
		}

		event.SetString("Couldn't init. CURL!");

		// Add Event Handler
		caGetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);
	}

	return (wxThread::ExitCode)0;
}


// Write Data to file
size_t WriteDataToFile(void *filePointer, size_t size, size_t nmemb, FILE *stream) {
	// Write
	if (filePointer != NULL) {
		return fwrite(filePointer, size, nmemb, stream);
	}

	return (size_t)-1;
}


// Progress Updated
int ProgressUpdated(void *progressPointer, double dltotal, double dlnow, double WXUNUSED(ultotal), double WXUNUSED(ulnow)) {
	// Get progress strucht
	struct dlProgress *progress = (struct dlProgress *)progressPointer;

	// valid?
	if (progress == NULL) {
		return -1;
	}

	// Get Curl
	CURL *curl = progress->curl;

	// Get Current time
	double curtime = 0;
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

	// Valid Data?
	if (dltotal > 0) {
		// Event
		wxCommandEvent event(wxEVT_THREAD_UPDATE);

		// Set Event Data
		event.SetString(wxString::Format("%.0f", (dlnow / 1024)) + "kB of " + wxString::Format("%.0f", (dltotal / 1024)) + "kB (" + wxString::Format("%.0f", ((dlnow / 1024) / curtime)) + "kB/s). Time: " + wxString::Format("%.2f", curtime) + " Seconds");
		event.SetInt((dlnow / dltotal) * 100);

		// Add Event Handler
		caGetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);
	}

	return 0;
}
