#ifndef CONFIG_H
#define CONFIG_H

/**
 * -----------------------------------------------------
 * File        config.h
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

#include <wx/config.h>


// Config Class
class Config : public wxConfig {
private:
	// Settings (Can be public)
	int step;
	int timeout;
	int maxAttempts;
	int numLastCalls;

	wxString page;
	wxString key;

	bool steamEnabled;
	bool hideOnMinimize;
	bool isAvailable;
	bool wantSound;
	bool isSpectator;

public:
	Config();

	// Parse the config
	bool ParseConfig();

	// Config accessors
	int GetStep();
	int GetTimeout();
	int GetMaxAttempts();
	int GetNumLastCalls();

	wxString GetPage();
	wxString GetKey();

	bool GetSteamEnabled();
	bool GetHideOnMinimize();
	bool GetIsAvailable();
	bool GetWantSound();
	bool GetIsSpectator();

	// Config Setters
	void SetStep(int step);
	void SetTimeout(int timeout);
	void SetMaxAttempts(int maxAttempts);
	void SetNumLastCalls(int numLastCalls);

	void SetPage(wxString page);
	void SetKey(wxString key);

	void SetSteamEnabled(bool steamEnabled);
	void SetHideOnMinimize(bool hideOnMinimize);
	void SetIsAvailable(bool isAvailable);
	void SetWantSound(bool wantSound);
	void SetIsSpectator(bool isSpectator);
};


#endif
