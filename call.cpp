/**
 * -----------------------------------------------------
 * File        call.cpp
 * Authors     David <popoklopsi> Ordnung, Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 * 
 * 
 * Copyright (C) 2013 David <popoklopsi> Ordnung, Impact
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
#include <ctime>
#include <stdlib.h>

// Includes Project
#include "main.h"
#include "call.h"
#include "opensteam.h"
#include "config.h"
#include "log.h"
#include "taskbar.h"
#include "calladmin-client.h"

// wx
#include <wx/statline.h>
#include <wx/tooltip.h>

// curl
#include <curl/curl.h>

// xml parser
#include "tinyxml2/tinyxml2.h"

// Call Dialogs
CallDialog *call_dialogs[MAXCALLS];



// Button ID's for Call Dialog
enum
{
	wxID_ConnectCall = wxID_HIGHEST+200,
	wxID_CheckDone,
	wxID_ContactClient,
	wxID_ContactTarget,
	wxID_ContactTrackers
};



// Button Events for Call Dialog
BEGIN_EVENT_TABLE(CallDialog, wxDialog)
	EVT_BUTTON(wxID_ConnectCall, CallDialog::OnConnect)
	EVT_BUTTON(wxID_CheckDone, CallDialog::OnCheck)
	EVT_BUTTON(wxID_ContactClient, CallDialog::OnContactClient)
	EVT_BUTTON(wxID_ContactTarget, CallDialog::OnContactTarget)
	EVT_BUTTON(wxID_ContactTrackers, CallDialog::OnContactTrackers)

	EVT_CLOSE(CallDialog::OnCloseWindow)
	EVT_ICONIZE(CallDialog::OnMinimizeWindow)
END_EVENT_TABLE()



// start the Call
void CallDialog::startCall(bool show)
{
	
	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);

	// Panel
	wxPanel* panel = new wxPanel(this, wxID_ANY);


	// Valid?
	if (sizerTop == NULL || panel == NULL)
	{
		return;
	}


	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;
	wxTextCtrl* text2;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// ToolTip for Contact friend 
	wxToolTip* contactTooltip = new wxToolTip("This will open a Steam chat with this client.");


	// Valid?
	if (contactTooltip == NULL)
	{
		return;
	}


	contactTooltip->SetDelay(500);
	contactTooltip->Enable(true);


	// Time
	char buffer[80];

	// But first we need a Time
	time_t tt = (time_t)getTime();

	struct tm* dt = localtime(&tt);

	strftime(buffer, sizeof(buffer), "%c", dt);


	// New Call
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(serverName));

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	sizerTop->Add(text, flags.Border(wxALL &~ wxBOTTOM, 20));


	// New Call At
	text = new wxStaticText(panel, wxID_ANY, "at " + (std::string)buffer);

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));



	// Add it
	sizerTop->Add(text, flags.Border(wxALL &~ wxTOP &~ wxBOTTOM, 20));



	// New Call At
	if (!isHandled)
	{
		doneText = new wxStaticText(panel, wxID_ANY, "Unfinished");

		doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		doneText->SetForegroundColour(wxColor("red"));
	}
	else
	{
		doneText = new wxStaticText(panel, wxID_ANY, "Finished");

		doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		doneText->SetForegroundColour(wxColour(34, 139, 34));
	}


	// Add it
	sizerTop->Add(doneText, flags.Border(wxALL &~ wxTOP, 20));



	


	// Restore Border
	flags.Border(wxALL, 10);



	// Static line 
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	wxSizer* const clientLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const clientDetails = new wxBoxSizer(wxVERTICAL);


	// Avatar
	#if defined(__WXMSW__)
		clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(avatarSize, avatarSize)));
	#else
		clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(getAppPath("resources/calladmin_avatar.bmp")).Rescale(avatarSize, avatarSize)));
	#endif

	clientLayout->Add(clientAvatar, flags);



	// Caller Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(client));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
	
	clientDetails->Add(text, flags);
	
	
	if (steamConnected && steamFriends != NULL && getClientCID() != NULL && getClientCID()->IsValid())
	{
		if (steamFriends->GetFriendRelationship(*getClientCID()) == k_EFriendRelationshipFriend)
		{
			// Add Contact Friend button
			wxButton *contactClient = new wxButton(panel, wxID_ContactClient, "Contact Friend");

			contactClient->SetToolTip(contactTooltip);

			clientDetails->Add(contactClient, 0, wxALL &~ wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
		}
	}




	// Steamid
	text2 = new wxTextCtrl(panel, wxID_ANY, clientID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE | wxTE_READONLY);

	text2->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clientDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	clientLayout->Add(clientDetails, flags);
	sizerTop->Add(clientLayout, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 10);



	// Reason
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("\xe2\x96\xbc") + " reported because of reason: \"" + wxString::FromUTF8(reason) + "\" " + wxString::FromUTF8("\xe2\x96\xbc"));
	text->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTop->Add(text, flags);




	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 10);



	wxSizer* const targetLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const targetDetails = new wxBoxSizer(wxVERTICAL);
	
	// Avatar
	#if defined(__WXMSW__)
		targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(avatarSize, avatarSize)));
	#else
		targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(getAppPath("resources/calladmin_avatar.bmp")).Rescale(avatarSize, avatarSize)));
	#endif

	targetLayout->Add(targetAvatar, flags);



	// Target Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(target));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	targetDetails->Add(text, flags);
	
	
	if (steamConnected && steamFriends != NULL && getTargetCID() != NULL && getTargetCID()->IsValid())
	{
		if (steamFriends->GetFriendRelationship(*getTargetCID()) == k_EFriendRelationshipFriend)
		{
			// Add Contact Friend button
			wxButton *contactTarget = new wxButton(panel, wxID_ContactTarget, "Contact Friend");

			contactTarget->SetToolTip(contactTooltip);

			targetDetails->Add(contactTarget, 0, wxALL &~ wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
		}
	}



	// Steamid
	text2 = new wxTextCtrl(panel, wxID_ANY, targetID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE | wxTE_READONLY);

	text2->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	targetDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	targetLayout->Add(targetDetails, flags);
	sizerTop->Add(targetLayout, flags);




	// Start the Timers
	avatarTimer = new AvatarTimer(getClientCID(), getTargetCID(), clientAvatar, targetAvatar);

	avatarTimer->startTimer();



	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);



	// ToolTip for Take over button
	wxToolTip* takeOverTooltip = new wxToolTip("This will mark the call as finished and will ignore it next time.");

	takeOverTooltip->SetDelay(500);
	takeOverTooltip->Enable(true);



	// Takeover button
	takeover = new wxButton(panel, wxID_CheckDone, "Take Over");
	takeover->SetToolTip(takeOverTooltip);



	// ToolTip for Contact Trackers button
	wxToolTip* contactTrackersTooltip = new wxToolTip("This will send all available trackers on your friendlist a message.");

	contactTrackersTooltip->SetDelay(500);
	contactTrackersTooltip->Enable(true);


	// Contect trackers button
	contactTrackers = new wxButton(panel, wxID_ContactTrackers, "Contact Trackers");
	contactTrackers->Enable(true);
	contactTrackers->SetToolTip(contactTrackersTooltip);



	// Hide and Exit Button
	sizerBtns->Add(new wxButton(panel, wxID_ConnectCall, "Connect"), 0, wxALL &~ wxRIGHT, 5);
	sizerBtns->Add(takeover, 0, wxALL &~ wxRIGHT &~ wxLEFT, 5);
	sizerBtns->Add(contactTrackers, 0, wxALL &~ wxLEFT, 5);



	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags);
	

	
	// Auto Size
	panel->SetSizerAndFit(sizerTop, true);


	// Fit
	this->Fit();

	// Centre to Screen
	Centre();

	// Set Icon
	#if defined(__WXMSW__)
		SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
	#else
		wxLogNull nolog;

		SetIcon(wxIcon(getAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
	#endif


	// Show the Window
	Show(show);
}




// We need the 64bit int id
CSteamID CallDialog::steamIDtoCSteamID (char* steamid)
{
	CSteamID csteam;

	// To small
	if (strlen(steamid) < 11)
	{
		return csteam;
	}

	int server = 0;
	int authID = 0;

	// Strip the steamid
	char *strip = strtok(steamid, ":");

	while (strip)
	{
		strip = strtok(NULL, ":");

		char *strip2 = strtok(NULL, ":");

		// Read server and authID
		if (strip2)
		{
			server = atoi(strip);
			authID = atoi(strip2);
		}
	}

	// Wrong Format
	if (authID == 0)
	{
		return csteam;
	}

	uint64 uintID = (uint64)authID * 2;

	// Convert to a uint64
	uintID += 76561197960265728 + server;

	csteam.SetFromUint64(uintID);

	// Return it
	return csteam;
}




// Button Event -> Connect to Server
void CallDialog::OnConnect(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Connected to the Server " + fullIP);

	#if defined(__WXMSW__)
		ShellExecute(NULL, L"open", s2ws("steam://connect/" + fullIP).c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("xdg-open steam://connect/" + fullIP).c_str());
	#endif

	Show(false);
}




// Mark it checked
void CallDialog::OnCheck(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Marke call " + callID + " as finished");

	// page
	std::string pager = (std::string)(page + "/takeover.php?callid=" + callID + "&key=" + key);

	// Get Page
	getPage(onChecked, pager, ID);
}





// Contact Client
void CallDialog::OnContactClient(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Contacted Client " + (wxString)getClientCID()->Render());

	// Open Chat
	#if defined(__WXMSW__)
		ShellExecute(NULL, L"open", s2ws("steam://friends/message/" + (wxString() << getClientCID()->ConvertToUint64())).c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("xdg-open steam://friends/message/" + (wxString() << getClientCID()->ConvertToUint64())).c_str());
	#endif
}




// Contact Target
void CallDialog::OnContactTarget(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Contacted Client " + (wxString)getTargetCID()->Render());

	// Open Chat
	#if defined(__WXMSW__)
		ShellExecute(NULL, L"open", s2ws("steam://friends/message/" + (wxString() << getTargetCID()->ConvertToUint64())).c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("xdg-open steam://friends/message/" + (wxString() << getTargetCID()->ConvertToUint64())).c_str());
	#endif
}




// Contact Trackers
void CallDialog::OnContactTrackers(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Contacting current Trackers");



	// Are we steam connected?
	if (steamFriends != NULL && steamConnected)
	{
		// page
		std::string pager = (std::string)(page + "/trackers.php?from=25&from_type=interval&key=" + key);

		getPage(onGetTrackers, pager, ID);

		return;
	}


	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Coulnd't contact trackers!", "You're not connected with STEAM!", this);
	}
}




// Contact Client
void onGetTrackers(char* errors, wxString result, int x)
{
	// Log Action
	LogAction("Got Trackers");

	wxString error = "";



	if (result != "")
	{
		// Everything good :)
		if (strcmp(errors, "") == 0)
		{
			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parsing good :)?
			if (parseError == tinyxml2::XML_SUCCESS)
			{
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin_Trackers
				if (node != NULL)
				{
					node = node->NextSibling();
				}

				// Found Trackers?
				if (node != NULL)
				{
					// found someone?
					bool found = false;
					
					
					// Tracker Loop
					for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling())
					{
						// API Error?
						if ((wxString)node2->Value() == "error")
						{
							error = (wxString)node2->FirstChild()->Value();

							break;
						}



						// Search admin steamids
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling())
						{
							// Found steamid
							if ((wxString)node3->Value() == "trackerID" && steamFriends != NULL && call_dialogs != NULL && call_dialogs[x] != NULL)
							{
								std::string steamidString = node3->FirstChild()->Value();

								// Build csteamid
								CSteamID steamidTracker = call_dialogs[x]->steamIDtoCSteamID((char*) steamidString.c_str());

								// Are we friends and is tracker online? :))
								if (steamidTracker.IsValid() && steamFriends->GetFriendRelationship(steamidTracker) == k_EFriendRelationshipFriend && steamFriends->GetFriendPersonaState(steamidTracker) != k_EPersonaStateOffline)
								{
									// Now we write a message
									steamFriends->ReplyToFriendMessage(steamidTracker, "Hey, i contact you because of the call from " + call_dialogs[x]->getClient() + " about " + call_dialogs[x]->getTarget());

									// And we found someone :)
									if (!found)
									{
										found = true;

										// So no contacting possible anymore
										call_dialogs[x]->contactTrackers->Enable(false);
									}
								}
							}
						}
					}

					// Have we found something?
					if (found)
					{
						// We are finished :)
						return;
					}

				}
			}
			else
			{
				// XML ERROR
				error = "XML ERROR: Couldn't parse the trackers API!";

				// Log Action
				LogAction("XML Error in trackers API");
			}
		}
		else
		{
			// Curl error
			error = errors;

			// Log Action
			LogAction("CURL Error " + error);
		}
	}
	else
	{
		// Curl error
		error = "Couldn't init. CURL connection";
	}


	// Seems we found no one
	if (error == "")
	{
		error = "Found no available tracker on your friendlist!";
	}

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Coulnd't contact trackers!", error, call_dialogs[x]);
	}
}





// Mark checked
void onChecked(char* errors, wxString result, int x)
{
	// Log Action
	LogAction("Marked call " + call_dialogs[x]->getID() + " as finished");

	wxString error = "";

	if (result != "")
	{
		// Everything good :)
		if (strcmp(errors, "") == 0)
		{
			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parsing good :)?
			if (parseError == tinyxml2::XML_SUCCESS)
			{
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin_Takeover
				if (node != NULL)
				{
					node = node->NextSibling();
				}

				// Found something?
				if (node != NULL)
				{
					// Goto first child
					node = node->FirstChild();

					// API Error?
					if ((wxString)node->Value() == "error")
					{
						error = (wxString)node->FirstChild()->Value();
					}

					// Success?
					if ((wxString)node->Value() == "success" && call_dialogs != NULL && call_dialogs[x] != NULL)
					{
						mainFrame->setHandled(x);

						return;
					}
				}
			}
			else
			{
				// XML ERROR
				error = "XML ERROR: Couldn't parse the takeover API!";
			}
		}
		else
		{
			// Curl error
			error = errors;
		}
	}
	else
	{
		// Curl error
		error = "Couldn't init. CURL connection";
	}

	// Seems empty
	if (error == "")
	{
		error = "Invalid XML structure!";
	}

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Coulnd't take over call!", error, call_dialogs[x]);
	}
}




// Window Event -> disable Window
void CallDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Show(false);
}




// Window Event -> Hide Window
void CallDialog::OnMinimizeWindow(wxIconizeEvent& WXUNUSED(event))
{
	if (hideOnMinimize)
	{
		Show(false);
	}
}
