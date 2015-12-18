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

// Precomp Header
#include <wx/wxprec.h>

// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "config.h"
#include "main_frame.h"
#include "taskbar.h"
#include "timer.h"
#include "opensteam.h"
#include "update_dialog.h"
#include "call_dialog.h"

// Version, update URL and update exe
#define CALLADMIN_CLIENT_VERSION "0.47B"
#define CALLADMIN_UPDATE_URL "http://popoklopsi.de/calladmin/version.txt"
#define CALLADMIN_UPDATE_EXE "http://popoklopsi.de/calladmin/calladmin-client.exe"

// Font
#if defined(__WXMSW__)
	#define FONT_FAMILY wxFONTFAMILY_SCRIPT
	#define FONT_WEIGHT_BOLD wxFONTWEIGHT_BOLD
#else
	#define FONT_FAMILY wxFONTFAMILY_DEFAULT
	#define FONT_WEIGHT_BOLD wxFONTWEIGHT_NORMAL
#endif

// Helpers
#define caGetApp wxGetApp

#define caGetConfig caGetApp().GetConfig
#define caGetTimer caGetApp().GetTimer
#define caGetSteamThread caGetApp().GetSteamThread
#define caGetTaskBarIcon caGetApp().GetTaskBarIcon
#define caGetCallDialogs caGetApp().GetCallDialogs
#define caGetUpdateDialog caGetApp().GetUpdateDialog
#define caLogAction caGetApp().LogAction

#define caGetMainFrame caGetApp().GetMainFrame
#define caGetNotebook caGetMainFrame()->GetNotebook
#define caGetMainPanel caGetNotebook()->GetMainPanel
#define caGetConfigPanel caGetNotebook()->GetConfigPanel
#define caGetTrackersPanel caGetNotebook()->GetTrackersPanel
#define caGetLogPanel caGetNotebook()->GetLogPanel
#define caGetAboutPanel caGetNotebook()->GetAboutPanel


// Custom events for Call Dialog closed or update dialog closed
wxDECLARE_EVENT(wxEVT_UPDATE_DIALOG_CLOSED, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_CALL_DIALOG_CLOSED, wxCommandEvent);


// App Class
class CallAdmin : public wxApp {
private:
	Timer *timer;
	Config *config;
	MainFrame *mainFrame;
	TaskBarIcon *taskBarIcon;
	SteamThread *steamThread;
	UpdateDialog *updateDialog;

	wxVector<CallDialog *> callDialogs;

	int avatarSize;
	int attempts;

	bool end;

	// Start in Taskbar?
	bool startInTaskbar;

public:
	CallAdmin();

	void StartTimer();
	void StartSteamThread();
	void StartUpdate();

	UpdateDialog* GetUpdateDialog();
	SteamThread* GetSteamThread();
	Config* GetConfig();
	MainFrame* GetMainFrame();
	TaskBarIcon* GetTaskBarIcon();

	wxVector<CallDialog *>* GetCallDialogs();

	int GetAvatarSize();
	wxString GetAppPath(wxString file);

	int GetAttempts();
	void SetAttempts(int attempts);

	void CheckUpdate();
	void CreateReconnect(wxString error);
	void ShowError(wxString error, wxString type);
	void ExitProgramm();
	void LogAction(wxString action);

	static void OnNotice(char* error, wxString result, int firstRun);
	static void OnUpdate(char* error, wxString result, int extra);

protected:
	// Where all began :)
	virtual bool OnInit();

	// Where all end ;)
	virtual int OnExit();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);

	void OnCallDialogClosed(wxCommandEvent &event);
	void OnUpdateDialogClosed(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
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
DECLARE_APP(CallAdmin)


#endif
