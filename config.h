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
#include <wx/fileconf.h>


enum LogLevel {
	LEVEL_DEBUG = 0,
	LEVEL_INFO,
	LEVEL_WARNING,
	LEVEL_ERROR,
	LEVEL_SIZE_OF
};

static wxString LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
// statically check that the size of LogLevel fits the number of log levels
static_assert(sizeof(LogLevelNames) / sizeof(wxString) == LEVEL_SIZE_OF, "Sizes do not match");


// Config Class
class Config : public wxFileConfig {
private:
	// Settings
	int step;
	int timeout;
	int maxAttempts;
	int numLastCalls;

	wxString page;
	wxString key;
	wxString soundFile;

	LogLevel logLevel;
	bool steamEnabled;
	bool showAsNotification;
	bool hideOnMinimize;
	bool minimizeOnExit;
	bool isAvailable;
	bool wantSound;
	bool isSpectator;

	bool converted;

public:
	Config();

	// Parse the config
	bool ParseConfig();

	// Path to the config folder
	static wxString GetConfigDir();

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

	wxString GetSoundFile() {
		return this->soundFile;
	}

	LogLevel GetLogLevel() {
		return this->logLevel;
	}

	bool GetSteamEnabled() {
		return this->steamEnabled;
	}

	bool GetShowAsNotification() {
		return this->showAsNotification;
	}

	bool GetHideOnMinimize() {
		return this->hideOnMinimize;
	}

	bool GetMinimizeOnExit() {
		return this->minimizeOnExit;
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

	bool SetSoundFile(wxString soundFile) {
		this->soundFile = soundFile;

		if (soundFile != "" && !IsSoundFileValid()) {
			this->soundFile = "";

			return false;
		} 

		Write("soundFile", soundFile);

		return true;
	}

	void SetLogLevel(LogLevel logLevel) {
		this->logLevel = logLevel;
		Write("logLevel", (int)logLevel);
	}

	void SetSteamEnabled(bool steamEnabled) {
		this->steamEnabled = steamEnabled;
		Write("steam", steamEnabled);
	}

	void SetShowAsNotification(bool showAsNotification) {
		this->showAsNotification = showAsNotification;
		Write("showasnotification", showAsNotification);
	}

	void SetHideOnMinimize(bool hideOnMinimize) {
		this->hideOnMinimize = hideOnMinimize;
		Write("hideonminimize", hideOnMinimize);
	}

	void SetMinimizeOnExit(bool minimizeOnExit) {
		this->minimizeOnExit = minimizeOnExit;
		Write("minimizeonexit", minimizeOnExit);
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

private:
	bool IsSoundFileValid();

	wxString GetConfigFile();
	void ConvertFromRegistry();
};


#endif
