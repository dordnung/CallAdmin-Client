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

// unused warning -> disable
#pragma warning(disable: 4100)

// Includes Project
#include "taskbar.h"
#include "calladmin-client.h"

// wx
#include <wx/stdpaths.h>


// Taskbar ID's
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
	caGetApp().ExitProgramm();
}

// TODO: Wxstring regkey benutzen
#if defined(__WXMSW__)
// Append/Remove to Autostart
void TaskBarIcon::OnMenuAutoStart(wxCommandEvent &event) {
	// Registry Key
	HKEY hkRegistry;

	// Open Key
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_WRITE, &hkRegistry) == ERROR_SUCCESS) {
		// He checked it
		if (event.IsChecked()) {
			// Get App Path
			wxString appPath = wxStandardPaths::Get().GetExecutablePath();

			// Close to Taskbar on Autostart
			appPath = "\"" + appPath + "\"" + " -taskbar";

			// Write in
			RegSetValueExW(hkRegistry, L"CallAdmin-Client", 0, REG_SZ, (BYTE*)appPath.wc_str(), (wcslen(appPath.wc_str()) + 1) * sizeof(wchar_t));

			caLogAction("Added Call Admin to the auto start list");
		} else {
			// Remove it
			RegDeleteValueA(hkRegistry, "CallAdmin-Client");

			caLogAction("Removed Call Admin from the auto start list");
		}

		// Close Key
		RegCloseKey(hkRegistry);
	}
}
#endif


// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent& WXUNUSED(event)) {
	caGetApp().CheckUpdate();
}


// Shows a Message
void TaskBarIcon::ShowMessage(wxString title, wxString message, wxWindow *parent) {
#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
	ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else
	// Not the taskbar message
	if (message != "Call Admin is now in the taskbar!") {
		wxMessageBox(message, title, wxICON_INFORMATION | wxOK, parent);
	}
#endif
}


// On doppel left click -> open Menu
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent& WXUNUSED(event)) {
	caGetMainFrame()->Show(true);
	caGetMainFrame()->Restore();
}


// Create the Taskbar Menu
// TODO Menu deconstructor?
wxMenu* TaskBarIcon::CreatePopupMenu() {
	wxMenu *menu = new wxMenu();

#if defined(__WXMSW__)
	// Check Auto run Key
	HKEY hkRegistry;
	bool autoRun = false;

	// Open it
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_QUERY_VALUE, &hkRegistry) == ERROR_SUCCESS) {
		// Path in the registry
		wchar_t wszPath[1024];
		memset(wszPath, 0, sizeof(wszPath));

		DWORD dwType, dwSize = sizeof(wszPath)-1;

		// Look for calladmin
		if (RegQueryValueExW(hkRegistry, L"CallAdmin-Client", 0, &dwType, (unsigned char *)wszPath, &dwSize) == ERROR_SUCCESS) {
			// Is Path the same?
			if (wxString(wszPath).IsSameAs(wxStandardPaths::Get().GetExecutablePath())) {
				// So it's on
				autoRun = true;
			}
		}

		// Close the Key
		RegCloseKey(hkRegistry);
	}
#endif

	menu->Append(PU_RESTORE, "Restore Windows");
	menu->AppendSeparator();
	menu->Append(PU_UPDATE, "Check For Update");
#if defined(__WXMSW__)
	menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK)->Check(autoRun);
#endif
	menu->AppendSeparator();
	menu->Append(PU_EXIT, "Exit");

	return menu;
}
