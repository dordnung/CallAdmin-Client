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
#include <wx/utils.h>
#include <wx/file.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
	#include "wx/msw/regconf.h"

	 // Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Create Config
Config::Config() : wxFileConfig("CallAdmin-Client", wxEmptyString, GetConfigPath()) {
	// Default Settings
	this->step = 5;
	this->timeout = 3;
	this->maxAttempts = 3;
	this->numLastCalls = 25;

	this->page = "";
	this->key = "";
	this->soundFile = "";

	this->logLevel = LogLevel::LEVEL_INFO;
	this->steamEnabled = true;
	this->showInTaskbar = false;
	this->hideOnMinimize = false;
	this->isAvailable = true;
	this->wantSound = true;
	this->isSpectator = false;

	this->converted = false;
}


// Parse Config
bool Config::ParseConfig() {
	bool isConfigSet = true;

	// Maybe convert from old registry config
	ConvertFromRegistry();

	// Was parsing good?
	if (!Exists("page") || !Exists("key")) {
		isConfigSet = false;
	} else {
		// Get files out of config
		this->step = ReadLong("step", 5l);
		this->timeout = ReadLong("timeout", 3l);
		this->maxAttempts = ReadLong("attempts", 5l);
		this->numLastCalls = ReadLong("lastcalls", 25l);

		this->logLevel = (LogLevel)ReadLong("logLevel", LogLevel::LEVEL_INFO);
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

		caLogAction("Timeout can't be higher then the intervall", LogLevel::LEVEL_WARNING);
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


wxString Config::GetConfigPath() {
	wxString fileDirectory;
	wxFileName fileName;

#ifdef __WXMSW__
	fileDirectory = wxStandardPaths::Get().GetUserConfigDir() + "\\CallAdmin";
	fileName = wxFileName(fileDirectory, "CallAdmin-Client", "ini");
#else
	// First check XDG_CONFIG_HOME var
	if (wxGetenv("XDG_CONFIG_HOME")) {
		fileDirectory = (wxString)wxGetenv("XDG_CONFIG_HOME") + "/CallAdmin";
	} else {
		// Other wise use ~/.config
		fileDirectory = wxFileName::GetHomeDir() + "/.config/CallAdmin";
	}

	fileName = wxFileName(fileDirectory, "CallAdmin-Client", "conf");
#endif

	if (!wxDirExists(fileDirectory)) {
		// Create directory if not exists
		wxFileName::Mkdir(fileDirectory, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	}

	return fileName.GetFullPath();
}

void Config::ConvertFromRegistry() {
#ifdef __WXMSW__
	if (this->converted) {
		return;
	}

	this->converted = true;

	wxRegConfig regConfig("Call Admin");
	wxString temp;

	if (regConfig.Exists("page")) {
		regConfig.Read("page", &temp, "");

		Write("page", temp);
	} else {
		// No config set
		return;
	}

	if (regConfig.Exists("key")) {
		regConfig.Read("key", &temp, "");

		Write("key", temp);
	}

	if (regConfig.Exists("step")) {
		Write("step", regConfig.ReadLong("step", 5L));
	}

	if (regConfig.Exists("timeout")) {
		Write("timeout", regConfig.ReadLong("timeout", 3L));
	}

	if (regConfig.Exists("attempts")) {
		Write("attempts", regConfig.ReadLong("attempts", 5L));
	}

	if (regConfig.Exists("lastcalls")) {
		Write("lastcalls", regConfig.ReadLong("lastcalls", 25L));
	}

	if (regConfig.Exists("steam")) {
		Write("steam", regConfig.ReadBool("steam", true));
	}

	if (regConfig.Exists("showInTaskbar")) {
		Write("showInTaskbar", regConfig.ReadBool("showInTaskbar", false));
	}

	if (regConfig.Exists("hideonminimize")) {
		Write("hideonminimize", regConfig.ReadBool("hideonminimize", false));
	}

	if (regConfig.Exists("available")) {
		Write("available", regConfig.ReadBool("available", true));
	}

	if (regConfig.Exists("sound")) {
		Write("sound", regConfig.ReadBool("sound", true));
	}

	if (regConfig.Exists("spectate")) {
		Write("spectate", regConfig.ReadBool("spectate", false));
	}

	// Now delete it
	regConfig.DeleteAll();

#endif
}