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

#include "update_dialog.h"
#include "calladmin-client-updater.h"

#include <wx/ffile.h>
#include <wx/xrc/xmlres.h>


wxDEFINE_EVENT(wxEVT_THREAD_UPDATE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_THREAD_FINISHED, wxCommandEvent);

// Events for Update Dialog
BEGIN_EVENT_TABLE(UpdateDialog, wxDialog)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_UPDATE, UpdateDialog::OnUpdate)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_FINISHED, UpdateDialog::OnFinish)

EVT_CLOSE(UpdateDialog::OnCloseWindow)
END_EVENT_TABLE()


// Open Update Dialog
UpdateDialog::UpdateDialog() {
	this->dlinfo = NULL;
	this->dlstatus = NULL;
	this->sizerTop = NULL;
	this->panel = NULL;
	this->progressBar = NULL;

	wxXmlResource::Get()->LoadDialog(this, NULL, "updateDialog");

	// Panel
	this->panel = XRCCTRL(*this, "updatePanel", wxPanel);

	// sizer
	this->sizerTop = this->panel->GetSizer();

	// sizer
	this->progressBar = XRCCTRL(*this->panel, "progressBar", wxGauge);

	// Download Info
	this->dlinfo = XRCCTRL(*this->panel, "dlinfo", wxStaticText);

	// Download Status
	this->dlstatus = XRCCTRL(*this->panel, "dlstatus", wxStaticText);

	// Set Icon
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
#else
	SetIcon(wxIcon(wxGetApp().GetPath("rc/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
#endif

	Fit();

	Show(true);

	// Now start the Update
	CreateThread(wxTHREAD_DETACHED);
	GetThread()->Run();
}


UpdateDialog::~UpdateDialog() {
	// Notice app that update is finished
	wxGetApp().OnUpdateDialogClosed();
}


// Progress Update
void UpdateDialog::OnUpdate(wxCommandEvent &event) {
	// Set new Text
	this->dlinfo->SetLabelText(event.GetString());

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
	wxString executablePath = wxGetApp().GetCallAdminPath();

	// Notice finish
	if (event.GetString() == "") {
		// Renaming Files
		if (wxFileExists(executablePath)) {
			wxRenameFile(executablePath, executablePath + ".old");
		}

		if (wxFileExists(executablePath + ".new")) {
			wxRenameFile(executablePath + ".new", executablePath);
		}

		// Refresh Status
		this->dlstatus->SetLabelText("Status: Finished!");
		this->dlstatus->SetForegroundColour(wxColour(34, 139, 34));
	} else {
		// Refresh Status
		this->dlstatus->SetLabelText("Status: Error!");
		this->dlstatus->SetForegroundColour(wxColour("red"));

		wxMessageBox("Couldn't download update\n " + event.GetString(), "Error on Update", wxOK | wxCENTRE | wxICON_INFORMATION, this);
	}

	// Fit Window
	this->panel->SetSizerAndFit(sizerTop, false);
	Fit();

	wxGetApp().ExitProgramm();
}


// Window Event -> Disable Window
void UpdateDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	// Thread still there?
	if (GetThread()) {
		if (GetThread()->IsRunning()) {
			wxMessageBox("Please wait until download is finished", "Please Wait", wxOK | wxCENTRE | wxICON_INFORMATION, this);
			return;
		}

		GetThread()->Delete();
	}

	// Wait until thread is finished
	while (1) {
		if (!GetThread()) {
			break;
		}

		wxThread::This()->Sleep(1);
	}

	// Delete .new file
	if (wxFileExists(wxGetApp().GetCallAdminPath() + ".new")) {
		wxRemoveFile(wxGetApp().GetCallAdminPath() + ".new");
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
			wxFFile *newFile;

			// Error
			char errorBuffer[CURL_ERROR_SIZE + 1];

			// Path
			wxString executablePath = wxGetApp().GetCallAdminPath();

			// Open File
			newFile = new wxFFile(executablePath + ".new", "wb");

			if (newFile == NULL || !newFile->IsOpened()) {
				event.SetString("Couldn't create file " + executablePath + ".new");
			} else {
				// Reset Prog time
				progress.curl = curl;

				// Configurate Curl
				curl_easy_setopt(curl, CURLOPT_URL, wxGetApp().GetCallAdminExecutable().mb_str().data());
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
				curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
				curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
				curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataToFile);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, newFile->fp());
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
				newFile->Close();
			}

			// Add Event Handler
			wxGetApp().GetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);

			return (wxThread::ExitCode)0;
		}

		event.SetString("Couldn't init. CURL!");

		// Add Event Handler
		wxGetApp().GetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);
	}

	return (wxThread::ExitCode)0;
}


// Write Data to file
size_t WriteDataToFile(void *data, size_t size, size_t nmemb, FILE *file) {
	// Write
	if (data != NULL) {
		return fwrite(data, size, nmemb, file);
	}

	return (size_t)-1;
}


// Progress Updated
int ProgressUpdated(void *progressPointer, double dltotal, double dlnow, double WXUNUSED(ultotal), double WXUNUSED(ulnow)) {
	// Get progress struct
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
		wxGetApp().GetUpdateDialog()->GetEventHandler()->AddPendingEvent(event);
	}

	return 0;
}
