/**
 * -----------------------------------------------------
 * File        call_dialog.h
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

// Includes Project
#include "curl_util.h"
#include "calladmin-client.h"

// wx
#include <wx/statline.h>
#include <wx/tooltip.h>

// xml parser
#include "tinyxml2/tinyxml2.h"


// Button ID's for Call Dialog
enum {
	wxID_ConnectCall = wxID_HIGHEST + 200,
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


CallDialog::CallDialog(const wxString &title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMINIMIZE_BOX) {
	// Initialize vars
	this->sizerTop = NULL;
	this->clientAvatar = NULL;
	this->targetAvatar = NULL;
	this->doneText = NULL;
	this->id = 0;
	this->isHandled = false;
	this->takeover = NULL;
	this->contactTrackers = NULL;
	this->avatarTimer = NULL;
}


// start the Call
void CallDialog::StartCall(bool show) {

	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);

	// Panel
	wxPanel* panel = new wxPanel(this, wxID_ANY);


	// Valid?
	if (sizerTop == NULL || panel == NULL) {
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
	if (contactTooltip == NULL) {
		return;
	}


	contactTooltip->SetDelay(500);
	contactTooltip->Enable(true);


	// Time
	char buffer[80];

	// But first we need a Time
	time_t tt = (time_t)GetTime();

	struct tm* dt = localtime(&tt);

	strftime(buffer, sizeof(buffer), "%c", dt);


	// New Call
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(serverName));

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	sizerTop->Add(text, flags.Border(wxALL &~wxBOTTOM, 20));


	// New Call At
	text = new wxStaticText(panel, wxID_ANY, "at " + (wxString)buffer);

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	sizerTop->Add(text, flags.Border(wxALL &~wxTOP &~wxBOTTOM, 20));


	// New Call At
	if (!isHandled) {
		doneText = new wxStaticText(panel, wxID_ANY, "Unfinished");

		doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		doneText->SetForegroundColour(wxColor("red"));
	} else {
		doneText = new wxStaticText(panel, wxID_ANY, "Finished");

		doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		doneText->SetForegroundColour(wxColour(34, 139, 34));
	}


	// Add it
	sizerTop->Add(doneText, flags.Border(wxALL &~wxTOP, 20));


	// Restore Border
	flags.Border(wxALL, 10);


	// Static line 
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);


	wxSizer* const clientLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const clientDetails = new wxBoxSizer(wxVERTICAL);


	// Avatar
#if defined(__WXMSW__)
	clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#else
	clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(aGetApp().GetAppPath("resources/calladmin_avatar.bmp")).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#endif

	clientLayout->Add(clientAvatar, flags);


	// Caller Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(client));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	clientDetails->Add(text, flags);

	if (caGetSteamThread()->IsConnected() && GetClientCID()->IsValid()) {
		if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(*GetClientCID()) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactClient = new wxButton(panel, wxID_ContactClient, "Contact Friend");

			contactClient->SetToolTip(contactTooltip);

			clientDetails->Add(contactClient, 0, wxALL &~wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
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
	targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#else
	targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(caGetApp().GetAppPath("resources/calladmin_avatar.bmp")).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#endif

	targetLayout->Add(targetAvatar, flags);



	// Target Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(target));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	targetDetails->Add(text, flags);


	if (caGetSteamThread()->IsConnected() && GetTargetCID()->IsValid()) {
		if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(*GetTargetCID()) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactTarget = new wxButton(panel, wxID_ContactTarget, "Contact Friend");

			contactTarget->SetToolTip(contactTooltip);

			targetDetails->Add(contactTarget, 0, wxALL &~wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
		}
	}



	// Steamid
	text2 = new wxTextCtrl(panel, wxID_ANY, targetID, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE | wxTE_READONLY);

	text2->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	targetDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	targetLayout->Add(targetDetails, flags);
	sizerTop->Add(targetLayout, flags);


	// Start the Timers
	avatarTimer = new AvatarTimer(GetClientCID(), GetTargetCID(), clientAvatar, targetAvatar);
	avatarTimer->StartTimer();


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
	sizerBtns->Add(new wxButton(panel, wxID_ConnectCall, "Connect"), 0, wxALL &~wxRIGHT, 5);
	sizerBtns->Add(takeover, 0, wxALL &~wxRIGHT &~wxLEFT, 5);
	sizerBtns->Add(contactTrackers, 0, wxALL &~wxLEFT, 5);

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags);

	// Auto Size
	panel->SetSizerAndFit(sizerTop, true);

	// Fit
	Fit();

	// Centre to Screen
	Centre();

	// Set Icon
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE));
#else
	wxLogNull nolog;

	SetIcon(wxIcon(caGetApp().GetAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON));
#endif


	// Show the Window
	Show(show);
}


// Return time as a int
long CallDialog::GetTime() const {
	return this->reportedAt;
}


// Methods for Details
wxString CallDialog::GetTarget() const {
	return this->target;
}


wxString CallDialog::GetClient() const {
	return this->client;
}


wxString CallDialog::GetID() const {
	return this->callID;
}


wxString CallDialog::GetServer() {
	return this->serverName;
}


wxString CallDialog::GetBoxText() {
	return this->boxText;
}


AvatarTimer* CallDialog::GetAvatarTimer() {
	return this->avatarTimer;
}


wxButton* CallDialog::GetContactTrackersButton() {
	return this->contactTrackers;
}


wxButton* CallDialog::GetTakeoverButton() {
	return this->takeover;
}


CSteamID* CallDialog::GetClientCID() {
	return &this->clientCID;
}


CSteamID* CallDialog::GetTargetCID() {
	return &this->targetCID;
}


bool CallDialog::GetHandled() {
	return this->isHandled;
}


// Privat -> set Methods
void CallDialog::SetCallID(const char* id) {
	this->callID = id;
}


void CallDialog::SetIP(const char* ip) {
	this->fullIP = ip;
}


void CallDialog::SetName(wxString name) {
	this->serverName = name;
}


void CallDialog::SetTarget(const char* name) {
	this->target = name;
}


void CallDialog::SetTargetID(const char* steamidID) {
	this->targetID = steamidID;
	this->targetCID = SteamIDtoCSteamID((char*)(steamidID));
}


void CallDialog::SetClient(const char* name) {
	this->client = name;
}


void CallDialog::SetClientID(const char* steamidID) {
	this->clientID = steamidID;
	this->clientCID = SteamIDtoCSteamID((char*)(steamidID));
}


void CallDialog::SetReason(const char* name) {
	this->reason = name;
}


void CallDialog::SetID(int num) {
	this->id = num;
}


void CallDialog::SetTime(long time) {
	this->reportedAt = time;
}


void CallDialog::SetBoxText(wxString text) {
	this->boxText = text;
}


void CallDialog::SetHandled(bool handled) {
	this->isHandled = handled;
}


void CallDialog::SetFinish() {
	this->doneText->SetLabelText("Finished");
	this->doneText->SetForegroundColour(wxColour(34, 139, 34));

	this->sizerTop->Layout();
}


// We need the 64bit int id
CSteamID CallDialog::SteamIDtoCSteamID(char* steamid) {
	CSteamID csteam;

	// To small
	if (strlen(steamid) < 11) {
		return csteam;
	}

	int server = 0;
	int authID = 0;

	// Strip the steamid
	char *strip = strtok(steamid, ":");

	while (strip) {
		strip = strtok(NULL, ":");

		char *strip2 = strtok(NULL, ":");

		// Read server and authID
		if (strip2) {
			server = atoi(strip);
			authID = atoi(strip2);
		}
	}

	// Wrong Format
	if (authID == 0) {
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
void CallDialog::OnConnect(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Connected to the Server " + fullIP);

#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://connect/" + fullIP).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://connect/" + fullIP).mb_str().data());
#endif

	Show(false);
}


// Mark it checked
void CallDialog::OnCheck(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Marke call " + callID + " as finished");

	// page
	wxString pager = (caGetConfig()->GetPage() + "/takeover.php?callid=" + callID + "&key=" + caGetConfig()->GetKey());

	// Get Page
	CurlThread::GetPage(OnChecked, pager, id);
}


// Contact Client
void CallDialog::OnContactClient(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + (wxString)GetClientCID()->Render());

	// Open Chat
#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://friends/message/" + (wxString() << GetClientCID()->ConvertToUint64())).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://friends/message/" + (wxString() << getClientCID()->ConvertToUint64())).mb_str().data());
#endif
}


// Contact Target
void CallDialog::OnContactTarget(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + (wxString)GetTargetCID()->Render());

	// Open Chat
#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://friends/message/" + (wxString() << GetTargetCID()->ConvertToUint64())).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://friends/message/" + (wxString() << getTargetCID()->ConvertToUint64())).mb_str().data());
#endif
}


// Contact Trackers
void CallDialog::OnContactTrackers(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacting current Trackers");

	// Are we steam connected?
	if (caGetSteamThread()->IsConnected()) {
		// page
		wxString pager = caGetConfig()->GetPage() + "/trackers.php?from=25&from_type=interval&key=" + caGetConfig()->GetKey();

		CurlThread::GetPage(OnGetTrackers, pager, id);

		return;
	}

	caGetTaskBarIcon()->ShowMessage("Coulnd't contact trackers!", "You're not connected with STEAM!", this);
}


// Contact Client
void OnGetTrackers(char* errors, wxString result, int x) {
	// Log Action
	caLogAction("Got Trackers");

	wxString error = "";

	if (result != "") {
		// Everything good :)
		if (strcmp(errors, "") == 0) {
			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parsing good :)?
			if (parseError == tinyxml2::XML_SUCCESS) {
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin_Trackers
				if (node != NULL) {
					node = node->NextSibling();
				}

				// Found Trackers?
				if (node != NULL) {
					// found someone?
					bool found = false;

					// Tracker Loop
					for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling()) {
						// API Error?
						if ((wxString)node2->Value() == "error") {
							error = node2->FirstChild()->Value();

							break;
						}

						// Search admin steamids
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling()) {
							// Found steamid
							if ((wxString)node3->Value() == "trackerID" && caGetSteamThread()->IsConnected()) {
								wxString steamidString = node3->FirstChild()->Value();

								// Build csteamid
								CSteamID steamidTracker = caGetCallDialogs()->at(x)->SteamIDtoCSteamID((char*)steamidString.mb_str().data());

								// Are we friends and is tracker online? :))
								if (steamidTracker.IsValid() && caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(steamidTracker) == k_EFriendRelationshipFriend && caGetSteamThread()->GetSteamFriends()->GetFriendPersonaState(steamidTracker) != k_EPersonaStateOffline) {
									// Now we write a message
									caGetSteamThread()->GetSteamFriends()->ReplyToFriendMessage(steamidTracker, "Hey, i contact you because of the call from " + caGetCallDialogs()->at(x)->GetClient() + " about " + caGetCallDialogs()->at(x)->GetTarget());

									// And we found someone :)
									if (!found) {
										found = true;

										// So no contacting possible anymore
										caGetCallDialogs()->at(x)->GetContactTrackersButton()->Enable(false);
									}
								}
							}
						}
					}

					// Have we found something?
					if (found) {
						// We are finished :)
						return;
					}

				}
			} else {
				// XML ERROR
				error = "XML ERROR: Couldn't parse the trackers API!";

				// Log Action
				caLogAction("XML Error in trackers API");
			}
		} else {
			// Curl error
			error = errors;

			// Log Action
			caLogAction("CURL Error " + error);
		}
	} else {
		// Curl error
		error = "Couldn't init. CURL connection";
	}


	// Seems we found no one
	if (error == "") {
		error = "Found no available tracker on your friendlist!";
	}

	caGetTaskBarIcon()->ShowMessage("Coulnd't contact trackers!", error, caGetCallDialogs()->at(x));
}


// Mark checked
void OnChecked(char* errors, wxString result, int x) {
	// Log Action
	caLogAction("Marked call " + caGetCallDialogs()->at(x)->GetID() + " as finished");

	wxString error = "";

	if (result != "") {
		// Everything good :)
		if (strcmp(errors, "") == 0) {
			// Proceed XML result!
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLNode *node;
			tinyxml2::XMLError parseError;

			// Parse the xml data
			parseError = doc.Parse(result);

			// Parsing good :)?
			if (parseError == tinyxml2::XML_SUCCESS) {
				// Goto xml child
				node = doc.FirstChild();

				// Goto CallAdmin_Takeover
				if (node != NULL) {
					node = node->NextSibling();
				}

				// Found something?
				if (node != NULL) {
					// Goto first child
					node = node->FirstChild();

					// API Error?
					if ((wxString)node->Value() == "error") {
						error = node->FirstChild()->Value();
					}

					// Success?
					if ((wxString)node->Value() == "success") {
						caGetMainPanel()->SetHandled(x);

						return;
					}
				}
			} else {
				error = "XML ERROR: Couldn't parse the takeover API!";
			}
		} else {
			// Curl error
			error = errors;
		}
	} else {
		// Curl error
		error = "Couldn't init. CURL connection";
	}

	// Seems empty
	if (error == "") {
		error = "Invalid XML structure!";
	}

	caGetTaskBarIcon()->ShowMessage("Coulnd't take over call!", error, caGetCallDialogs()->at(x));
}


// Window Event -> disable Window
void CallDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	Show(false);
}


// Window Event -> Hide Window
void CallDialog::OnMinimizeWindow(wxIconizeEvent& WXUNUSED(event)) {
	if (caGetConfig()->GetHideOnMinimize()) {
		Show(false);
	}
}
