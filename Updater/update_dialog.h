#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H

/**
 * -----------------------------------------------------
 * File        update_dialog.h
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

#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <curl/curl.h>


// Struct for progress
struct dlProgress {
	CURL *curl;
};


wxDECLARE_EVENT(wxEVT_THREAD_UPDATE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_THREAD_FINISHED, wxCommandEvent);


// Update Dialog Class
class UpdateDialog : public wxDialog, wxThreadHelper {
public:
	UpdateDialog();
	~UpdateDialog();

private:
	// We need information about the update ;)
	wxStaticText *dlinfo;
	wxStaticText *dlstatus;

	// Layout
	wxSizer *sizerTop;

	// Panel
	wxPanel *panel;

	// Progress Bar
	wxGauge *progressBar;

protected:
	virtual wxThread::ExitCode Entry();

	// Events
	void OnUpdate(wxCommandEvent &event);
	void OnFinish(wxCommandEvent &event);

	void OnCloseWindow(wxCloseEvent &event);

	DECLARE_EVENT_TABLE()
};


// CURL Callbacks
size_t WriteDataToFile(void *data, size_t size, size_t nmemb, FILE *file);
int ProgressUpdated(void *progressPointer, double dltotal, double dlnow, double ultotal, double ulnow);

#endif
