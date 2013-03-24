/**
 * -----------------------------------------------------
 * File        calladmin-client.h
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Header File
 * Copyright (C) 2013 Impact, David <popoklopsi> Ordnung
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


// c++ libs
#include <string>

// Wee need WX
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/timer.h>


extern int avatarSize;

// Version
extern wxString version;


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
private:
	int attempts;

public:
	Timer() : wxTimer(this, 1) {}

	void run(int milliSecs);
	void update(wxTimerEvent&);
	
	int getAttempts() {return attempts;}
	void setAttempts(int attempt) {attempts = attempt;}

	DECLARE_EVENT_TABLE()
};



void checkUpdate();
void createReconnect(wxString error);
void createError(int errorCode, wxString error, wxString type);
void exitProgramm();


std::wstring s2ws(wxString s);
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);


// Declare the app
DECLARE_APP(CallAdmin)


// Timer
extern Timer *timer;
