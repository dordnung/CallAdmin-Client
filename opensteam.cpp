/**
 * -----------------------------------------------------
 * File        opensteam.cpp
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Header File
 * Copyright (C) 2013 Impact, David <popoklopsi> Ordnung
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
#include "calladmin-client.h"



// Steamid
std::string steamid = "";

// Is Connected?
bool steamConnected = false;

// Var to send messages
HSteamPipe pipe = 0;
HSteamUser clientUser = 0;
ISteamFriends013* steamFriends = NULL;
ISteamClient012* steamClient = NULL;
ISteamUser016* steamUser = NULL;
ISteamUtils005* steamUtils = NULL;


// Threader
pipeThread *pThread = NULL;


// The Thread Class
bool pipeThread::loadSteam()
{
	if (!TestDestroy())
	{
		// Lock Steam
		wxMutexLocker lockerSteam(m_steamLock);

		// Load Factore
		CreateInterfaceFn factory = loader.GetSteam3Factory();

		if (!factory || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}

		// Load Steam Client
		steamClient = (ISteamClient012*)factory(STEAMCLIENT_INTERFACE_VERSION_012, NULL);

		if (!steamClient || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}
        
		// Create Pipe
		pipe = steamClient->CreateSteamPipe();

		if (!pipe || pipe == -1 || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}
	
		// Connect User
		clientUser = steamClient->ConnectToGlobalUser(pipe);
	
		if (!clientUser || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}

		// Load SteamFriends
		steamFriends = (ISteamFriends013*)steamClient->GetISteamFriends(clientUser, pipe, STEAMFRIENDS_INTERFACE_VERSION_013);

		if(!steamFriends || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}

		// Load Steam User
		steamUser = (ISteamUser016*)steamClient->GetISteamUser(clientUser, pipe, STEAMUSER_INTERFACE_VERSION_016);

		if(!steamUser || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}

		// Load Steam Utils
		steamUtils = (ISteamUtils005*)steamClient->GetISteamUtils(pipe, STEAMUTILS_INTERFACE_VERSION_005);

		if(!steamUtils || TestDestroy())
		{
			// Clean Steam
			::cleanSteam();

			return false;
		}

		return true;
	}

	return false;
}


// Thread started
void* pipeThread::Entry()
{
	// No? Load it
	if(TestDestroy() || !loadSteam())
	{
		// Clean Steam
		::cleanSteam();

		steamConnected = false;

		if (main_dialog != NULL)
		{
			main_dialog->setSteamStatus("Steam is currently not running", (wxString)"red");
		}

		// Steamid not known
		steamid = "";
	}
	else
	{
		// Connected :)
		steamConnected = true;
		steamid = steamUser->GetSteamID().Render(); 

		main_dialog->setSteamStatus("Steam is currently running", wxColour(34, 139, 34));
	}

	return NULL;
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
		uint8* rgbaBuffer = (uint8*)_alloca(4 * 184 * 184);

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

				// It's loaded
				return true;
			}
		}
	}

	return false;
}



// Clean Up Steam Stuff
void cleanSteam()
{
	// Close Steam Stuff
	if (pipe && pipe != -1 && clientUser && steamClient)
	{
		steamClient->ReleaseUser(pipe, clientUser);
		steamClient->BReleaseSteamPipe(pipe);
		steamClient->BShutdownIfAllPipesClosed();
	}

	// Reset
	pipe = 0;
	clientUser = 0;
	steamClient = NULL;
	steamFriends = NULL;
	steamUser = NULL;
	steamUtils = NULL;
}
