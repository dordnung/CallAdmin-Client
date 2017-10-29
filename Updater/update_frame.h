#ifndef UPDATE_FRAME_H
#define UPDATE_FRAME_H

/**
 * -----------------------------------------------------
 * File        update_frame.h
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
#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <curl/curl.h>

struct UpdateInfo {
	double totalSize;
	double downloadedSize;
	double currentTime;
};


// Update Frame Class
class UpdateFrame : public wxFrame, wxThreadHelper {
private:
	// We need information about the update ;)
	wxStaticText *downloadedSize;
	wxStaticText *downloadedSizeTotal;
	wxStaticText *speed;
	wxStaticText *elapsedTime;
	wxStaticText *remainingTime;

	// Layout
	wxSizer *sizerTop;

	// Panel
	wxPanel *panel;

	// Progress Bar
	wxGauge *progressBar;

public:
	UpdateFrame() : downloadedSize(NULL), downloadedSizeTotal(NULL), speed(NULL), elapsedTime(NULL),
		remainingTime(NULL), sizerTop(NULL), panel(NULL), progressBar(NULL) {};

	bool ShowFrame();

	// Events
	void OnUpdate(UpdateInfo updateInfo);
	void OnFinish(wxString error);

protected:
	void OnCloseWindow(wxCloseEvent &event);

	virtual wxThread::ExitCode Entry();

	wxDECLARE_EVENT_TABLE();
};


// CURL Callbacks
size_t WriteDataToFile(void *data, size_t size, size_t nmemb, FILE *file);
int ProgressUpdated(void *curlPointer, double dlTotal, double dlNow, double ulTotal, double ulNow);

#endif
