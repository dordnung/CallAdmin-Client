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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/config.h>


// Config Class
class Config : public wxConfig {
private:
	// Settings
	int step;
	int timeout;
	int maxAttempts;
	int numLastCalls;

	wxString page;
	wxString key;

	bool steamEnabled;
	bool showInTaskbar;
	bool hideOnMinimize;
	bool isAvailable;
	bool wantSound;
	bool isSpectator;

public:
	Config();

	// Parse the config
	bool ParseConfig();

	// Config accessors
	int GetStep() {
		return this->step;
	}

	int GetTimeout() {
		return this->timeout;
	}

	int GetMaxAttempts() {
		return this->maxAttempts;
	}

	int GetNumLastCalls() {
		return this->numLastCalls;
	}

	wxString GetPage() {
		return this->page;
	}

	wxString GetKey() {
		return this->key;
	}

	bool GetSteamEnabled() {
		return this->steamEnabled;
	}

	bool GetShowInTaskbar() {
		return this->showInTaskbar;
	}

	bool GetHideOnMinimize() {
		return this->hideOnMinimize;
	}

	bool GetIsAvailable() {
		return this->isAvailable;
	}

	bool GetWantSound() {
		return this->wantSound;
	}

	bool GetIsSpectator() {
		return this->isSpectator;
	}


	// Config setters
	void SetStep(int step) {
		this->step = step;
		Write("step", step);
	}

	void SetTimeout(int timeout) {
		this->timeout = timeout;
		Write("timeout", timeout);
	}

	void SetMaxAttempts(int maxAttempts) {
		this->maxAttempts = maxAttempts;
		Write("attempts", maxAttempts);
	}

	void SetNumLastCalls(int numLastCalls) {
		this->numLastCalls = numLastCalls;
		Write("lastcalls", numLastCalls);
	}

	void SetPage(wxString page) {
		this->page = page;
		Write("page", page);
	}

	void SetKey(wxString key) {
		this->key = key;
		Write("key", key);
	}

	void SetSteamEnabled(bool steamEnabled) {
		this->steamEnabled = steamEnabled;
		Write("steam", steamEnabled);
	}

	void SetShowInTaskbar(bool showInTaskbar) {
		this->showInTaskbar = showInTaskbar;
		Write("showInTaskbar", showInTaskbar);
	}

	void SetHideOnMinimize(bool hideOnMinimize) {
		this->hideOnMinimize = hideOnMinimize;
		Write("hideonminimize", hideOnMinimize);
	}

	void SetIsAvailable(bool isAvailable) {
		this->isAvailable = isAvailable;
		Write("available", isAvailable);
	}

	void SetWantSound(bool wantSound) {
		this->wantSound = wantSound;
		Write("sound", wantSound);
	}

	void SetIsSpectator(bool isSpectator) {
		this->isSpectator = isSpectator;
		Write("spectate", isSpectator);
	}
};


#endif
