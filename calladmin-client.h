#ifndef CALLADMIN_CLIENT_H
#define CALLADMIN_CLIENT_H

/**
 * -----------------------------------------------------
 * File        calladmin-client.h
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


#define MAXCALLS 200



// Precomp Header
#include <wx/wxprec.h>

// c++ libs
#include <string>

// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/cmdline.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>


// Font
#if defined(__WXMSW__)
	#define FONT_FAMILY wxFONTFAMILY_SCRIPT
	#define FONT_WEIGHT_BOLD wxFONTWEIGHT_BOLD
#else
	#define FONT_FAMILY wxFONTFAMILY_DEFAULT
	#define FONT_WEIGHT_BOLD wxFONTWEIGHT_NORMAL
#endif




extern int avatarSize;

// Version
extern wxString version;

// Timer already run?
extern bool timerStarted;

// Attempts
extern int attempts;





// App Class
class CallAdmin : public wxApp
{
public:
	// Where all began :)
	virtual bool OnInit();

	// Where all end ;)
	virtual ~CallAdmin();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

	// Start in Taskbar?
	static bool start_taskbar;
};





// Timer Class
class Timer : public wxTimer
{
public:
	Timer() : wxTimer(this, 1) {}

	void run(int milliSecs);
	void update(wxTimerEvent&);

	DECLARE_EVENT_TABLE()
};





// Thread for Curl Performances
typedef void (*callback)(char*, wxString, int);

class curlThread: public wxThread
{
private:
	// Callback function
	callback function;

	// Page
	wxString page;

	// Optional Parameter
	int x;

public:

	// Create and Start
	curlThread(callback f, wxString p, int extra) : wxThread() {function = f; page = p; x = extra; this->Create(); this->Run();}

	virtual ExitCode Entry();
};



// Client Data for Thread
class ThreadData: public wxClientData
{
private:
	// Callback function
	callback function;

	// Callback function
	wxString content;

	// Error
	char* error;

	// Optional Parameter
	int x;

public:
	ThreadData(callback func, wxString cont, char* err, int extra) {function = func; content = cont, error = err, x = extra;}

	callback getCallback() {return function;}
	wxString getContent() {return content;}
	char* getError() {return error;}
	int getExtra() {return x;}
};




// For Windows: Check if other app is in fullscreen mode
#if defined(__WXMSW__)
	inline bool IsTopMost(HWND hwnd)
	{
		WINDOWINFO info;

		GetWindowInfo(hwnd, &info);

		return (info.dwExStyle & WS_EX_TOPMOST) ? true : false;
	}

	inline bool IsFullScreenSize(HWND hwnd, const int cx, const int cy)
	{
		RECT r;
		::GetWindowRect(hwnd, &r);

		return r.right - r.left == cx && r.bottom - r.top == cy;

	}

	inline bool IsFullscreenAndMaximized(HWND hwnd)
	{
		if (IsTopMost(hwnd))
		{
			const int cx = GetSystemMetrics(SM_CXSCREEN);
			const int cy = GetSystemMetrics(SM_CYSCREEN);

			if (IsFullScreenSize(hwnd, cx, cy))
			{
				return true;
			}
		}

		return false;
	}

	inline BOOL CALLBACK CheckMaximized(HWND hwnd, LPARAM lParam)
	{
		if(IsFullscreenAndMaximized(hwnd))
		{
			*(bool*) lParam = true;

			return FALSE;
		}

		return TRUE;
	}
#endif


inline bool isOtherInFullscreen()
{
	#if defined(__WXMSW__)
		bool otherAppInFullscreen = false;

		EnumWindows((WNDENUMPROC) CheckMaximized, (LPARAM) &otherAppInFullscreen);

		// Return result
		return otherAppInFullscreen;
	#else
		// Not for Linux
		return false;
	#endif
}


// Main methods
void checkUpdate();
void createReconnect(wxString error);
void showError(wxString error, wxString type);
void exitProgramm();

wxString getAppPath(wxString file);


#if defined(__WXMSW__)
	std::wstring s2ws(wxString s);
#endif




// Curl Stuff
void getPage(callback function, wxString page, int x=0);
void onNotice(char* error, wxString result, int x);
void onUpdate(char* error, wxString result, int x);

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);




// Declare the app
DECLARE_APP(CallAdmin)



// Timer
extern Timer *timer;



#endif
