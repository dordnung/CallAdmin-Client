/**
 * -----------------------------------------------------
 * File        taskbar.h
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
#include <wx/taskbar.h>



// Taskbar Class
class TaskBarIcon : public wxTaskBarIcon
{
public:
	TaskBarIcon() {}

	// Taskbar Events
	void OnLeftButtonDClick(wxTaskBarIconEvent&);
	void OnMenuRestore(wxCommandEvent&);
	void OnMenuExit(wxCommandEvent&);
	void OnMenuUpdate(wxCommandEvent&);
	void OnMenuAutoStart(wxCommandEvent&);

	virtual wxMenu *CreatePopupMenu();

	DECLARE_EVENT_TABLE()
};


// Taskbar
extern TaskBarIcon *m_taskBarIcon;
