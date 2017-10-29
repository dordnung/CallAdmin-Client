#ifndef CALLADMIN_CLIENT_UPDATER_H
#define CALLADMIN_CLIENT_UPDATER_H

/**
 * -----------------------------------------------------
 * File        calladmin_client_updater.h
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

#include "update_frame.h"

// App Class
class CallAdminUpdater : public wxApp {
private:
	bool appEnded;

	wxString callAdminVersion;
	wxString callAdminUrl;
	wxString callAdminExecutable;

	UpdateFrame *updateFrame;

protected:
	// Where all began :)
	virtual bool OnInit();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);

public:
	CallAdminUpdater() : appEnded(false), callAdminVersion(wxString()),
		callAdminUrl(wxString()), callAdminExecutable(wxString()), updateFrame(NULL) {};

	wxString GetRelativePath(wxString relativeFileOrPath);

	bool CheckForUpdate();

	void StartUpdate();
	void OnUpdateFrameClosed();

	void ExitProgramm();

	bool OnGetVersion(wxString error, wxString result);

	wxString GetCallAdminVersion() {
		return this->callAdminVersion;
	}

	wxString GetCallAdminExecutable() {
		return this->callAdminExecutable;
	}

	UpdateFrame* GetUpdateFrame() {
		return this->updateFrame;
	}

	wxString GetCallAdminExecutablePath() {
		#if defined(__WXMSW__)
			return GetRelativePath("calladmin-client.exe");
		#else
			return GetRelativePath("calladmin-client");
		#endif
	}
};

// CURL write callback
size_t CurlWriteData(void *data, size_t size, size_t nmemb, void *stream);

// Defined in resource.cpp
extern void InitXmlResource();


// Declare the app
wxDECLARE_APP(CallAdminUpdater);

#endif
