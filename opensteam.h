#ifndef OPENSTEAM_H
#define OPENSTEAM_H

/**
 * -----------------------------------------------------
 * File        opensteam.h
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

// Steamworks warning -> disable
#pragma warning(disable: 4245)

// Steam Works for Client tools
#include "Steamworks.h"

// Steamworks warning -> enable
#pragma warning(default: 4245)


enum STEAM_ERROR_TYP {
	STEAM_NO_ERROR = 0,
	STEAM_DISABLED,
	STEAM_ERROR,
};


class SteamThread : public wxThread {
private:
	wxString steamid;
	bool isConnected;

	CSteamAPILoader loader;
	wxMutex steamLock;

	// Last Steam Error
	STEAM_ERROR_TYP lastError;

	HSteamPipe pipeSteam;
	HSteamUser clientUser;
	ISteamFriends015 *steamFriends;
	ISteamClient017 *steamClient;
	ISteamUser017 *steamUser;
	ISteamUtils007 *steamUtils;

public:
	SteamThread();
	~SteamThread();

	virtual ExitCode Entry();

	wxString GetUserSteamId();
	bool IsConnected();

	ISteamFriends015* GetSteamFriends();
	ISteamUtils007* GetSteamUtils();

	STEAM_ERROR_TYP Load();
	void Check();

	// Cleanup Steam
	void Clean();
};


// Avatar Timer Class
class AvatarTimer : public wxTimer {
private:
	int attempts;

	// The ID's
	CSteamID *clientId;
	CSteamID *targetId;

	// The avatars
	wxStaticBitmap *clientAvatar;
	wxStaticBitmap *targetAvatar;

	bool clientLoaded;
	bool targetLoaded;

public:
	// Init Timer
	AvatarTimer(CSteamID *clientId, CSteamID *targetId, wxStaticBitmap *clientAvatar, wxStaticBitmap *targetAvatar);

	void StartTimer();
	bool SetAvatar(CSteamID *id, wxStaticBitmap *map);

	void Notify();
};


// Name Timer Class
class NameTimer : public wxTimer {
private:
	int attempts;

	// The ID
	CSteamID client;

public:
	// Init Timer
	NameTimer(CSteamID client);

	void Notify();
};

#endif
