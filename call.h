/**
 * -----------------------------------------------------
 * File        call.h
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

#pragma once


// c++ libs
#include <string>
#include <sstream>

// wx
#include <wx/wx.h>

// Steam Class
#include "opensteam.h"



// Call Dialog Class
class CallDialog: public wxDialog
{
private:

	// We need information about a call ;)
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


	// take Over Button
	wxButton* takeover;

	// And for the Steam API
	CSteamID* clientCID;
	CSteamID* targetCID;

	// Item List
	int ID;

public:
	CallDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX) {};

	// Privat -> set Methods
	void setIP(const char* IP) {fullIP = IP;}
	void setName(const char* name) {serverName = name;}
	void setTarget(const char* name) {target = name;}
	void setTargetID(const char* steamidID) {targetID = steamidID; targetCID = steamIDtoCSteamID((char*)(steamidID));}
	void setClient(const char* name) {client = name;}
	void setClientID (const char* steamidID) {clientID = steamidID; clientCID = steamIDtoCSteamID((char*)(steamidID));}
	void setReason (const char* name) {reason = name;}
	void setID(int num) {ID = num;}
	void setTime(const char* time) {reportedAt = time;}
	void setBoxText(const char* text) {boxText = text;}

	// Take over the job
	void setTakeOver(bool enable = true) {takeover->Enable(enable);}

	// Convert to community ID
	CSteamID* steamIDtoCSteamID(char* steamid);

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

	// Methids for Details
	const char* getTargetID() const {return targetID.c_str();}
	const char* getClientID() const {return clientID.c_str();}
	const char* getServer() {return serverName.c_str();}
	const char* getBoxText() {return boxText.c_str();}

	CSteamID* getClientCID() {return clientCID;}
	CSteamID* getTargetCID() {return targetCID;}


	// Start the call
	void startCall();

	// Operator overloadings
	friend bool operator==(const CallDialog& x, const CallDialog& y) { return (x.getTime() == y.getTime() && (std::string)x.getClientID() == (std::string)y.getClientID() && (std::string)y.getTargetID() == (std::string)y.getTargetID()); }
	friend bool operator!=(const CallDialog& x, const CallDialog& y) { return !(x == y);}

protected:
	// Button Events
	void OnConnect(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnContactClient(wxCommandEvent& event);
	void OnContactTarget(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};


// Call Dialogs
extern CallDialog *call_dialogs[100];
