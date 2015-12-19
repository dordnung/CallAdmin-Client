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

#include "curl_util.h"
#include "calladmin-client.h"

#include <wx/tokenzr.h>
#include <wx/statline.h>
#include <wx/tooltip.h>

#include "tinyxml2/tinyxml2.h"


// Event ID's for Call Dialog
enum {
	wxID_ConnectCall = wxID_HIGHEST + 50,
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


CallDialog::~CallDialog() {
	if (this->avatarTimer != NULL) {
		this->avatarTimer->Stop();
		delete this->avatarTimer;
	}
}


// Start the Call
void CallDialog::StartCall(bool show) {

	// Create Box
	this->sizerTop = new wxBoxSizer(wxVERTICAL);

	// Panel
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	// Border and Center
	wxSizerFlags flags;
	wxStaticText* text;
	wxTextCtrl* text2;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();

	// ToolTip for Contact friend 
	wxToolTip* contactTooltip = new wxToolTip("This will open a Steam chat with this client.");

	contactTooltip->SetDelay(500);
	contactTooltip->Enable(true);

	// Time
	char buffer[80];

	// But first we need a Time
	time_t tt = (time_t)GetTime();

	struct tm* dt = localtime(&tt);

	strftime(buffer, sizeof(buffer), "%c", dt);

	// New Call
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(this->serverName));

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	this->sizerTop->Add(text, flags.Border(wxALL &~wxBOTTOM, 20));


	// New Call At
	text = new wxStaticText(panel, wxID_ANY, "at " + (wxString)buffer);

	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	// Add it
	this->sizerTop->Add(text, flags.Border(wxALL &~wxTOP &~wxBOTTOM, 20));

	// New Call At
	if (!this->isHandled) {
		this->doneText = new wxStaticText(panel, wxID_ANY, "Unfinished");

		this->doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		this->doneText->SetForegroundColour(wxColor("red"));
	} else {
		this->doneText = new wxStaticText(panel, wxID_ANY, "Finished");

		this->doneText->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));
		this->doneText->SetForegroundColour(wxColour(34, 139, 34));
	}

	// Add it
	this->sizerTop->Add(this->doneText, flags.Border(wxALL &~wxTOP, 20));

	// Restore Border
	flags.Border(wxALL, 10);

	// Static line 
	this->sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);

	wxSizer* const clientLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const clientDetails = new wxBoxSizer(wxVERTICAL);

	// Avatar
#if defined(__WXMSW__)
	this->clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#else
	this->clientAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(aGetApp().GetAppPath("resources/calladmin_avatar.bmp")).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#endif

	clientLayout->Add(this->clientAvatar, flags);

	// Caller Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(this->client));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	clientDetails->Add(text, flags);

	if (caGetSteamThread()->IsConnected() && this->clientCId.IsValid()) {
		if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(this->clientCId) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactClient = new wxButton(panel, wxID_ContactClient, "Contact Friend");

			contactClient->SetToolTip(contactTooltip);

			clientDetails->Add(contactClient, 0, wxALL &~wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
		}
	}

	// Steamid
	text2 = new wxTextCtrl(panel, wxID_ANY, clientId, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE | wxTE_READONLY);

	text2->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clientDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);

	clientLayout->Add(clientDetails, flags);
	this->sizerTop->Add(clientLayout, flags);


	// Static line
	this->sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 10);

	// Reason
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("\xe2\x96\xbc") + " reported because of reason: \"" + wxString::FromUTF8(reason) + "\" " + wxString::FromUTF8("\xe2\x96\xbc"));
	text->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	this->sizerTop->Add(text, flags);

	// Static line
	this->sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 10);

	wxSizer* const targetLayout = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* const targetDetails = new wxBoxSizer(wxVERTICAL);

	// Avatar
#if defined(__WXMSW__)
	this->targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage("calladmin_avatar", wxBITMAP_TYPE_RESOURCE).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#else
	this->targetAvatar = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(caGetApp().GetAppPath("resources/calladmin_avatar.bmp")).Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize())));
#endif

	targetLayout->Add(this->targetAvatar, flags);

	// Target Name
	text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(this->target));
	text->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	targetDetails->Add(text, flags);

	if (caGetSteamThread()->IsConnected() && this->targetCId.IsValid()) {
		if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(this->targetCId) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactTarget = new wxButton(panel, wxID_ContactTarget, "Contact Friend");

			contactTarget->SetToolTip(contactTooltip);

			targetDetails->Add(contactTarget, 0, wxALL &~wxTOP | wxALIGN_CENTER_HORIZONTAL, 5);
		}
	}


	// Steamid
	text2 = new wxTextCtrl(panel, wxID_ANY, targetId, wxDefaultPosition, wxSize(220, -1), wxTE_CENTRE | wxTE_READONLY);

	text2->SetFont(wxFont(14, FONT_FAMILY, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	targetDetails->Add(text2, 0, wxEXPAND | wxALL | wxHORIZONTAL, 10);


	targetLayout->Add(targetDetails, flags);
	this->sizerTop->Add(targetLayout, flags);

	// Start the Timers
	this->avatarTimer = new AvatarTimer(&this->clientCId, &this->targetCId, this->clientAvatar, this->targetAvatar);
	this->avatarTimer->Start(100);

	// Static line
	sizerTop->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND | wxALL, 5);

	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// ToolTip for Take over button
	wxToolTip* takeOverTooltip = new wxToolTip("This will mark the call as finished and will ignore it next time.");

	takeOverTooltip->SetDelay(500);
	takeOverTooltip->Enable(true);

	// Takeover button
	this->takeover = new wxButton(panel, wxID_CheckDone, "Take Over");
	this->takeover->SetToolTip(takeOverTooltip);

	// ToolTip for Contact Trackers button
	wxToolTip* contactTrackersTooltip = new wxToolTip("This will send all available trackers on your friendlist a message.");

	contactTrackersTooltip->SetDelay(500);
	contactTrackersTooltip->Enable(true);

	// Contect trackers button
	this->contactTrackers = new wxButton(panel, wxID_ContactTrackers, "Contact Trackers");
	this->contactTrackers->Enable(true);
	this->contactTrackers->SetToolTip(contactTrackersTooltip);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(panel, wxID_ConnectCall, "Connect"), 0, wxALL &~wxRIGHT, 5);
	sizerBtns->Add(this->takeover, 0, wxALL &~wxRIGHT &~wxLEFT, 5);
	sizerBtns->Add(this->contactTrackers, 0, wxALL &~wxLEFT, 5);

	// Add Buttons to Box
	this->sizerTop->Add(sizerBtns, flags);

	// Auto Size
	panel->SetSizerAndFit(this->sizerTop, true);

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


void CallDialog::SetFinish() {
	this->doneText->SetLabelText("Finished");
	this->doneText->SetForegroundColour(wxColour(34, 139, 34));

	this->sizerTop->Layout();
}


// We need the 64bit int id
CSteamID CallDialog::SteamIdtoCSteamId(wxString steamId) {
	CSteamID cSteamId;

	// To small
	if (wxStrlen(steamId) < 11) {
		return cSteamId;
	}

	// Strip the steamid
	wxStringTokenizer tokenizer(steamId, ":");

	// There should be three tokens
	if (tokenizer.CountTokens() < 3) {
		return cSteamId;
	}

	tokenizer.GetNextToken();
	int server = wxAtoi(tokenizer.GetNextToken());
	int authId = wxAtoi(tokenizer.GetNextToken());

	// Wrong Format
	if (authId == 0) {
		return cSteamId;
	}

	uint64 uintId = (uint64)authId * 2;

	// Convert to a uint64
	uintId += 76561197960265728 + server;

	cSteamId.SetFromUint64(uintId);

	// Return it
	return cSteamId;
}


// Button Event -> Connect to Server
void CallDialog::OnConnect(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Connected to the Server " + this->fullIP);

#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://connect/" + this->fullIP).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://connect/" + this->fullIP).mb_str().data());
#endif

	Show(false);
}


// Mark it checked
void CallDialog::OnCheck(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Marke call " + this->callId + " as finished");

	// page
	wxString page = (caGetConfig()->GetPage() + "/takeover.php?callid=" + this->callId + "&key=" + caGetConfig()->GetKey());

	// Get Page
	caGetApp().GetPage(CallDialog::OnChecked, page, this->id);
}


// Contact Client
void CallDialog::OnContactClient(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + (wxString)this->clientCId.Render());

	// Open Chat
#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://friends/message/" + (wxString() << this->clientCId.ConvertToUint64())).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://friends/message/" + (wxString() << this->clientCId.ConvertToUint64())).mb_str().data());
#endif
}


// Contact Target
void CallDialog::OnContactTarget(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + (wxString)this->targetCId.Render());

	// Open Chat
#if defined(__WXMSW__)
	ShellExecute(NULL, L"open", ("steam://friends/message/" + (wxString() << this->targetCId.ConvertToUint64())).wc_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	system(("xdg-open steam://friends/message/" + (wxString() << this->targetCId.ConvertToUint64())).mb_str().data());
#endif
}


// Contact Trackers
void CallDialog::OnContactTrackers(wxCommandEvent& WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacting current Trackers");

	// Are we steam connected?
	if (caGetSteamThread()->IsConnected()) {
		// page
		wxString page = caGetConfig()->GetPage() + "/trackers.php?from=25&from_type=interval&key=" + caGetConfig()->GetKey();

		caGetApp().GetPage(CallDialog::OnGetTrackers, page, this->id);

		return;
	}

	caGetTaskBarIcon()->ShowMessage("Couldn't contact trackers!", "You're not connected with STEAM!", this);
}


// Contact Client
void CallDialog::OnGetTrackers(char* errors, wxString result, int x) {
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
								CSteamID steamidTracker = caGetCallDialogs()->at(x)->SteamIdtoCSteamId(steamidString);

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

	caGetTaskBarIcon()->ShowMessage("Couldn't contact trackers!", error, caGetCallDialogs()->at(x));
}


// Mark checked
void CallDialog::OnChecked(char* errors, wxString result, int x) {
	// Log Action
	caLogAction("Marked call " + caGetCallDialogs()->at(x)->GetId() + " as finished");

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

	caGetTaskBarIcon()->ShowMessage("Couldn't take over call!", error, caGetCallDialogs()->at(x));
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




// Init. Timer
AvatarTimer::AvatarTimer(CSteamID *clientId, CSteamID *targetId, wxStaticBitmap* clientAvatar, wxStaticBitmap* targetAvatar) : wxTimer(this, -1) {
	this->clientId = clientId;
	this->targetId = targetId;
	this->clientAvatar = clientAvatar;
	this->targetAvatar = targetAvatar;
	this->attempts = 0;

	this->clientLoaded = false;
	this->targetLoaded = false;

	// Invalid Steamids -> Loaded
	if (this->clientId == NULL || !this->clientId->IsValid()) {
		this->clientLoaded = true;
	}

	if (this->targetId == NULL || !this->targetId->IsValid()) {
		this->targetLoaded = true;
	}
}


// Timer to update avatars
void AvatarTimer::Notify() {
	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		// Do we have information about the users?
		// Load the images
		if (!this->clientLoaded) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(*this->clientId, false)) {
				// Try to laod caller avatar
				this->clientLoaded = SetAvatar(this->clientId, this->clientAvatar);
			}
		}

		if (!this->targetLoaded) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(*this->targetId, false)) {
				// Try to laod target avatar
				this->targetLoaded = SetAvatar(this->targetId, this->targetAvatar);
			}
		}
	}

	// All loaded or 10 seconds gone?
	if (++this->attempts == 100 || (this->clientLoaded && this->targetLoaded)) {
		// Enough, stop timer
		Stop();
	}
}


// Set new Avatar
bool AvatarTimer::SetAvatar(CSteamID *id, wxStaticBitmap* map) {
	// Load avatar
	int avatar = caGetSteamThread()->GetSteamFriends()->GetLargeFriendAvatar(*id);

	// Could it load?
	if (avatar > 0) {
		// Buffer to store picture
		//uint8* rgbaBuffer = (uint8 *)ALLOCA(4 * 184 * 184);
		uint8 *rgbaBuffer = new uint8[4 * 184 * 184];
		uint32 *size = new uint32(184);

		// Is Size valid?
		if (caGetSteamThread()->GetSteamUtils()->GetImageSize(avatar, size, size)) {
			// Load Image
			if (caGetSteamThread()->GetSteamUtils()->GetImageRGBA(avatar, rgbaBuffer, (4 * 184 * 184))) {
				// Image
				wxImage image(184, 184);

				// RGBA to Image
				for (int y = 0; y < 184; y++) {
					int start = 184 * y * 4;

					for (int x = 0; x < 184; x++) {
						// Set Colour
						image.SetRGB(x, y, rgbaBuffer[start], rgbaBuffer[start + 1], rgbaBuffer[start + 2]);

						start += 4;
					}
				}

				if (caGetApp().GetAvatarSize() != 184) {
					image.Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize());
				}

				// Set new Avatar
				map->SetBitmap(wxBitmap(image));

				caLogAction("Loaded Avatar of " + (wxString)id->Render());

				// Clean
				delete size;
				delete rgbaBuffer;

				// It's loaded
				return true;
			}
		}

		// Clean
		delete size;
		delete rgbaBuffer;
	}

	return false;
}