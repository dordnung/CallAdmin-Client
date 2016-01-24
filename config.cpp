/**
 * -----------------------------------------------------
 * File        config.cpp
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
#include "config.h"
#include "calladmin-client.h"
#include <wx/file.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Create Config
Config::Config() : wxConfig("Call Admin") {
	// Default Settings
	this->step = 5;
	this->timeout = 3;
	this->maxAttempts = 3;
	this->numLastCalls = 25;

	this->page = "";
	this->key = "";
	this->soundFile = "";

	this->logLevel = LEVEL_INFO;
	this->steamEnabled = true;
	this->showInTaskbar = false;
	this->hideOnMinimize = false;
	this->isAvailable = true;
	this->wantSound = true;
	this->isSpectator = false;
}


// Parse Config
bool Config::ParseConfig() {
	bool isConfigSet = false;

	// Was parsing good?
	if (!Exists("page") || !Exists("key")) {
		isConfigSet = false;
	} else {
		// Get files out of config
		try {
			this->step = ReadLong("step", 5l);
			this->timeout = ReadLong("timeout", 3l);
			this->maxAttempts = ReadLong("attempts", 5l);
			this->numLastCalls = ReadLong("lastcalls", 25l);

			this->logLevel = (LogLevel)ReadLong("logLevel", LEVEL_INFO);
			this->steamEnabled = ReadBool("steam", true);
			this->showInTaskbar = ReadBool("showInTaskbar", false);
			this->hideOnMinimize = ReadBool("hideonminimize", false);
			this->isAvailable = ReadBool("available", true);
			this->wantSound = ReadBool("sound", true);
			this->isSpectator = ReadBool("spectate", false);

			Read("key", &this->key, "");
			Read("page", &this->page, "");
			Read("soundFile", &this->soundFile, "");

			// Strip last /
			if (this->page.find_last_of("/") == this->page.length() - 1) {
				// Delete it
				this->page.RemoveLast();
			}
		} catch (...) {
			isConfigSet = false;
		}
	}

	// Check invalid values
	if (!IsSoundFileValid()) {
		this->soundFile = "";
	}

	if (this->step < 5) {
		this->step = 5;
	}

	if (this->step > 20) {
		this->step = 20;
	}

	if (this->timeout < 3) {
		this->timeout = 3;
	}

	if (this->timeout > 10) {
		this->timeout = 10;
	}

	if (this->maxAttempts < 3) {
		this->maxAttempts = 3;
	}

	if (this->maxAttempts > 10) {
		this->maxAttempts = 10;
	}

	if (this->numLastCalls < 0) {
		this->numLastCalls = 0;
	}

	if (this->numLastCalls > 50) {
		this->numLastCalls = 50;
	}

	if (this->timeout >= step) {
		this->timeout = step - 1;
	}

	return isConfigSet;
}


bool Config::IsSoundFileValid() {
	if (!wxFile::Exists(this->soundFile)) {
		return false;
	}

	wxFile soundFile(this->soundFile);

	if (!soundFile.IsOpened()) {
		return false;
	}

	char fileStart[4];

	// Check if start with RIFF
	if (soundFile.Read(&fileStart, sizeof(fileStart)) == wxInvalidOffset || !((wxString)fileStart != "RIFF")) {
		return false;
	}

	return true;
}
