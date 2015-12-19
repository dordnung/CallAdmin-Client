/**
 * -----------------------------------------------------
 * File        trackers_panel.cpp
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

#include "trackers_panel.h"
#include "calladmin-client.h"

#include "tinyxml2/tinyxml2.h"
#include "curl_util.h"


// Event Id
enum {
	wxID_UpdateTrackers = wxID_HIGHEST + 10,
};


// Events for Trackers Panel
BEGIN_EVENT_TABLE(TrackerPanel, wxPanel)
EVT_BUTTON(wxID_UpdateTrackers, TrackerPanel::OnUpdate)

EVT_CLOSE(TrackerPanel::OnCloseWindow)
END_EVENT_TABLE()


// Create Tracker Panel
TrackerPanel::TrackerPanel() : wxPanel(caGetNotebook(), wxID_ANY) {
	this->currentNameTimerId = 0;

	// Border and Center
	wxSizerFlags flags;

	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();

	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);

	this->trackerBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	this->trackerBox->SetFont(wxFont(14, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Add Log Box
	sizerTop->Add(this->trackerBox, 1, wxEXPAND);

	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_UpdateTrackers, "Update Trackers"), flags.Border(wxALL, 5));

	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));

	// Auto Size
	SetSizerAndFit(sizerTop, true);
}


// Button Event -> Update List
void TrackerPanel::OnUpdate(wxCommandEvent& WXUNUSED(event)) {
	// Get the Trackers Page
	caGetApp().GetPage(TrackerPanel::RefreshTrackers, caGetConfig()->GetPage() + "/trackers.php?from=20&from_type=interval&key=" + caGetConfig()->GetKey());
}


// Refresh Trackers
void TrackerPanel::RefreshTrackers(char* errors, wxString result, int WXUNUSED(extra)) {
	// Log Action
	caLogAction("Got Trackers");

	wxString error = "";

	// Delete old ones
	caGetTrackersPanel()->DeleteTrackers();

	// Not empty?
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
							if ((wxString)node3->Value() == "trackerID") {
								wxString steamidString = node3->FirstChild()->Value();

								// Build csteamid
								CSteamID steamidTracker = CallDialog::SteamIdtoCSteamId((char*)steamidString.mb_str().data());

								// Valid Tracker ID?
								if (steamidTracker.IsValid()) {
									// Create Name Timer
									caGetTrackersPanel()->GetNameTimers()->push_back(new NameTimer(steamidTracker, caGetTrackersPanel()->GetAndIncraseCurrentNameTimerId()));

									found = true;
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
		error = "Found no available trackers!";
	}

	caGetTaskBarIcon()->ShowMessage("Coulnd't retrieve trackers!", error, NULL);
}


void TrackerPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event)) {
	for (wxVector<NameTimer *>::iterator nameTimer = nameTimers.begin(); nameTimer != nameTimers.end(); ++nameTimer) {
		(*nameTimer)->Stop();
		delete (*nameTimer);
	}

	nameTimers.clear();

	Destroy();
}




NameTimer::NameTimer(CSteamID client, int id) : wxTimer(this, -1) {
	this->client = client;
	this->id = id;
	this->attempts = 0;

	Start(100, wxTIMER_CONTINUOUS);
}


NameTimer::~NameTimer() {
	bool found = false;

	// Remove name timer
	wxVector<NameTimer *>* nameTimers = caGetTrackersPanel()->GetNameTimers();
	wxVector<NameTimer *>::iterator nameTimerIterator;

	for (nameTimerIterator = nameTimers->begin(); nameTimerIterator != nameTimers->end(); ++nameTimerIterator) {
		NameTimer *nameTimer = *nameTimerIterator;

		if (nameTimer->GetId() == this->id) {
			found = true;
			break;
		}
	}

	if (found) {
		nameTimers->erase(nameTimerIterator);
	}
}


// Timer to update trackers
void NameTimer::Notify() {
	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		if (this->client.IsValid()) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(this->client, true)) {
				wxString isFriend = "No Friends";
				wxString isOnline = "Offline";

				// Is Friend?
				if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(this->client) == k_EFriendRelationshipFriend) {
					isFriend = "Friends";
				}

				// Is Online?
				if (caGetSteamThread()->GetSteamFriends()->GetFriendPersonaState(this->client) != k_EPersonaStateOffline) {
					// Online
					if (isFriend == "Friends") {
						isOnline = "Online";
					} else {
						// We can't know
						isOnline = "Unknown Status";
					}
				}


				// Add Tracker
				if (this->client.Render() != caGetSteamThread()->GetUserSteamId()) {
					caGetTrackersPanel()->AddTracker("" + (wxString)caGetSteamThread()->GetSteamFriends()->GetFriendPersonaName(this->client) + " - " + (wxString)this->client.Render() + " - " + isFriend + " - " + isOnline);
				} else {
					caGetTrackersPanel()->AddTracker("" + (wxString)caGetSteamThread()->GetSteamFriends()->GetFriendPersonaName(this->client) + " - " + (wxString)this->client.Render());
				}

				//Stop
				Stop();
				delete this;
			}
		}
	}


	// All loaded or 5 seconds gone?
	if (++this->attempts == 50) {
		// Enough, stop timer
		Stop();
		delete this;
	}
}