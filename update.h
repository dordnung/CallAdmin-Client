#ifndef UPDATE_H
#define UPDATE_H

/**
 * -----------------------------------------------------
 * File        update.h
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

#pragma once



// c++
#include <stdio.h>


// Precomp Header
#include <wx/wxprec.h>



// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif


// Curl
#include <curl/curl.h>

// Project
#include "calladmin-client.h"




// Struct for progress
struct dlProgress 
{
	CURL *curl;
};




// Update Dialog Class
class UpdateDialog: public wxDialog
{
private:

	// We need information about the update ;)
	wxStaticText* dlinfo;
	wxStaticText* dlstatus;

	// Layout
	wxSizer* sizerTop;

	// Panel
	wxPanel* panel;

	// Progress Bar
	wxGauge* progressBar;

public:
	UpdateDialog();

protected:
	// Button Events
	void OnHide(wxCommandEvent& event);
	void OnUpdate(wxCommandEvent& event);
	void OnFinish(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};





// Thread for Curl Performances
class updateThread: public wxThread
{
public:

	// Create and Start
	updateThread() : wxThread() {this->Create(); this->Run();}

	virtual ExitCode Entry();
};




// CURL Callbacks
size_t write_data_file(void *ptr, size_t size, size_t nmemb, FILE *stream);
int progress_updated(void *p, double dltotal, double dlnow, double ultotal, double ulnow);



// Update Dialogs
extern UpdateDialog *update_dialog;

// thread
extern updateThread *update_thread;

#endif
