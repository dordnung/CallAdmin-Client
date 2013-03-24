/**
 * -----------------------------------------------------
 * File        main.h
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


// Wee need WX
#include <wx/wx.h>
#include <wx/listbox.h>



// Main Dialog Class
class MainDialog: public wxDialog
{
private:
	wxCheckBox* store;
	wxCheckBox* available;
	wxCheckBox* sound;

	wxButton* reconnectButton;

	wxListBox* callBox;
	wxSizer* sizerBody;

	wxStaticText* eventText;
	wxStaticText* steamText;

public:
	MainDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX) {}

	// Create the window
	void createWindow(bool taskbar = false);

	// Check checkboxes
	bool isAvailable() {return (available != NULL && available->IsChecked());}
	bool wantStore() {return (store != NULL && !store->IsChecked());}
	bool wantSound() {return (sound != NULL && sound->IsChecked());}

	// Update Window
	void setEventText(wxString text) {eventText->SetLabelText(text); SetSizerAndFit(sizerBody, false);}
	void setReconnectButton(bool enable=false) {reconnectButton->Enable(enable);}

	void setSteamStatus(wxString text, wxString color) {steamText->SetLabelText(text); steamText->SetForegroundColour(wxColor(color)); SetSizerAndFit(sizerBody, false);}
	void setSteamStatus(wxString text, wxColor color) {steamText->SetLabelText(text); steamText->SetForegroundColour(color); SetSizerAndFit(sizerBody, false);}

	// Update Call list
	void updateCall();

protected:
	// Button Events
	void OnExit(wxCommandEvent& event);
	void OnHide(wxCommandEvent& event);
	void OnReconnect(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	void OnBoxClick(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};


// Main Dialog
extern MainDialog *main_dialog;
