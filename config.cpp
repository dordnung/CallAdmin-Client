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


// Create Config
Config::Config() : wxConfig("Call Admin") {
	// Default Settings
	this->step = 5;
	this->timeout = 3;
	this->maxAttempts = 3;
	this->numLastCalls = 25;

	this->page = "";
	this->key = "";

	this->steamEnabled = true;
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

			this->steamEnabled = ReadBool("steam", true);
			this->hideOnMinimize = ReadBool("hideonminimize", false);
			this->isAvailable = ReadBool("available", true);
			this->wantSound = ReadBool("sound", true);
			this->isSpectator = ReadBool("spectate", false);

			Read("page", &this->page, "");

			// Strip last /
			if (this->page.find_last_of("/") == this->page.length() - 1) {
				// Delete it
				this->page.RemoveLast();
			}

			Read("key", &this->key, "");
		} catch (...) {
			isConfigSet = false;
		}
	}

	// Check invalid values
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