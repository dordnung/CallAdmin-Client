#ifndef TASKBAR_H
#define TASKBAR_H

/**
 * -----------------------------------------------------
 * File        taskbar.h
 * Authors     David David O., Impact
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

#include <wx/taskbar.h>


// Taskbar Icon for Windows
class TaskBarIcon : public wxTaskBarIcon {
public:
	TaskBarIcon();

	// Show a information whether in the taskbar or as dialog if taskbar messages are not available
	void ShowMessage(wxString title, wxString message, wxWindow *parent = NULL);

protected:
	// Taskbar Events
	void OnMenuRestore(wxCommandEvent &event);
	void OnMenuExit(wxCommandEvent &event);
	void OnMenuUpdate(wxCommandEvent &event);
	#if defined(__WXMSW__)
		// Autostart only for Windows
		void OnMenuAutoStart(wxCommandEvent &event);
	#endif

	void OnLeftButtonDClick(wxTaskBarIconEvent &event);

	// Override CreatePopupMenu
	virtual wxMenu *CreatePopupMenu();

	wxDECLARE_EVENT_TABLE();
};

#endif
