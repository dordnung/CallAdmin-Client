/**
 * -----------------------------------------------------
 * File        timer.cpp
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
#include "timer.h"
#include "curl_util.h"
#include "calladmin-client.h"
#include "sound.h"
#include "tinyxml2/tinyxml2.h"

#include <wx/sound.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Timer events
wxBEGIN_EVENT_TABLE(Timer, wxTimer)
	EVT_TIMER(TIMER_ID, Timer::OnExecute)
wxEND_EVENT_TABLE()


// We need something to print for a XML Error!
wxString XMLErrorString[] =
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


// Timer executed
void Timer::OnExecute(wxTimerEvent &WXUNUSED(event)) {
	if (!caGetApp().IsRunning()) {
		return;
	}

	Config *config = caGetConfig();

	// Build page
	wxString page;

	if (this->isFirstShoot) {
		this->firstFetch = wxGetUTCTime();

		page = config->GetPage() + "/notice.php?from=0&from_type=unixtime&key=" + config->GetKey() + "&sort=desc&limit=" + (wxString() << config->GetNumLastCalls());
	} else {
		page = config->GetPage() + "/notice.php?from=" + (wxString() << (config->GetStep() * 2)) + "&from_type=interval&key=" +
			config->GetKey() + "&sort=asc&handled=" + (wxString() << (wxGetUTCTime() - this->firstFetch));
	}

	// Store Player
	if (!config->GetIsSpectator()) {
		page = page + "&store=1&steamid=" + caGetSteamThread()->GetUserSteamId();
	}

	// Get the Page
	caGetApp().GetPage(Timer::OnNotice, page, this->isFirstShoot);

	this->isFirstShoot = false;
}


// Got call list page
void Timer::OnNotice(wxString error, wxString result, int firstRun) {
	// Valid result?
	if (result != "" && error == "") {
		// Everything good :)
		bool foundError = false;
		bool foundNew = false;

		// Proceed XML result!
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLNode *node;

		// Parse the xml data
		tinyxml2::XMLError parseError = doc.Parse(result);

		// Parsing Error?
		if (parseError != tinyxml2::XML_SUCCESS) {
			foundError = true;

			caGetApp().ShowError(XMLErrorString[parseError], "XML");
		}

		// No error so far, yeah!
		if (!foundError) {
			// Goto xml child
			node = doc.FirstChild();

			// Goto CallAdmin
			if (node != NULL) {
				node = node->NextSibling();
			}

			// Are there new calls?
			if (node != NULL) {
				// Init. Call List
				int dialog = caGetCallDialogs()->size();

				// On first run get number of found rows
				if (firstRun) {
					for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling()) {
						// Is foundRows?
						if ((wxString)node2->Value() == "foundRows") {
							// The dialog id is the number of the latest call
							dialog = wxAtoi(node2->FirstChild()->Value()) - 1;

							// Go on
							break;
						}
					}
				}

				for (tinyxml2::XMLNode *node2 = node->FirstChild(); node2; node2 = node2->NextSibling()) {
					// API Error?
					if ((wxString)node2->Value() == "error") {
						foundError = true;
						caGetApp().ShowError(node2->FirstChild()->Value(), "API");

						break;
					}

					// Row Count is not important
					if ((wxString)node2->Value() == "foundRows") {
						continue;
					}

					// Is API fine?
					int found = 0;

					// Create the new CallDialog
					CallDialog *newDialog = new CallDialog();
					newDialog->SetId(dialog);

					// Put in all needed data
					for (tinyxml2::XMLNode *node3 = node2->FirstChild(); node3; node3 = node3->NextSibling()) {
						if ((wxString)node3->Value() == "callID") {
							found++;
							newDialog->SetCallId(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "fullIP") {
							found++;
							newDialog->SetIP(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "serverName") {
							found++;
							newDialog->SetName(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "targetName") {
							found++;
							newDialog->SetTarget(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "targetID") {
							found++;
							newDialog->SetTargetId(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "targetReason") {
							found++;
							newDialog->SetReason(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "clientName") {
							found++;
							newDialog->SetClient(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "clientID") {
							found++;
							newDialog->SetClientId(node3->FirstChild()->Value());
						}

						if ((wxString)node3->Value() == "reportedAt") {
							found++;
							newDialog->SetTime(wxAtol(node3->FirstChild()->Value()));
						}

						if ((wxString)node3->Value() == "callHandled") {
							found++;
							newDialog->SetHandled(wxString(node3->FirstChild()->Value()) == "1");
						}
					}

					// Found all necessary items?
					if (found != 10) {
						newDialog->Destroy();
					} else {
						// Check for duplicate Entry
						bool foundDuplicate = false;
						int index = -1;

						for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
							index++;

							// Operator overloading :)
							if ((**callDialog) == (*newDialog)) {
								foundDuplicate = true;

								// Check if handled state changed
								if (newDialog->IsHandled() && !(*callDialog)->IsHandled()) {
									caGetMainPanel()->SetHandled(index);
								}

								// That's enough
								break;
							}
						}

						if (foundDuplicate) {
							// Found duplicate Entry
							newDialog->Destroy();
							continue;
						}

						// New call
						foundNew = true;

						// Don't show calls on first Run
						if (firstRun) {
							dialog--;
						} else {
							caLogAction("A new call received", LogLevel::LEVEL_INFO);
						}

						// Now start it
						bool success = newDialog->StartCall(!firstRun && caGetConfig()->GetIsAvailable() && !isOtherInFullscreen());
						
						if (!success) {
							newDialog->Destroy();
							caLogAction("A call dialog couldn't be created!", LogLevel::LEVEL_ERROR);
						} else {
							caGetCallDialogs()->push_back(newDialog);
						}
					}
				}
			}
		}

		// Everything is good, set attempts to zero
		if (!foundError) {
			// Reset attempts
			caGetApp().ResetAttempts();

			// Update Call List
			if (foundNew) {
				// Update call list
				caGetMainPanel()->UpdateCalls();

				// Play Sound
				if (caGetConfig()->GetWantSound() && !firstRun && caGetConfig()->GetIsAvailable()) {
					wxSound sound;
					wxString soundFile = caGetConfig()->GetSoundFile();

					if (soundFile == "") {
						sound.Create(wave_sound_size, wave_sound);
					} else {
						sound.Create(soundFile);
					}

					sound.Play(wxSOUND_ASYNC);
				}
			}
		}
	} else {
		// Something went wrong ):
		caGetApp().ShowError(error, "CURL");
	}
}
