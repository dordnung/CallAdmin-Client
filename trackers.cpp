/**
 * -----------------------------------------------------
 * File        trackers.cpp
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




// Include Project
#include "log.h"
#include "main.h"
#include "call.h"
#include "config.h"
#include "tinyxml2/tinyxml2.h"
#include "taskbar.h"
#include "trackers.h"
#include "calladmin-client.h"


// Tracker Panel
TrackerPanel* trackerPanel = NULL;



// Button ID's for Tracker Panel
enum
{
	wxID_ExitTrackers = wxID_HIGHEST+700,
	wxID_UpdateTrackers,
	wxID_HideTrackers,
};


// Button Events for Log Panel
BEGIN_EVENT_TABLE(TrackerPanel, wxPanel)
	EVT_BUTTON(wxID_ExitTrackers, TrackerPanel::OnExit)
	EVT_BUTTON(wxID_UpdateTrackers, TrackerPanel::OnUpdate)
	EVT_BUTTON(wxID_HideTrackers, TrackerPanel::OnHide)
END_EVENT_TABLE()



// Create Tracker Panel
TrackerPanel::TrackerPanel(wxNotebook* note) : wxPanel(note, wxID_ANY)
{
	// Set Tracker Panel
	trackerPanel = this;


	// Border and Center
	wxSizerFlags flags;


	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// Create Box
	wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);


	trackerBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	trackerBox->SetFont(wxFont(14, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	// Add Log Box
	sizerTop->Add(trackerBox, 1, wxEXPAND);

	
	wxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);

	// Hide and Exit Button
	sizerBtns->Add(new wxButton(this, wxID_HideTrackers, "Hide"), flags.Border(wxALL &~ wxRIGHT, 5));
	sizerBtns->Add(new wxButton(this, wxID_UpdateTrackers, "Update Trackers"), flags.Border(wxALL &~ wxRIGHT &~ wxLEFT, 5));
	sizerBtns->Add(new wxButton(this, wxID_ExitTrackers, "Exit"), flags.Border(wxALL &~ wxLEFT, 5));



	// Add Buttons to Box
	sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));



	// Auto Size
	SetSizerAndFit(sizerTop, true);
}






// Button Event -> Exit programm
void TrackerPanel::OnExit(wxCommandEvent& WXUNUSED(event))
{
	exitProgramm();
}




// Button Event -> Update List
void TrackerPanel::OnUpdate(wxCommandEvent& WXUNUSED(event))
{
	// Get the Trackers Page
	getPage(refreshTrackers, page + "/trackers.php?from=20&from_type=interval&key=" + key);
}



// Button Event -> Hide to Taskbar
void TrackerPanel::OnHide(wxCommandEvent& WXUNUSED(event))
{
	// Log Action
	LogAction("Hide to taskbar");

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Call Admin", "Call Admin is now in the taskbar!", this);

		main_dialog->Show(false);
	}
	else
	{
		main_dialog->Iconize(true);
	}
}




// Add new Tracker
void addTracker(wxString text)
{
	// Add the new tracker to the  box
	trackerPanel->newTracker(text);
}





// Refresh Trackers
void refreshTrackers(char* errors, wxString result, int WXUNUSED(x))
{
	// Log Action
	LogAction("Got Trackers");

	wxString error = "";


	// found someone?
	bool found = false;

	// Delete old ones
	trackerPanel->delTrackers();


	// Not empty?
	if (result != "")
	{
		// Everything good :)
		if ((wxString)errors == "")
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
							if ((wxString)node3->Value() == "trackerID")
							{
								std::string steamidString = node3->FirstChild()->Value();

								// Build csteamid
								CSteamID steamidTracker = CallDialog::steamIDtoCSteamID((char*) steamidString.c_str());

								// Valid Tracker ID?
								if (steamidTracker.IsValid())
								{
									// Create Name Timer
									new NameTimer(steamidTracker);

									found = true;
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
		error = "Found no available trackers!";
	}

	if (m_taskBarIcon != NULL)
	{
		m_taskBarIcon->ShowMessage("Coulnd't retrieve trackers!", error, NULL);
	}
}
