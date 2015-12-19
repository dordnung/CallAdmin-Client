/**
 * -----------------------------------------------------
 * File        taskbar.cpp
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

#include "taskbar.h"
#include "calladmin-client.h"

#include <wx/stdpaths.h>


// Taskbar event ID's
enum {
	PU_RESTORE = wxID_HIGHEST + 1,
	PU_UPDATE,
#if defined(__WXMSW__)
	PU_AUTOSTART,
#endif
	PU_EXIT,
};


// Taskbar Events
BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
EVT_MENU(PU_UPDATE, TaskBarIcon::OnMenuUpdate)
#if defined(__WXMSW__)
EVT_MENU(PU_AUTOSTART, TaskBarIcon::OnMenuAutoStart)
#endif
EVT_MENU(PU_EXIT, TaskBarIcon::OnMenuExit)
EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()


// Constructor: Set Taskbar icon
TaskBarIcon::TaskBarIcon() {
#if defined(__WXMSW__)
	SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE), "Call Admin Client");
#else
	wxLogNull nolog;

	SetIcon(wxIcon(caGetApp().GetAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON), "Call Admin Client");
#endif
}


// Restore main dialog
void TaskBarIcon::OnMenuRestore(wxCommandEvent& WXUNUSED(event)) {
	caGetMainFrame()->Show(true);
}


// On exit -> Exit whole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent& WXUNUSED(event)) {
	caLogAction("Closed via taskbar");
	caGetApp().ExitProgramm();
}


#if defined(__WXMSW__)
// Append/Remove to Autostart
void TaskBarIcon::OnMenuAutoStart(wxCommandEvent &event) {
	// Registry Key
	wxRegKey regKey(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\");

	// Check key exists
	if (regKey.Exists()) {
		// He checked it
		if (event.IsChecked()) {
			// Get App Path
			wxString appPath = wxStandardPaths::Get().GetExecutablePath();

			// Close to Taskbar on Autostart
			appPath = "\"" + appPath + "\"" + " -taskbar";

			// Write in
			regKey.SetValue("CallAdmin-Client", appPath);

			caLogAction("Added Call Admin to the auto start list");
		} else {
			// Remove it
			regKey.DeleteValue("CallAdmin-Client");

			caLogAction("Removed Call Admin from the auto start list");
		}
	}
}
#endif


// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent& WXUNUSED(event)) {
	caGetApp().CheckUpdate();
}


// Shows a Message
// TODO check message
void TaskBarIcon::ShowMessage(wxString title, wxString message, wxWindow *
#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
							  WXUNUSED(parent)) {
	ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else
							  parent) {
	// Not the taskbar message
	if (message != "Call Admin is now in the taskbar!") {
		wxMessageBox(message, title, wxICON_INFORMATION | wxOK, parent);
	}
#endif
}


// On doppel left click -> open Menu
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent& WXUNUSED(event)) {
	caGetMainFrame()->Show(true);
}


// Create the Taskbar Menu
wxMenu* TaskBarIcon::CreatePopupMenu() {
	wxMenu *menu = new wxMenu();

	menu->Append(PU_RESTORE, "Restore Windows");
	menu->AppendSeparator();
	menu->Append(PU_UPDATE, "Check For Update");

#if defined(__WXMSW__)
	wxMenuItem *autostartItem = menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK);

	// Do not check the autostart item by default
	autostartItem->Check(false);

	// Check Auto run Key
	wxRegKey regKey(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\");

	// Check if it exists
	if (regKey.Exists()) {
		// String to store value in
		wxString value;

		// Look for CallAdmin-Client
		if (regKey.QueryValue("CallAdmin-Client", value)) {
			// Is Path the same?
			if (value.IsSameAs(wxStandardPaths::Get().GetExecutablePath())) {
				// So it's on
				autostartItem->Check(true);
			}
		}
	}
#endif

	menu->AppendSeparator();
	menu->Append(PU_EXIT, "Exit");

	// Menu will be automatically deleted by wxTaskBarIconBase
	return menu;
}
