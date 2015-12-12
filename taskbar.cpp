/**
 * -----------------------------------------------------
 * File        taskbar.cpp
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


// Includes Project
#include "taskbar.h"
#include "mainframe.h"
#include "log.h"
#include "config.h"
#include "calladmin-client.h"


// Wx
#include <wx/stdpaths.h>

// Global icon
TaskBarIcon *m_taskBarIcon = NULL;


// unused warning -> disable
#pragma warning(disable: 4100)


// Taskbar ID's
enum
{
	PU_RESTORE = wxID_HIGHEST+600,
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
	EVT_TASKBAR_LEFT_DCLICK (TaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()



// Set Icon of Taskbar
TaskBarIcon::TaskBarIcon()
{
	#if defined(__WXMSW__)
		SetIcon(wxIcon("calladmin_icon", wxBITMAP_TYPE_ICO_RESOURCE), "Call Admin Client");
	#else
		wxLogNull nolog;

		SetIcon(wxIcon(getAppPath("resources/calladmin_icon.ico"), wxBITMAP_TYPE_ICON), "Call Admin Client");
	#endif
}


// Restore main dialog
void TaskBarIcon::OnMenuRestore(wxCommandEvent&)
{
	if (mainFrame != NULL)
	{
		mainFrame->Show(true);
	}
}

// On exit -> Exit hole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent&)
{
	exitProgramm();
}


#if defined(__WXMSW__)
	// Append/Remove to Autostart
	void TaskBarIcon::OnMenuAutoStart(wxCommandEvent& event)
	{
		// Registry Key
		HKEY hkRegistry;

		// Open Key
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_WRITE, &hkRegistry) == ERROR_SUCCESS)
		{
			// He checked it
			if (event.IsChecked())
			{
				// Get App Path
				wxString appPath = wxStandardPaths::Get().GetExecutablePath();

				// Close to Taskbar on Autostart
				appPath = "\"" + appPath + "\"" + " -taskbar";

				// Write in
				RegSetValueExW(hkRegistry, L"CallAdmin-Client", 0, REG_SZ, (BYTE*)appPath.wc_str(), (wcslen(appPath.wc_str()) + 1) * sizeof(wchar_t));

				LogAction("Added Call Admin to the auto start list");
			}
			else
			{
				// Remove it
				RegDeleteValueA(hkRegistry, "CallAdmin-Client");

				LogAction("Removed Call Admin from the auto start list");
			}

			// Close Key
			RegCloseKey(hkRegistry);
		}
	}
#endif


// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent&)
{
	checkUpdate();
}


// Shows a Message
void TaskBarIcon::ShowMessage(wxString title, wxString message, wxWindow* parent)
{
	#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
		m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
	#else
		// Not the taskbar message
		if (message != "Call Admin is now in the taskbar!")
		{
			wxMessageBox(message, title, wxICON_INFORMATION|wxOK, parent);
		}
	#endif
}



// Create the Taskbar Menu
wxMenu *TaskBarIcon::CreatePopupMenu()
{
	wxMenu *menu = new wxMenu();

	menu->Append(PU_RESTORE, "Restore Windows");
	menu->AppendSeparator();

	#if defined(__WXMSW__)
		// Check Auto run Key
		HKEY hkRegistry;
		bool autoRun = false;

		// Open it
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_QUERY_VALUE, &hkRegistry) == ERROR_SUCCESS)
		{
			// Path in the registry
			wchar_t wszPath[4096];
			memset(wszPath, 0, sizeof(wszPath));

			DWORD dwType, dwSize = sizeof(wszPath) - 1;

			// Look for calladmin
			if (RegQueryValueExW(hkRegistry, L"CallAdmin-Client", 0, &dwType, (unsigned char *)wszPath, &dwSize) == ERROR_SUCCESS)
			{
				// Is Path the same?
				if (wxString(wszPath).Contains(wxStandardPaths::Get().GetExecutablePath()))
				{
					// So it's on
					autoRun = true;
				}
			}

			// Close the Key
			RegCloseKey(hkRegistry);
		}
	#endif

	menu->Append(PU_UPDATE, "Check For Update");

	#if defined(__WXMSW__)
		menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK)->Check(autoRun);
	#endif

	menu->AppendSeparator();

	menu->Append(PU_EXIT, "Exit");

	return menu;
}


// On doppel left click -> open Menu
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if (mainFrame != NULL)
	{
		mainFrame->Show(true);
		mainFrame->Restore();
	}
}
