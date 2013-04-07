#ifndef TASKBAR_H
#define TASKBAR_H

/**
 * -----------------------------------------------------
 * File        taskbar.h
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
#include <wx/taskbar.h>



// Taskbar Class
class TaskBarIcon : public wxTaskBarIcon
{
public:
	TaskBarIcon();

	// Taskbar Events
	void OnLeftButtonDClick(wxTaskBarIconEvent&);
	void OnMenuRestore(wxCommandEvent&);
	void OnMenuExit(wxCommandEvent&);
	void OnMenuUpdate(wxCommandEvent&);

	#if defined(__WXMSW__)
		void OnMenuAutoStart(wxCommandEvent&);
	#endif

	// Show a information
	void ShowMessage(wxString title, wxString message, wxWindow* parent);

	virtual wxMenu *CreatePopupMenu();

	DECLARE_EVENT_TABLE()
};


// Taskbar
extern TaskBarIcon *m_taskBarIcon;

#endif
