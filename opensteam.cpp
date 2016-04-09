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
#include "opensteam.h"
#include "calladmin-client.h"

#include <wx/tokenzr.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// The Thread Class
SteamThread::SteamThread() {
	this->steamid = "";
	this->isConnected = false;

	// No Last Error
	this->lastError = STEAM_NO_ERROR;

	// Start Thread
	CreateThread(wxTHREAD_DETACHED);
	GetThread()->Run();
}


SteamThread::~SteamThread() {
	if (GetThread()) {
		GetThread()->Delete();
	}

	// Wait until thread is finished
	while (true) {
		if (!GetThread()) {
			break;
		}

		wxThread::This()->Sleep(1);
	}
}


// Load Steam
STEAM_ERROR_TYP SteamThread::Load() {
	if (!caGetConfig()->GetSteamEnabled()) {
		// Don't want Steam
		return STEAM_DISABLED;
	}

	// Use Steam helper
	if (!OpenSteamHelper::GetInstance()->SteamAPI_Init()) {
		return STEAM_ERROR;
	}

	// Everything is good :)
	return STEAM_NO_ERROR;
}


// Clean Up Steam Stuff
void SteamThread::Clean() {
	// Reset
	this->isConnected = false;
	this->steamid = "";

	// Use Steam helper
	OpenSteamHelper::GetInstance()->SteamAPI_Shutdown();
}


// We need the 64bit int id
CSteamID SteamThread::SteamIdtoCSteamId(wxString steamId) {
	CSteamID cSteamId;

	// To small
	if (wxStrlen(steamId) < 11) {
		return cSteamId;
	}

	// Strip the steamid
	wxStringTokenizer tokenizer(steamId, ":");

	// There should be three tokens
	if (tokenizer.CountTokens() < 3) {
		return cSteamId;
	}

	tokenizer.GetNextToken();
	int server = wxAtoi(tokenizer.GetNextToken());
	int authId = wxAtoi(tokenizer.GetNextToken());

	// Wrong Format
	if (authId == 0) {
		return cSteamId;
	}

	uint64 uintId = (uint64)authId * 2;

	// Convert to a uint64
	uintId += 76561197960265728 + server;

	cSteamId.SetFromUint64(uintId);

	// Return it
	return cSteamId;
}


// Thread started
wxThread::ExitCode SteamThread::Entry() {
	for (; !GetThread()->TestDestroy(); wxMilliSleep(100)) {
		if (!this->isConnected) {
			// Load Steam
			STEAM_ERROR_TYP steamError = Load();

			// Check if steam could be loaded
			if (steamError != STEAM_NO_ERROR) {
				// Clean
				Clean();

				if (steamError == STEAM_ERROR) {
					// Only if connected before or was disabled before
					if (this->isConnected || this->lastError == STEAM_DISABLED) {
						// Notice changes to main panel
						caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 1);
						caGetApp().CallAfter(&CallAdmin::LogAction, "Couldn't connect to Steam", LogLevel::LEVEL_ERROR);
					}
				} else {
					// Disabled
					if (this->lastError != STEAM_DISABLED) {
						// Notice changes to main panel
						caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 0);
					}
				}
			} else {
				// Connected :)
				this->steamid = OpenSteamHelper::GetInstance()->SteamUser()->GetSteamID().Render();

				// Notice changes to main panel
				this->isConnected = true;

				caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 2);
				caGetApp().CallAfter(&CallAdmin::LogAction, "Connected to Steam", LogLevel::LEVEL_INFO);

				// Load avatars that not loaded, yet
				for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
					if (!(*callDialog)->GetAvatarsLoaded()) {
						(*callDialog)->GetEventHandler()->CallAfter(&CallDialog::LoadAvatars);
					}
				}
			}


			this->lastError = steamError;
		}

		// If connected check if steam is still opened
		if (this->isConnected) {
			static unsigned int i = 0;

			if (++i % 10 == 0) {
				if (!OpenSteamHelper::GetInstance()->SteamAPI_IsSteamRunning() || !caGetConfig()->GetSteamEnabled()) {
					// Notice changes to main panel
					caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 1);
					caGetApp().CallAfter(&CallAdmin::LogAction, "Disconnected from Steam", LogLevel::LEVEL_INFO);

					// Clean Steam
					Clean();

					continue;
				}
			}
		}
	}

	// Clean on end
	Clean();

	return (wxThread::ExitCode)0;
}
