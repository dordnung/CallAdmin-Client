#ifndef OPENSTEAM_H
#define OPENSTEAM_H

/**
 * -----------------------------------------------------
 * File        opensteam.h
 * Authors     David <popoklopsi> Ordnung, Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 * 
 * 
 * Copyright (C) 2013 David <popoklopsi> Ordnung, Impact
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
#include <wx/thread.h>


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
extern ISteamUtils005* steamUtils;



// Thread Class for Steam pipe
class pipeThread: public wxThread
{
private:
	// Steamid API Loader
	CSteamAPILoader loader;

public:

	// Create and Start
	pipeThread() : wxThread(wxTHREAD_JOINABLE), loader(CSteamAPILoader::k_ESearchOrderSteamInstallFirst) {this->Create(); this->Run();}

	bool loadSteam();

	virtual void* Entry();

	wxMutex m_steamLock;
};



// Second Timer Class
class SecondTimer : public wxTimer
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
	SecondTimer(CSteamID *cid, CSteamID *tid, wxStaticBitmap* cAvatar, wxStaticBitmap* tAvatar);

	void startTimer() {Start(100);}
	bool setAvatar(CSteamID *id, wxStaticBitmap* map);

	void Notify();
};


// Cleanup Steam
void cleanSteam();


// Threader
extern pipeThread *pThread;

#endif
