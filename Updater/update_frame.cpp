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

#ifdef __WXMSW__
// Memory leak detection for debugging 
#include <wx/msw/msvcrt.h>
#endif


#define FIND_OR_FAIL(var, ptr, str) var = ptr; if (var == NULL) {\
	wxMessageBox("Error: Couldn't find XRCID " str, "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this); \
	return false; \
}

// Events for Update Frame
wxBEGIN_EVENT_TABLE(UpdateFrame, wxFrame)
EVT_CLOSE(UpdateFrame::OnCloseWindow)
wxEND_EVENT_TABLE()


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
void UpdateFrame::OnUpdate(wxString infoText, int progress) {
	// Set new Text
	this->dlInfo->SetLabelText(infoText);

	// Update Progress Bar
	this->progressBar->SetValue(progress);

	this->panel->SetSizerAndFit(this->sizerTop, false);
	Fit();
}


// Update Finished
void UpdateFrame::OnFinish(wxString error) {
	// Update Progress Bar
	this->progressBar->SetValue(100);

	// Path
	wxString executablePath = wxGetApp().GetCallAdminExecutablePath();

	// Notice finish
	if (error == "") {
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

		wxMessageBox("Couldn't download update\n" + error, "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this);
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
		wxString error;

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
				error = "Couldn't create file " + newFile->GetName();
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
					// Status should be 200
					long responseCode;
					curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

					if (responseCode != 200) {
						error = "Invalid Response Code: " + wxString() << responseCode;
					}
				} else {
					// Error ):
					error = errorBuffer;
				}

				// Clean Curl
				curl_easy_cleanup(curl);
			}

			// Close File
			newFile->Close();
			delete newFile;
		} else {
			error = "Couldn't init. CURL!";
		}

		// Add Event Handler
		wxGetApp().GetUpdateFrame()->GetEventHandler()->CallAfter(&UpdateFrame::OnFinish, error);
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
			// Set Event Data
			wxString infoText = wxString::Format("%.0f", (dlNow / 1024)) + "kB of " + wxString::Format("%.0f", (dlTotal / 1024)) + "kB (" + wxString::Format("%.0f", ((dlNow / 1024) / curtime)) + "kB/s). Time: " + wxString::Format("%.2f", curtime) + " Seconds";
			int progress = (dlNow / dlTotal) * 100;

			// Add Event Handler
			wxGetApp().GetUpdateFrame()->GetEventHandler()->CallAfter(&UpdateFrame::OnUpdate, infoText, progress);
		}
	}

	return 0;
}
