#ifndef UPDATE_FRAME_H
#define UPDATE_FRAME_H

/**
 * -----------------------------------------------------
 * File        update_frame.h
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


// Update Frame Class
class UpdateFrame : public wxFrame, wxThreadHelper {
private:
	// We need information about the update ;)
	wxStaticText *dlInfo;
	wxStaticText *dlStatus;

	// Layout
	wxSizer *sizerTop;

	// Panel
	wxPanel *panel;

	// Progress Bar
	wxGauge *progressBar;

public:
	UpdateFrame() : dlInfo(NULL), dlStatus(NULL),
		sizerTop(NULL), panel(NULL), progressBar(NULL) {};
	~UpdateFrame();

	bool ShowFrame();

	// Events
	void OnUpdate(wxString infoText, int progress);
	void OnFinish(wxString error);

protected:
	virtual wxThread::ExitCode Entry();

	void OnCloseWindow(wxCloseEvent &event);

	wxDECLARE_EVENT_TABLE();
};


// CURL Callbacks
size_t WriteDataToFile(void *data, size_t size, size_t nmemb, FILE *file);
int ProgressUpdated(void *curlPointer, double dlTotal, double dlNow, double ulTotal, double ulNow);

#endif
