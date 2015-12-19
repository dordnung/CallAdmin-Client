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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "opensteam.h"


// Avatar Timer Class
class AvatarTimer : public wxTimer {
private:
	int attempts;

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
	AvatarTimer(CSteamID *clientId, CSteamID *targetId, wxStaticBitmap *clientAvatar, wxStaticBitmap *targetAvatar);

	bool SetAvatar(CSteamID *id, wxStaticBitmap *map);

protected:
	virtual void Notify();
};


// Call Dialog Class
class CallDialog : public wxDialog {
private:
	// Timer
	AvatarTimer *avatarTimer;

	// We need information about a call ;)
	wxString callId;
	wxString fullIP;
	wxString serverName;
	wxString target;
	wxString targetId;
	wxString client;
	wxString clientId;
	wxString reason;
	wxString boxText;
	long reportedAt;

	// Layout
	wxSizer *sizerTop;

	// Avatars
	wxStaticBitmap *clientAvatar;
	wxStaticBitmap *targetAvatar;
	wxStaticText *doneText;

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
	CallDialog(const wxString &title);
	~CallDialog();

	// Convert to community ID
	static CSteamID SteamIdtoCSteamId(wxString steamId);

	// Privat -> Get Methods
	long GetTime() {
		return this->reportedAt;
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

	wxString GetBoxText() {
		return this->boxText;
	}

	AvatarTimer* GetAvatarTimer() {
		return this->avatarTimer;
	}

	wxButton* GetContactTrackersButton() {
		return this->contactTrackers;
	}

	wxButton* GetTakeoverButton() {
		return this->takeover;
	}

	CSteamID* GetClientCId() {
		return &this->clientCId;
	}

	CSteamID* GetTargetCId() {
		return &this->targetCId;
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
		this->serverName = name;
	}

	void SetTarget(wxString name) {
		this->target = name;
	}

	void SetTargetId(wxString steamidId) {
		this->targetId = steamidId;
		this->targetCId = SteamIdtoCSteamId(steamidId);
	}

	void SetClient(wxString name) {
		this->client = name;
	}

	void SetClientId(wxString steamidId) {
		this->clientId = steamidId;
		this->clientCId = SteamIdtoCSteamId(steamidId);
	}

	void SetReason(wxString name) {
		this->reason = name;
	}

	void SetId(int id) {
		this->id = id;
	}

	void SetTime(long time) {
		this->reportedAt = time;
	}

	void SetBoxText(wxString text) {
		this->boxText = text;
	}

	void SetHandled(bool handled) {
		this->isHandled = handled;
	}

	// Mark the call as finished
	void SetFinish();

	// Start the call
	void StartCall(bool show);

	// CURL Callbacks
	static void OnGetTrackers(char *errors, wxString result, int extra);
	static void OnChecked(char *error, wxString result, int extra);

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
	void OnMinimizeWindow(wxIconizeEvent &event);

	DECLARE_EVENT_TABLE()
};


#endif
