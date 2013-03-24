/**
 * -----------------------------------------------------
 * File        call.cpp
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Header File
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
#include <ctime>

// Includes Project
#include "call.h"
#include "opensteam.h"
#include "calladmin-client.h"

// wx
#include <wx/statline.h>


// Call Dialogs
CallDialog *call_dialogs[100] = {NULL};



// Button ID's for Call Dialog
enum
{
	wxID_CloseCall = 5100,
	wxID_ConnectCall,
	wxID_CheckDone,
	wxID_ContactClient,
	wxID_ContactTarget
};



// Button Events for Call Dialog
BEGIN_EVENT_TABLE(CallDialog, wxDialog)
	EVT_BUTTON(wxID_ConnectCall, CallDialog::OnConnect)
	EVT_BUTTON(wxID_CloseCall, CallDialog::OnClose)
	EVT_BUTTON(wxID_CheckDone, CallDialog::OnCheck)
	EVT_BUTTON(wxID_ContactClient, CallDialog::OnContactClient)
	EVT_BUTTON(wxID_ContactTarget, CallDialog::OnContactTarget)
	EVT_CLOSE(CallDialog::OnCloseWindow)
END_EVENT_TABLE()



// start the Call
void CallDialog::startCall()
{
	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);
	
	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;
	wxTextCtrl* text2;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();



	// Time
	char buffer[80];

	// But first we need a Time
	time_t tt = (time_t)getTime();

	struct tm* dt = localtime(&tt);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", dt);

	// New Call
	text = new wxStaticText(this, wxID_ANY, "New Call made on " + wxString::FromUTF8(serverName) + " at " + (std::string)buffer);

	text->SetFont(wxFont(18, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));


	// Add it
	sizerTop->Add(text, flags.Border(wxALL, 20));
	

	// Restore Border
	flags.Border(wxALL, 10);


	// Static line 
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);




	wxSizer* const clientLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const clientBody = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const clientDetails = new wxBoxSizer(wxVERTICAL);

	// Avatar
	clientAvatar = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(avatarSize, avatarSize)));

	clientLayout->Add(clientAvatar, flags);

	// Caller Name
	text = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(client));
	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	
	clientBody->Add(text, flags);
	
	if (steamConnected && steamFriends != NULL)
	{
		if (steamFriends->GetFriendRelationship(*getClientCID()) == k_EFriendRelationshipFriend)
		{
			clientBody->Add(new wxButton(this, wxID_ContactClient, "Contact Friend"), flags);
		}
	}


	// Add Client Body
	clientDetails->Add(clientBody, flags);


	// Steamid
	text2 = new wxTextCtrl(this, wxID_ANY, clientID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE|wxTE_READONLY);

	text2->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clientDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	clientLayout->Add(clientDetails, flags);
	sizerTop->Add(clientLayout, flags);


	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 10);


	// Reason
	text = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("\xe2\x96\xbc") + " reported because of reason: \"" + wxString::FromUTF8(reason) + "\" " + wxString::FromUTF8("\xe2\x96\xbc"));
	text->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	sizerTop->Add(text, flags);



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 10);



	wxSizer* const targetLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const targetBody = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const targetDetails = new wxBoxSizer(wxVERTICAL);

	// Avatar
	targetAvatar = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(avatarSize, avatarSize)));

	targetLayout->Add(targetAvatar, flags);


	// Target Name
	text = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(target));
	text->SetFont(wxFont(16, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	targetBody->Add(text, flags);
	
	if (steamConnected && steamFriends != NULL)
	{
		if (steamFriends->GetFriendRelationship(*getTargetCID()) == k_EFriendRelationshipFriend)
		{
			targetBody->Add(new wxButton(this, wxID_ContactTarget, "Contact Friend"), flags);
		}
	}


	// Add Target Body
	targetDetails->Add(targetBody, flags);


	// Steamid
	text2 = new wxTextCtrl(this, wxID_ANY, targetID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE|wxTE_READONLY);

	text2->SetFont(wxFont(14, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	targetDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	targetLayout->Add(targetDetails, flags);
	sizerTop->Add(targetLayout, flags);


	// Start the Timers
	avatarTimer = new SecondTimer(getClientCID(), getTargetCID(), clientAvatar, targetAvatar);

	avatarTimer->startTimer();



	// Static line
	sizerTop->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);



	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Takeover button
	takeover = new wxButton(this, wxID_CheckDone, "Take Over");
	takeover->Enable(steamConnected);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_ConnectCall, "Connect"), flags);
	sizerBtns->Add(new wxButton(this, wxID_CloseCall, "Close"), flags);
	sizerBtns->Add(takeover, flags);


	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags);

	// Auto Size
	SetSizerAndFit(sizerTop, true);
	
	// Centre to Screen
	Centre();

	// Show the Window
	Show(true);

	// Set Icon
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
}


// We need the 64bit int id
CSteamID* CallDialog::steamIDtoCSteamID(char* steamid)
{
	// To small
	if (strlen(steamid) < 11)
	{
		return NULL;
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
		return NULL;
	}

	uint64 uintID = (uint64)authID * 2;

	// Convert to a uint64
	uintID += 76561197960265728 + server;

	CSteamID *csteam = new CSteamID();
	csteam->SetFromUint64(uintID);

	// Return it
	return csteam;
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

// Mark it checked
void CallDialog::OnCheck(wxCommandEvent& WXUNUSED(event))
{
	// Disable Take Over Button
	setTakeOver(false);

	ShellExecute(NULL, L"open", s2ws("steam://connect/" + fullIP).c_str(), NULL, NULL, SW_SHOWNORMAL);
	
	Show(false);
}


// Contact Client
void CallDialog::OnContactClient(wxCommandEvent& WXUNUSED(event))
{
	// Open Chat
	ShellExecute(NULL, L"open", s2ws("steam://friends/message/" + (wxString() << getClientCID()->ConvertToUint64())).c_str(), NULL, NULL, SW_SHOWNORMAL);
}


// Contact Target
void CallDialog::OnContactTarget(wxCommandEvent& WXUNUSED(event))
{
	// Open Chat
	ShellExecute(NULL, L"open", s2ws("steam://friends/message/" + (wxString() << getTargetCID()->ConvertToUint64())).c_str(), NULL, NULL, SW_SHOWNORMAL);
}


// Window Event -> disable Window
void CallDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Show(false);
}
