#ifndef CONFIG_H
#define CONFIG_H

/**
 * -----------------------------------------------------
 * File        config.h
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

// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <wx/config.h>


// Settings
extern int step;
extern int timeout;
extern int maxAttempts;
extern int lastCalls;

extern wxString page;
extern wxString key;

extern bool steamEnabled;


// Config
extern wxConfig *g_config;


// Config Dialog Class
class ConfigPanel: public wxPanel
{
private:
	wxSpinCtrl* stepSlider;
	wxSpinCtrl* timeoutSlider;
	wxSpinCtrl* attemptsSlider;
	wxSpinCtrl* callsSlider;
	wxTextCtrl* pageText;
	wxTextCtrl* keyText;
	wxCheckBox* steamEnable;

public:
	ConfigPanel(wxNotebook* note);

protected:
	void OnSet(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnHide(wxCommandEvent& event);

	void OnCheckBox(wxCommandEvent& event);

	void parseConfig();

	DECLARE_EVENT_TABLE()
};


#endif
