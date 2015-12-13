#ifndef TRACKERS_H
#define TRACKERS_H

/**
* -----------------------------------------------------
* File        trackers_panel.h
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

#include <wx/listctrl.h>
#include <wx/notebook.h>



// Tracker Panel Class
class TrackerPanel: public wxPanel
{
private:
	wxListBox* trackerBox;

public:
	TrackerPanel(wxNotebook* note);

	void newTracker(wxString text) {trackerBox->Append(wxString::FromUTF8(text));}
	void delTrackers() {trackerBox->Clear();}

protected:
	void OnExit(wxCommandEvent& event);
	void OnUpdate(wxCommandEvent& event);
	void OnHide(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};


// Refresh the tracker list
void refreshTrackers(char* error, wxString result, int x);
void addTracker(wxString text);


#endif