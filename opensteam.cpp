/**
 * -----------------------------------------------------
 * File        opensteam.cpp
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

#include "opensteam.h"
#include "main.h"
#include "log.h"
#include "config.h"
#include "calladmin-client.h"


// Alloca
#if defined(__WXMSW__)
	#define ALLOCA _alloca
#else
	#define ALLOCA alloca
#endif



// Steamid
std::string steamid = "";


// Is Connected?
bool steamConnected = false;


// Var to send messages
HSteamPipe pipeSteam = 0;
HSteamUser clientUser = 0;
ISteamFriends013* steamFriends = NULL;
ISteamClient012* steamClient = NULL;
ISteamUser016* steamUser = NULL;
ISteamUtils005* steamUtils = NULL;


// Steam Thread
steamThread *steamThreader = NULL;


// The Thread Class
steamThread::steamThread() : wxThread(wxTHREAD_DETACHED), loader(CSteamAPILoader::k_ESearchOrderSteamInstallFirst)
{
	// Reset
	pipeSteam = 0;
	clientUser = 0;
	steamClient = NULL;
	steamFriends = NULL;
	steamUser = NULL;
	steamUtils = NULL;

	// No Last Error
	lastError = STEAM_NO_ERROR;

    this->Create();
    this->Run();
}


// Destroy
steamThread::~steamThread()
{
	cleanSteam();
}


STEAM_ERROR_TYP steamThread::loadSteam()
{
	if (!steamEnabled)
	{
		// Don't want Steam
		return STEAM_DISABLED;
	}
	else
	{
		// Load Factore
		CreateInterfaceFn factory = loader.GetSteam3Factory();
		
		if (!factory)
		{
			return STEAM_ERROR;
		}

		// Load Steam Client
		steamClient = (ISteamClient012*)factory(STEAMCLIENT_INTERFACE_VERSION_012, NULL);
		
		if (!steamClient)
		{
			return STEAM_ERROR;
		}
		
		// Create Pipe
		pipeSteam = steamClient->CreateSteamPipe();
		
		if (!pipeSteam || pipeSteam == -1)
		{
			return STEAM_ERROR;
		}


		// Workaround for deadlock
        steamClient->BReleaseSteamPipe(pipeSteam);
        wxMilliSleep(5000);


		// Create Pipe again
		pipeSteam = steamClient->CreateSteamPipe();
		
		if (!pipeSteam || pipeSteam == -1)
		{
			return STEAM_ERROR;
		}
	

	
		// Connect User
		clientUser = steamClient->ConnectToGlobalUser(pipeSteam);
		
		if (!clientUser)
		{
			return STEAM_ERROR;
		}

		// Load SteamFriends
		steamFriends = (ISteamFriends013*)steamClient->GetISteamFriends(clientUser, pipeSteam, STEAMFRIENDS_INTERFACE_VERSION_013);

		if (!steamFriends)
		{
			return STEAM_ERROR;
		}

		// Load Steam User
		steamUser = (ISteamUser016*)steamClient->GetISteamUser(clientUser, pipeSteam, STEAMUSER_INTERFACE_VERSION_016);
		
		if (!steamUser)
		{
			return STEAM_ERROR;
		}

		// Load Steam Utils
		steamUtils = (ISteamUtils005*)steamClient->GetISteamUtils(pipeSteam, STEAMUTILS_INTERFACE_VERSION_005);

		if (!steamUtils)
		{
			return STEAM_ERROR;
		}

		// Everything is good :)
		return STEAM_NO_ERROR;
	}
}


// Thread started
void steamThread::checkSteam()
{
	for(; !TestDestroy(); Sleep(100))
	{
		wxMutexLocker steamLocker(steamLock);
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_SteamChanged);

		if (!pipeSteam || pipeSteam == -1 || !steamEnabled)
		{
			// Load Steam
			STEAM_ERROR_TYP steamError = loadSteam();

			// Handling
			if (steamError != STEAM_NO_ERROR)
			{
				if (steamError == STEAM_ERROR)
				{
					// Only if connected before
					if (main_dialog != NULL && steamConnected)
					{
						// Notice Main the changes
						event.SetInt(1);

						main_dialog->GetEventHandler()->AddPendingEvent(event);

						LogAction("Disonnected from Steam");
					}
				}
				else
				{
					// Disabled
					if (main_dialog != NULL && lastError != STEAM_DISABLED)
					{
						// Notice Main the changes
						event.SetInt(0);

						main_dialog->GetEventHandler()->AddPendingEvent(event);

						// clean
						cleanSteam();
					}
				}

				steamConnected = false;
				steamid = "";

				continue;
			}
			else
			{
				// Connected :)
				steamid = steamUser->GetSteamID().Render(); 

				// Notice Main the changes
				event.SetInt(2);

				main_dialog->GetEventHandler()->AddPendingEvent(event);

				LogAction("Connected to Steam");

				steamConnected = true;
			}


			lastError = steamError;
		}

		static unsigned int i = 0;

		if (++i % 50 == 0)
		{
            // Check if logged in
			if (steamUser)
			{
				if (!steamUser->BLoggedOn())
				{
					// Only if connected before
					if (main_dialog != NULL && steamConnected)
					{
						// Notice Main the changes
						event.SetInt(1);

						main_dialog->GetEventHandler()->AddPendingEvent(event);

						LogAction("Disonnected from Steam");
					}

					steamConnected = false;

					// Clean Steam
					cleanSteam();
					steamid = "";

					continue;
				}
			}
		}


		CallbackMsg_t callbackMsg;

		while (Steam_BGetCallback(pipeSteam, &callbackMsg))
        {
			if (callbackMsg.m_iCallback == IPCFailure_t::k_iCallback)
            {
				// Only if connected before
				if (main_dialog != NULL && steamConnected)
				{
					// Notice Main the changes
					event.SetInt(1);

					main_dialog->GetEventHandler()->AddPendingEvent(event);

					LogAction("Disonnected from Steam");
				}

				steamConnected = false;
				steamid = "";

				// Clean Steam
				cleanSteam();

                break;
			}

            Steam_FreeLastCallback(pipeSteam);
        }
	}
}



// Clean Up Steam Stuff
void steamThread::cleanSteam()
{
	// Close Steam Stuff
	if (pipeSteam && pipeSteam != -1 && steamClient)
	{
		if (clientUser)
		{
			steamClient->ReleaseUser(pipeSteam, clientUser);
		}

		steamClient->BReleaseSteamPipe(pipeSteam);
	}

	// Reset
	pipeSteam = 0;
	clientUser = 0;
	steamClient = NULL;
	steamFriends = NULL;
	steamUser = NULL;
	steamUtils = NULL;
}



wxThread::ExitCode steamThread::Entry()
{
	// Check Steam
	checkSteam();

	return (wxThread::ExitCode)0;
}



// Init. Timer
SecondTimer::SecondTimer(CSteamID *cid, CSteamID *tid, wxStaticBitmap* cAvatar, wxStaticBitmap* tAvatar) : wxTimer(this, -1) 
{
	clientsID = cid; 
	targetsID = tid; 
	attempts = 0; 
	clientsAvatar = cAvatar; 
	targetsAvatar = tAvatar; 

	clientLoaded = targetLoaded = false;

	// Invalid Steamid?
	if (clientsID == NULL || !clientsID->IsValid())
	{
		clientLoaded = true;
	}

	if (targetsID == NULL || !targetsID->IsValid())
	{
		targetLoaded = true;
	}
}



// Timer to update avatars
void SecondTimer::Notify()
{
	// Steam available?
	if (steamConnected && steamFriends != NULL && steamUtils != NULL)
	{
		// Do we have information about the users?
		// Load the images
		if (!clientLoaded)
		{
			if (!steamFriends->RequestUserInformation(*clientsID, false))
			{
				// Try to laod caller avatar
				clientLoaded = setAvatar(clientsID, clientsAvatar);
			}
		}

		if (!targetLoaded)
		{
			if (!steamFriends->RequestUserInformation(*targetsID, false))
			{
				// Try to laod target avatar
				targetLoaded = setAvatar(targetsID, targetsAvatar);
			}
		}
	}

	// All loaded or 10 seconds gone?
	if (++attempts == 100 || (clientLoaded && targetLoaded))
	{
		// Enough, stop timer
		Stop();
	}
}


// Set new Avatar
bool SecondTimer::setAvatar(CSteamID *id, wxStaticBitmap* map)
{
	// Load avatar
	int avatar = steamFriends->GetLargeFriendAvatar(*id);

	// Could it load?
	if (avatar > 0)
	{
		// Buffer to store picture
		uint8* rgbaBuffer = (uint8*)ALLOCA(4 * 184 * 184);

		uint32* size = new uint32(184);

		// Is Size valid?
		if (steamUtils->GetImageSize(avatar, size, size))
		{
			// Load Image
			if (steamUtils->GetImageRGBA(avatar, rgbaBuffer, (4 * 184 * 184)))
			{
				// Image
				wxImage image(184, 184);

				// RGBA to Image
				for (int y=0; y < 184; y++)
				{
					int start = 184 * y * 4;

					for (int x=0; x < 184; x++)
					{
						// Set Colour
						image.SetRGB(x, y, rgbaBuffer[start], rgbaBuffer[start+1], rgbaBuffer[start+2]);

						start += 4;
					}
				}
				
				if (avatarSize != 184)
				{
					image.Rescale(avatarSize, avatarSize);
				}

				// Set new Avatar
				map->SetBitmap(wxBitmap(image));


				LogAction("Loaded Avatar of " + (wxString)id->Render());


				// It's loaded
				return true;
			}
		}
	}

	return false;
}