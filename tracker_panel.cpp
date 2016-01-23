/**
 * -----------------------------------------------------
 * File        tracker_panel.cpp
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
#include "tracker_panel.h"
#include "calladmin-client.h"

#include "tinyxml2/tinyxml2.h"
#include "curl_util.h"

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Events for Tracker Panel
wxBEGIN_EVENT_TABLE(TrackerPanel, wxPanel)
	EVT_BUTTON(XRCID("updateTrackers"), TrackerPanel::OnUpdate)
wxEND_EVENT_TABLE()


// Clean Up
TrackerPanel::~TrackerPanel() {
	wxVector<NameTimer *>::iterator nameTimer = nameTimers.begin();

	while (nameTimer != nameTimers.end()) {
		// Ensure that destructor of NameTimer does not delete something out of the vector
		wxVector<NameTimer *>::iterator oldNameTimer = nameTimer;
		nameTimer = nameTimers.erase(nameTimer);

		delete (*oldNameTimer);
	}
}


// Init Panel with controls
bool TrackerPanel::InitPanel() {
	// TrackerPanel is a ScrolledWindows, but a ScrolledWindow is also a panel
	if (!wxXmlResource::Get()->LoadObject(this, caGetNotebook()->GetWindow(), "trackerPanel", "wxScrolledWindow")) {
		wxMessageBox("Error: Couldn't find XRCID trackerPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// The tracker list box
	FIND_OR_FAIL(this->trackerBox, XRCCTRL(*this, "trackerBox", wxListBox), "trackerBox");

	return true;
}


// Update Trackers List
void TrackerPanel::UpdateTrackerList() {
	// Get the Trackers Page
	caGetApp().GetPage(TrackerPanel::RefreshTrackers, caGetConfig()->GetPage() + "/trackers.php?from=25&from_type=interval&key=" + caGetConfig()->GetKey());
}


// Got current trackers
void TrackerPanel::RefreshTrackers(wxString errorStr, wxString result, int WXUNUSED(extra)) {
	wxString error = "";

	// First delete old ones
	caGetTrackerPanel()->GetCurrentTrackers()->clear();

	// Not empty?
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

						// Search for steamid
						for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling()) {
							// Found tracker steamid
							if ((wxString)node3->Value() == "trackerID") {
								wxString steamidString = node3->FirstChild()->Value();

								// Build csteamid
								CSteamID steamidTracker = SteamThread::SteamIdtoCSteamId(steamidString);

								// Valid Tracker ID?
								if (steamidTracker.IsValid()) {
									caGetTrackerPanel()->GetCurrentTrackers()->push_back(steamidString);
								}
							}
						}
					}

					// Have we found something and there is no error
					if (found && error == "") {
						// We are finished :)
						return;
					}
				}
			} else {
				// XML ERROR
				error = "XML ERROR: Couldn't parse the trackers API!";
			}
		} else {
			// Curl error
			error = errorStr;
		}
	} else {
		// Curl error
		error = "Couldn't init. CURL connection";
	}


	// Seems we found no tracker
	if (error != "") {
		caGetTaskBarIcon()->ShowMessage("Couldn't retrieve trackers!", error, caGetTrackerPanel(), true);
	}
}


void TrackerPanel::OnUpdate(wxCommandEvent &WXUNUSED(event)) {
	this->trackerBox->Clear();

	if (this->currentTrackers.empty()) {
		AddTracker("No trackers available");
	}

	for (wxVector<wxString>::iterator tracker = this->currentTrackers.begin(); tracker != this->currentTrackers.end(); ++tracker) {
		// Build csteamid
		CSteamID steamidTracker = SteamThread::SteamIdtoCSteamId(*tracker);

		// Valid Tracker ID?
		if (steamidTracker.IsValid()) {
			// Just add SteamId if Steam is not available
			if (caGetSteamThread()->IsConnected()) {
				// Create Name Timer
				this->nameTimers.push_back(new NameTimer(steamidTracker));
			} else {
				AddTracker(*tracker);
			}
		}
	}

	#if !defined(__WXMSW__)
		this->FitInside();
	#endif
}



// Name Timer to load names from Steam Lib
NameTimer::NameTimer(CSteamID client) : wxTimer(this, -1) {
	this->client = client;
	this->attempts = 0;

	Start(100, wxTIMER_CONTINUOUS);
}


// Clean Up
NameTimer::~NameTimer() {
	// Remove name timer from name timers list
	wxVector<NameTimer *>* nameTimers = caGetTrackerPanel()->GetNameTimers();

	for (wxVector<NameTimer *>::iterator nameTimerIterator = nameTimers->begin(); nameTimerIterator != nameTimers->end(); ++nameTimerIterator) {
		NameTimer *nameTimer = *nameTimerIterator;

		if (nameTimer == this) {
			nameTimers->erase(nameTimerIterator);
			break;
		}
	}
}


// Timer to find tracker name
void NameTimer::Notify() {
	if (!caGetApp().IsRunning()) {
		// App already ended
		delete this;
		return;
	}

	// Get steamId
	wxString steamId = this->client.Render();

	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		OpenSteamHelper *helper = OpenSteamHelper::GetInstance();

		if (!helper->SteamFriends()->RequestUserInformation(this->client, true)) {
			bool isFriend = false;
			bool isOnline = false;

			// Is Friend?
			if (helper->SteamFriends()->GetFriendRelationship(this->client) == k_EFriendRelationshipFriend) {
				isFriend = true;
			}

			// Is Online?
			if (helper->SteamFriends()->GetFriendPersonaState(this->client) != k_EPersonaStateOffline) {
				isOnline = true;
			}

			// Format tracker Text
			wxString trackerText = (wxString)helper->SteamFriends()->GetFriendPersonaName(this->client) + " - " + steamId;

			if (steamId != caGetSteamThread()->GetUserSteamId()) {
				if (isFriend) {
					trackerText += " - Friend";

					if (isOnline) {
						trackerText += " - Online";
					} else {
						trackerText += " - Offline";
					}
				}
			} else {
				trackerText = "Yourself: " + trackerText;
			}

			// Finally add tracker
			caGetTrackerPanel()->AddTracker(trackerText);

			// Delete because it's finished
			delete this;
		}
	}

	// 5 seconds gone?
	if (++this->attempts == 50) {
		caGetTrackerPanel()->AddTracker(steamId);

		// Enough, delete timer
		delete this;
	}
}
