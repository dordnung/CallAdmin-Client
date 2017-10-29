/**
 * -----------------------------------------------------
 * File        update.cpp
 * Authors     David Ordnung, Impact
 * License     GPLv3
 * Web         http://dordnung.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung, Impact
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
#include "update.h"
#include "log.h"
#include "config.h"
#include "taskbar.h"
#include "calladmin-client.h"




// Update Dialogs
UpdateDialog *update_dialog = NULL;


// Thread
updateThread *update_thread = NULL;


// Button ID's for Update Dialog
enum
{
	wxID_UpdateUpdated = wxID_HIGHEST+800,
	wxID_UpdateFinish
};



// Button Events for Update Dialog
BEGIN_EVENT_TABLE(UpdateDialog, wxDialog)
	EVT_COMMAND(wxID_UpdateUpdated, wxEVT_COMMAND_MENU_SELECTED, UpdateDialog::OnUpdate)
	EVT_COMMAND(wxID_UpdateFinish, wxEVT_COMMAND_MENU_SELECTED, UpdateDialog::OnFinish)

	EVT_CLOSE(UpdateDialog::OnCloseWindow)
END_EVENT_TABLE()



// Open Update Dialog
UpdateDialog::UpdateDialog() : wxDialog(NULL, wxID_ANY, "Update CallAdmin", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMINIMIZE_BOX)
{
	// Set Update Dialog
	update_dialog = this;

	// Action
	LogAction("Start downloading Update");


	// Create Box
	sizerTop = new wxBoxSizer(wxVERTICAL);


	// Panel
	panel = new wxPanel(this, wxID_ANY);
	
	// Border and Center
	wxSizerFlags flags;


	// Border and Centre
	flags.Border(wxALL, 10);
	flags.Centre();


	// Create Progress Bar
	progressBar = new wxGauge(panel, wxID_ANY, 100);

	// Add it
	sizerTop->Add(progressBar, 0, wxALL | wxEXPAND, 10);





	// Download Info
	dlinfo = new wxStaticText(panel, wxID_ANY, "0000kB of 0000KB (0000 kB/s). Time: 0,00 Seconds");

	dlinfo->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));


	// Add it
	sizerTop->Add(dlinfo, flags.Border(wxALL, 10));




	// Download Status
	dlstatus = new wxStaticText(panel, wxID_ANY, "Status: Downloading...");

	dlstatus->SetFont(wxFont(16, FONT_FAMILY, wxFONTSTYLE_NORMAL, FONT_WEIGHT_BOLD));

	// Add it
	sizerTop->Add(dlstatus, flags.Border(wxALL, 10));



	
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
	Show(true);




	// Now start the Update

	new updateThread();
}



// Progress Update
void UpdateDialog::OnUpdate(wxCommandEvent& event)
{
	// valid?
	if (dlinfo == NULL || progressBar == NULL || panel == NULL)
	{
		return;
	}

	// Set new Text
	dlinfo->SetLabelText(event.GetString());
	sizerTop->Layout();

	// Update Progress Bar
	progressBar->SetValue(event.GetInt());

	
	panel->SetSizerAndFit(sizerTop, false); 

	Fit();
}





// Update Finished
void UpdateDialog::OnFinish(wxCommandEvent& event)
{
	// valid?
	if (dlstatus == NULL || progressBar == NULL || m_taskBarIcon == NULL || panel == NULL)
	{
		return;
	}

	// Update Progress Bar
	progressBar->SetValue(100);

	// Path
	wxString path = wxStandardPaths::Get().GetExecutablePath();

	// Delete Thread
	if (update_thread != NULL)
	{
		update_thread = NULL;
	}

	// Notice finish
	if (event.GetString() == "")
	{
		// Renaming Files
		rename(path, path + ".old");
		rename(path + ".new", path); 

		// Refresh Status
		dlstatus->SetLabelText("Status: Finished!");
		dlstatus->SetForegroundColour(wxColour(34, 139, 34));
		sizerTop->Layout(); 

		m_taskBarIcon->ShowMessage("Update Finished", "Downloading update is finished\nPlease restart your Call Admin Client", this);
	}
	else
	{
		// Refresh Status
		dlstatus->SetLabelText("Status: Error!");
		dlstatus->SetForegroundColour(wxColour("red"));
		sizerTop->Layout();

		m_taskBarIcon->ShowMessage("Error on Update", "Couldn't download update\n" + event.GetString(), this);
	}

	// Fit Window
	panel->SetSizerAndFit(sizerTop, false);

	Fit();
}





// Curl Thread started
wxThread::ExitCode updateThread::Entry()
{
	update_thread = this;

	if (!TestDestroy())
	{
		// Event
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_UpdateFinish);

		// dl Progress
		struct dlProgress prog;

		// Init Curl
		CURL *curl = curl_easy_init();


		if (curl != NULL)
		{
			// File Pointer
			FILE *fp;
			
			// Error
			char ebuf[CURL_ERROR_SIZE];
			
			// Path
			wxString path = wxStandardPaths::Get().GetExecutablePath();

			// Open File
			fp = fopen(path + ".new", "wb");


			if (fp == NULL)
			{
				return (wxThread::ExitCode)1;
			}


			// Reset Prog time
			prog.curl = curl;

			// Configurate Curl
			curl_easy_setopt(curl, CURLOPT_URL, "http://dordnung.de/sourcemod/calladmin/calladmin-client.exe");
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_file);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);

			// Perform Curl
			CURLcode res = curl_easy_perform(curl);

			// Everything good :)
			if (res == CURLE_OK)
			{
				event.SetString("");
			}
			else
			{
				// Error ):
				event.SetString(ebuf);
			}

			// Clean Curl
			curl_easy_cleanup(curl);

			// Close File
			fclose(fp);

			// Add Event Handler
			if (update_dialog != NULL)
			{
				update_dialog->GetEventHandler()->AddPendingEvent(event);
			}


			return (wxThread::ExitCode)0;
		}

		event.SetString("Couldn't Init CURL!");

		// Add Event Handler
		if (update_dialog != NULL)
		{
			update_dialog->GetEventHandler()->AddPendingEvent(event);
		}
	}

	// Delete Thread
	if (update_thread != NULL)
	{
		update_thread = NULL;
	}

	return (wxThread::ExitCode)0;
}





// Write Data to file
size_t write_data_file(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	// Write
	if (ptr != NULL)
	{
		size_t written = fwrite(ptr, size, nmemb, stream);

		return written;
	}

	return (size_t) -1;
}





// Progress Updated
int progress_updated(void *p, double dltotal, double dlnow, double WXUNUSED(ultotal), double WXUNUSED(ulnow))
{
	// Get progress strucht
	struct dlProgress *myp = (struct dlProgress *)p;

	// valid?
	if (myp == NULL)
	{
		return -1;
	}


	// Get Curl
	CURL *curl = myp->curl;

	// Get Current time
	double curtime = 0;
 
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

	// Valid Data?
	if (dltotal > 0)
	{
		// Event
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_UpdateUpdated);

		// Set Event Data
		event.SetString(wxString::Format("%.0f", (dlnow / 1024)) + "kB of " +  wxString::Format("%.0f", (dltotal / 1024)) + "kB (" + wxString::Format("%.0f", ((dlnow / 1024)/curtime)) + "kB/s). Time: " + wxString::Format("%.2f", curtime) + " Seconds");
		event.SetInt((dlnow/dltotal) * 100);
		
		
		// Add Event Handler
		if (update_dialog != NULL)
		{
			update_dialog->GetEventHandler()->AddPendingEvent(event);
		}
	}

	return 0;
}




// Window Event -> Disable Window
void UpdateDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	// Update running?
	if (update_thread != NULL && m_taskBarIcon != NULL)
	{
		// Update is running
		m_taskBarIcon->ShowMessage("Please Wait", "Please wait until download is finished", this);
	}
	else
	{
		// Destroy
		update_dialog = NULL;
		Destroy();
	}
}
