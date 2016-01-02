/**
* -----------------------------------------------------
* File        update_frame.cpp
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

#include "update_frame.h"
#include "calladmin_client_updater.h"

#include <wx/ffile.h>
#include <wx/xrc/xmlres.h>


#define FIND_OR_FAIL(var, ptr, str) var = ptr;\
if (var == NULL) {\
	wxMessageBox("Error: Couldn't find XRCID " ## str, "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this); \
	return false; \
}


wxDEFINE_EVENT(wxEVT_THREAD_UPDATE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_THREAD_FINISHED, wxCommandEvent);

// Events for Update Frame
BEGIN_EVENT_TABLE(UpdateFrame, wxFrame)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_UPDATE, UpdateFrame::OnUpdate)
EVT_COMMAND(wxID_ANY, wxEVT_THREAD_FINISHED, UpdateFrame::OnFinish)

EVT_CLOSE(UpdateFrame::OnCloseWindow)
END_EVENT_TABLE()


// Open Update Frame
bool UpdateFrame::ShowFrame() {
	if (!wxXmlResource::Get()->LoadFrame(this, NULL, "updateFrame")) {
		wxMessageBox("Error: Couldn't find XRCID updateFrame", "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this);

		return false;
	}

	// Panel
	FIND_OR_FAIL(this->panel, XRCCTRL(*this, "updatePanel", wxPanel), "updatePanel");

	// Sizer
	FIND_OR_FAIL(this->sizerTop, this->panel->GetSizer(), "sizer");

	// Progress bar
	FIND_OR_FAIL(this->progressBar, XRCCTRL(*this->panel, "progressBar", wxGauge), "progressBar");

	// Download Info
	FIND_OR_FAIL(this->dlInfo, XRCCTRL(*this->panel, "dlInfo", wxStaticText), "dlInfo");

	// Download Status
	FIND_OR_FAIL(this->dlStatus, XRCCTRL(*this->panel, "dlStatus", wxStaticText), "dlStatus");

	Fit();
	Show(true);

	// Now start the Update
	CreateThread(wxTHREAD_DETACHED);
	GetThread()->Run();

	return true;
}


UpdateFrame::~UpdateFrame() {
	// Notice app that update is finished
	wxGetApp().OnUpdateFrameClosed();
}


// Progress Update
void UpdateFrame::OnUpdate(wxCommandEvent &event) {
	// Set new Text
	this->dlInfo->SetLabelText(event.GetString());

	// Update Progress Bar
	this->progressBar->SetValue(event.GetInt());

	this->panel->SetSizerAndFit(this->sizerTop, false);
	Fit();
}


// Update Finished
void UpdateFrame::OnFinish(wxCommandEvent &event) {
	// Update Progress Bar
	this->progressBar->SetValue(100);

	// Path
	wxString executablePath = wxGetApp().GetCallAdminExecutablePath();

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
		this->dlStatus->SetLabelText("Status: Finished");
		this->dlStatus->SetForegroundColour(wxColour(34, 139, 34));

		wxMessageBox("Update finished successfully", "Update finished", wxOK | wxCENTRE | wxICON_INFORMATION, this);
	} else {
		// Refresh Status
		this->dlStatus->SetLabelText("Status: Error!");
		this->dlStatus->SetForegroundColour(wxColour("red"));

		wxMessageBox("Couldn't download update\n " + event.GetString(), "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this);
	}

	// Fit Window
	this->panel->SetSizerAndFit(this->sizerTop, false);
	Fit();

	wxGetApp().ExitProgramm();
}


// Window Event -> Disable Window
void UpdateFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
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
	if (wxFileExists(wxGetApp().GetCallAdminExecutablePath() + ".new")) {
		wxRemoveFile(wxGetApp().GetCallAdminExecutablePath() + ".new");
	}

	Destroy();
}


// Curl Thread started
wxThread::ExitCode UpdateFrame::Entry() {
	if (!GetThread()->TestDestroy()) {
		// Event
		wxCommandEvent event(wxEVT_THREAD_FINISHED);

		// Init Curl
		CURL *curl = curl_easy_init();

		if (curl != NULL) {
			// Error
			char errorBuffer[CURL_ERROR_SIZE + 1];

			// Path
			wxString executablePath = wxGetApp().GetCallAdminExecutablePath();

			// Open File
			wxFFile *newFile = new wxFFile(executablePath + ".new", "wb");

			if (!newFile->IsOpened()) {
				event.SetString("Couldn't create file " + newFile->GetName());
			} else {
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
				curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, curl);

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
			}

			// Close File
			newFile->Close();
		} else {
			event.SetString("Couldn't init. CURL!");
		}

		// Add Event Handler
		wxGetApp().GetUpdateFrame()->GetEventHandler()->AddPendingEvent(event);
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
int ProgressUpdated(void *curlPointer, double dlTotal, double dlNow, double WXUNUSED(ulTotal), double WXUNUSED(ulNow)) {
	static int numUpdates = 0;

	// Update only every 10 times
	if (++numUpdates % 10 == 0) {
		// Get curl
		CURL *curl = (CURL *)curlPointer;

		// valid?
		if (curl == NULL) {
			return -1;
		}

		// Get Current time
		double curtime = 0;
		curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

		// Valid Data?
		if (dlTotal > 0) {
			// Event
			wxCommandEvent event(wxEVT_THREAD_UPDATE);

			// Set Event Data
			event.SetString(wxString::Format("%.0f", (dlNow / 1024)) + "kB of " + wxString::Format("%.0f", (dlTotal / 1024)) + "kB (" + wxString::Format("%.0f", ((dlNow / 1024) / curtime)) + "kB/s). Time: " + wxString::Format("%.2f", curtime) + " Seconds");
			event.SetInt((dlNow / dlTotal) * 100);

			// Add Event Handler
			wxGetApp().GetUpdateFrame()->GetEventHandler()->AddPendingEvent(event);
		}
	}

	return 0;
}
