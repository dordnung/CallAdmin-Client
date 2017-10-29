#ifndef OPENSTEAM_H
#define OPENSTEAM_H

/**
 * -----------------------------------------------------
 * File        opensteam.h
 * Authors     David Ordnung, Impact
 * License     GPLv3
 * Web         http://dordnung.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung, Impact
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

// c++ libs
#include <string>


// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif


// Steamworks warning -> disable
#pragma warning(disable: 4245)

// Steam Workds for Client tools
#include "Steamworks.h"

// Steamworks warning -> enable
#pragma warning(default: 4245)



// Steamid
extern std::string steamid;

// Is Connected?
extern bool steamConnected;


// Var to send messages
extern HSteamPipe pipeSteam;
extern HSteamUser clientUser;
extern ISteamFriends013* steamFriends;
extern ISteamClient012* steamClient;
extern ISteamUser016* steamUser;
extern ISteamUtils006* steamUtils;


enum STEAM_ERROR_TYP
{
	STEAM_NO_ERROR = 0,
	STEAM_DISABLED,
	STEAM_ERROR,
};



class steamThread : public wxThread
{
private:
	CSteamAPILoader loader;
	wxMutex steamLock;

	// Last Steam Error
	STEAM_ERROR_TYP lastError;

public:
	steamThread();
	~steamThread();

	virtual ExitCode Entry();

	STEAM_ERROR_TYP loadSteam();
	void checkSteam();

	// Cleanup Steam
	void cleanSteam();
};





// Avatar Timer Class
class AvatarTimer : public wxTimer
{
private:
	int attempts;

	// The ID's
	CSteamID *clientsID;
	CSteamID *targetsID;

	// The avatars
	wxStaticBitmap *clientsAvatar;
	wxStaticBitmap *targetsAvatar;

	bool clientLoaded;
	bool targetLoaded;

public:
	// Init Timer
	AvatarTimer(CSteamID *cid, CSteamID *tid, wxStaticBitmap* cAvatar, wxStaticBitmap* tAvatar);

	void startTimer() {Start(100);}
	bool setAvatar(CSteamID *id, wxStaticBitmap* map);

	void Notify();
};




// Name Timer Class
class NameTimer : public wxTimer
{
private:
	int attempts;

	// The ID
	CSteamID client;

public:
	// Init Timer
	NameTimer(CSteamID clients) : wxTimer(this, -1) {attempts = 0; client = clients; Start(100);}

	void Notify();
};



// Gobal Thread 
extern steamThread* steamThreader;

#endif
