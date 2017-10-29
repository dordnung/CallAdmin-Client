#ifndef CALL_DIALOG_H
#define CALL_DIALOG_H

/**
 * -----------------------------------------------------
 * File        call_dialog.h
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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "opensteam.h"

class CallDialog;
class AvatarTimer;


// Call Dialog Class
class CallDialog : public wxDialog {
private:
	// Avatars
	bool avatarsLoaded;

	// We need information about a call ;)
	wxString callId;
	wxString fullIP;
	wxString serverName;
	wxString target;
	wxString targetId;
	wxString client;
	wxString clientId;
	wxString reason;
	wxDateTime reportedAt;

	// The panel
	wxPanel *panel;

	// Avatars with Timer
	wxStaticBitmap *clientAvatar;
	wxStaticBitmap *targetAvatar;
	AvatarTimer *avatarTimer;

	// Tracker button
	wxButton *contactTrackers;

	// Take Over Button
	wxButton *takeover;

	// And for the Steam API
	CSteamID clientCId;
	CSteamID targetCId;

	// Item List
	int id;
	bool isHandled;

public:
	CallDialog();
	~CallDialog();

	// Start the call
	bool StartCall(bool show);

	void LoadAvatars();

	// Mark the call as finished
	void SetFinish();

	void SetAvatarsLoaded(bool successfull);

	// CURL Callback
	static void OnChecked(wxString errorStr, wxString result, int extra);

	// Privat -> Get Methods
	bool GetAvatarsLoaded() {
		return this->avatarsLoaded;
	}

	wxString GetTime() {
		return this->reportedAt.Format("%Y-%m-%d %H:%M:%S");
	}

	wxString GetTarget() {
		return this->target;
	}

	wxString GetClient() {
		return this->client;
	}

	wxString GetId() {
		return this->callId;
	}

	wxString GetServer() {
		return this->serverName;
	}

	wxButton *GetContactTrackersButton() {
		return this->contactTrackers;
	}

	wxButton *GetTakeoverButton() {
		return this->takeover;
	}

	bool IsHandled() {
		return this->isHandled;
	}

	// Privat -> Set Methods
	void SetCallId(wxString id) {
		this->callId = id;
	}

	void SetIP(wxString ip) {
		this->fullIP = ip;
	}

	void SetName(wxString name) {
		this->serverName = wxString::FromUTF8(name);
	}

	void SetTarget(wxString name) {
		this->target = name;
	}

	void SetTargetId(wxString steamidId) {
		this->targetId = steamidId;
		this->targetCId = SteamThread::SteamIdtoCSteamId(steamidId);
	}

	void SetClient(wxString name) {
		this->client = name;
	}

	void SetClientId(wxString steamidId) {
		this->clientId = steamidId;
		this->clientCId = SteamThread::SteamIdtoCSteamId(steamidId);
	}

	void SetReason(wxString name) {
		this->reason = name;
	}

	void SetId(int id) {
		this->id = id;
	}

	void SetTime(long time) {
		this->reportedAt = wxDateTime((time_t)time);
	}

	void SetHandled(bool handled) {
		this->isHandled = handled;
	}

	// Operator overloadings
	bool operator==(CallDialog &callDialog) {
		// A CallDialog is unique by it's time and ID
		return (GetTime() == callDialog.GetTime() && (GetId() == callDialog.GetId()));
	}

	bool operator!=(CallDialog &callDialog) {
		return !(operator==(callDialog));
	}

protected:
	// Events
	void OnConnect(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
	void OnContactClient(wxCommandEvent &event);
	void OnContactTarget(wxCommandEvent &event);
	void OnContactTrackers(wxCommandEvent &event);

	void OnCloseWindow(wxCloseEvent &event);

	wxDECLARE_EVENT_TABLE();
};


// Avatar Timer Class
class AvatarTimer : public wxTimer {
private:
	int attempts;

	// The dialog
	CallDialog *dialog;

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
	AvatarTimer(CallDialog *dialog, CSteamID *clientId, CSteamID *targetId, wxStaticBitmap *clientAvatar, wxStaticBitmap *targetAvatar);

	bool SetAvatar(CSteamID *id, wxStaticBitmap *map);

protected:
	virtual void Notify();
};


#endif
