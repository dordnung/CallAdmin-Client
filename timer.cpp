/**
 * -----------------------------------------------------
 * File        timer.cpp
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

// Project
#include "timer.h"
#include "curl_util.h"
#include "calladmin-client.h"


// Timer events
BEGIN_EVENT_TABLE(Timer, wxTimer)
EVT_TIMER(TIMER_ID, Timer::OnExecute)
END_EVENT_TABLE()


// Constructor
Timer::Timer() : wxTimer(this, TIMER_ID) {
	this->isFirstShoot = true;
	this->firstFetch = 0;
}


// Run the timer
void Timer::Run(int repeatInterval) {
	// Log Action
	caLogAction("Started the Timer");

	Start(repeatInterval);
}


// Timer executed
void Timer::OnExecute(wxTimerEvent& WXUNUSED(event)) {
	Config *config = caGetConfig();

	// Build page
	wxString page;

	if (this->isFirstShoot) {
		this->firstFetch = wxGetUTCTime();

		page = (config->GetPage() + "/notice.php?from=0&from_type=unixtime&key=" + config->GetKey() + "&sort=desc&limit=" + (wxString() << config->GetNumLastCalls()));
	} else {
		page = (config->GetPage() + "/notice.php?from=" + (wxString() << (config->GetStep() * 2)) + "&from_type=interval&key=" + config->GetKey() + "&sort=asc&handled=" + (wxString() << (wxGetUTCTime() - this->firstFetch)));
	}

	// Store Player
	if (!config->GetIsSpectator()) {
		page = page + "&store=1&steamid=" + caGetSteamThread()->GetUserSteamId();
	}

	// Get the Page
	CurlThread::GetPage(CallAdmin::OnNotice, page, this->isFirstShoot);

	this->isFirstShoot = false;
}