/**
 * -----------------------------------------------------
 * File        calladmin-client.cpp
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Main File
 * Copyright (C) 2013 Impact, David <popoklopsi> Ordnung
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

// Include WX WIDGETS
#include <wx/wx.h>

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



// Project
#include "calladmin-client.h"
#include "config.h"
#include "error.h"
#include "main.h"
#include "call.h"
#include "taskbar.h"


// Timer
Timer *timer = NULL;


// Avatar Size
int avatarSize = 184;


// Version
wxString version = "1.0";
std::string updateURL = "http://popoklopsi.de/calladmin/version.txt";


// Last check
wxString last = "0";


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


// Implement the APP
IMPLEMENT_APP(CallAdmin)


// Default no taskbar start
bool CallAdmin::start_taskbar = false;


// App Started
bool CallAdmin::OnInit()
{
	// Something went wrong
	if (!wxApp::OnInit())
	{
		return false;
	}

	int y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

	// Set Avatar Size
	if (y < 800 && y >= 700)
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

	// Reset
	for (int i=0; i < 100; i++)
	{
		call_dialogs[i] = NULL;
	}

	// Add Icon if available
	if (wxTaskBarIcon::IsAvailable())
	{
		m_taskBarIcon = new TaskBarIcon();
		m_taskBarIcon->SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE), "CallAdmin-Client.");
	}

	// Reset Calls
	for (int i=0; i < 100; i++)
	{
		call_dialogs[i] = NULL;
	}

	// Check Config
	if (!parseConfig())
	{
		config_dialog = new ConfigDialog("CallAdmin-Settings");
	}

	// Update Time
	last = wxString() << std::time(0);

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
	Start(milliSecs);
}

// Timer executed
void Timer::update(wxTimerEvent& WXUNUSED(event))
{
	// Check for Update
	if (!timerStarted)
	{
		checkUpdate();

		timerStarted = true;
	}

	// Delete old pipe Thread
	if (pThread != NULL)
	{
		pThread->Delete();
	}

	// Check Steam
	pThread = new pipeThread();


	CURL *curl;
	CURLcode res;

	// Error
	char ebuf[CURL_ERROR_SIZE];

	// Response
	std::ostringstream stream;
	wxString result;

	// Page
	std::string pager = (page + "?from=" + (std::string)last.c_str() + "&key=" + key + "&sort=asc" + "&steamid=" + steamid);
	
	// Activate for testing:
	//pager = (page + "?from=0" + "&key=" + key + "&sort=asc" + "&steamid=" + steamid);

	// Store Player
	if (main_dialog->wantStore())
	{
		pager = pager + "&store=1";
	}

	// Init Curl
	curl = curl_easy_init();

	// Player Available?
	if (curl && main_dialog->isAvailable())
	{
		// Configurate Curl
		curl_easy_setopt(curl, CURLOPT_URL, pager.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

		// Perform Curl
		res = curl_easy_perform(curl);

		// Get Result
		result = stream.str();

		// Everything good :)
		if (res == CURLE_OK)
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

				// Max attempts reached?
				if (attempts == maxAttempts)
				{
					// Close Dialogs and create reconnect main dialog
					createReconnect("XML Error: " + (wxString)XMLErrorString[parseError]);
				}
				else
				{
					// Create Parse Error
					createError(0, XMLErrorString[parseError], "XML");
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
								createError(0, (wxString)node2->FirstChild()->Value(), "API");
							}

							break;
						}

						int dialog = -1;

						// Look for a free place
						for (int i=0; i < 100; i++)
						{
							if (call_dialogs[i] == NULL)
							{
								dialog = i;

								break;
							}
						}

						// Everything is full, so clear Everything, client's problem oO, 100 is enough!
						if (dialog == -1)
						{
							for (int i=0; i < 100; i++)
							{
								if (call_dialogs[i] != NULL)
								{
									call_dialogs[i]->Destroy();
									call_dialogs[i] = NULL;
								}
							}

							dialog = 0;
						}

						// Api is fine :)
						int found = 0;

						// Create the new CallDialog
						call_dialogs[dialog] = new CallDialog("New Incoming Call");

						// Put in ALL needed DATA
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling())
						{
							if ((wxString)node3->Value() == "fullIP")
							{
								found++;
								call_dialogs[dialog]->setIP(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "serverName")
							{
								found++;
								call_dialogs[dialog]->setName(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetName")
							{
								found++;
								call_dialogs[dialog]->setTarget(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetID")
							{
								found++;
								call_dialogs[dialog]->setTargetID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "targetReason")
							{
								found++;
								call_dialogs[dialog]->setReason(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientName")
							{
								found++;
								call_dialogs[dialog]->setClient(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "clientID")
							{
								found++;
								call_dialogs[dialog]->setClientID(node3->FirstChild()->Value());
							}

							if ((wxString)node3->Value() == "reportedAt")
							{
								found++;
								call_dialogs[dialog]->setTime(node3->FirstChild()->Value());
							}
						}

						bool findDuplicate = false;

						// Found all necessary items?
						// Check duplicate Entries
						for (int i=0; i < 100; i++)
						{
							if (i != dialog)
							{
								if (call_dialogs[i] != NULL)
								{
									// Operator overloading :)
									if ((*call_dialogs[i]) == (*call_dialogs[dialog]))
									{
										findDuplicate = true;

										// That's enough
										break;
									}
								}
							}
						}

						if (found != 8 || findDuplicate)
						{
							// Something went wrong or duplicate
							call_dialogs[dialog]->Destroy();
							call_dialogs[dialog] = NULL;
						}
						else
						{
							// New call
							foundNew = true;

							// Add the new Call to the Call box
							char buffer[80];

							std::string text;

							// But first we need a Time
							time_t tt = (time_t)call_dialogs[dialog]->getTime();

							struct tm* dt = localtime(&tt);

							strftime(buffer, sizeof(buffer), "%H:%M:%S", dt);


							call_dialogs[dialog]->SetTitle("Call at " + (wxString)buffer);


							text = (wxString)buffer + " - " + wxString::FromUTF8(call_dialogs[dialog]->getServer());

							// Add the Text
							call_dialogs[dialog]->setBoxText(text.c_str());

							// Now START IT!
							call_dialogs[dialog]->setBoxText(text.c_str());
							call_dialogs[dialog]->setID(dialog);
							call_dialogs[dialog]->startCall();
						}
					}
				}
			}

			// Everything is good, set attempts to zero
			if (!foundError)
			{
				// Refresh Time
				last = wxString() << std::time(0);

				// Reset attempts
				attempts = 0;

				// Delete old error logs
				if (error_dialog != NULL)
				{
					error_dialog->Destroy();
					error_dialog = NULL;
				}

				// Updated Main Interface
				main_dialog->SetTitle("CallAdmin Welcome");
				main_dialog->setEventText("Waiting for a new Event...");

				// Update Call List
				if (foundNew)
				{
					// Update call list
					main_dialog->updateCall();

					// Play Sound
					if (main_dialog->wantSound())
					{
						wxSound* soundfile = new wxSound("calladmin_sound", true);

						if (soundfile != NULL && soundfile->IsOk())
						{
							soundfile->Play(wxSOUND_ASYNC);
						}
					}
				}
			}
		}
		else
		{
			// Something went wrong ):
			attempts++;

			// Max attempts reached?
			if (attempts == maxAttempts)
			{
				// Close Dialogs and create reconnect main dialog
				createReconnect("CURL Error " + (wxString() << res) + ": " + ebuf);
			}
			else
			{
				// Show the error to client
				createError(res, ebuf, "CURL");
			}
		}

		// Clean Curl
		curl_easy_cleanup(curl);
	}
	else if (!main_dialog->isAvailable())
	{
		// Updated time if not available
		last = wxString() << std::time(0);
	}
}

// Curl receive data -> write to buffer
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	std::ostringstream *data = (std::ostringstream*)userp;
	size_t count = size * nmemb;

	data->write((char*)buffer, count);

	return count;
}







// Create the Window as a reconnecter
void createReconnect(wxString error)
{
	main_dialog->SetTitle("Couldn't Connect");
	main_dialog->setEventText(error);
	main_dialog->setReconnectButton(true);

	// Show it
	main_dialog->Show(true);
	main_dialog->Restore();

	// Stop timer
	timer->Stop();

	// Delete error logs
	if (error_dialog != NULL)
	{
		error_dialog->Destroy();
		error_dialog = NULL;
	}
}





// Create an new Error Dialog
void createError(int errorCode, wxString error, wxString type)
{
	// Delete old ones before
	if (error_dialog != NULL)
	{
		error_dialog->setErrorText(type + " Error " + (wxString() << errorCode) + ": " + error);
		error_dialog->setTrayText("Try again... " + (wxString() << timer->getAttempts()) + "/" + (wxString() << maxAttempts));

		error_dialog->Show(true);
		error_dialog->Restore();
	}
	else
	{
		error_dialog = new ErrorDialog("An Error occured!", errorCode, error, type);
	}
}





// Close Taskbar Icon and destroy all dialogs
void exitProgramm()
{
	// Timer... STOP!
	if (timer != NULL)
	{
		timer->Stop();
		timer = NULL;
	}

	// Stop thread
	if (pThread != NULL)
	{
		pThread->Delete();

		pThread = NULL;
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

	// Go away errors :)
	if (error_dialog != NULL)
	{
		error_dialog->Destroy();
		error_dialog = NULL;
	}

	// We don't need to set settins any more :)
	if (config_dialog != NULL)
	{
		config_dialog->Destroy();
		config_dialog = NULL;
	}

	// Calls are unimportant
	for (int i=0; i < 100; i++)
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

	// Last -> Clean Steam
	cleanSteam();
}



// Stupid Microsoft Strings...
std::wstring s2ws(wxString s)
{
	// Convert a normal String to a microsoft String :)
	int len;
	int slength = (int)s.length() + 1;

	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 

	wchar_t* buf = new wchar_t[len];

	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);

	std::wstring r(buf);

	delete[] buf;
	return r;
}


// Check for a new Version
void checkUpdate()
{
	if (m_taskBarIcon != NULL)
	{
		CURL *curl;
		CURLcode res;

		// Error
		char ebuf[CURL_ERROR_SIZE];

		// Response
		std::ostringstream stream;
		wxString result = "";
		wxString newVersion;

		// Init Curl
		curl = curl_easy_init();

		if (curl)
		{
			// Configurate Curl
			curl_easy_setopt(curl, CURLOPT_URL, updateURL.c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

			// Perform Curl
			res = curl_easy_perform(curl);

			// Get Result
			result = stream.str();

			// Everything good :)
			if (res == CURLE_OK)
			{
				if (result.length() > 20)
				{
					// Maybe an Error Page?
					m_taskBarIcon->ShowBalloon("Update Check Failed", "Error: PAGE_TOO_LONG");
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
				m_taskBarIcon->ShowBalloon("Update Check Failed", "Error: " + (wxString)ebuf);
			}
		}

		// We get something
		if (newVersion != "")
		{
			// Check Version
			if (newVersion != version)
			{
				m_taskBarIcon->ShowBalloon("New Update", "Version " + newVersion + " is available! Download here:\n\nhttp://popoklopsi.de/calladmin/calladmin-client.zip");
			}
			else
			{
				m_taskBarIcon->ShowBalloon("Up to Date", "Your CallAdmin Client is Up to Date");
			}
		}
	}
}
