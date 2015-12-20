/**
 * -----------------------------------------------------
 * File        calladmin-client-updater.cpp
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

#include <curl/curl.h>
#include <sstream>

#include <wx/cmdline.h>
#include <wx/snglinst.h>
#include <wx/stdpaths.h>

#include "calladmin-client-updater.h"


// Help for the CMDLine
static const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_OPTION, "version", NULL, "Specify current Call Admin Client version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_OPTION, "url", NULL, "URL with new Call Admin version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_OPTION, "executable", NULL, "URL to Call Admin Client executable", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_NONE }
};


// Implement the APP
IMPLEMENT_APP(CallAdminUpdater)


// App Started
bool CallAdminUpdater::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}

	// Check duplicate
	static wxSingleInstanceChecker checkInstance("Call Admin Updater - " + wxGetUserId());

	if (checkInstance.IsAnotherRunning()) {
		wxMessageBox("Call Admin Client Updater is already running.", "Call Admin Client Updater", wxOK | wxCENTRE | wxICON_WARNING);

		return false;
	}

	// No update found? -> Delete App
	if (!CheckForUpdate()) {
		ExitProgramm();

		return false;
	}

	return true;
}


// Clean Up
int CallAdminUpdater::OnExit() {
	ExitProgramm();

	return 1;
}


// Set Help text
void CallAdminUpdater::OnInitCmdLine(wxCmdLineParser &parser) {
	// Add Help
	parser.SetDesc(cmdLineDesc);

	// Start with -
	parser.SetSwitchChars("-");
}


// Find -version XXX
bool CallAdminUpdater::OnCmdLineParsed(wxCmdLineParser &parser) {
	parser.Found("version", &this->callAdminVersion);
	parser.Found("url", &this->callAdminUrl);
	parser.Found("executable", &this->callAdminExecutable);

	return true;
}


// Get a relative from current path
wxString CallAdminUpdater::GetPath(wxString file) {
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	// Windows format?
	size_t start = path.find_last_of("\\");

	if (start == 0 || start == wxString::npos) {
		// No... Linux Format ;)
		start = path.find_last_of("/");
	}

	// Strip app name
	path = path.replace(start, path.size(), "");

	// Add file name
#if defined(__WXMSW__)
	return path + "\\" + file;
#else
	return path + "/" + file;
#endif
}


// Check for a new Version
bool CallAdminUpdater::CheckForUpdate() {
	// Response
	std::ostringstream stream;

	// Init Curl
	CURL *curl = curl_easy_init();

	if (curl != NULL) {
		// Error
		char ebuf[CURL_ERROR_SIZE + 1];

		// Configurate Curl
		curl_easy_setopt(curl, CURLOPT_URL, this->callAdminUrl.mb_str().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

		// Perform Curl
		CURLcode res = curl_easy_perform(curl);

		// Clean Curl
		curl_easy_cleanup(curl);

		if (res == CURLE_OK) {
			return OnGetVersion(wxString(), stream.str());
		}

		// Error ):
		return OnGetVersion(ebuf, stream.str());
	}

	return OnGetVersion(wxString(), wxString());
}


void CallAdminUpdater::StartUpdate() {
	this->updateDialog = new UpdateDialog();
}


void CallAdminUpdater::OnUpdateDialogClosed() {
	this->updateDialog = NULL;

	ExitProgramm();
}


// End App
void CallAdminUpdater::ExitProgramm() {
	if (!this->appEnded) {
		this->appEnded = true;

		// Disappear update dialog
		if (this->updateDialog != NULL) {
			this->updateDialog->Show(false);
		}

		// Close update dialog
		if (this->updateDialog != NULL) {
			this->updateDialog->Close();
		}

		if (wxMessageBox("Updater ended.\nStart Call Admin Client again?", "End Of Update", wxCANCEL | wxYES_NO | wxCENTRE | wxICON_QUESTION, NULL) == wxYES) {
			wxExecute(this->GetCallAdminPath());
		}
	}
}


// Handle Update Page
bool CallAdminUpdater::OnGetVersion(wxString error, wxString result) {
	wxString newVersion;

	if (error == "") {
		// Everything good :)
		if (result != "") {
			if (result.length() > 30) {
				// Maybe an Error Page?
				wxMessageBox("Error: Result page is too long", "Update Check Failed", wxOK | wxCENTRE | wxICON_EXCLAMATION);
			} else {
				// Find version in brackets
				int start = result.find_first_of("{") + 1;
				int end = result.find_first_of("}");

				newVersion = result.substr(start, end - start);
			}
		} else {
			wxMessageBox("Error: Version check is empty", "Update Check Failed", wxOK | wxCENTRE | wxICON_ERROR);
		}
	} else {
		wxMessageBox("Error: " + error, "Update Check Failed", wxOK | wxCENTRE | wxICON_ERROR);
	}

	// We got something
	if (newVersion != "") {
		// Check Version
		if (newVersion != wxGetApp().GetCallAdminVersion()) {
			wxGetApp().StartUpdate();

			return true;
		} else {
			wxMessageBox("Your Call Admin Client is up to date", "Up To Date", wxOK | wxCENTRE | wxICON_INFORMATION);
		}
	}

	return false;
}


// Curl receive data -> write to buffer
size_t CurlWriteData(void *buffer, size_t size, size_t nmemb, void *userp) {
	std::ostringstream *data = (std::ostringstream*) userp;

	if (buffer != NULL) {
		size_t count = size * nmemb;

		data->write((char*)buffer, count);

		return count;
	}

	return (size_t)-1;
}