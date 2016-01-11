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
	if (!wxXmlResource::Get()->LoadFrame(this, NULL, "frame")) {
		wxMessageBox("Error: Couldn't find XRCID frame", "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this);

		return false;
	}

	// Panel
	FIND_OR_FAIL(this->panel, XRCCTRL(*this, "panel", wxPanel), "panel");

	// Sizer
	FIND_OR_FAIL(this->sizerTop, this->panel->GetSizer(), "topSizer");

	// Progress bar
	FIND_OR_FAIL(this->progressBar, XRCCTRL(*this->panel, "progressBar", wxGauge), "progressBar");

	// Download Size
	FIND_OR_FAIL(this->downloadedSize, XRCCTRL(*this->panel, "downloadedSize", wxStaticText), "downloadedSize");

	// Total Size
	FIND_OR_FAIL(this->downloadedSizeTotal, XRCCTRL(*this->panel, "downloadedSizeTotal", wxStaticText), "downloadedSizeTotal");

	// Speed
	FIND_OR_FAIL(this->speed, XRCCTRL(*this->panel, "speed", wxStaticText), "speed");

	// Elapsed Time
	FIND_OR_FAIL(this->elapsedTime, XRCCTRL(*this->panel, "elapsedTime", wxStaticText), "elapsedTime");

	// Remaining Time
	FIND_OR_FAIL(this->remainingTime, XRCCTRL(*this->panel, "remainingTime", wxStaticText), "remainingTime");

	// Show frame
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
void UpdateFrame::OnUpdate(UpdateInfo updateInfo) {
	// Update Progress Bar
	this->progressBar->SetValue((updateInfo.downloadedSize / updateInfo.totalSize) * 100);

	// Update download info
	double speed = (updateInfo.downloadedSize / 1024) / updateInfo.currentTime;

	this->downloadedSize->SetLabelText(wxString::Format("%.0f", updateInfo.downloadedSize / 1024));
	this->downloadedSizeTotal->SetLabelText(wxString::Format("%.0f", updateInfo.totalSize / 1024));
	this->speed->SetLabelText(wxString::Format("%.0f", speed));
	this->elapsedTime->SetLabelText(wxString::Format("%.2f", updateInfo.currentTime));
	this->remainingTime->SetLabelText(wxString::Format("%.2f", ((updateInfo.totalSize - updateInfo.downloadedSize) / 1024) / speed));

	this->sizerTop->Layout();
	this->sizerTop->Fit(this);
}


// Update Finished
void UpdateFrame::OnFinish(wxString error) {
	// Show correct finish state
	this->progressBar->SetValue(100);
	this->remainingTime->SetLabelText("0.00");

	// Path
	wxString executablePath = wxGetApp().GetCallAdminExecutablePath();

	// Notice finish
	if (error == "") {
		// Renaming Files
		if (wxFileExists(executablePath) && wxFileExists(executablePath + ".new")) {
			wxRenameFile(executablePath, executablePath + ".old");
			wxRenameFile(executablePath + ".new", executablePath);
		}

		wxMessageBox("Update finished successfully", "Update finished", wxOK | wxCENTRE | wxICON_INFORMATION, this);
	} else {
		wxMessageBox("Couldn't download update\n" + error, "Error on Update", wxOK | wxCENTRE | wxICON_ERROR, this);
	}

	// Fit Window
	this->sizerTop->Layout();
	this->sizerTop->Fit(this);

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

	// Update only every 50 times
	if (++numUpdates % 50 == 0) {
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
			UpdateInfo updateInfo = { dlTotal, dlNow, curtime };

			// Add Event Handler
			wxGetApp().GetUpdateFrame()->GetEventHandler()->CallAfter(&UpdateFrame::OnUpdate, updateInfo);
		}
	}

	return 0;
}
