#ifndef CALLADMIN_CLIENT_UPDATER_H
#define CALLADMIN_CLIENT_UPDATER_H

/**
 * -----------------------------------------------------
 * File        calladmin-client-updater.h
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

#include "update_dialog.h"


// Font
#if defined(__WXMSW__)
#define FONT_FAMILY wxFONTFAMILY_SCRIPT
#define FONT_WEIGHT_BOLD wxFONTWEIGHT_BOLD
#else
#define FONT_FAMILY wxFONTFAMILY_DEFAULT
#define FONT_WEIGHT_BOLD wxFONTWEIGHT_NORMAL
#endif


// App Class
class CallAdminUpdater : public wxApp {
private:
	bool appEnded;

	wxString callAdminVersion;
	wxString callAdminUrl;
	wxString callAdminExecutable;

	UpdateDialog *updateDialog;

public:
	CallAdminUpdater()
		: appEnded(false), callAdminVersion(wxString()), callAdminUrl(wxString()), callAdminExecutable(wxString()), updateDialog(NULL) {};

	wxString GetCallAdminVersion() {
		return this->callAdminVersion;
	}

	wxString GetCallAdminExecutable() {
		return this->callAdminExecutable;
	}

	UpdateDialog* GetUpdateDialog() {
		return this->updateDialog;
	}

	wxString GetPath(wxString file);

	wxString GetCallAdminPath() {
#if defined(__WXMSW__)
		return GetPath("calladmin-client.exe");
#else
		return GetPath("calladmin-client");
#endif
	}

	bool CheckForUpdate();
	void StartUpdate();

	void ExitProgramm();

	void OnUpdateDialogClosed();
	bool OnGetVersion(wxString error, wxString result);

protected:
	// Where all began :)
	virtual bool OnInit();

	// Where all end ;)
	virtual int OnExit();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
};

size_t CurlWriteData(void *buffer, size_t size, size_t nmemb, void *userp);


// Declare the app
DECLARE_APP(CallAdminUpdater)

#endif
