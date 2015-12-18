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

// Curl
#include "curl_util.h"

// We need a xml parser
#include "tinyxml2/tinyxml2.h"

// Command line arguments
#include <wx/cmdline.h>

// Sound Notification
#include <wx/sound.h>

// Only one Instance
#include <wx/snglinst.h>

// Path utils
#include <wx/stdpaths.h>

// Project
#include "calladmin-client.h"


// We need something to print for a XML Error!
wxString XMLErrorString[] =
{
	"XML_NO_ERROR",

	"XML_NO_ATTRIBUTE",
	"XML_WRONG_ATTRIBUTE_TYPE",

	"XML_ERROR_FILE_NOT_FOUND",
	"XML_ERROR_FILE_COULD_NOT_BE_OPENED",
	"XML_ERROR_FILE_READ_ERROR",
	"XML_ERROR_ELEMENT_MISMATCH",
	"XML_ERROR_PARSING_ELEMENT",
	"XML_ERROR_PARSING_ATTRIBUTE",
	"XML_ERROR_IDENTIFYING_TAG",
	"XML_ERROR_PARSING_TEXT",
	"XML_ERROR_PARSING_CDATA",
	"XML_ERROR_PARSING_COMMENT",
	"XML_ERROR_PARSING_DECLARATION",
	"XML_ERROR_PARSING_UNKNOWN",
	"XML_ERROR_EMPTY_DOCUMENT",
	"XML_ERROR_MISMATCHED_ELEMENT",
	"XML_ERROR_PARSING",

	"XML_CAN_NOT_CONVERT_TEXT",
	"XML_NO_TEXT_NODE"
};


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

// Events
BEGIN_EVENT_TABLE(CallAdmin, wxApp)
EVT_COMMAND(wxID_ANY, wxEVT_CALL_DIALOG_CLOSED, CallAdmin::OnCallDialogClosed)
EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_DIALOG_CLOSED, CallAdmin::OnUpdateDialogClosed)
END_EVENT_TABLE()


CallAdmin::CallAdmin() {
	// Reset vars
	this->timer = NULL;
	this->config = NULL;
	this->mainFrame = NULL;
	this->taskBarIcon = NULL;
	this->steamThread = NULL;
	this->updateDialog = NULL;

	this->startInTaskbar = false;

	// Attempts to Zero
	this->attempts = 0;

	// Avatar Size
	this->avatarSize = 184;

	// program ended already?
	this->end = false;
}


// App Started
bool CallAdmin::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}

	// Check duplicate
	static wxSingleInstanceChecker checkInstance("Call Admin - " + wxGetUserId());

	if (checkInstance.IsAnotherRunning()) {
		wxMessageBox("Call Admin is already running.", "Call Admin", wxOK | wxCENTRE | wxICON_EXCLAMATION);

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


void CallAdmin::OnCallDialogClosed(wxCommandEvent &WXUNUSED(event)) {

}


void CallAdmin::OnUpdateDialogClosed(wxCommandEvent &WXUNUSED(event)) {
	this->updateDialog = NULL;
}


void CallAdmin::StartTimer() {
	if (this->timer == NULL) {
		this->timer = new Timer();
	}

	if (this->timer->IsRunning()) {
		this->timer->Stop();
	}

	this->attempts = 0;
	this->timer->Start(this->config->GetStep() * 1000);
}


void CallAdmin::StartSteamThread() {
	if (this->steamThread != NULL) {
		steamThread->Delete();
	}

	this->steamThread = new SteamThread();
}


void CallAdmin::StartUpdate() {
	this->updateDialog = new UpdateDialog();
}


UpdateDialog* CallAdmin::GetUpdateDialog() {
	return this->updateDialog;
}


SteamThread* CallAdmin::GetSteamThread() {
	return this->steamThread;
}


Config* CallAdmin::GetConfig() {
	return this->config;
}


MainFrame* CallAdmin::GetMainFrame() {
	return this->mainFrame;
}


TaskBarIcon* CallAdmin::GetTaskBarIcon() {
	return this->taskBarIcon;
}


wxVector<CallDialog *>* CallAdmin::GetCallDialogs() {
	return &this->callDialogs;
}


int CallAdmin::GetAvatarSize() {
	return this->avatarSize;
}


int CallAdmin::GetAttempts() {
	return this->attempts;
}


void CallAdmin::SetAttempts(int attempts) {
	this->attempts = attempts;
}


void CallAdmin::OnNotice(char* error, wxString result, int firstRun) {
	// Valid result?
	if (result != "") {
		// Everything good :)
		if (strcmp(error, "") == 0) {
			bool foundError = false;
			bool foundNew = false;

			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parse Error?
			if (parseError != tinyxml2::XML_SUCCESS) {
				foundError = true;
				caGetApp().SetAttempts(caGetApp().GetAttempts() + 1);

				// Log Action
				caLogAction("Found a XML Error: " + XMLErrorString[parseError]);

				// Max attempts reached?
				if (caGetApp().GetAttempts() == caGetConfig()->GetMaxAttempts()) {
					// Close Dialogs and create reconnect main dialog
					caGetApp().CreateReconnect("XML Error: " + XMLErrorString[parseError]);
				} else {
					// Create Parse Error
					caGetApp().ShowError(XMLErrorString[parseError], "XML");
				}
			}

			// No error so far, yeah!
			if (!foundError) {
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin
				if (node != NULL) {
					node = node->NextSibling();
				}

				// New Calls?
				if (node != NULL) {
					// Init. Call List
					int foundRows = 0;

					// Only if first run
					if (firstRun) {
						for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling()) {
							// Search for foundRows
							if ((wxString)node2->Value() == "foundRows") {
								// Get Rows
								foundRows = wxAtoi(node2->FirstChild()->Value());

								// Go on
								break;
							}
						}
					}

					for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling()) {
						// API Error?
						if ((wxString)node2->Value() == "error") {
							foundError = true;
							caGetApp().SetAttempts(caGetApp().GetAttempts() + 1);

							// Max attempts reached?
							if (caGetApp().GetAttempts() == caGetConfig()->GetMaxAttempts()) {
								// Close Dialogs and create reconnect main dialog
								caGetApp().CreateReconnect("API Error: " + (wxString)node2->FirstChild()->Value());
							} else {
								// API Errpr
								caGetApp().ShowError(node2->FirstChild()->Value(), "API");
							}

							break;
						}

						// Row Count
						if ((wxString)node2->Value() == "foundRows") {
							continue;
						}

						int dialog = foundRows - 1;

						// First run
						if (!firstRun) {
							dialog = caGetCallDialogs()->size();
						}

						// Api is fine :)
						int found = 0;

						// Create the new CallDialog
						CallDialog *newDialog = new CallDialog("New Incoming Call");

						// Put in ALL needed DATA
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling()) {
							if ((wxString)node3->Value() == "callID") {
								found++;
								newDialog->SetCallID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "fullIP") {
								found++;
								newDialog->SetIP(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "serverName") {
								found++;
								newDialog->SetName(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetName") {
								found++;
								newDialog->SetTarget(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetID") {
								found++;
								newDialog->SetTargetID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetReason") {
								found++;
								newDialog->SetReason(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientName") {
								found++;
								newDialog->SetClient(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientID") {
								found++;
								newDialog->SetClientID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "reportedAt") {
								found++;
								newDialog->SetTime(wxAtol(node3->FirstChild()->Value()));
							}

							if ((wxString)node3->Value() == "callHandled") {
								found++;
								newDialog->SetHandled(strcmp(node3->FirstChild()->Value(), "1") == 0);
							}
						}

						bool foundDuplicate = false;

						// Check duplicate Entries
						int index = -1;

						for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
							index++;

							// Operator overloading :)
							if ((**callDialog) == (*newDialog)) {
								foundDuplicate = true;

								// Call is now handled
								if (newDialog->GetHandled() && !(*callDialog)->GetHandled()) {
									caGetMainPanel()->SetHandled(index);
								}

								// That's enough
								break;
							}
						}

						// Found all necessary items?
						if (found != 10 || foundDuplicate) {
							// Something went wrong or duplicate
							newDialog->Destroy();
						} else {
							// New call
							foundNew = true;

							// Add the new Call to the Call box
							char buffer[80];

							wxString text;

							// But first we need a Time
							time_t tt = (time_t)newDialog->GetTime();

							struct tm* dt = localtime(&tt);

							strftime(buffer, sizeof(buffer), "%H:%M", dt);

							newDialog->SetTitle("Call At " + (wxString)buffer);

							text = (wxString)buffer + " - " + newDialog->GetServer();

							// Add the Text
							newDialog->SetBoxText(text);

							// Now START IT!
							newDialog->SetID(dialog);

							// Don't show calls on first Run
							if (firstRun) {
								foundRows--;
								newDialog->StartCall(false);
							} else {
								// Log Action
								caLogAction("We have a new Call");
								newDialog->StartCall(caGetConfig()->GetIsAvailable() && !isOtherInFullscreen());
							}

							newDialog->GetTakeoverButton()->Enable(!newDialog->GetHandled());

							caGetCallDialogs()->push_back(newDialog);
						}
					}
				}
			}

			// Everything is good, set attempts to zero
			if (!foundError) {
				// Reset attempts
				caGetApp().SetAttempts(0);

				// Updated Main Interface
				caGetMainFrame()->SetTitle("Call Admin Client");
				caGetMainPanel()->SetEventText("Waiting for a new report...");

				// Update Call List
				if (foundNew) {
					// Update call list
					caGetMainPanel()->UpdateCalls();

					// Play Sound
					if (caGetConfig()->GetWantSound() && !firstRun && caGetConfig()->GetIsAvailable()) {
						wxSound* soundfile;
#if defined(__WXMSW__)
						soundfile = new wxSound("calladmin_sound", true);
#else
						wxLogNull nolog;

						soundfile = new wxSound(getAppPath("resources/calladmin_sound.wav"), false);
#endif
						if (soundfile != NULL && soundfile->IsOk()) {
							soundfile->Play(wxSOUND_ASYNC);

							// Clean
							delete soundfile;
						}
					}
				}
			}
		} else {
			// Something went wrong ):
			caGetApp().SetAttempts(caGetApp().GetAttempts() + 1);

			// Log Action
			caLogAction("New CURL Error: " + (wxString)error);

			// Max attempts reached?
			if (caGetApp().GetAttempts() == caGetConfig()->GetMaxAttempts()) {
				// Create reconnect main dialog
				caGetApp().CreateReconnect("CURL Error: " + (wxString)error);
			} else {
				// Show the error to client
				caGetApp().ShowError(error, "CURL");
			}
		}
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
	mainFrame->Show(true);
	mainFrame->Restore();

	// Go to first page
	mainFrame->GetNotebook()->SetSelection(0);

	// Stop timer
	this->timer->Stop();
}


// Create an new Error Dialog
void CallAdmin::ShowError(wxString error, wxString type) {
	// Log Action
	LogAction(type + " Error: " + error);

	if (this->taskBarIcon != NULL) {
		this->taskBarIcon->ShowMessage("An error occured", type + " Error : " + error + "\nTry again... " + (wxString() << attempts) + "/" + (wxString() << config->GetMaxAttempts()), this->mainFrame);
	}
}


// Close Taskbar Icon and destroy all dialogs
void CallAdmin::ExitProgramm() {
	// Prevent double end
	if (!this->end) {
		// Mark as ended
		this->end = true;

		// First disappear Windows
		this->mainFrame->Show(false);

		// No more Update dialog needed
		this->updateDialog->Show(false);

		// Timer... STOP!
		timer->Stop();
		delete timer;

		// Taskbar goodbye :)
		this->taskBarIcon->RemoveIcon();
		this->taskBarIcon->Destroy();

		// No more Main dialog needed
		mainFrame->Destroy();

		// No more Update dialog needed
		updateDialog->Destroy();

		// Calls are unimportant
		for (wxVector<CallDialog *>::iterator callDialog = callDialogs.begin(); callDialog != callDialogs.end(); ++callDialog) {
			CallDialog *dialog = *callDialog;

			// Stop Avatar Timer
			if (dialog->GetAvatarTimer() != NULL && dialog->GetAvatarTimer()->IsRunning()) {
				dialog->GetAvatarTimer()->Stop();
				delete dialog->GetAvatarTimer();
			}

			dialog->Destroy();
		}


		// We don't need Steam support
		this->steamThread->Delete();
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

	CurlThread::GetPage(CallAdmin::OnUpdate, CALLADMIN_UPDATE_URL);
}


void CallAdmin::LogAction(wxString action) {
	this->mainFrame->GetNotebook()->GetLogPanel()->AddLog(action);
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
			caGetAboutPanel()->UpdateVersion(newVersion, wxColor("red"));

			// Show Main
			if (!isOtherInFullscreen()) {
				caGetMainFrame()->Show(true);
				caGetMainFrame()->Restore();
			}

			// Goto About
			caGetNotebook()->SetSelection(4);

			caGetTaskBarIcon()->ShowMessage("New Version", "New version " + newVersion + " is now available!", caGetMainFrame());
		} else {
			// Log Action
			caLogAction("Version is up to date");

			caGetTaskBarIcon()->ShowMessage("Up to Date", "Your Call Admin Client is up to date", caGetMainFrame());
		}
	}
}
