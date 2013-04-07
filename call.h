#ifndef CALL_H
#define CALL_H

/**
 * -----------------------------------------------------
 * File        call.h
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
#include <sstream>

// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// Steam Class
#include "opensteam.h"
#include "calladmin-client.h"


// Call Dialog Class
class CallDialog: public wxDialog
{
private:

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
	std::string reportedAt;

	// Layout
	wxSizer* sizerTop;

	// Avatars
	wxStaticBitmap* clientAvatar;
	wxStaticBitmap* targetAvatar;


	// And for the Steam API
	CSteamID clientCID;
	CSteamID targetCID;

	// Item List
	int ID;

public:
	CallDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX) {};

	// Tracker button
	wxButton* contactTrackers;

	// take Over Button
	wxButton* takeover;

	// Privat -> set Methods
	void setCallID(const char* ID) {callID = ID;}
	void setIP(const char* IP) {fullIP = IP;}
	void setName(wxString name) {serverName = name;}
	void setTarget(const char* name) {target = name;}
	void setTargetID(const char* steamidID) {targetID = steamidID; targetCID = steamIDtoCSteamID((char*)(steamidID));}
	void setClient(const char* name) {client = name;}
	void setClientID (const char* steamidID) {clientID = steamidID; clientCID = steamIDtoCSteamID((char*)(steamidID));}
	void setReason (const char* name) {reason = name;}
	void setID(int num) {ID = num;}
	void setTime(const char* time) {reportedAt = time;}
	void setBoxText(wxString text) {boxText = text;}



	// Convert to community ID
	CSteamID steamIDtoCSteamID(char* steamid);



	// Return time as a int
	inline int getTime() const 
	{
		int timestamp;
		std::stringstream timeS(reportedAt);

		timeS >> timestamp;

		return timestamp;
	}



	// Timers
	SecondTimer *avatarTimer;


	// Methods for Details
	wxString getTarget() const {return target;}
	wxString getClient() const {return client;}
	
	wxString getID() const {return callID;}
	wxString getServer() {return serverName;}
	wxString getBoxText() {return boxText;}

	CSteamID* getClientCID() {return &clientCID;}
	CSteamID* getTargetCID() {return &targetCID;}



	// Start the call
	void startCall(bool show);

	// Operator overloadings
	friend bool operator==(const CallDialog& x, const CallDialog& y) { return (x.getTime() == y.getTime() && (x.getID() == y.getID())); }
	friend bool operator!=(const CallDialog& x, const CallDialog& y) { return !(x == y);}

protected:
	// Button Events
	void OnConnect(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnContactClient(wxCommandEvent& event);
	void OnContactTarget(wxCommandEvent& event);
	void OnContactTrackers(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};



// CURL Callbacks
void onGetTrackers(char* errors, wxString result, int x);
void onChecked(char* error, wxString result, int x);



// Call Dialogs
extern CallDialog *call_dialogs[MAXCALLS];

#endif