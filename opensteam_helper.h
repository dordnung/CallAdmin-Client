#ifndef OPENSTEAM_HELPER_H
#define OPENSTEAM_HELPER_H

/**
 * -----------------------------------------------------
 * File        opensteam_helper.h
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

 // Steamworks warning -> disable
#pragma warning(disable: 4245)

#include "Steamworks.h"

// Steamworks warning -> enable
#pragma warning(default: 4245)

#ifdef _WIN32
	#include "Win32Library.h"
	#define PATH_LENGTH MAX_PATH
#elif defined(__APPLE__) && defined(__MACH__)
	#include "POSIXLibrary.h"
	#include <CoreServices/CoreServices.h>
	#include <sys/param.h>

	#define PATH_LENGTH MAXPATHLEN
#elif defined(__linux__)
	#include "POSIXLibrary.h"
	#include <libgen.h>
	#include <limits.h>

	#define PATH_LENGTH PATH_MAX
#endif


class OpenSteamHelper {
private:
	DynamicLibrary *library;

	HSteamPipe hSteamPipe;
	HSteamUser hSteamUser;

	ISteamClient017 *steamClient;
	ISteamUser017 *steamUser;
	ISteamFriends015 *steamFriends;
	ISteamUtils007 *steamUtils;
	ISteamMatchmaking009 *steamMatchmaking;
	ISteamUserStats011 *steamUserStats;
	ISteamApps006 *steamApps;
	ISteamNetworking005 *steamNetworking;
	ISteamMatchmakingServers002 *steamMatchmakingServers;
	ISteamRemoteStorage012 *steamRemoteStorage;
	ISteamScreenshots002 *steamScreenshots;
	ISteamHTTP002 *steamHTTP;
	ISteamUnifiedMessages001 *steamUnifiedMessages;
	ISteamController001 *steamController;
	ISteamUGC003 *steamUGC;
	ISteamAppList001 *steamAppList;
	ISteamMusic001 *steamMusic;
	ISteamMusicRemote *steamMusicRemote;
	ISteamHTMLSurface *steamHTMLSurface;
	ISteamInventory001 *steamInventory;
	ISteamVideo *steamVideo;

	OpenSteamHelper();
	OpenSteamHelper(OpenSteamHelper const&);
	void operator=(OpenSteamHelper const&);

	// Finds the path to the dynamic library of the steamclient
	bool FindSteamClientLibrary(char *libraryFile, size_t size);

	// Finds where Steam is installed
	bool FindSteamPath(char *steamPath, size_t size);

	// Initialize all Steam interfaces
	bool InitializeSteamInterfaces();

public:
	static OpenSteamHelper *GetInstance();

	// Initalize the Steam API
	bool SteamAPI_Init();

	// Checks if Steam is still running
	bool SteamAPI_IsSteamRunning();

	// Shutdown the Steam API
	bool SteamAPI_Shutdown();

	/* Getters to the interfaces */
	ISteamClient017 *SteamClient();
	ISteamUser017 *SteamUser();
	ISteamFriends015 *SteamFriends();
	ISteamUtils007 *SteamUtils();
	ISteamMatchmaking009 *SteamMatchmaking();
	ISteamUserStats011 *SteamUserStats();
	ISteamApps006 *SteamApps();
	ISteamNetworking005 *SteamNetworking();
	ISteamMatchmakingServers002 *SteamMatchmakingServers();
	ISteamRemoteStorage012 *SteamRemoteStorage();
	ISteamScreenshots002 *SteamScreenshots();
	ISteamHTTP002 *SteamHTTP();
	ISteamUnifiedMessages001 *SteamUnifiedMessages();
	ISteamController001 *SteamController();
	ISteamUGC003 *SteamUGC();
	ISteamAppList001 *SteamAppList();
	ISteamMusic001 *SteamMusic();
	ISteamMusicRemote *SteamMusicRemote();
	ISteamHTMLSurface *SteamHTMLSurface();
	ISteamInventory001 *SteamInventory();
	ISteamVideo *SteamVideo();
};

#endif
