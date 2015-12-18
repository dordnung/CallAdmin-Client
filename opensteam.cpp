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

// Alloca
#if defined(__WXMSW__)
#define ALLOCA _alloca
#else
#define ALLOCA alloca
#endif


// The Thread Class
SteamThread::SteamThread() {
	this->loader = CSteamAPILoader(CSteamAPILoader::k_ESearchOrderSteamInstallFirst);

	// Reset
	this->pipeSteam = 0;
	this->clientUser = 0;
	this->steamClient = NULL;
	this->steamFriends = NULL;
	this->steamUser = NULL;
	this->steamUtils = NULL;

	// No Last Error
	this->lastError = STEAM_NO_ERROR;

	Create();
	Run();
}


// Destroy
SteamThread::~SteamThread() {
	Clean();
}


wxString SteamThread::GetUserSteamId() {
	return this->steamid;
}


bool SteamThread::IsConnected() {
	return isConnected;
}


ISteamFriends015* SteamThread::GetSteamFriends() {
	return this->steamFriends;
}


ISteamUtils007* SteamThread::GetSteamUtils() {
	return this->steamUtils;
}


STEAM_ERROR_TYP SteamThread::Load() {
	if (!caGetConfig()->GetSteamEnabled()) {
		// Don't want Steam
		return STEAM_DISABLED;
	}

	// Load Factore
	CreateInterfaceFn factory = this->loader.GetSteam3Factory();

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
	for (; !TestDestroy(); Sleep(100)) {
		wxMutexLocker steamLocker(steamLock);
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_SteamChanged);

		if (!this->pipeSteam || this->pipeSteam == -1 || !caGetConfig()->GetSteamEnabled()) {
			// Load Steam
			STEAM_ERROR_TYP steamError = Load();

			// Handling
			if (steamError != STEAM_NO_ERROR) {
				if (steamError == STEAM_ERROR) {
					// Only if connected before
					if (this->isConnected) {
						// Notice Main the changes
						event.SetInt(1);

						caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);
						caLogAction("Disonnected from Steam");
					}
				} else {
					// Disabled
					if (this->lastError != STEAM_DISABLED) {
						// Notice Main the changes
						event.SetInt(0);

						caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);

						// clean
						Clean();
					}
				}

				this->isConnected = false;
				this->steamid = "";

				continue;
			} else {
				// Connected :)
				this->steamid = this->steamUser->GetSteamID().Render();

				// Notice Main the changes
				event.SetInt(2);

				caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);
				caLogAction("Connected to Steam");

				this->isConnected = true;
			}


			this->lastError = steamError;
		}

		static unsigned int i = 0;

		if (++i % 50 == 0) {
			// Check if logged in
			if (this->steamUser) {
				if (!this->steamUser->BLoggedOn()) {
					// Only if connected before
					if (this->isConnected) {
						// Notice Main the changes
						event.SetInt(1);

						caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);
						caLogAction("Disonnected from Steam");
					}

					this->isConnected = false;

					// Clean Steam
					Clean();
					this->steamid = "";

					continue;
				}
			}
		}

		CallbackMsg_t callbackMsg;

		while (Steam_BGetCallback(this->pipeSteam, &callbackMsg)) {
			if (callbackMsg.m_iCallback == IPCFailure_t::k_iCallback) {
				// Only if connected before
				if (this->isConnected) {
					// Notice Main the changes
					event.SetInt(1);

					caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);
					caLogAction("Disonnected from Steam");
				}

				this->isConnected = false;
				this->steamid = "";

				// Clean Steam
				Clean();

				break;
			}

			Steam_FreeLastCallback(this->pipeSteam);
		}
	}
}


// Clean Up Steam Stuff
void SteamThread::Clean() {
	// Close Steam Stuff
	if (this->pipeSteam && this->pipeSteam != -1 && this->steamClient) {
		if (this->clientUser) {
			this->steamClient->ReleaseUser(this->pipeSteam, this->clientUser);
		}

		this->steamClient->BReleaseSteamPipe(this->pipeSteam);
	}

	// Reset
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




// Init. Timer
AvatarTimer::AvatarTimer(CSteamID *clientId, CSteamID *targetId, wxStaticBitmap* clientAvatar, wxStaticBitmap* targetAvatar) : wxTimer(this, -1) {
	this->clientId = clientId;
	this->targetId = targetId;
	this->clientAvatar = clientAvatar;
	this->targetAvatar = targetAvatar;
	this->attempts = 0;

	this->clientLoaded = this->targetLoaded = false;

	// Invalid Steamids -> Loaded
	if (this->clientId == NULL || !this->clientId->IsValid()) {
		this->clientLoaded = true;
	}

	if (this->targetId == NULL || !this->targetId->IsValid()) {
		this->targetLoaded = true;
	}
}


// Timer to update avatars
void AvatarTimer::Notify() {
	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		// Do we have information about the users?
		// Load the images
		if (!this->clientLoaded) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(*this->clientId, false)) {
				// Try to laod caller avatar
				this->clientLoaded = SetAvatar(this->clientId, this->clientAvatar);
			}
		}

		if (!this->targetLoaded) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(*this->targetId, false)) {
				// Try to laod target avatar
				this->targetLoaded = SetAvatar(this->targetId, this->targetAvatar);
			}
		}
	}

	// All loaded or 10 seconds gone?
	if (++this->attempts == 100 || (this->clientLoaded && this->targetLoaded)) {
		// Enough, stop timer
		Stop();
	}
}


void AvatarTimer::StartTimer() {
	Start(100);
}


// Set new Avatar
bool AvatarTimer::SetAvatar(CSteamID *id, wxStaticBitmap* map) {
	// Load avatar
	int avatar = caGetSteamThread()->GetSteamFriends()->GetLargeFriendAvatar(*id);

	// Could it load?
	if (avatar > 0) {
		// Buffer to store picture
		uint8* rgbaBuffer = (uint8*)ALLOCA(4 * 184 * 184);
		uint32* size = new uint32(184);

		// Is Size valid?
		if (caGetSteamThread()->GetSteamUtils()->GetImageSize(avatar, size, size)) {
			// Load Image
			if (caGetSteamThread()->GetSteamUtils()->GetImageRGBA(avatar, rgbaBuffer, (4 * 184 * 184))) {
				// Image
				wxImage image(184, 184);

				// RGBA to Image
				for (int y = 0; y < 184; y++) {
					int start = 184 * y * 4;

					for (int x = 0; x < 184; x++) {
						// Set Colour
						image.SetRGB(x, y, rgbaBuffer[start], rgbaBuffer[start + 1], rgbaBuffer[start + 2]);

						start += 4;
					}
				}

				if (caGetApp().GetAvatarSize() != 184) {
					image.Rescale(caGetApp().GetAvatarSize(), caGetApp().GetAvatarSize());
				}

				// Set new Avatar
				map->SetBitmap(wxBitmap(image));

				caLogAction("Loaded Avatar of " + (wxString)id->Render());

				// It's loaded
				return true;
			}
		}
	}

	return false;
}




NameTimer::NameTimer(CSteamID client) : wxTimer(this, -1) {
	this->attempts = 0;
	this->client = client;

	Start(100);
}


// Timer to update trackers
void NameTimer::Notify() {
	// Steam available?
	if (caGetSteamThread()->IsConnected()) {
		if (this->client.IsValid()) {
			if (!caGetSteamThread()->GetSteamFriends()->RequestUserInformation(this->client, true)) {
				wxString isFriend = "No Friends";
				wxString isOnline = "Offline";

				// Is Friend?
				if (caGetSteamThread()->GetSteamFriends()->GetFriendRelationship(client) == k_EFriendRelationshipFriend) {
					isFriend = "Friends";
				}

				// Is Online?
				if (caGetSteamThread()->GetSteamFriends()->GetFriendPersonaState(client) != k_EPersonaStateOffline) {
					// Online
					if (isFriend == "Friends") {
						isOnline = "Online";
					} else {
						// We can't know
						isOnline = "Unknown Status";
					}
				}


				// Add Tracker
				if (this->client.Render() != wxGetApp().GetSteamThread()->GetUserSteamId()) {
					caGetTrackersPanel()->AddTracker("" + (wxString)caGetSteamThread()->GetSteamFriends()->GetFriendPersonaName(this->client) + " - " + (wxString)this->client.Render() + " - " + isFriend + " - " + isOnline);
				} else {
					caGetTrackersPanel()->AddTracker("" + (wxString)caGetSteamThread()->GetSteamFriends()->GetFriendPersonaName(this->client) + " - " + (wxString)this->client.Render());
				}

				//Stop
				Stop();
			}
		}
	}


	// All loaded or 5 seconds gone?
	if (++this->attempts == 50) {
		// Enough, stop timer
		Stop();
	}
}