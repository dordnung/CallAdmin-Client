#ifndef CALLADMIN_CLIENT_H
#define CALLADMIN_CLIENT_H

/**
 * -----------------------------------------------------
 * File        calladmin-client.h
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

#include "config.h"
#include "main_frame.h"
#include "taskbar.h"
#include "timer.h"
#include "opensteam.h"
#include "call_dialog.h"
#include "curl_util.h"


// Version, update URL and update exe
#define CALLADMIN_CLIENT_VERSION "0.47B"
#define CALLADMIN_UPDATE_URL "http://popoklopsi.de/calladmin/version.txt"
#define CALLADMIN_UPDATE_EXE "http://popoklopsi.de/calladmin/calladmin-client.exe"

// Helpers
#define caGetApp wxGetApp

#define caGetConfig caGetApp().GetConfig
#define caGetTimer caGetApp().GetTimer
#define caGetTaskBarIcon caGetApp().GetTaskBarIcon
#define caGetSteamThread caGetApp().GetSteamThread
#define caGetCallDialogs caGetApp().GetCallDialogs
#define caLogAction caGetApp().LogAction

#define caGetMainFrame caGetApp().GetMainFrame
#define caGetNotebook caGetMainFrame()->GetNotebook
#define caGetMainPanel caGetNotebook()->GetMainPanel
#define caGetConfigPanel caGetNotebook()->GetConfigPanel
#define caGetTrackerPanel caGetNotebook()->GetTrackerPanel
#define caGetLogPanel caGetNotebook()->GetLogPanel
#define caGetAboutPanel caGetNotebook()->GetAboutPanel

#define FIND_OR_FAIL(var, ptr, str) var = ptr; if (var == NULL) {\
	wxMessageBox("Error: Couldn't find XRCID " str, "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR); \
	return false; \
}


// App Class
class CallAdmin : public wxApp {
private:
	Timer *timer;
	Config *config;
	MainFrame *mainFrame;
	TaskBarIcon *taskBarIcon;
	SteamThread *steamThread;
	CurlThread *curlThread;

	wxVector<CallDialog *> callDialogs;

	int avatarSize;
	int attempts;

	// Start in Taskbar?
	bool startInTaskbar;

	// Still running?
	bool isRunning;

public:
	CallAdmin();

	void StartTimer();
	void StartSteamThread();
	void StartUpdate();

	void GetPage(CurlCallback callbackFunction, wxString page, int extra = 0);

	bool IsRunning() {
		return this->isRunning;
	}

	Config* GetConfig() {
		return this->config;
	}

	MainFrame* GetMainFrame() {
		return this->mainFrame;
	}

	TaskBarIcon* GetTaskBarIcon() {
		return this->taskBarIcon;
	}

	SteamThread* GetSteamThread() {
		return this->steamThread;
	}

	wxVector<CallDialog *>* GetCallDialogs() {
		return &this->callDialogs;
	}

	int GetAvatarSize() {
		return this->avatarSize;
	}

	int GetAttempts() {
		return this->attempts;
	}

	void SetAttempts(int attempts) {
		this->attempts = attempts;
	}

	wxString GetRelativePath(wxString relativeFileOrPath);

	void CheckUpdate();
	void CreateReconnect(wxString error);
	void ShowError(wxString error, wxString type);

	void ExitProgramm();

	void LogAction(wxString action, LogLevel logLevel = LEVEL_INFO) {
		this->mainFrame->GetNotebook()->GetLogPanel()->AddLog(action, logLevel);
	}

	static void OnUpdate(wxString error, wxString result, int extra);

	void OnCurlThread(CurlThreadData *data);

protected:
	// Where all began :)
	virtual bool OnInit();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
};


// For Windows: Check if other app is in fullscreen mode
#if defined(__WXMSW__)
inline BOOL CALLBACK CheckMaximized(HWND hwnd, LPARAM lParam) {
	WINDOWINFO info;
	GetWindowInfo(hwnd, &info);

	// Only if top most
	if (info.dwExStyle & WS_EX_TOPMOST) {
		RECT rect;
		GetWindowRect(hwnd, &rect);

		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		// Only if in fullscreen
		if (rect.right - rect.left == cx && rect.bottom - rect.top == cy) {
			*(bool *)lParam = true;

			return FALSE;
		}
	}

	return TRUE;
}
#endif


inline bool isOtherInFullscreen() {
#if defined(__WXMSW__)
	bool otherAppInFullscreen = false;

	EnumWindows((WNDENUMPROC)CheckMaximized, (LPARAM)&otherAppInFullscreen);

	// Return result
	return otherAppInFullscreen;
#else
	// Not for Linux
	return false;
#endif
}


// Declare the app
wxDECLARE_APP(CallAdmin);


// Defined in resource.cpp
extern size_t xml_res_size_2;
extern unsigned char xml_res_file_2[];
extern void InitXmlResource();

#endif
