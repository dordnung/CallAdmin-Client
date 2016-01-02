/**
 * -----------------------------------------------------
 * File        calladmin-client.cpp
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

#include <wx/cmdline.h>
#include <wx/snglinst.h>
#include <wx/stdpaths.h>

#include "calladmin-client.h"
#include "curl_util.h"


// Help for the CMDLine
static const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, "taskbar", "taskbar", "Move GUI to taskbar on Start" },
	{ wxCMD_LINE_NONE }
};


// Implement the APP
IMPLEMENT_APP(CallAdmin)


wxDEFINE_EVENT(wxEVT_UPDATE_DIALOG_CLOSED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_CALL_DIALOG_CLOSED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_CURL_THREAD_FINISHED, wxCommandEvent);

// Events
BEGIN_EVENT_TABLE(CallAdmin, wxApp)
EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_DIALOG_CLOSED, CallAdmin::OnUpdateDialogClosed)
EVT_COMMAND(wxID_ANY, wxEVT_CURL_THREAD_FINISHED, CallAdmin::OnCurlThread)
END_EVENT_TABLE()


CallAdmin::CallAdmin() {
	// Reset vars
	this->timer = NULL;
	this->config = NULL;
	this->mainFrame = NULL;
	this->taskBarIcon = NULL;
	this->steamThread = NULL;
	this->curlThread = NULL;
	this->updateDialog = NULL;

	this->startInTaskbar = false;

	// Attempts to Zero
	this->attempts = 0;

	// Avatar Size
	this->avatarSize = 184;
}


// App Started
bool CallAdmin::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}

	// Check duplicate
	static wxSingleInstanceChecker checkInstance("Call Admin Client - " + wxGetUserId());

	if (checkInstance.IsAnotherRunning()) {
		wxMessageBox("Call Admin Client is already running.", "Call Admin Client", wxOK | wxCENTRE | wxICON_EXCLAMATION);

		return false;
	}

	// Calculate avatar size
	int y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

	// Set Avatar Size
	if (y < 900) {
		// Only use small avatars
		this->avatarSize = 128;
	} else if (y < 700) {
		this->avatarSize = 96;
	} else if (y < 600) {
		this->avatarSize = 64;
	}

	// Create CURL Thread
	this->curlThread = new CurlThread();

	// Create Icon
	this->taskBarIcon = new TaskBarIcon();

	// Create Config
	this->config = new Config();
	this->config->ParseConfig();

	// Delete .old file
	remove(wxStandardPaths::Get().GetExecutablePath() + ".old");
	remove(wxStandardPaths::Get().GetExecutablePath() + ".new");

	// Create main Dialog
	this->mainFrame = new MainFrame("Call Admin Client");
	this->mainFrame->CreateWindow(this->startInTaskbar);

	// Update Call List
	this->mainFrame->GetNotebook()->GetConfigPanel()->ParseConfig();
	this->mainFrame->GetNotebook()->GetMainPanel()->UpdateCalls();

	CheckUpdate();

	return true;
}


// Clean Up
int CallAdmin::OnExit() {
	ExitProgramm();

	return 1;
}


// Set Help text
void CallAdmin::OnInitCmdLine(wxCmdLineParser &parser) {
	// Add Help
	parser.SetDesc(cmdLineDesc);

	// Start with -
	parser.SetSwitchChars("-");
}


// Find -taskbar
bool CallAdmin::OnCmdLineParsed(wxCmdLineParser &parser) {
	this->startInTaskbar = parser.Found("taskbar");

	return true;
}


void CallAdmin::OnUpdateDialogClosed(wxCommandEvent &WXUNUSED(event)) {
	this->updateDialog = NULL;
}


// Curl thread handled -> Call function
void CallAdmin::OnCurlThread(wxCommandEvent &event) {
	// Get Content
	CurlThreadData *data = static_cast<CurlThreadData *>(event.GetClientObject());

	// Get Function
	CurlCallback function = data->GetCallbackFunction();

	// Call it
	function(data->GetError(), data->GetContent(), data->GetExtra());

	// Delete data
	delete data;
}


void CallAdmin::StartTimer() {
	if (this->timer) {
		this->timer->Stop();
		delete this->timer;
	}

	this->attempts = 0;

	this->timer = new Timer();
	this->timer->Run(this->config->GetStep() * 1000);
}


void CallAdmin::StartSteamThread() {
	delete this->steamThread;
	this->steamThread = new SteamThread();
}


void CallAdmin::StartUpdate() {
	this->updateDialog = new UpdateDialog();
}


// Get the content of a page
void CallAdmin::GetPage(CurlCallback callbackFunction, wxString page, int extra) {
	if (!this->curlThread->GetThread()) {
		this->curlThread->SetCallbackFunction(callbackFunction);
		this->curlThread->SetPage(page);
		this->curlThread->SetExtra(extra);

		this->curlThread->CreateThread(wxTHREAD_DETACHED);
		this->curlThread->GetThread()->Run();
	}
}


// Create the Window as a reconnecter
void CallAdmin::CreateReconnect(wxString error) {
	// Log Action
	LogAction("Create a reconnect window");

	mainFrame->SetTitle("Couldn't Connect");
	mainFrame->GetNotebook()->GetMainPanel()->SetEventText(error);
	mainFrame->GetNotebook()->GetMainPanel()->SetReconnectButton(true);

	// Show it
	if (!isOtherInFullscreen()) {
		mainFrame->Show(true);
		mainFrame->Restore();
	}

	// Go to first page
	mainFrame->GetNotebook()->SetSelection(0);

	// Stop timer
	this->timer->Stop();
}


// Create an new Error Dialog
void CallAdmin::ShowError(wxString error, wxString type) {
	// Log Action
	LogAction(type + " Error: " + error);

	this->taskBarIcon->ShowMessage("An error occured", type + " Error : " + error + "\nTry again... " + (wxString() << attempts) + "/" + (wxString() << config->GetMaxAttempts()), this->mainFrame);
}


// Close Taskbar Icon and destroy all dialogs
void CallAdmin::ExitProgramm() {
	// At first hide all windows
	if (this->mainFrame != NULL) {
		this->mainFrame->Show(false);
	}

	if (this->updateDialog != NULL) {
		this->updateDialog->Show(false);
	}

	for (wxVector<CallDialog *>::iterator callDialog = callDialogs.begin(); callDialog != callDialogs.end(); ++callDialog) {
		(*callDialog)->Show(false);
	}

	// First of all stop the timer
	if (this->timer != NULL) {
		this->timer->Stop();
		delete this->timer;
	}

	// Curl thread destroy
	delete this->curlThread;

	// Delete zhe steam thread
	delete this->steamThread;

	// Taskbar goodbye :)
	this->taskBarIcon->RemoveIcon();
	this->taskBarIcon->Destroy();

	// First delete update dialog
	if (this->updateDialog != NULL) {
		this->updateDialog->Close();
	}

	// Then delete call dialogs
	for (wxVector<CallDialog *>::iterator callDialog = callDialogs.begin(); callDialog != callDialogs.end(); ++callDialog) {
		(*callDialog)->Destroy();
	}

	callDialogs.clear();

	// Delete notebook
	this->mainFrame->GetNotebook()->Close();

	// Delete main frame
	if (this->mainFrame != NULL) {
		this->mainFrame->Destroy();
	}
}


// Get the Path of the App
wxString CallAdmin::GetAppPath(wxString file) {
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
	return path + "/" + file;
}


// Check for a new Version
void CallAdmin::CheckUpdate() {
	// Log Action
	LogAction("Checking for a new Update");

	GetPage(CallAdmin::OnUpdate, CALLADMIN_UPDATE_URL);
}


// Handle Update Page
void CallAdmin::OnUpdate(char* error, wxString result, int WXUNUSED(x)) {
	// Log Action
	caLogAction("Retrieve information about new version");

	wxString newVersion;

	if (result != "") {
		// Everything good :)
		if (strcmp(error, "") == 0) {
			if (result.length() > 30) {
				// Maybe an Error Page?
				caGetTaskBarIcon()->ShowMessage("Update Check Failed", "Error: PAGE_TOO_LONG", caGetMainFrame());
			} else {
				// Find version in brackets
				int start = result.find_first_of("{") + 1;
				int end = result.find_first_of("}");

				newVersion = result.substr(start, end - start);
			}
		} else {
			// Log Action
			caLogAction("Update check failed: " + (wxString)error);
			caGetTaskBarIcon()->ShowMessage("Update Check Failed", "Error: " + (wxString)error, caGetMainFrame());
		}
	}

	// We got something
	if (newVersion != "") {
		// Check Version
		if (newVersion != CALLADMIN_CLIENT_VERSION) {
			// Log Action
			caLogAction("Found a new Version: " + newVersion);

			// Update About Panel
			caGetAboutPanel()->EnableDownload(true);
			caGetAboutPanel()->UpdateVersionText(newVersion, wxColor("red"));

			// Show Main, but only if no other app is in fullscreen
			if (!isOtherInFullscreen()) {
				caGetMainFrame()->Show(true);
				caGetMainFrame()->Restore();
			}

			// Goto About
			caGetNotebook()->SetSelection(4);

			caGetTaskBarIcon()->ShowMessage("New Version available", "New version " + newVersion + " is now available!", caGetMainFrame());
		} else {
			// Log Action
			caLogAction("Version is up to date");

			caGetTaskBarIcon()->ShowMessage("Up to Date", "Your Call Admin Client is up to date", caGetMainFrame());
		}
	}
}
