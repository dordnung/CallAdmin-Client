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
#include <wx/xrc/xmlres.h>

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


// Events
wxDEFINE_EVENT(wxEVT_CALL_DIALOG_CLOSED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_CURL_THREAD_FINISHED, wxCommandEvent);

BEGIN_EVENT_TABLE(CallAdmin, wxApp)
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

	this->startInTaskbar = false;
	this->appEnded = false;

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
	static wxSingleInstanceChecker checkInstance("CallAdmin Client - " + wxGetUserId());

	if (checkInstance.IsAnotherRunning()) {
		wxMessageBox("CallAdmin Client is already running.", "CallAdmin Client", wxOK | wxCENTRE | wxICON_EXCLAMATION);

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

	// Init XML Resources
	wxImage::AddHandler(new wxICOHandler());
	wxImage::AddHandler(new wxBMPHandler());

	wxXmlResource::Get()->InitAllHandlers();
	InitXmlResource();

	// Create Config
	this->config = new Config();

	// TODO: IsconfigSet?
	this->config->ParseConfig();

	// Create MainFrame
	this->mainFrame = new MainFrame();

	// Init Frame
	if (!this->mainFrame->InitFrame(this->startInTaskbar)) {
		ExitProgramm();

		return false;
	}

	// Create CURL Thread
	this->curlThread = new CurlThread();

	// Create Icon
	this->taskBarIcon = new TaskBarIcon();

	// Update Call List
	this->mainFrame->GetNotebook()->GetConfigPanel()->ParseConfig();
	this->mainFrame->GetNotebook()->GetMainPanel()->UpdateCalls();

	CheckUpdate();

	return true;
}


// Clean Up
int CallAdmin::OnExit() {
	ExitProgramm();

	return 0;
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
	if (this->steamThread) {
		delete this->steamThread;
	}

	this->steamThread = new SteamThread();
}


void CallAdmin::StartUpdate() {
	ExitProgramm();

#if defined(__WXMSW__)
	wxExecute(GetRelativePath("calladmin-client-updater.exe"));
#else
	wxExecute(GetRelativePath("calladmin-client-updater"));
#endif
}


// Get the content of a page
void CallAdmin::GetPage(CurlCallback callbackFunction, wxString page, int extra) {
	if (!this->appEnded && !this->curlThread->GetThread()) {
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

	// Stop timer
	this->timer->Stop();

	mainFrame->SetTitle("Couldn't Connect");
	mainFrame->GetNotebook()->GetMainPanel()->SetEventText(error);
	mainFrame->GetNotebook()->GetMainPanel()->SetReconnectButton(true);

	// Show it
	if (!isOtherInFullscreen()) {
		mainFrame->Show(true);
		mainFrame->Restore();
	}

	// Go to first page
	mainFrame->GetNotebook()->GetWindow()->SetSelection(0);
}


// Create an new Error Dialog
void CallAdmin::ShowError(wxString error, wxString type) {
	// Log Action
	LogAction(type + " Error: " + error);

	this->taskBarIcon->ShowMessage("An error occured", type + " Error : " + error + "\nTry again... " + (wxString() << attempts) + "/" + (wxString() << config->GetMaxAttempts()), this->mainFrame);
}


// Close Taskbar Icon and destroy all dialogs
void CallAdmin::ExitProgramm() {
	// App ended now
	this->appEnded = true;

	// Then hide all windows
	if (this->mainFrame) {
		this->mainFrame->Show(false);
	}

	for (wxVector<CallDialog *>::iterator callDialog = callDialogs.begin(); callDialog != callDialogs.end(); ++callDialog) {
		(*callDialog)->Show(false);
	}

	// Curl thread destroy
	delete this->curlThread;
	this->curlThread = NULL;

	// Delete the steam thread
	delete this->steamThread;
	this->steamThread = NULL;

	// Stop the timer
	if (this->timer) {
		this->timer->Stop();

		delete this->timer;
		this->timer = NULL;
	}

	// Taskbar goodbye :)
	if (this->taskBarIcon) {
		this->taskBarIcon->RemoveIcon();
		this->taskBarIcon->Destroy();

		this->taskBarIcon = NULL;
	}

	// First delete call dialogs
	for (wxVector<CallDialog *>::iterator callDialog = callDialogs.begin(); callDialog != callDialogs.end(); ++callDialog) {
		(*callDialog)->Destroy();
	}

	callDialogs.clear();

	// Destroy mainFrame
	if (this->mainFrame) {
		// Delete main frame
		this->mainFrame->Destroy();
		this->mainFrame = NULL;
	}
}


// Get the Path of the App
wxString CallAdmin::GetRelativePath(wxString relativeFileOrPath) {
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
	return path + "\\" + relativeFileOrPath;
#else
	return path + "/" + relativeFileOrPath;
#endif
}


// Check for a new Version
void CallAdmin::CheckUpdate() {
	// Log Action
	LogAction("Checking for a new Update");

	GetPage(CallAdmin::OnUpdate, CALLADMIN_UPDATE_URL);
}


// Handle Update Page
void CallAdmin::OnUpdate(wxString error, wxString result, int WXUNUSED(x)) {
	if (caGetApp().AppEnded()) {
		return;
	}

	// Log Action
	caLogAction("Retrieve information about new version");

	wxString newVersion;

	if (result != "") {
		// Everything good :)
		if (error == "") {
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
			caLogAction("Update check failed: " + error);
			caGetTaskBarIcon()->ShowMessage("Update Check Failed", "Error: " + error, caGetMainFrame());
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
			caGetNotebook()->GetWindow()->SetSelection(4);

			caGetTaskBarIcon()->ShowMessage("New Version available", "New version " + newVersion + " is now available!", caGetMainFrame());
		} else {
			// Log Action
			caLogAction("Version is up to date");

			caGetTaskBarIcon()->ShowMessage("Up to Date", "Your CallAdmin Client is up to date", caGetMainFrame());
		}
	}
}
