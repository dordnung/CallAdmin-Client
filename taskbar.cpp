/**
 * -----------------------------------------------------
 * File        taskbar.cpp
 * Authors     David O., Impact
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
#include <wx/mstream.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


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
wxBEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
	EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_EXIT, TaskBarIcon::OnMenuExit)
	EVT_MENU(PU_UPDATE, TaskBarIcon::OnMenuUpdate)
	#if defined(__WXMSW__)
		EVT_MENU(PU_AUTOSTART, TaskBarIcon::OnMenuAutoStart)
	#endif

	EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::OnLeftButtonDClick)
wxEND_EVENT_TABLE()


// Add the icon
void TaskBarIcon::AddIcon() {
	if (!IsIconInstalled()) {
		SetIcon(caGetMainFrame()->GetIcon(), "CallAdmin Client");
	}
}


// Show a information whether in the taskbar or as dialog if taskbar messages are not available
void TaskBarIcon::ShowMessage(wxString title, wxString message, wxWindow *parent, bool isError) {
	caLogAction("[" + title + "] " + message, isError ? LogLevel::LEVEL_ERROR : LogLevel::LEVEL_INFO);

	// Only if not other app is in fullscreen, otherwise it would be minimized
	if (!isOtherInFullscreen()) {
		bool wasShown = false;

		wxNotificationMessage notificationMessage(title, message, parent, isError ? wxICON_ERROR : wxICON_INFORMATION);

		// Use notification or just use a window?
		if (caGetConfig()->GetShowInTaskbar()) {
			wasShown = notificationMessage.Show();
		}

		// No taskbar balloon available or disabled, so show as message box
		wxMessageBox(message, title, wxOK | wxCENTRE | (isError ? wxICON_ERROR : wxICON_INFORMATION), parent);
	}
}


// Restore main frame
void TaskBarIcon::OnMenuRestore(wxCommandEvent &WXUNUSED(event)) {
	caLogAction("Taskbar restore event fired", LogLevel::LEVEL_DEBUG);
	caGetMainFrame()->Show(true);
	caGetMainFrame()->Raise();
}


// On exit -> Exit whole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent &WXUNUSED(event)) {
	caLogAction("Taskbar close event fired", LogLevel::LEVEL_DEBUG);
	caGetApp().ExitProgramm();
}


// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent &WXUNUSED(event)) {
	caLogAction("Taskbar update event fired", LogLevel::LEVEL_DEBUG);
	caGetApp().CheckUpdate();
}


#if defined(__WXMSW__)
// Append/Remove from Autostart
void TaskBarIcon::OnMenuAutoStart(wxCommandEvent &event) {
	// Autostart Registry Key
	wxRegKey regKey(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\");

	// Check key exists
	if (regKey.Exists()) {
		// User want autostart
		if (event.IsChecked()) {
			// Get App Path
			wxString appPath = wxStandardPaths::Get().GetExecutablePath();

			// Close to Taskbar on Autostart
			appPath = "\"" + appPath + "\"" + " -taskbar";

			// Write in
			regKey.SetValue("CallAdmin-Client", appPath);

			caLogAction("Added CallAdmin to the autostart list", LogLevel::LEVEL_INFO);
		} else {
			// Remove it
			regKey.DeleteValue("CallAdmin-Client");

			caLogAction("Removed CallAdmin from the autostart list", LogLevel::LEVEL_INFO);
		}
	} else {
		ShowMessage("Couldn't set autostart key", "Registry key HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\ isn't available", caGetMainFrame(), true);
	}
}
#endif


// On double left click -> open main frame
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent& WXUNUSED(event)) {
	caLogAction("Taskbar double click event fired", LogLevel::LEVEL_DEBUG);
	caGetMainFrame()->Show(true);
	caGetMainFrame()->Iconize(false);
	caGetMainFrame()->Raise();
}


// Create the Taskbar Menu
wxMenu* TaskBarIcon::CreatePopupMenu() {
	wxMenu *menu = new wxMenu();

	menu->Append(PU_RESTORE, "Restore CallAdmin");
	menu->AppendSeparator();
	menu->Append(PU_UPDATE, "Check For Update");

	#if defined(__WXMSW__)
		wxMenuItem *autostartItem = menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK);

		// Do not check the autostart item by default
		autostartItem->Check(false);

		// Check Autorun Key
		wxRegKey regKey(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\");

		// Check if it exists and has CallAdmin value
		if (regKey.Exists() && regKey.HasValue("CallAdmin-Client")) {
			// String to store value in
			wxString value;

			// Look for CallAdmin-Client value
			if (regKey.QueryValue("CallAdmin-Client", value)) {
				// Is Path the same?
				if (value.Contains(wxStandardPaths::Get().GetExecutablePath())) {
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
