/**
 * -----------------------------------------------------
 * File        calladmin-client.cpp
 * Authors     David Ordnung, Impact
 * License     GPLv3
 * Web         http://dordnung.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung, Impact
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


// c++ libs
#include <string>
#include <sstream>
#include <ctime>


// Curl
#include <curl/curl.h>

// We need a xml parser
#include "tinyxml2/tinyxml2.h"

// Command line arguments
#include <wx/cmdline.h>

// TextCtrl
#include <wx/textctrl.h>

// checkBox
#include <wx/checkbox.h>

// SpinCtrl
#include <wx/spinctrl.h>

// ToolTip
#include <wx/tooltip.h>

// ListBox
#include <wx/listbox.h>

// Static Line
#include <wx/statline.h>

// Sound Notification
#include <wx/sound.h>

// Only one Instance
#include <wx/snglinst.h>


// Project
#include "calladmin-client.h"
#include "config.h"
#include "main.h"
#include "log.h"
#include "update.h"
#include "trackers.h"
#include "about.h"
#include "call.h"
#include "taskbar.h"


// Timer
Timer *timer = NULL;



// Attempts to Zero
int attempts = 0;


// Avatar Size
int avatarSize = 184;


// program ended already?
bool end = false;


// Version
wxString version = "0.48B";
std::string updateURL = "http://dordnung.de/sourcemod/calladmin/version.txt";



// We need something to print for a XML Error!
wxString XMLErrorString[20] = 
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
static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
		{wxCMD_LINE_SWITCH, "taskbar", "taskbar", "Move GUI to taskbar on Start"},
		{wxCMD_LINE_NONE}
};




// Timer already run?
bool timerStarted = false;


// First fetch time
time_t firstFetch;


// Implement the APP
IMPLEMENT_APP(CallAdmin)


// Default no taskbar start
bool CallAdmin::start_taskbar = false;




// App Started
bool CallAdmin::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}


	// Check duplicate
	static wxSingleInstanceChecker checkInstance("Call Admin - " + wxGetUserId());

	if (checkInstance.IsAnotherRunning())
	{
		wxMessageBox("Call Admin is already running.", "Call Admin", wxOK | wxCENTRE | wxICON_EXCLAMATION);

		return false;
	}


	// Create Config
	g_config = new wxConfig("Call Admin");

	// Valid?
	if (g_config == NULL)
	{
		return false;
	}


	int y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

	// Set Avatar Size
	if (y < 900 && y >= 700)
	{
		// Only use small avatars
		avatarSize = 128;
	}

	else if (y < 700)
	{
		avatarSize = 96;
	}

	else if (y < 600)
	{
		avatarSize = 64;
	}


	// First set Steamid to not known
	steamid = "";

	// Add Icon if available
	if (wxTaskBarIcon::IsAvailable())
	{
		m_taskBarIcon = new TaskBarIcon();
	}

	// Reset Calls
	for (int i=0; i < MAXCALLS; i++)
	{
		call_dialogs[i] = NULL;
	}


	// Delete .old file
	remove(wxStandardPaths::Get().GetExecutablePath() + ".old");
	remove(wxStandardPaths::Get().GetExecutablePath() + ".new");

	// Create main Dialog
	main_dialog = new MainDialog("Call Admin Client");


	// Valid?
	if (main_dialog == NULL)
	{
		return false;
	}
	
	main_dialog->createWindow(start_taskbar);


	return true;
}




// Destroy
CallAdmin::~CallAdmin()
{
	exitProgramm();
}



// Set Help text
void CallAdmin::OnInitCmdLine(wxCmdLineParser& parser)
{
	// Add Help
	parser.SetDesc(g_cmdLineDesc);

	// Start with -
	parser.SetSwitchChars("-");
}
 


// Find -tasbar
bool CallAdmin::OnCmdLineParsed(wxCmdLineParser& parser)
{
	start_taskbar = parser.Found("taskbar");
 
	return true;
}




// Timer events
BEGIN_EVENT_TABLE(Timer, wxTimer)
	EVT_TIMER(1, Timer::update)
END_EVENT_TABLE()


// Run the timer
void Timer::run(int milliSecs)
{
	// Log Action
	LogAction("Start the Timer");

	Start(milliSecs);
}

// Timer executed
void Timer::update(wxTimerEvent& WXUNUSED(event))
{
	// Check for Update
	if (!timerStarted)
	{
		checkUpdate();
	}


	std::string pager;


	// Page
	if (!timerStarted)
	{
		pager = (page + "/notice.php?from=0&from_type=unixtime&key=" + key + "&sort=desc&limit=" + (wxString() << lastCalls));
	}
	else
	{
		pager = (page + "/notice.php?from=" + (wxString() << (step * 2)) + "&from_type=interval&key=" + key + "&sort=asc&handled=" + (wxString() << (time(0) - firstFetch)));
	}


	// Store Player
	if (main_dialog != NULL && main_dialog->wantStore())
	{
		pager = pager + "&store=1&steamid=" + steamid;
	}
	
	// Get the Page
	getPage(onNotice, pager);
}




void onNotice(char* error, wxString result, int WXUNUSED(x))
{
	bool firstRun = false;

	// First Run?
	if (!timerStarted)
	{
		firstFetch = time(0);

		timerStarted = true;
		firstRun = true;
	}

	// Valid result?
	if (result != "")
	{
		// Everything good :)
		if (strcmp(error, "") == 0)
		{
			bool foundError = false;
			bool foundNew = false;

			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parse Error?
			if (parseError != tinyxml2::XML_SUCCESS)
			{
				foundError = true;
				attempts++;

				// Log Action
				LogAction("Found a XML Error: " + (wxString)XMLErrorString[parseError]);

				// Max attempts reached?
				if (attempts == maxAttempts)
				{
					// Close Dialogs and create reconnect main dialog
					createReconnect("XML Error: " + (wxString)XMLErrorString[parseError]);
				}
				else
				{
					// Create Parse Error
					showError(XMLErrorString[parseError], "XML");
				}
			}

			// No error so far, yeah!
			if (!foundError)
			{
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin
				if (node != NULL)
				{
					node = node->NextSibling();
				}
				
				// New Calls?
				if (node != NULL)
				{
					// Init. Call List
					int foundRows = 0;

					// Only if first run
					if (firstRun)
					{
						for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling())
						{
							// Search for foundRows
							if ((wxString)node2->Value() == "foundRows")
							{
								// Get Rows
								std::stringstream rows(node2->FirstChild()->Value());

								rows >> foundRows;
							
								// Go on
								break;
							}
						}
					}


					for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling())
					{
						// API Error?
						if ((wxString)node2->Value() == "error")
						{
							foundError = true;
							attempts++;

							// Max attempts reached?
							if (attempts == maxAttempts)
							{
								// Close Dialogs and create reconnect main dialog
								createReconnect("API Error: " + (wxString)node2->FirstChild()->Value());
							}
							else
							{
								// API Errpr
								showError((wxString)node2->FirstChild()->Value(), "API");
							}

							break;
						}

						// Row Count
						if ((wxString)node2->Value() == "foundRows")
						{
							continue;
						}


						int dialog = -1;


						// Normal Stepp
						if (!firstRun)
						{
							// Look for a free place
							for (int i=0; i < MAXCALLS; i++)
							{
								if (call_dialogs[i] == NULL)
								{
									dialog = i;

									break;
								}
							}

							// Everything is full, so clear Everything, client's problem oO, MAXCALLS is enough!
							if (dialog == -1)
							{
								for (int i=0; i < MAXCALLS; i++)
								{
									if (call_dialogs[i] != NULL)
									{
										call_dialogs[i]->Destroy();
										call_dialogs[i] = NULL;
									}
								}

								dialog = 0;
							}
						}
						else
						{
							// First run, update call list
							dialog = foundRows - 1;
						}


						// Api is fine :)
						int found = 0;


						// Create the new CallDialog
						CallDialog *newDialog = new CallDialog("New Incoming Call");


						// Valid?
						if (newDialog == NULL)
						{
							return;
						}


						// Put in ALL needed DATA
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling())
						{
							if ((wxString)node3->Value() == "callID")
							{
								found++;
								newDialog->setCallID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "fullIP")
							{
								found++;
								newDialog->setIP(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "serverName")
							{
								found++;
								newDialog->setName(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetName")
							{
								found++;
								newDialog->setTarget(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetID")
							{
								found++;
								newDialog->setTargetID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetReason")
							{
								found++;
								newDialog->setReason(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientName")
							{
								found++;
								newDialog->setClient(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientID")
							{
								found++;
								newDialog->setClientID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "reportedAt")
							{
								found++;
								newDialog->setTime(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "callHandled")
							{
								found++;
								newDialog->setHandled(strcmp(node3->FirstChild()->Value(), "1") == 0);
							}
						}

						bool findDuplicate = false;


						// Check duplicate Entries
						for (int i=0; i < MAXCALLS; i++)
						{
							if (i != dialog && call_dialogs != NULL)
							{
								if (call_dialogs[i] != NULL)
								{
									// Operator overloading :)
									if ((*call_dialogs[i]) == (*newDialog))
									{
										findDuplicate = true;

										// Call is now handled
										if (newDialog->getHandled() && !call_dialogs[i]->getHandled())
										{
											main_dialog->setHandled(i);
										}

										// That's enough
										break;
									}
								}
							}
						}

						// Found all necessary items?
						if (found != 10 || findDuplicate)
						{
							// Something went wrong or duplicate
							newDialog->Destroy();
						}
						else
						{
							// New call
							foundNew = true;

							// Add the new Call to the Call box
							char buffer[80];

							wxString text;

							// But first we need a Time
							time_t tt = (time_t)newDialog->getTime();

							struct tm* dt = localtime(&tt);

							strftime(buffer, sizeof(buffer), "%H:%M", dt);


							newDialog->SetTitle("Call At " + (wxString)buffer);


							text = (wxString)buffer + " - " + newDialog->getServer();

							// Add the Text
							newDialog->setBoxText(text);

							// Now START IT!
							newDialog->setID(dialog);


							// Don't show calls on first Run
							if (firstRun)
							{
								foundRows--;
								newDialog->startCall(false);
							}
							else
							{
								// Log Action
								LogAction("We have a new Call");
								newDialog->startCall(main_dialog->isAvailable() && !isOtherInFullscreen());
							}

							newDialog->takeover->Enable(!newDialog->getHandled());

							call_dialogs[dialog] = newDialog;
						}
					}
				}
			}

			// Everything is good, set attempts to zero
			if (!foundError && main_dialog != NULL)
			{
				// Reset attempts
				attempts = 0;

				// Updated Main Interface
				main_dialog->SetTitle("Call Admin Client");
				main_dialog->setEventText("Waiting for a new report...");

				// Update Call List
				if (foundNew)
				{
					// Update call list
					main_dialog->updateCall();

					// Play Sound
					if (main_dialog->wantSound() && !firstRun && main_dialog->isAvailable())
					{
						wxSound* soundfile;

						#if defined(__WXMSW__)
							soundfile = new wxSound("calladmin_sound", true);
						#else
							wxLogNull nolog;

							soundfile = new wxSound(getAppPath("resources/calladmin_sound.wav"), false);
						#endif
						
						if (soundfile != NULL && soundfile->IsOk())
						{
							soundfile->Play(wxSOUND_ASYNC);

							// Clean
							delete soundfile;
						}
					}
				}
			}
		}
		else
		{
			// Something went wrong ):
			attempts++;

			// Log Action
			LogAction("New CURL Error: " + (std::string)error);

			// Max attempts reached?
			if (attempts == maxAttempts)
			{
				// Create reconnect main dialog
				createReconnect("CURL Error: " + (std::string)error);
			}
			else
			{
				// Show the error to client
				showError(error, "CURL");
			}
		}
	}
}




// Curl Thread started
wxThread::ExitCode curlThread::Entry()
{
	if (!TestDestroy())
	{
		// Event
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_ThreadHandled);

		// Response
		std::ostringstream stream;

		// Init Curl
		CURL *curl = curl_easy_init();
		
		if (curl != NULL)
		{
			// Error
			char ebuf[CURL_ERROR_SIZE];
			
			
			// Configurate Curl
			curl_easy_setopt(curl, CURLOPT_URL, ((std::string)page).c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout*2);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

			// Perform Curl
			CURLcode res = curl_easy_perform(curl);

			// Everything good :)
			if (res == CURLE_OK)
			{
				event.SetClientObject(new ThreadData(function, stream.str(), "", x));
			}
			else
			{
				// Error ):
				event.SetClientObject(new ThreadData(function, stream.str(), ebuf, x));
			}

			// Clean Curl
			curl_easy_cleanup(curl);

			// Add Event Handler
			if (main_dialog != NULL)
			{
				main_dialog->GetEventHandler()->AddPendingEvent(event);
			}

			return (wxThread::ExitCode)0;
		}

		event.SetClientObject(new ThreadData(function, "", "", x));

		// Add Event Handler
		if (main_dialog != NULL)
		{
			main_dialog->GetEventHandler()->AddPendingEvent(event);
		}
	}

	return (wxThread::ExitCode)0;
}




// Curl receive data -> write to buffer
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	std::ostringstream *data = (std::ostringstream*)userp;

	if (data != NULL)
	{
		size_t count = size * nmemb;

		data->write((char*)buffer, count);

		return count;
	}

	return (size_t) -1;
}







// Get Page
void getPage(callback function, wxString page, int x)
{
	new curlThread(function, page, x);
}







// Create the Window as a reconnecter
void createReconnect(wxString error)
{
	// Valid?
	if (main_dialog == NULL || notebook == NULL)
	{
		return;
	}

	// Log Action
	LogAction("Create a reconnect window");

	main_dialog->SetTitle("Couldn't Connect");
	main_dialog->setEventText(error);
	main_dialog->setReconnectButton(true);

	// Show it
	main_dialog->Show(true);
	main_dialog->Restore();

	// Go to first page
	notebook->SetSelection(0);

	// Stop timer
	timer->Stop();
}






// Create an new Error Dialog
void showError(wxString error, wxString type)
{
	// Log Action
	LogAction(type + " Error: " + error);

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("An error occured", type + " Error : " + error + "\nTry again... " + (wxString() << attempts) + "/" + (wxString() << maxAttempts), main_dialog);
	}
}





// Close Taskbar Icon and destroy all dialogs
void exitProgramm()
{
	if (!end)
	{
		// Mark as ended
		end = true;

		// First disappear Windows
		if (main_dialog != NULL)
		{
			main_dialog->Show(false);
		}

		// No more Update dialog needed
		if (update_dialog != NULL)
		{
			update_dialog->Show(false);
		}


		// Timer... STOP!
		if (timer != NULL)
		{
			timer->Stop();
			timer = NULL;
		}

		// Taskbar goodbye :)
		if (m_taskBarIcon != NULL)
		{
			m_taskBarIcon->RemoveIcon();
			m_taskBarIcon->Destroy();

			m_taskBarIcon = NULL;
		}

		// No more Main dialog needed
		if (main_dialog != NULL)
		{
			main_dialog->Destroy();
			main_dialog = NULL;
		}

		// No more Update dialog needed
		if (update_dialog != NULL)
		{
			update_dialog->Destroy();
			update_dialog = NULL;
		}

		// Calls are unimportant
		for (int i=0; i < MAXCALLS; i++)
		{
			if (call_dialogs[i] != NULL)
			{
				// Stop Avatar Timer
				if (call_dialogs[i]->avatarTimer != NULL && call_dialogs[i]->avatarTimer->IsRunning())
				{
					call_dialogs[i]->avatarTimer->Stop();
				}

				call_dialogs[i]->Destroy();
				call_dialogs[i] = NULL;
			}
		}


		// We don't need Steam support
		if (steamThreader != NULL)
		{
			steamThreader->Delete();
			steamThreader = NULL;
		}

		// Delete Update
		if (update_thread != NULL)
		{
			update_thread->Delete();
			update_thread = NULL;
		}
	}
}





#if defined(__WXMSW__)
	// Stupid Wchars...
	std::wstring s2ws(wxString s)
	{
		// Convert a normal String to a wchar :)
		int len;
		int slength = (int)s.length() + 1;

		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 

		wchar_t* buf = new wchar_t[len];

		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);

		std::wstring r(buf);

		delete[] buf;

		return r;
	}
#endif




// Get the Path of the App
wxString getAppPath(wxString file)
{
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	// Windows format?
	size_t start = path.find_last_of("\\");


	if (start == 0 || start == wxString::npos)
	{
		// No... Linux Format ;)
		start = path.find_last_of("/");
	}

	// Strip app name
	path = path.replace(start, path.size(), "");

	// Add file name
	return path + "/" + file;
}






// Check for a new Version
void checkUpdate()
{
	// Log Action
	LogAction("Checking for a new Update");

	if (m_taskBarIcon != NULL)
	{
		getPage(onUpdate, updateURL);
	}
}



// Handle Update Page
void onUpdate(char* error, wxString result, int WXUNUSED(x))
{
	// Log Action
	LogAction("Retrieve information about new version");

	wxString newVersion;

	if (result != "")
	{
		// Everything good :)
		if (strcmp(error, "") == 0)
		{
			if (result.length() > 30)
			{
				// Maybe an Error Page?
				if (m_taskBarIcon != NULL)
				{
					m_taskBarIcon->ShowMessage("Update Check Failed", "Error: PAGE_TOO_LONG", main_dialog);
				}
			}
			else
			{
				// Find version in brackets
				int start = result.find_first_of("{") + 1;
				int end = result.find_first_of("}");

				newVersion = result.substr(start, end-start);
			}
		}
		else
		{
			// Log Action
			LogAction("Update check failed: " + (wxString)error);

			if (m_taskBarIcon != NULL)
			{
				m_taskBarIcon->ShowMessage("Update Check Failed", "Error: " + (wxString)error, main_dialog);
			}
		}
	}

	// We got something
	if (newVersion != "")
	{
		// Check Version
		if (newVersion != version && about != NULL && notebook != NULL)
		{
			// Log Action
			LogAction("Found a new Version: " + newVersion);

			// Update About Panel
			about->enableDownload(true);
			about->updateVersion(newVersion, wxColor("red"));

			// Show Main
			if (main_dialog != NULL && !isOtherInFullscreen())
			{
				main_dialog->Show(true);
				main_dialog->Restore();
			}

			// Goto About
			notebook->SetSelection(4);

			if (m_taskBarIcon != NULL)
			{
				m_taskBarIcon->ShowMessage("New Version", "New version " + newVersion + " is now available!", main_dialog);
			}
		}
		else
		{
			// Log Action
			LogAction("Version is up to date");

			if (m_taskBarIcon != NULL)
			{
				m_taskBarIcon->ShowMessage("Up to Date", "Your Call Admin Client is up to date", main_dialog);
			}
		}
	}
}
