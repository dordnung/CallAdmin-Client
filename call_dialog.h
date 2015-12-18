#ifndef CALL_DIALOG_H
#define CALL_DIALOG_H

/**
 * -----------------------------------------------------
 * File        call_dialog.h
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

// Steam Class
#include "opensteam.h"


// Call Dialog Class
class CallDialog : public wxDialog {
private:
	// Timer
	AvatarTimer *avatarTimer;

	// We need information about a call ;)
	wxString callID;
	wxString fullIP;
	wxString serverName;
	wxString target;
	wxString targetID;
	wxString client;
	wxString clientID;
	wxString reason;
	wxString boxText;
	long reportedAt;

	// Layout
	wxSizer* sizerTop;

	// Avatars
	wxStaticBitmap *clientAvatar;
	wxStaticBitmap *targetAvatar;
	wxStaticText *doneText;

	// Tracker button
	wxButton *contactTrackers;

	// take Over Button
	wxButton *takeover;

	// And for the Steam API
	CSteamID clientCID;
	CSteamID targetCID;

	// Item List
	int id;
	bool isHandled;

public:
	CallDialog(const wxString &title);

	// Privat -> set Methods
	void SetCallID(const char* id);
	void SetIP(const char* ip);
	void SetName(wxString name);
	void SetTarget(const char* name);
	void SetTargetID(const char* steamidId);
	void SetClient(const char* name);
	void SetClientID(const char* steamidId);
	void SetReason(const char* name);
	void SetID(int num);
	void SetTime(long time);
	void SetBoxText(wxString text);
	void SetHandled(bool handled);

	void SetFinish();

	// Convert to community ID
	static CSteamID SteamIDtoCSteamID(char* steamid);

	// Return time as a int
	inline long GetTime() const;

	// Methods for Details
	wxString GetTarget() const;
	wxString GetClient() const;

	wxString GetID() const;
	wxString GetServer();
	wxString GetBoxText();
	AvatarTimer* GetAvatarTimer();
	wxButton* GetContactTrackersButton();
	wxButton* GetTakeoverButton();

	CSteamID* GetClientCID();
	CSteamID* GetTargetCID();

	bool GetHandled();

	// Start the call
	void StartCall(bool show);

	// Operator overloadings
	friend bool operator==(const CallDialog &x, const CallDialog &y) {
		return (x.GetTime() == y.GetTime() && (x.GetID() == y.GetID()));
	}
	friend bool operator!=(const CallDialog &x, const CallDialog &y) {
		return !(x == y);
	}

protected:
	// Button Events
	void OnConnect(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnContactClient(wxCommandEvent& event);
	void OnContactTarget(wxCommandEvent& event);
	void OnContactTrackers(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);
	void OnMinimizeWindow(wxIconizeEvent& event);

	DECLARE_EVENT_TABLE()
};


// CURL Callbacks
void OnGetTrackers(char *errors, wxString result, int extra);
void OnChecked(char *error, wxString result, int extra);


#endif
