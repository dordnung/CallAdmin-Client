#ifndef TIMER_H
#define TIMER_H

/**
 * -----------------------------------------------------
 * File        timer.h
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

#include <wx/timer.h>

// The timer ID
#define TIMER_ID 1


// Timer Class
class Timer : public wxTimer {
private:
	bool isFirstShoot;
	long firstFetch;

protected:
	void OnExecute(wxTimerEvent &event);

public:
	Timer() : wxTimer(this, TIMER_ID), isFirstShoot(true), firstFetch(false) {};

	static void OnNotice(wxString error, wxString result, int firstRun);

	void Run(int repeatInterval) {
		Start(repeatInterval, wxTIMER_CONTINUOUS);
	}

	wxDECLARE_EVENT_TABLE();
};

#endif
