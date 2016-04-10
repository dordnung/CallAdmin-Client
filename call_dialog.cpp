/**
 * -----------------------------------------------------
 * File        call_dialog.cpp
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

#include "tinyxml2/tinyxml2.h"

#include <wx/statline.h>
#include <wx/tooltip.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Button Events for Call Dialog
wxBEGIN_EVENT_TABLE(CallDialog, wxDialog)
	EVT_BUTTON(XRCID("connectButton"), CallDialog::OnConnect)
	EVT_BUTTON(XRCID("takeoverButton"), CallDialog::OnCheck)
	EVT_BUTTON(XRCID("contactClientButton"), CallDialog::OnContactClient)
	EVT_BUTTON(XRCID("contactTargetButton"), CallDialog::OnContactTarget)
	EVT_BUTTON(XRCID("contactTrackersButton"), CallDialog::OnContactTrackers)

	EVT_CLOSE(CallDialog::OnCloseWindow)
wxEND_EVENT_TABLE()


CallDialog::CallDialog() {
	// Initialize vars
	this->avatarsLoaded = false;
	this->clientAvatar = NULL;
	this->targetAvatar = NULL;
	this->avatarTimer = NULL;
	this->id = 0;
	this->isHandled = false;
	this->takeover = NULL;
	this->contactTrackers = NULL;
}


CallDialog::~CallDialog() {
	if (this->avatarTimer) {
		this->avatarTimer->Stop();
		wxDELETE(this->avatarTimer);
	}
}

// Start the Call
bool CallDialog::StartCall(bool show) {
	if (!wxXmlResource::Get()->LoadDialog(this, NULL, "callDialog")) {
		wxMessageBox("Error: Couldn't find XRCID callDialog", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Set title to Finished or not finished
	if (!this->isHandled) {
		this->SetTitle("Unfinished " + wxString::FromUTF8("\xE2\x9C\x96"));
	} else {
		this->SetTitle("Finished " + wxString::FromUTF8("\xE2\x9C\x94"));
	}

	// Text helper
	wxStaticText *text;
	wxTextCtrl *textCtrl;

	// Panel
	FIND_OR_FAIL(this->panel, (wxPanel *)this->GetChildren().GetFirst()->GetData(), "callPanel");

	// New Call
	FIND_OR_FAIL(text, XRCCTRL(*this, "contactText", wxStaticText), "contactText");
	text->SetLabel(this->serverName);

	// New Call At
	FIND_OR_FAIL(text, XRCCTRL(*this, "callAtText", wxStaticText), "callAtText");
	text->SetLabel(text->GetLabel() + this->reportedAt.Format("%c"));

	// Client Avatar
	FIND_OR_FAIL(this->clientAvatar, XRCCTRL(*this, "clientAvatar", wxStaticBitmap), "clientAvatar");

	// Caller Name
	FIND_OR_FAIL(text, XRCCTRL(*this, "clientName", wxStaticText), "clientName");
	text->SetLabel(wxString::FromUTF8(this->client));

	if (caGetSteamThread()->IsConnected() && this->clientCId.IsValid()) {
		if (OpenSteamHelper::GetInstance()->SteamFriends()->GetFriendRelationship(this->clientCId) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactClient;

			FIND_OR_FAIL(contactClient, XRCCTRL(*this, "contactClientButton", wxButton), "contactClientButton");
			contactClient->Enable(true);
		}
	}

	// Steamid
	FIND_OR_FAIL(textCtrl, XRCCTRL(*this, "clientSteamId", wxTextCtrl), "clientSteamId");
	textCtrl->SetValue(this->clientId);

	// Reason
	FIND_OR_FAIL(text, XRCCTRL(*this, "reasonText", wxStaticText), "reasonText");
	text->SetLabel(wxString::FromUTF8(reason));

	// Client Avatar
	FIND_OR_FAIL(this->targetAvatar, XRCCTRL(*this, "targetAvatar", wxStaticBitmap), "targetAvatar");

	// Target Name
	FIND_OR_FAIL(text, XRCCTRL(*this, "targetName", wxStaticText), "targetName");
	text->SetLabel(wxString::FromUTF8(this->target));

	if (caGetSteamThread()->IsConnected() && this->targetCId.IsValid()) {
		if (OpenSteamHelper::GetInstance()->SteamFriends()->GetFriendRelationship(this->targetCId) == k_EFriendRelationshipFriend) {
			// Add Contact Friend button
			wxButton *contactTarget;

			FIND_OR_FAIL(contactTarget, XRCCTRL(*this, "contactTargetButton", wxButton), "contactTargetButton");
			contactTarget->Enable(true);
		}
	}

	// Steamid
	FIND_OR_FAIL(textCtrl, XRCCTRL(*this, "targetSteamId", wxTextCtrl), "targetSteamId");
	textCtrl->SetValue(targetId);

	// Takeover button
	FIND_OR_FAIL(this->takeover, XRCCTRL(*this, "takeoverButton", wxButton), "takeoverButton");
	this->takeover->Enable(!this->isHandled);

	// Contect trackers button
	FIND_OR_FAIL(this->contactTrackers, XRCCTRL(*this, "contactTrackersButton", wxButton), "contactTrackersButton");

	// Resize default avatars depending on the screen height
	wxImage clientAvatarImage = this->clientAvatar->GetBitmap().ConvertToImage();
	wxImage targetAvatarImage = this->targetAvatar->GetBitmap().ConvertToImage();
	clientAvatarImage.Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize());
	targetAvatarImage.Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize());
	clientAvatar->SetBitmap(wxBitmap(clientAvatarImage));
	targetAvatar->SetBitmap(wxBitmap(targetAvatarImage));

	// Load avatars from Steam
	LoadAvatars();

	// Fit panel
	this->panel->Layout();
	this->panel->Fit();

	// Fit
	Fit();

	// Centre to Screen
	Centre();

	// Show the Window
	Show(show);

	return true;
}


void CallDialog::LoadAvatars() {
	if (!this->avatarsLoaded && caGetApp().IsRunning()) {
		if (this->avatarTimer) {
			this->avatarTimer->Stop();
			wxDELETE(this->avatarTimer);
		}

		// Start the Timer (It will be killed by itself)
		this->avatarTimer = new AvatarTimer(this, &this->clientCId, &this->targetCId, this->clientAvatar, this->targetAvatar);
		this->avatarTimer->Start(100, wxTIMER_CONTINUOUS);
	}
}


void CallDialog::SetFinish() {
	this->SetTitle("Finished " + wxString::FromUTF8("\xE2\x9C\x94"));
}


// Mark checked
void CallDialog::OnChecked(wxString errorStr, wxString result, int x) {
	// Log Action
	caLogAction("Marked call " + caGetCallDialogs()->at(x)->GetId() + " as finished", LogLevel::LEVEL_INFO);

	wxString error = "";

	if (result != "") {
		// Everything good :)
		if (errorStr == "") {
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
						caGetCallPanel()->SetHandled(x);

						return;
					}
				}
			} else {
				error = "XML ERROR: Couldn't parse the takeover API!";
			}
		} else {
			// Curl error
			error = errorStr;
		}
	} else {
		// Curl error
		error = "Couldn't init. CURL connection";
	}

	// Seems empty
	if (error == "") {
		error = "Invalid XML structure!";
	}

	caGetTaskBarIcon()->ShowMessage("Couldn't take over call!", error, caGetCallDialogs()->at(x), true);
}


// Button Event -> Connect to Server
void CallDialog::OnConnect(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Connect to the Server " + this->fullIP, LogLevel::LEVEL_INFO);

	#if defined(__WXMSW__)
		wxExecute("explorer steam://connect/" + this->fullIP);
	#else
		wxExecute("steam steam://connect/" + this->fullIP);
	#endif

	Show(false);
}


// Mark it as finished
void CallDialog::OnCheck(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Mark call " + this->callId + " as finished", LogLevel::LEVEL_INFO);

	// page
	wxString page = caGetConfig()->GetPage() + "/takeover.php?callid=" + this->callId + "&key=" + caGetConfig()->GetKey();

	// Get Page
	caGetApp().GetPage(CallDialog::OnChecked, page, this->id);
}


// Contact Client
void CallDialog::OnContactClient(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + this->clientId, LogLevel::LEVEL_INFO);

	// Open Chat
	#if defined(__WXMSW__)
		wxExecute("explorer steam://friends/message/" + wxString() << this->clientCId.ConvertToUint64());
	#else
		wxExecute("steam steam://friends/message/" + wxString() << this->clientCId.ConvertToUint64());
	#endif
}


// Contact Target
void CallDialog::OnContactTarget(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted target " + this->targetId, LogLevel::LEVEL_INFO);

	// Open Chat
	#if defined(__WXMSW__)
		wxExecute("explorer steam://friends/message/" + wxString() << this->targetCId.ConvertToUint64());
	#else
		wxExecute("steam steam://friends/message/" + wxString() << this->targetCId.ConvertToUint64());
	#endif
}


// Contact Trackers
void CallDialog::OnContactTrackers(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacting current Trackers", LogLevel::LEVEL_INFO);

	if (!caGetSteamThread()->IsConnected()) {
		caGetTaskBarIcon()->ShowMessage("Couldn't contact trackers!", "You're not connected to STEAM!", this, false);
	} else {
		bool found = false;

		for (wxVector<wxString>::iterator tracker = caGetTrackerPanel()->GetCurrentTrackers()->begin(); tracker != caGetTrackerPanel()->GetCurrentTrackers()->end(); ++tracker) {
			// Build csteamid
			CSteamID steamidTracker = SteamThread::SteamIdtoCSteamId(*tracker);

			// Are we friends and is tracker online? :))
			OpenSteamHelper *helper = OpenSteamHelper::GetInstance();

			if (steamidTracker.IsValid() && helper->SteamFriends()->GetFriendRelationship(steamidTracker) == k_EFriendRelationshipFriend && helper->SteamFriends()->GetFriendPersonaState(steamidTracker) != k_EPersonaStateOffline) {
				// Now we write a message
				helper->SteamFriends()->ReplyToFriendMessage(steamidTracker, "Hey, i contact you because of the call from " + this->clientId + " about " + this->targetId);

				// And we found someone, so no contacting possible anymore
				this->contactTrackers->Enable(false);
				found = true;
			}
		}

		if (!found) {
			caGetTaskBarIcon()->ShowMessage("Couldn't contact trackers!", "Found no available tracker on your friendlist!", this, false);
		}
	}
}


// Window Event -> Hide Window
void CallDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) {
	Show(false);
}


// Init. Timer
AvatarTimer::AvatarTimer(CallDialog *dialog, CSteamID *clientId, CSteamID *targetId, wxStaticBitmap *clientAvatar, wxStaticBitmap *targetAvatar) : wxTimer(this, -1) {
	this->dialog = dialog;

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


// Set new Avatar
bool AvatarTimer::SetAvatar(CSteamID *id, wxStaticBitmap *map) {
	// Load avatar
	int avatar = OpenSteamHelper::GetInstance()->SteamFriends()->GetLargeFriendAvatar(*id);

	// Could it load?
	if (avatar > 0) {
		unsigned int width;
		unsigned int height;

		// Is Size valid?
		if (OpenSteamHelper::GetInstance()->SteamUtils()->GetImageSize(avatar, &width, &height)) {
			// Buffer to store picture
			unsigned char *rgbaBuffer = new uint8[4 * width * height];

			// Load Image
			if (OpenSteamHelper::GetInstance()->SteamUtils()->GetImageRGBA(avatar, rgbaBuffer, (4 * width * height))) {
				// Image
				wxImage image(width, height);

				// RGBA to Image
				for (unsigned int y = 0; y < width; y++) {
					int start = height * y * 4;

					for (unsigned int x = 0; x < height; x++) {
						// Set Colour
						image.SetRGB(x, y, rgbaBuffer[start], rgbaBuffer[start + 1], rgbaBuffer[start + 2]);

						start += 4;
					}
				}

				// Set new Avatar
				image.Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize());
				map->SetBitmap(wxBitmap(image));

				caLogAction("Loaded Avatar of " + (wxString)id->Render(), LogLevel::LEVEL_INFO);

				// Clean
				delete[] rgbaBuffer;

				// It's loaded
				return true;
			}

			// Clean
			delete[] rgbaBuffer;
		}
	}

	return false;
}


// Timer to update avatars
void AvatarTimer::Notify() {
	if (!caGetApp().IsRunning()) {
		// Already ended
		delete this;
		return;
	}

	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		// Do we have information about the users?
		if (!this->clientLoaded) {
			if (!OpenSteamHelper::GetInstance()->SteamFriends()->RequestUserInformation(*this->clientId, false)) {
				// Try to laod caller avatar
				this->clientLoaded = SetAvatar(this->clientId, this->clientAvatar);
			}
		}

		if (!this->targetLoaded) {
			if (!OpenSteamHelper::GetInstance()->SteamFriends()->RequestUserInformation(*this->targetId, false)) {
				// Try to laod target avatar
				this->targetLoaded = SetAvatar(this->targetId, this->targetAvatar);
			}
		}
	}

	// All loaded or 10 seconds gone?
	if (++this->attempts == 100 || (this->clientLoaded && this->targetLoaded)) {
		this->dialog->SetAvatarsLoaded();

		// Enough, stop timer
		delete this;
	}
}
