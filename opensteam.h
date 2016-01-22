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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "opensteam_helper.h"


enum STEAM_ERROR_TYP {
	STEAM_NO_ERROR = 0,
	STEAM_DISABLED,
	STEAM_ERROR,
};


class SteamThread : public wxThreadHelper {
private:
	wxString steamid;
	bool isConnected;

	// Last Steam Error
	STEAM_ERROR_TYP lastError;

public:
	SteamThread();
	~SteamThread();

	// Load Steam
	STEAM_ERROR_TYP Load();

	// Cleanup Steam
	void Clean();

	// Convert to community ID
	static CSteamID SteamIdtoCSteamId(wxString steamId);

	wxString GetUserSteamId() {
		return this->steamid;
	}

	bool IsConnected() {
		return this->isConnected;
	}

protected:
	virtual wxThread::ExitCode Entry();
};

#endif
