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

#include <wx/tokenzr.h>
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
	EVT_ICONIZE(CallDialog::OnMinimizeWindow)
wxEND_EVENT_TABLE()


CallDialog::CallDialog() {
	// Initialize vars
	this->avatarsLoaded = false;
	this->sizerTop = NULL;
	this->clientAvatar = NULL;
	this->targetAvatar = NULL;
	this->doneText = NULL;
	this->id = 0;
	this->isHandled = false;
	this->takeover = NULL;
	this->contactTrackers = NULL;
}


// Start the Call
bool CallDialog::StartCall(bool show) {
	if (!wxXmlResource::Get()->LoadDialog(this, NULL, "callDialog")) {
		wxMessageBox("Error: Couldn't find XRCID callDialog", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Text helper
	wxStaticText *text;
	wxTextCtrl *textCtrl;

	// Panel
	wxWindow *panel;
	FIND_OR_FAIL(panel, this->GetChildren().GetFirst()->GetData(), "callPanel");

	// Sizer
	FIND_OR_FAIL(this->sizerTop, panel->GetSizer(), "callDialogSizerTop");

	// New Call
	FIND_OR_FAIL(text, XRCCTRL(*this, "contactText", wxStaticText), "contactText");
	text->SetLabel(wxString::FromUTF8(this->serverName));

	// New Call At
	FIND_OR_FAIL(text, XRCCTRL(*this, "callAtText", wxStaticText), "callAtText");
	text->SetLabel(text->GetLabel() + wxDateTime((time_t)GetTime()).Format("%c"));

	// Handled
	FIND_OR_FAIL(this->doneText, XRCCTRL(*this, "doneText", wxStaticText), "doneText");

	if (!this->isHandled) {
		this->doneText->SetLabel("Unfinished");
		this->doneText->SetForegroundColour(wxColor("red"));
	} else {
		this->doneText->SetLabel("Finished");
		this->doneText->SetForegroundColour(wxColour(34, 139, 34));
	}

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
	text->SetLabel(wxString::FromUTF8("\xe2\x96\xbc") + " reported because of reason: \"" + wxString::FromUTF8(reason) + "\" " + wxString::FromUTF8("\xe2\x96\xbc"));

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

	// Contect trackers button
	FIND_OR_FAIL(this->contactTrackers, XRCCTRL(*this, "contactTrackersButton", wxButton), "contactTrackersButton");

	// Load avatars
	LoadAvatars();

	// Auto Size
	this->sizerTop->Layout();
	this->sizerTop->Fit(panel);

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
		// Start the Timer (It will be killed by itself)
		AvatarTimer *timer = new AvatarTimer(this, &this->clientCId, &this->targetCId, this->clientAvatar, this->targetAvatar);
		timer->Start(100, wxTIMER_CONTINUOUS);
	}
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
void CallDialog::OnConnect(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Connected to the Server " + this->fullIP);

	#if defined(__WXMSW__)
		wxExecute("explorer steam://connect/" + this->fullIP);
	#else
		wxExecute("steam steam://connect/" + this->fullIP);
	#endif

	Show(false);
}


// Mark it checked
void CallDialog::OnCheck(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Mark call " + this->callId + " as finished");

	// page
	wxString page = caGetConfig()->GetPage() + "/takeover.php?callid=" + this->callId + "&key=" + caGetConfig()->GetKey();

	// Get Page
	caGetApp().GetPage(CallDialog::OnChecked, page, this->id);
}


// Contact Client
void CallDialog::OnContactClient(wxCommandEvent &WXUNUSED(event)) {
	// Log Action
	caLogAction("Contacted Client " + (wxString)this->clientCId.Render());

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
	caLogAction("Contacted Client " + (wxString)this->targetCId.Render());

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
	caLogAction("Contacting current Trackers");

	// Are we steam connected?
	if (caGetSteamThread()->IsConnected()) {
		// page
		wxString page = caGetConfig()->GetPage() + "/trackers.php?from=25&from_type=interval&key=" + caGetConfig()->GetKey();

		caGetApp().GetPage(CallDialog::OnGetTrackers, page, this->id);

		return;
	}

	caGetTaskBarIcon()->ShowMessage("Couldn't contact trackers!", "You're not connected to STEAM!", this);
}


// Contact Client
void CallDialog::OnGetTrackers(wxString errorStr, wxString result, int x) {
	// Log Action
	caLogAction("Got Trackers");

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
								if (steamidTracker.IsValid() && OpenSteamHelper::GetInstance()->SteamFriends()->GetFriendRelationship(steamidTracker) == k_EFriendRelationshipFriend && OpenSteamHelper::GetInstance()->SteamFriends()->GetFriendPersonaState(steamidTracker) != k_EPersonaStateOffline) {
									// Now we write a message
									OpenSteamHelper::GetInstance()->SteamFriends()->ReplyToFriendMessage(steamidTracker, "Hey, i contact you because of the call from " + caGetCallDialogs()->at(x)->GetClient() + " about " + caGetCallDialogs()->at(x)->GetTarget());

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
				caLogAction("XML Error in trackers API", LogLevel::LEVEL_ERROR);
			}
		} else {
			// Curl error
			error = errorStr;

			// Log Action
			caLogAction("CURL Error " + error, LogLevel::LEVEL_ERROR);
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
void CallDialog::OnChecked(wxString errorStr, wxString result, int x) {
	// Log Action
	caLogAction("Marked call " + caGetCallDialogs()->at(x)->GetId() + " as finished");

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
						caGetMainPanel()->SetHandled(x);

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


// Timer to update avatars
void AvatarTimer::Notify() {
	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		// Do we have information about the users?
		// Load the images
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

				caLogAction("Loaded Avatar of " + (wxString)id->Render());

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