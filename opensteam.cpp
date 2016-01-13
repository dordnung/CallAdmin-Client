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

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// The Thread Class
SteamThread::SteamThread() {
	// Reset
	this->pipeSteam = 0;
	this->clientUser = 0;
	this->steamClient = NULL;
	this->steamFriends = NULL;
	this->steamUser = NULL;
	this->steamUtils = NULL;

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


STEAM_ERROR_TYP SteamThread::Load() {
	if (!caGetConfig()->GetSteamEnabled()) {
		// Don't want Steam
		return STEAM_DISABLED;
	}

	// Load Factory
	CSteamAPILoader loader = CSteamAPILoader(CSteamAPILoader::k_ESearchOrderSteamInstallFirst);
	CreateInterfaceFn factory = loader.GetSteam3Factory();

	if (!factory) {
		return STEAM_ERROR;
	}

	// Load Steam Client
	this->steamClient = (ISteamClient017*)factory(STEAMCLIENT_INTERFACE_VERSION_017, NULL);

	if (!this->steamClient) {
		return STEAM_ERROR;
	}

	// Create Pipe
	this->pipeSteam = this->steamClient->CreateSteamPipe();

	if (!this->pipeSteam || this->pipeSteam == -1) {
		return STEAM_ERROR;
	}

	// Workaround for deadlock
	this->steamClient->BReleaseSteamPipe(this->pipeSteam);
	wxMilliSleep(5000);

	// Create Pipe again
	this->pipeSteam = this->steamClient->CreateSteamPipe();

	if (!this->pipeSteam || this->pipeSteam == -1) {
		return STEAM_ERROR;
	}

	// Connect User
	this->clientUser = this->steamClient->ConnectToGlobalUser(this->pipeSteam);

	if (!this->clientUser) {
		return STEAM_ERROR;
	}

	// Load SteamFriends
	this->steamFriends = (ISteamFriends015*)this->steamClient->GetISteamFriends(this->clientUser, this->pipeSteam, STEAMFRIENDS_INTERFACE_VERSION_015);

	if (!this->steamFriends) {
		return STEAM_ERROR;
	}

	// Load Steam User
	this->steamUser = (ISteamUser017*)this->steamClient->GetISteamUser(this->clientUser, this->pipeSteam, STEAMUSER_INTERFACE_VERSION_017);

	if (!this->steamUser) {
		return STEAM_ERROR;
	}

	// Load Steam Utils
	this->steamUtils = (ISteamUtils007*)this->steamClient->GetISteamUtils(this->pipeSteam, STEAMUTILS_INTERFACE_VERSION_007);

	if (!this->steamUtils) {
		return STEAM_ERROR;
	}

	// Everything is good :)
	return STEAM_NO_ERROR;
}


// Thread started
void SteamThread::Check() {
	for (; !GetThread()->TestDestroy(); wxMilliSleep(100)) {
		if (this->pipeSteam <= 0) {
			// Load Steam
			STEAM_ERROR_TYP steamError = Load();

			// Handling
			if (steamError != STEAM_NO_ERROR) {
				if (steamError == STEAM_ERROR) {
					// Only if connected before
					if (this->isConnected) {
						// Notice changes to main panel
						caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 1);
						caGetLogPanel()->GetEventHandler()->CallAfter(&LogPanel::AddLog, "Disonnected from Steam", LogLevel::LEVEL_INFO);

						// Clean
						Clean();
					}
				} else {
					// Disabled
					if (this->lastError != STEAM_DISABLED) {
						// Notice changes to main panel
						caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 0);
					}

					// Clean
					Clean();
				}

				this->isConnected = false;
				this->steamid = "";
			} else {
				// Connected :)
				this->steamid = this->steamUser->GetSteamID().Render();

				// Notice changes to main panel
				caGetMainPanel()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 2);
				caGetLogPanel()->GetEventHandler()->CallAfter(&LogPanel::AddLog, "Connected to Steam", LogLevel::LEVEL_INFO);

				this->isConnected = true;
			}


			this->lastError = steamError;
		}

		// If connected check if still logged in and Steam is still opened
		if (this->isConnected) {
			static unsigned int i = 0;

			if (++i % 50 == 0) {
				// Check if logged in
				if (!this->steamUser->BLoggedOn()) {
					// Notice changes to main panel
					caGetMainFrame()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 1);
					caGetLogPanel()->GetEventHandler()->CallAfter(&LogPanel::AddLog, "Disonnected from Steam", LogLevel::LEVEL_INFO);

					// Clean Steam
					Clean();

					continue;
				}
			}

			CallbackMsg_t callbackMsg;

			/*while (Steam_BGetCallback(this->pipeSteam, &callbackMsg)) {
				if (callbackMsg.m_iCallback == IPCFailure_t::k_iCallback || callbackMsg.m_iCallback == SteamServerConnectFailure_t::k_iCallback
					|| callbackMsg.m_iCallback == SteamServersDisconnected_t::k_iCallback) {
					// Notice changes to main panel
					caGetMainFrame()->GetEventHandler()->CallAfter(&MainPanel::OnSteamChange, 1);
					caGetLogPanel()->GetEventHandler()->CallAfter(&LogPanel::AddLog, "Disonnected from Steam", LogLevel::LEVEL_INFO);

					// Free Callback
					Steam_FreeLastCallback(this->pipeSteam);

					// Clean Steam
					Clean();

					break;
				}

				Steam_FreeLastCallback(this->pipeSteam);
			}*/
		}
	}

	// Clean on end
	Clean();
}


// Clean Up Steam Stuff
void SteamThread::Clean() {
	// Close Steam Stuff
	if (this->pipeSteam > 0 && this->steamClient) {
		if (this->clientUser) {
			this->steamClient->ReleaseUser(this->pipeSteam, this->clientUser);
		}

		this->steamClient->BShutdownIfAllPipesClosed();
		this->steamClient->BReleaseSteamPipe(this->pipeSteam);
	}

	// Reset
	this->isConnected = false;
	this->steamid = "";

	this->pipeSteam = 0;
	this->clientUser = 0;
	this->steamClient = NULL;
	this->steamFriends = NULL;
	this->steamUser = NULL;
	this->steamUtils = NULL;
}



wxThread::ExitCode SteamThread::Entry() {
	// Check Steam
	Check();

	return (wxThread::ExitCode)0;
}