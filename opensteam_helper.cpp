/**
 * -----------------------------------------------------
 * File        opensteam.cpp
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
#include <stdlib.h>
#include "opensteam_helper.h"

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif

#define SetOrReturn(var, val)  if (val == NULL) return false; var = val


OpenSteamHelper::OpenSteamHelper() {
	// Reset
	this->library = NULL;

	this->hSteamPipe = 0;
	this->hSteamUser = 0;

	this->steamClient = NULL;
	this->steamUser = NULL;
	this->steamFriends = NULL;
	this->steamUtils = NULL;
	this->steamMatchmaking = NULL;
	this->steamUserStats = NULL;
	this->steamApps = NULL;
	this->steamNetworking = NULL;
	this->steamMatchmakingServers = NULL;
	this->steamRemoteStorage = NULL;
	this->steamScreenshots = NULL;
	this->steamHTTP = NULL;
	this->steamUnifiedMessages = NULL;
	this->steamController = NULL;
	this->steamUGC = NULL;
	this->steamAppList = NULL;
	this->steamMusic = NULL;
	this->steamMusicRemote = NULL;
	this->steamHTMLSurface = NULL;
	this->steamInventory = NULL;
	this->steamVideo = NULL;
}


// Finds the path to the dynamic library of the steamclient
bool OpenSteamHelper::FindSteamClientLibrary(char *libraryFile, size_t size) {
#ifdef _WIN32
	// On Windows get the path to the steamclient.dll from the registry
	HKEY phkResult;

	// Open Steam registry key
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess", 0, KEY_READ, &phkResult) == ERROR_SUCCESS) {
		BYTE data[PATH_LENGTH] = { 0 };
		DWORD dataSize = PATH_LENGTH;

		// Read path to the dll file
		if (RegQueryValueExA(phkResult, "SteamClientDll", 0, NULL, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
			strncpy(libraryFile, (const char *)&data, size);
			libraryFile[size - 1] = 0;

			RegCloseKey(phkResult);
			return true;
		}

		RegCloseKey(phkResult);
	}
#elif __linux__
	// On linux it's located in $HOME/.steam/sdk32/steamclient.so
	char data[PATH_LENGTH] = { 0 };
	char *home = getenv("HOME");
	snprintf(data, PATH_MAX, "%s/.steam/sdk32", home);

	char resolved;
	if (realpath(data, &resolved)) {
		strncpy(data, &resolved, PATH_LENGTH);
	}

	size_t len = strlen(data);
	snprintf(&data[len], PATH_LENGTH - len, "/steamclient.so");

	if (!data[0]) {
		return false;
	}

	strncpy(libraryFile, data, size);
	libraryFile[size - 1] = 0;

	return true;
#elif __APPLE__ && __MACH__
	// Currently not supported
	return false;
#endif

	return false;
}


// Finds where Steam is installed
bool OpenSteamHelper::FindSteamPath(char *steamPath, size_t size) {
#ifdef _WIN32
	// On Windows get the path to steam from the registry
	HKEY phkResult;

	// Open Steam registry key
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &phkResult) == ERROR_SUCCESS) {
		BYTE data[PATH_LENGTH] = { 0 };
		DWORD dataSize = PATH_LENGTH;

		// Read path to the dll file
		if (RegQueryValueExA(phkResult, "SteamPath", 0, NULL, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
			strncpy(steamPath, (const char *)&data, size);
			steamPath[size - 1] = 0;

			RegCloseKey(phkResult);
			return true;
		}

		RegCloseKey(phkResult);
	}

	return false;
#else
	// Not needed for other Operating Systems
	return true;
#endif
}


// Initialize all Steam interfaces
bool OpenSteamHelper::InitializeSteamInterfaces() {
	SetOrReturn(this->steamUser, (ISteamUser017*)this->steamClient->GetISteamUser(this->hSteamUser, this->hSteamPipe, STEAMUSER_INTERFACE_VERSION_017));
	SetOrReturn(this->steamFriends, (ISteamFriends015*)this->steamClient->GetISteamFriends(this->hSteamUser, this->hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION_015));
	SetOrReturn(this->steamUtils, (ISteamUtils007*)this->steamClient->GetISteamUtils(this->hSteamPipe, STEAMUTILS_INTERFACE_VERSION_007));
	SetOrReturn(this->steamMatchmaking, (ISteamMatchmaking009*)this->steamClient->GetISteamMatchmaking(this->hSteamUser, this->hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION_009));
	SetOrReturn(this->steamMatchmakingServers, (ISteamMatchmakingServers002*)this->steamClient->GetISteamMatchmakingServers(this->hSteamUser, this->hSteamPipe, STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION_002));
	SetOrReturn(this->steamUserStats, (ISteamUserStats011*)this->steamClient->GetISteamUserStats(this->hSteamUser, this->hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION_011));
	SetOrReturn(this->steamApps, (ISteamApps006*)this->steamClient->GetISteamApps(this->hSteamUser, this->hSteamPipe, STEAMAPPS_INTERFACE_VERSION_006));
	SetOrReturn(this->steamNetworking, (ISteamNetworking005*)this->steamClient->GetISteamNetworking(this->hSteamUser, this->hSteamPipe, STEAMNETWORKING_INTERFACE_VERSION_005));
	SetOrReturn(this->steamRemoteStorage, (ISteamRemoteStorage012*)this->steamClient->GetISteamRemoteStorage(this->hSteamUser, this->hSteamPipe, STEAMREMOTESTORAGE_INTERFACE_VERSION_012));
	SetOrReturn(this->steamScreenshots, (ISteamScreenshots002*)this->steamClient->GetISteamScreenshots(this->hSteamUser, this->hSteamPipe, STEAMSCREENSHOTS_INTERFACE_VERSION_002));
	SetOrReturn(this->steamHTTP, (ISteamHTTP002*)this->steamClient->GetISteamHTTP(this->hSteamUser, this->hSteamPipe, STEAMHTTP_INTERFACE_VERSION_002));
	SetOrReturn(this->steamUnifiedMessages, (ISteamUnifiedMessages001*)this->steamClient->GetISteamUnifiedMessages(this->hSteamUser, this->hSteamPipe, STEAMUNIFIEDMESSAGES_INTERFACE_VERSION_001));
	SetOrReturn(this->steamController, (ISteamController001*)this->steamClient->GetISteamController(this->hSteamUser, this->hSteamPipe, STEAMCONTROLLER_INTERFACE_VERSION_001));
	SetOrReturn(this->steamUGC, (ISteamUGC003*)this->steamClient->GetISteamUGC(this->hSteamUser, this->hSteamPipe, STEAMUGC_INTERFACE_VERSION_003));
	SetOrReturn(this->steamAppList, (ISteamAppList001*)this->steamClient->GetISteamAppList(this->hSteamUser, this->hSteamPipe, STEAMAPPLIST_INTERFACE_VERSION_001));
	SetOrReturn(this->steamMusic, (ISteamMusic001*)this->steamClient->GetISteamMusic(this->hSteamUser, this->hSteamPipe, STEAMMUSIC_INTERFACE_VERSION_001));
	SetOrReturn(this->steamMusicRemote, (ISteamMusicRemote*)this->steamClient->GetISteamMusicRemote(this->hSteamUser, this->hSteamPipe, "STEAMMUSICREMOTE_INTERFACE_VERSION001"));
	SetOrReturn(this->steamHTMLSurface, (ISteamHTMLSurface*)this->steamClient->GetISteamHTMLSurface(this->hSteamUser, this->hSteamPipe, "STEAMHTMLSURFACE_INTERFACE_VERSION_003"));
	SetOrReturn(this->steamInventory, (ISteamInventory001*)this->steamClient->GetISteamInventory(this->hSteamUser, this->hSteamPipe, "STEAMINVENTORY_INTERFACE_V001"));
	SetOrReturn(this->steamVideo, (ISteamVideo*)this->steamClient->GetISteamVideo(this->hSteamUser, this->hSteamPipe, "STEAMVIDEO_INTERFACE_V001"));

	return true;
}


OpenSteamHelper *OpenSteamHelper::GetInstance() {
	static OpenSteamHelper instance;

	return &instance;
}


// Initalize the Steam API
bool OpenSteamHelper::SteamAPI_Init() {
	// Already initalized
	if (this->steamClient) {
		return true;
	}

	// Steam has to run
	if (!SteamAPI_IsSteamRunning()) {
		return false;
	}

	char libraryFile[PATH_LENGTH + 1];
	char steamPath[PATH_LENGTH + 1];

	if (!FindSteamClientLibrary(libraryFile, sizeof(libraryFile))) {
		return false;
	}

	if (!FindSteamPath(steamPath, sizeof(steamPath))) {
		return false;
	}

#ifdef _WIN32
	// steamclient.dll expects to be able to load tier0_s without an absolute
	// path, so we'll need to add the steam dir to the search path.
	SetDllDirectoryA(steamPath);
#endif

	// Load steamclient library
	this->library = new DynamicLibrary(libraryFile);

	if (!this->library->IsLoaded()) {
		delete this->library;
		this->library = NULL;

		return false;
	}

	// Create the factory
	CreateInterfaceFn factory = (CreateInterfaceFn)this->library->GetSymbol("CreateInterface");

	if (!factory) {
		delete this->library;
		this->library = NULL;

		return false;
	}

	// Load the steam client
	this->steamClient = (ISteamClient017*)factory(STEAMCLIENT_INTERFACE_VERSION_017, NULL);

	if (!this->steamClient) {
		delete this->library;
		this->library = NULL;

		return false;
	}

	// Create a pipe to steam and initalize the global user
	this->hSteamPipe = this->steamClient->CreateSteamPipe();
	this->hSteamUser = this->steamClient->ConnectToGlobalUser(this->hSteamPipe);

	if (!this->hSteamPipe || !this->hSteamUser || !InitializeSteamInterfaces()) {
		// Shutdown on error
		SteamAPI_Shutdown();

		return false;
	}

	return true;
}


// Checks if Steam is still running
bool OpenSteamHelper::SteamAPI_IsSteamRunning() {
#ifdef _WIN32
	// On windows get PID ID in registry and check if it's still active
	HKEY phkResult;

	// Open Steam registry key
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess", 0, KEY_READ, &phkResult) == ERROR_SUCCESS) {
		DWORD data;
		DWORD dataSize = 4;

		// Read pid number
		if (RegQueryValueExA(phkResult, "pid", 0, NULL, (LPBYTE)&data, &dataSize) == ERROR_SUCCESS) {
			HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, 0, data);
			DWORD exitCode = 0;

			// Send an pseudo kill signal to the process to check if it response
			if (process) {
				if (GetExitCodeProcess(process, &exitCode) && exitCode == STILL_ACTIVE) {
					CloseHandle(process);
					RegCloseKey(phkResult);

					return true;
				}

				CloseHandle(process);
			}

			RegCloseKey(phkResult);

			return false;
		}

		RegCloseKey(phkResult);
	}
#elif __linux__
	// On linux get the PID from $HOME/.steam/steam.pid
	char data;
	char *home = getenv("HOME");
	snprintf(&data, PATH_MAX, "%s/.steam/steam.pid", home);

	char resolved;
	if (realpath(&data, &resolved)) {
		strncpy(&data, &resolved, PATH_LENGTH);
	}

	FILE *process = fopen((const char *)&data, "r");

	if (process) {
		signed __int32 pid = fread(&resolved, 1, 64, process);
		fclose(process);

		if (pid > 0) {
			__int32 pidInt = strtol(&resolved, 0, 10);

			// Also send pseudo kill signal
			if (pidInt <= 0 || kill(pidInt, 0)) {
				return false;
			}

			return true;
		}
	}

	return false;
#elif __APPLE__ && __MACH__
	// Currently not supported
	return false;
#endif

	return false;
}


// Shutdown the Steam API
bool OpenSteamHelper::SteamAPI_Shutdown() {
	/* Use if implemented
	if (this->steamHTMLSurface) {
		this->steamHTMLSurface->Shutdown();
	} */

	// Reset vars
	this->steamUser = NULL;
	this->steamFriends = NULL;
	this->steamUtils = NULL;
	this->steamMatchmaking = NULL;
	this->steamUserStats = NULL;
	this->steamApps = NULL;
	this->steamNetworking = NULL;
	this->steamMatchmakingServers = NULL;
	this->steamRemoteStorage = NULL;
	this->steamScreenshots = NULL;
	this->steamHTTP = NULL;
	this->steamUnifiedMessages = NULL;
	this->steamController = NULL;
	this->steamUGC = NULL;
	this->steamAppList = NULL;
	this->steamMusic = NULL;
	this->steamMusicRemote = NULL;
	this->steamHTMLSurface = NULL;
	this->steamInventory = NULL;
	this->steamVideo = NULL;

	// If Steam is running and there is a valid pipe then close it
	if (SteamAPI_IsSteamRunning() && this->hSteamPipe && this->steamClient) {
		if (this->hSteamUser) {
			this->steamClient->ReleaseUser(this->hSteamPipe, this->hSteamUser);
		}

		this->steamClient->BReleaseSteamPipe(this->hSteamPipe);
		this->steamClient->BShutdownIfAllPipesClosed();
	}

	this->hSteamUser = 0;
	this->hSteamPipe = 0;
	this->steamClient = NULL;

	// Also free the dynamic library
	if (this->library) {
		delete this->library;
		this->library = NULL;
	}

	return true;
}


/* Getters to the interfaces */
ISteamClient017 *OpenSteamHelper::SteamClient() {
	return this->steamClient;
}

ISteamUser017 *OpenSteamHelper::SteamUser() {
	return this->steamUser;
}

ISteamFriends015 *OpenSteamHelper::SteamFriends() {
	return this->steamFriends;
}

ISteamUtils007 *OpenSteamHelper::SteamUtils() {
	return this->steamUtils;
}

ISteamMatchmaking009 *OpenSteamHelper::SteamMatchmaking() {
	return this->steamMatchmaking;
}

ISteamUserStats011 *OpenSteamHelper::SteamUserStats() {
	return this->steamUserStats;
}

ISteamApps006 *OpenSteamHelper::SteamApps() {
	return this->steamApps;
}

ISteamNetworking005 *OpenSteamHelper::SteamNetworking() {
	return this->steamNetworking;
}

ISteamMatchmakingServers002 *OpenSteamHelper::SteamMatchmakingServers() {
	return this->steamMatchmakingServers;
}

ISteamRemoteStorage012 *OpenSteamHelper::SteamRemoteStorage() {
	return this->steamRemoteStorage;
}

ISteamScreenshots002 *OpenSteamHelper::SteamScreenshots() {
	return this->steamScreenshots;
}

ISteamHTTP002 *OpenSteamHelper::SteamHTTP() {
	return this->steamHTTP;
}

ISteamUnifiedMessages001 *OpenSteamHelper::SteamUnifiedMessages() {
	return this->steamUnifiedMessages;
}

ISteamController001 *OpenSteamHelper::SteamController() {
	return this->steamController;
}

ISteamUGC003 *OpenSteamHelper::SteamUGC() {
	return this->steamUGC;
}

ISteamAppList001 *OpenSteamHelper::SteamAppList() {
	return this->steamAppList;
}

ISteamMusic001 *OpenSteamHelper::SteamMusic() {
	return this->steamMusic;
}

ISteamMusicRemote *OpenSteamHelper::SteamMusicRemote() {
	return this->steamMusicRemote;
}

ISteamHTMLSurface *OpenSteamHelper::SteamHTMLSurface() {
	return this->steamHTMLSurface;
}

ISteamInventory001 *OpenSteamHelper::SteamInventory() {
	return this->steamInventory;
}

ISteamVideo *OpenSteamHelper::SteamVideo() {
	return this->steamVideo;
}
