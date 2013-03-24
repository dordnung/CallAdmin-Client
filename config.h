/**
 * -----------------------------------------------------
 * File        config.h
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
#include <wx/spinctrl.h>



// Settings
extern int step;
extern int timeout;
extern int maxAttempts;

extern std::string page;
extern std::string key;


// Config Dialog Class
class ConfigDialog: public wxDialog
{
private:
	wxSpinCtrl* stepSlider;
	wxSpinCtrl* timeoutSlider;
	wxSpinCtrl* attemptsSlider;
	wxTextCtrl* pageText;
	wxTextCtrl* keyText;

public:
	ConfigDialog(const wxString& title);

protected:
	void OnSet(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};


// Parse Config
bool parseConfig();

// Config Dialog
extern ConfigDialog *config_dialog;
