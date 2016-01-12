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


// Constructor: Set Taskbar icon
TaskBarIcon::TaskBarIcon() {
	wxImage image;
	wxIcon icon;

	// Load icon from resource.cpp
	wxMemoryInputStream iconStream(&xml_res_file_2, xml_res_size_2);

	// Load data into image and convert to the image to an icon
	image.LoadFile(iconStream, wxBITMAP_TYPE_ICO);
	icon.CopyFromBitmap(image);

	SetIcon(icon, "CallAdmin Client");
}


// Show a information whether in the taskbar or as dialog on unix
void TaskBarIcon::ShowMessage(wxString title, wxString message, wxWindow *parent) {
	// Only if not other app is in fullscreen, otherwise it would be minimized
	if (!isOtherInFullscreen()) {
		#if defined(__WXMSW__) && defined(wxUSE_TASKBARICON_BALLOONS) && wxUSE_TASKBARICON_BALLOONS
			if (caGetConfig()->GetShowInTaskbar()) {
				// Show as balloon
				ShowBalloon(title, message, 15000, wxICON_INFORMATION);

				return;
			}
		#endif

		// No taskbar balloon available or disabled, so show as message dialog
		wxMessageBox(message, title, wxICON_INFORMATION | wxOK, parent);
	}
}


// Restore main frame
void TaskBarIcon::OnMenuRestore(wxCommandEvent &WXUNUSED(event)) {
	caGetMainFrame()->Restore();
	caGetMainFrame()->Show(true);
}


// On exit -> Exit whole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent &WXUNUSED(event)) {
	caGetApp().ExitProgramm();
}


// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent &WXUNUSED(event)) {
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

			caLogAction("Added CallAdmin to the auto start list");
		} else {
			// Remove it
			regKey.DeleteValue("CallAdmin-Client");

			caLogAction("Removed CallAdmin from the auto start list");
		}
	}
}
#endif


// On double left click -> open main frame
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent& WXUNUSED(event)) {
	caGetMainFrame()->Restore();
	caGetMainFrame()->Show(true);
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
