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



// libcurl is static
#define CURL_STATICLIB

// c++ libs
#include <string>
#include <sstream>
#include <ctime>

// Curl
#include <curl/curl.h>

// We need a xml parser
#include "tinyxml2/tinyxml2.h"

// And an ini parser
#include "simpleini/SimpleIni.h"

// Include WX WIDGETS
#include <wx/wxprec.h>

// For compilers with precompilation
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// Command line arguments
#include <wx/cmdline.h>

// Taskbar
#include <wx/taskbar.h>

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

// Path to the APP
#include <wx/stdpaths.h>

// Sound Notification
#include <wx/sound.h>

// Header
#include "calladmin-client.h"

// Settings
int step;
int timeout;
int maxAttempts;

std::string page;
std::string key;

// Last check
wxString last = "0";

// We need something to print for a XML Error!
wxString XMLErrorString[20] = {
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

// Timer already run?
bool timerStarted = false;

// Versions String
wxString version = "1.0";
std::string updateURL = "http://popoklopsi.de/calladmin/version.txt";

// Dialogs
static MainDialog *main_dialog = NULL;
static ErrorDialog *error_dialog = NULL;
static ConfigDialog *config_dialog = NULL;
static CallDialog *call_dialogs[100];

// Timer
static Timer *timer = NULL;

// TaskBar Icon
static TaskBarIcon *m_taskBarIcon = NULL;

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

	if (!parseConfig())
	{
		config_dialog = new ConfigDialog("CallAdmin-Settings");
	}

    return true;
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

	CURL *curl;
	CURLcode res;

	// Error
	char ebuf[CURL_ERROR_SIZE];

	// Response
	std::ostringstream stream;
	wxString result;

	// Page
	std::string pager = (page + "?from=" + (std::string)last.c_str() + "&key=" + key + "&sort=asc");
	
	// Activate for testing:
	//pager = (page + "?from=0" + "&key=" + key + "&sort=asc");

	if (main_dialog->wantStore())
	{
		pager = pager + "&store=1";
	}

	// Init Curl
	curl = curl_easy_init();

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

			parseError = doc.Parse(result);

			if (parseError != tinyxml2::XML_SUCCESS)
			{
				foundError = true;
				attempts++;

				// Max attempts reached?
				if (attempts == maxAttempts)
				{
					// Close Dialogs and create reconnect main dialog
					createReconnect("Failed to connect. XML Error: " + (wxString)XMLErrorString[parseError]);
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
								createReconnect("Failed to connect. API Error: " + (wxString)node2->FirstChild()->Value());
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

							text = (wxString)buffer + " - " + call_dialogs[dialog]->getServer();

							// Add the Text
							call_dialogs[dialog]->setBoxText(text);

							// Now START IT!
							call_dialogs[dialog]->setBoxText(text);
							call_dialogs[dialog]->setID(dialog);
							call_dialogs[dialog]->startCall();
						}
					}
				}
			}

			// Everything is good, set attempts to zero
			if (!foundError)
			{
				// Only here we refresh our time
				last = wxString() << std::time(0);

				// Reset attempts
				attempts = 0;

				// Delete old error logs
				if (error_dialog != NULL)
				{
					error_dialog->Destroy();
					error_dialog = NULL;
				}

				// Update Call List
				if (foundNew)
				{
					main_dialog->updateCall();
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
				createReconnect("Failed to connect. CURL Error " + (wxString() << res) + ": " + ebuf);
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
}

// Curl receive data -> write to buffer
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	std::ostringstream *data = (std::ostringstream*)userp;
	size_t count = size * nmemb;

	data->write((char*)buffer, count);

	return count;
}







// Button ID's for Main Dialog
enum
{
    wxID_Exit = 4999,
    wxID_Hide,
    wxID_Reconnect,
	wxID_BoxClick,
};

// Button Events for Main Dialog
BEGIN_EVENT_TABLE(MainDialog, wxDialog)
    EVT_BUTTON(wxID_Exit, MainDialog::OnExit)
    EVT_BUTTON(wxID_Hide, MainDialog::OnHide)
    EVT_BUTTON(wxID_Reconnect, MainDialog::OnReconnect)
    EVT_CLOSE(MainDialog::OnCloseWindow)
	EVT_LISTBOX_DCLICK(wxID_BoxClick, MainDialog::OnBoxClick)
END_EVENT_TABLE()


// Create Main Window
void MainDialog::createWindow(bool reconnect, wxString error, bool taskbar)
{
	// Border and Center
    wxSizerFlags flags;
	wxStaticText* text;

	// Hole Body
	wxSizer* const sizerBody = new wxBoxSizer(wxHORIZONTAL);

	// Box for all Calls
	callBox = new wxListBox(this, wxID_BoxClick, wxDefaultPosition, wxSize(200, 230), 0, NULL, wxLB_HSCROLL);

	for (int i=0; i < 100; i++)
	{
		if (call_dialogs[i] != NULL)
		{
			callBox->Append(call_dialogs[i]->getBoxText());
		}
	}

	// Add to Body
	sizerBody->Add(callBox, 0, wxEXPAND | wxALL, 5);
	
	// Create Box
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);


	// Border and Centre
    flags.Border(wxALL, 10);
	flags.Centre();

	// Welcome Text
	text = new wxStaticText(this, wxID_ANY, "Welcome to the Admin Caller");

	text->SetFont(wxFont(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("red"));

	// Add it
    sizerTop->Add(text, flags);

	// Welcome Text
	text = new wxStaticText(this, wxID_ANY, "By Popoklopsi and Impact");
	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, flags);



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	if (!reconnect)
	{
		// If max attempts not reached, try again
		if (error.empty())
		{
			text = new wxStaticText(this, wxID_ANY,  "Waiting for a new Event...");
		}
		else
		{
			text = new wxStaticText(this, wxID_ANY,  error);
		}

		text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		text->SetForegroundColour(wxColor("blue"));

		sizerTop->Add(text, flags);
	}
	else
	{
		//Stop the timer
		timer->Stop();

		// If max attempts reached, notice it.
		text = new wxStaticText(this, wxID_ANY, error);

		text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		text->SetForegroundColour(wxColor("blue"));

		sizerTop->Add(text, flags);
	}


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


    wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide, Check and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_Hide, "Hide"), flags);

	// Only Add available if not reconnect display
	// With tooltip :)
	if (!reconnect)
	{
		// ToolTip for first Checkbox
		wxToolTip* tipAvailable = new wxToolTip("You will receive no more calls when you uncheck this.");

		tipAvailable->SetDelay(500);
		tipAvailable->Enable(true);

		// The available Checkbox
		available = new wxCheckBox(this, wxID_ANY, "I'm available");
		available->SetValue(true);
		available->SetToolTip(tipAvailable);

		sizerBtns->Add(available, flags);

		// ToolTip for second Checkbox
		wxToolTip* tipSound = new wxToolTip("You will hear a notification sound on an incoming call.");

		tipSound->SetDelay(500);
		tipSound->Enable(true);

		// The sound Checkbox
		sound = new wxCheckBox(this, wxID_ANY, "Sound on Call");
		sound->SetValue(true);
		sound->SetToolTip(tipSound);

		sizerBtns->Add(sound, flags);

		// ToolTip for third Checkbox
		wxToolTip* specAvailable = new wxToolTip("You will only receive calls but you will not be stored in the database");

		specAvailable->SetDelay(500);
		specAvailable->Enable(true);

		// The store Checkbox
		store = new wxCheckBox(this, wxID_ANY, "Spectate Only");
		store->SetValue(false);
		store->SetToolTip(specAvailable);

		sizerBtns->Add(store, flags);
	}

    sizerBtns->Add(new wxButton(this, wxID_Exit, "Exit"), flags);

	// If max attempts reached, add a reconnect button
	if (reconnect)
	{
		sizerBtns->Add(new wxButton(this, wxID_Reconnect, "Reconnect"), flags);
	}

	// Add Buttons to Box
    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Add to Body
	sizerBody->Add(sizerTop, flags.Align(wxALIGN_RIGHT));

	// Auto Size
    SetSizerAndFit(sizerBody, true);

	// Centre to Screen
    Centre();

	// Start in taskbar
	if (taskbar && m_taskBarIcon != NULL)
	{
		main_dialog->Show(false);
				
		m_taskBarIcon->ShowBalloon("CallAdmin Started", "CallAdmin started in taskbar");
	}
	else
	{
		// Show
		Show(true);
	}

	// Set the Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));

	// Update Call List
	updateCall();
}



// Button Event -> Hide to Taskbar
void MainDialog::OnHide(wxCommandEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");

		Show(false);
	}
	else
	{
		Iconize(true);
	}
}

// Button Event -> Exit hole programm
void MainDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}

// Button Event -> Reconnect
void MainDialog::OnReconnect(wxCommandEvent& WXUNUSED(event))
{
	// Reset attempts
	timer->setAttempts(0);

	if (main_dialog != NULL)
	{
		main_dialog->Destroy();
	}

	// Create new Main Window
	main_dialog = new MainDialog("Couldn't connect!");

	main_dialog->createWindow(false, "Reconnecting...");

    // Start the Timer again
	timer->Start(step*1000);
}

// Window Event -> Open Call
void MainDialog::OnBoxClick(wxCommandEvent& WXUNUSED(event))
{
	int selection = callBox->GetCount() - callBox->GetSelection() - 1;

	if (call_dialogs[selection] != NULL)
	{
		call_dialogs[selection]->Show(true);

	}
}

// Window Event -> Hide Window
void MainDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		Show(false);

		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");
	}
	else
	{
		Iconize(true);
	}
}

// Update Call List
void MainDialog::updateCall()
{
	callBox->Clear();

	for (int i=100; i >= 0; i--)
	{
		if (call_dialogs[i] != NULL)
		{
			callBox->Append(call_dialogs[i]->getBoxText());
		}
	}
}




// Button ID's for Error Dialog
enum
{
    wxID_ExitError = 5003,
    wxID_HideError,
	wxID_CloseError,
};

// Button Events for Error Dialog
BEGIN_EVENT_TABLE(ErrorDialog, wxDialog)
    EVT_BUTTON(wxID_ExitError, ErrorDialog::OnExit)
	EVT_BUTTON(wxID_CloseError, ErrorDialog::OnClose)
    EVT_BUTTON(wxID_HideError, ErrorDialog::OnHide)
    EVT_CLOSE(ErrorDialog::OnCloseWindow)
END_EVENT_TABLE()


// Create Error Window
ErrorDialog::ErrorDialog(const wxString& title, int error, wxString errorMessage, wxString type) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
	// Create Box
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	// Border and Center
    wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
    flags.Border(wxALL, 10);
	flags.Centre();



	// Error Text
	text = new wxStaticText(this, wxID_ANY, "An error occured while connecting/reading the Page");

	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("red"));

	// Add it
    sizerTop->Add(text, flags);



	// Error Code and Message
	text = new wxStaticText(this, wxID_ANY, type + " Error " + (wxString("") << error) + ": " + errorMessage);

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, flags);



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	// Try again
	text = new wxStaticText(this, wxID_ANY,  "Try again... " + (wxString("") << timer->getAttempts()) + "/" + (wxString("") << maxAttempts));

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	text->SetForegroundColour(wxColor("blue"));

	sizerTop->Add(text, flags);


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


    wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_HideError, "Hide"), flags);
	sizerBtns->Add(new wxButton(this, wxID_ExitError, "Exit"), flags);
    sizerBtns->Add(new wxButton(this, wxID_CloseError, "Close"), flags);

	// Add Buttons to Box
    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Auto Size
    SetSizerAndFit(sizerTop, true);

	// Centre to Screen
    Centre();

	// Show
	Show(true);

	// Set the Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
}



// Button Event -> Hide to Taskbar
void ErrorDialog::OnHide(wxCommandEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		Show(false);

		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");
	}
	else
	{
		Iconize(true);
	}
}

// Button Event -> Exit only Error
void ErrorDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
	error_dialog = NULL;

	Destroy();
}

// Button Event -> Exit only Error
void ErrorDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}

// Window Event -> Hide Window
void ErrorDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	if (m_taskBarIcon != NULL)
	{
		Show(false);

		m_taskBarIcon->ShowBalloon("CallAdmin", "CallAdmin is now in the Taskbar!");
	}
	else
	{
		Iconize(true);
	}
}







// Button ID's for Config Dialog
enum
{
    wxID_SetConfig = 5008,
    wxID_ExitConfig,
};

// Button Events for Error Dialog
BEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
    EVT_BUTTON(wxID_ExitConfig, ConfigDialog::OnExit)
	EVT_BUTTON(wxID_SetConfig, ConfigDialog::OnSet)
    EVT_CLOSE(ConfigDialog::OnCloseWindow)
END_EVENT_TABLE()


// Create Error Window
ConfigDialog::ConfigDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
	// Create Box
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	// Border and Center
    wxSizerFlags flags;
	wxStaticText* text;

	// Border and Centre
    flags.Border(wxALL, 10);
	flags.Centre();


	// Config
	text = new wxStaticText(this, wxID_ANY, "Set up your Config");

	text->SetFont(wxFont(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("blue"));

	// Add it
	sizerTop->Add(text, flags);


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	// Ask for Page
    wxSizer* const sizerPage = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Your page to the notice.php: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerPage->Add(text, flags);

	pageText = new wxTextCtrl(this, wxID_ANY, "http://yourpage.com/notice.php", wxDefaultPosition, wxSize(220, 20));
	pageText->SetMaxLength(256);

	sizerPage->Add(pageText, flags);

	// Add text and slider
    sizerTop->Add(sizerPage, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Ask for Key
    wxSizer* const sizerKey = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Your private access Key: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerKey->Add(text, flags);

	keyText = new wxTextCtrl(this, wxID_ANY, "accesskey", wxDefaultPosition, wxSize(120, 20));
	keyText->SetMaxLength(64);

	sizerKey->Add(keyText, flags);

	// Add text and slider
    sizerTop->Add(sizerKey, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	// Ask for Step
    wxSizer* const sizerStep = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Time Intervall to search for new Calls: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerStep->Add(text, flags);

	stepSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 5, 60, 5, "Time Inverall");
	sizerStep->Add(stepSlider, flags);

	// Add text and slider
    sizerTop->Add(sizerStep, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Ask for Timeout
    wxSizer* const sizerTimeout = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Timeout for connection: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTimeout->Add(text, flags);

	timeoutSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 10, 3, "Timout");

	sizerTimeout->Add(timeoutSlider, flags);

	// Add text and slider
    sizerTop->Add(sizerTimeout, flags.Align(wxALIGN_CENTER_HORIZONTAL));




	// Ask for Attempts
    wxSizer* const sizerAttempts = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, wxID_ANY, "Max. auto. reconnect attempts: ");
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerAttempts->Add(text, flags);

	attemptsSlider = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, 22), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 3, 10, 3, "Max Attempts");

	sizerAttempts->Add(attemptsSlider, flags);

	// Add text and slider
    sizerTop->Add(sizerAttempts, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);




    wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_SetConfig, "Create Config"), flags);
	sizerBtns->Add(new wxButton(this, wxID_ExitConfig, "Exit"), flags);

	// Add Buttons to Box
    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Auto Size
    SetSizerAndFit(sizerTop, true);

	// Centre to Screen
    Centre();

	// Show
	Show(true);

	// Set the Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
}



// Button Event -> Try to set new config
void ConfigDialog::OnSet(wxCommandEvent& WXUNUSED(event))
{
	timeout = timeoutSlider->GetValue();
	step = stepSlider->GetValue();
	maxAttempts = attemptsSlider->GetValue();

	page = pageText->GetValue();
	key = keyText->GetValue();

	CSimpleIniA config;

	// Read config file
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	size_t start = path.find_last_of("\\");

	if (start < 0)
	{
		start = path.find_last_of("/");
	}

	path = path.replace(start, path.size(), "/calladmin-client_settings.ini");

	config.SetValue("settings", "step", (wxString() << step));
	config.SetValue("settings", "timeout", (wxString() << timeout));
	config.SetValue("settings", "attempts", (wxString() << maxAttempts));
	config.SetValue("settings", "page", page.c_str());
	config.SetValue("settings", "key", key.c_str());

	config.SaveFile(((std::string)path).c_str());

	config_dialog = NULL;

	Destroy();

	parseConfig();
}

// Button Event -> Exit programm
void ConfigDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}

// Window Event -> Close Window
void ConfigDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	exitProgramm();
}






// Button ID's for Call Dialog
enum
{
    wxID_CloseCall = 5006,
    wxID_ConnectCall,
};

// Button Events for Call Dialog
BEGIN_EVENT_TABLE(CallDialog, wxDialog)
    EVT_BUTTON(wxID_ConnectCall, CallDialog::OnConnect)
	EVT_BUTTON(wxID_CloseCall, CallDialog::OnClose)
    EVT_CLOSE(CallDialog::OnCloseWindow)
END_EVENT_TABLE()

// start the Call
void CallDialog::startCall()
{
	// Create Box
    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);
	
	// Border and Center
    wxSizerFlags flags;
	wxStaticText* text;
	wxTextCtrl* text2;

	// Border and Centre
    flags.Border(wxALL, 10);
	flags.Centre();

	// New Call
	text = new wxStaticText(this, wxID_ANY, "New incoming Call!");

	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	text->SetForegroundColour(wxColor("red"));

	// Add it
    sizerTop->Add(text, flags);


	// Server
	text = new wxStaticText(this, wxID_ANY,  "Server: " + serverName + " (" + fullIP + ")");

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTop->Add(text, flags);


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	wxSizer* const sizerClient = new wxBoxSizer(wxHORIZONTAL);

	// Player reported Player
	text = new wxStaticText(this, wxID_ANY, client);

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Steamid
	text2 = new wxTextCtrl(this, wxID_ANY, clientID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE|wxTE_READONLY);

	text2->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	// Add it
	sizerClient->Add(text, flags);
	sizerClient->Add(text2, 0, wxEXPAND | wxALL, 10);
	sizerTop->Add(sizerClient, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	text = new wxStaticText(this, wxID_ANY, "reported Player");

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerTop->Add(text, flags);


	wxSizer* const sizerClient2 = new wxBoxSizer(wxHORIZONTAL);

	// Target
	text = new wxStaticText(this, wxID_ANY, target);

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Steamid
	text2 = new wxTextCtrl(this, wxID_ANY, targetID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE|wxTE_READONLY);

	text2->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add it
	sizerClient2->Add(text, flags);
	sizerClient2->Add(text2, 0, wxEXPAND | wxALL, 10);
	sizerTop->Add(sizerClient2, flags.Align(wxALIGN_CENTER_HORIZONTAL));


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	// Reason
	text = new wxStaticText(this, wxID_ANY,  "Reason: " + reason);

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTop->Add(text, flags);


	// Time
	char buffer[120];

	// But first we need a Time
	time_t tt = (time_t)getTime();

	struct tm* dt = localtime(&tt);

	strftime(buffer, sizeof(buffer), "%c", dt);


	text = new wxStaticText(this, wxID_ANY,  "Time: " + (std::string)buffer);

	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTop->Add(text, flags);


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);


    wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_ConnectCall, "Connect"), flags);
	sizerBtns->Add(new wxButton(this, wxID_CloseCall, "Close"), flags);

	// Add Buttons to Box
    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Play Sound
	if (main_dialog->wantSound())
	{
		wxSound* soundfile = new wxSound("calladmin_sound", true);

		if (soundfile != NULL && soundfile->IsOk())
		{
			soundfile->Play(wxSOUND_ASYNC);
		}
	}

	// Auto Size
    SetSizerAndFit(sizerTop, true);

	// Centre to Screen
    Centre();

	// Show the Window
	Show(true);

	// Set Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
}




// Button Event -> disable window
void CallDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
	Show(false);
}

// Button Event -> Connect to Server
void CallDialog::OnConnect(wxCommandEvent& WXUNUSED(event))
{
	ShellExecute(NULL, L"open", s2ws("steam://connect/" + fullIP).c_str(), NULL, NULL, SW_SHOWNORMAL);
	
	Show(false);
}

// Window Event -> disable Window
void CallDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Show(false);
}






// Taskbar ID's
enum
{
    PU_RESTORE = 10001,
    PU_UPDATE,
	PU_AUTOSTART,
    PU_EXIT,
};

// Taskbar Events
BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_UPDATE, TaskBarIcon::OnMenuUpdate)
	EVT_MENU(PU_AUTOSTART, TaskBarIcon::OnMenuAutoStart)
    EVT_MENU(PU_EXIT, TaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_DCLICK (TaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()


// Restore all dialogs
void TaskBarIcon::OnMenuRestore(wxCommandEvent&)
{
	if (main_dialog != NULL)
	{
		main_dialog->Show(true);
	}

	if (error_dialog != NULL)
	{
		error_dialog->Show(true);
	}

	if (config_dialog != NULL)
	{
		config_dialog->Show(true);
	}
}

// On exit -> Exit hole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent&)
{
	exitProgramm();
}

// Append/Remove to Autostart
void TaskBarIcon::OnMenuAutoStart(wxCommandEvent& event)
{
	// Registry Key
	HKEY hkRegistry;

	// Open Key
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_WRITE, &hkRegistry) == ERROR_SUCCESS)
    {
		// He checked it
        if (event.IsChecked())
        {
			// Get App Path
            wxString appPath = wxStandardPaths::Get().GetExecutablePath();

			// Close to Taskbar on Autostart
			appPath = "\"" + appPath + "\"" + " -taskbar";

			// Write in
            RegSetValueExW(hkRegistry, L"CallAdmin-Client", 0, REG_SZ, (BYTE*)appPath.wc_str(), (wcslen(appPath.wc_str()) + 1) * sizeof(wchar_t));
        }
        else
        {
			// Remove it
			RegDeleteValueA(hkRegistry, "CallAdmin-Client");
        }

		// Close Key
        RegCloseKey(hkRegistry);
    }
}

// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent&)
{
	checkUpdate();
}



// Create the Taskbar Menu
wxMenu *TaskBarIcon::CreatePopupMenu()
{
	bool autoRun = false;

    wxMenu *menu = new wxMenu();

    menu->Append(PU_RESTORE, "Restore Windows");
    menu->AppendSeparator();


	// Check Auto run Key
	HKEY hkRegistry;

	// Open it
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_QUERY_VALUE, &hkRegistry) == ERROR_SUCCESS)
    {
		// Path in the registry
        wchar_t wszPath[4096];
        memset(wszPath, 0, sizeof(wszPath));

        DWORD dwType, dwSize = sizeof(wszPath) - 1;

		// Look for calladmin
        if (RegQueryValueExW(hkRegistry, L"CallAdmin-Client", 0, &dwType, (unsigned char *)wszPath, &dwSize) == ERROR_SUCCESS)
        {
			// Is Path the same?
			if (wxString(wszPath).Contains(wxStandardPaths::Get().GetExecutablePath()))
			{
				// So it's on
				autoRun = true;
			}
        }

		// Close the Key
        RegCloseKey(hkRegistry);
    }

	menu->Append(PU_UPDATE, "Check For Update");
	menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK)->Check(autoRun);
    menu->AppendSeparator();

	menu->Append(PU_EXIT, "Exit");

    return menu;
}


// On doppel left click -> open Menu
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if (main_dialog != NULL)
	{
		main_dialog->Show(true);
	}

	if (error_dialog != NULL)
	{
		error_dialog->Show(true);
	}

	if (config_dialog != NULL)
	{
		config_dialog->Show(true);
	}
}








// Create the Window as a reconnecter
void createReconnect(wxString error)
{
	if (main_dialog != NULL)
	{
		main_dialog->Destroy();
	}

	if (error_dialog != NULL)
	{
		error_dialog->Destroy();
		error_dialog = NULL;
	}

	main_dialog = new MainDialog("Couldn't connect!");
	main_dialog->createWindow(true, error);
}





// Create an new Error Dialog
void createError(int errorCode, wxString error, wxString type)
{
	// Delete old ones before
	if (error_dialog != NULL)
	{
		error_dialog->Destroy();
	}

	error_dialog = new ErrorDialog("An Error occured!", errorCode, error, type);
}





// Close Taskbar Icon and destroy all dialogs
void exitProgramm()
{
	// Taskbar goodbye :)
	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->Destroy();
		m_taskBarIcon = NULL;
	}

	// Timer... STOP!
	if (timer != NULL)
	{
		timer->Stop();
		timer = NULL;
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
			call_dialogs[i]->Destroy();
			call_dialogs[i] = NULL;
		}
	}
}




bool parseConfig()
{
	bool foundConfigError = false;

	// Read Config file ;)
	CSimpleIniA config;

	// Read config file
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	size_t start = path.find_last_of("\\");

	if (start < 0)
	{
		start = path.find_last_of("/");
	}

	path = path.replace(start, path.size(), "/calladmin-client_settings.ini");

	SI_Error rc = config.LoadFile(((std::string)path).c_str());

	// Was parsing good?
	if (rc != SI_OK)
	{
		foundConfigError = true;
	}
	else
	{
		// Get files out of config
		try
		{
			std::string stepS = config.GetValue("settings", "step", "5");
			std::stringstream s(stepS);

			s >> step;

			std::string timeoutS = config.GetValue("settings", "timeout", "3");
			std::stringstream t(timeoutS);

			t >> timeout;

			std::string maxAttemptsS = config.GetValue("settings", "attempts", "5");
			std::stringstream m(maxAttemptsS);

			m >> maxAttempts;

			page = config.GetValue("settings", "page", "");
			key = config.GetValue("settings", "key", "");
		}
		catch(...) {foundConfigError = true;}
		
		if (!foundConfigError)
		{
			// Check invalid values
			if (step < 5)
			{
				step = 5;
			}

			if (step > 60)
			{
				step = 60;
			}

			if (timeout < 1)
			{
				timeout = 1;
			}

			if (timeout > 10)
			{
				timeout = 10;
			}

			if (maxAttempts < 3)
			{
				maxAttempts = 3;
			}

			if (maxAttempts > 10)
			{
				maxAttempts = 10;
			}

			if (timeout >= step)
			{
				timeout = step - 1;
			}

			// Current Time
			last = wxString() << std::time(0);

			// Create main Dialog
			main_dialog = new MainDialog("CallAdmin Welcome");
	
			main_dialog->createWindow(false, "", CallAdmin::start_taskbar);

			// Start Timer
			timer = new Timer();
			timer->run(step*1000);

			timer->setAttempts(0);
		}
	}

	return !foundConfigError;
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

		if (newVersion != "")
		{
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
