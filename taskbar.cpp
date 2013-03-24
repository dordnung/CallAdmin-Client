/**
 * -----------------------------------------------------
 * File        taskbar.cpp
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


// Includes Project
#include "taskbar.h"
#include "main.h"
#include "error.h"
#include "config.h"
#include "calladmin-client.h"


// Wx
#include <wx/stdpaths.h>

// Global icon
TaskBarIcon *m_taskBarIcon = NULL;


// Taskbar ID's
enum
{
    PU_RESTORE = 10001,
    PU_UPDATE,
	PU_AUTOSTART,
    PU_EXIT,
};



// Taskbar Events
BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_UPDATE, TaskBarIcon::OnMenuUpdate)
	EVT_MENU(PU_AUTOSTART, TaskBarIcon::OnMenuAutoStart)
    EVT_MENU(PU_EXIT, TaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_DCLICK (TaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()


// Restore all dialogs
void TaskBarIcon::OnMenuRestore(wxCommandEvent&)
{
	if (main_dialog != NULL)
	{
		main_dialog->Show(true);
	}

	if (error_dialog != NULL)
	{
		error_dialog->Show(true);
	}

	if (config_dialog != NULL)
	{
		config_dialog->Show(true);
	}
}

// On exit -> Exit hole programm
void TaskBarIcon::OnMenuExit(wxCommandEvent&)
{
	exitProgramm();
}

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
        }
        else
        {
			// Remove it
			RegDeleteValueA(hkRegistry, "CallAdmin-Client");
        }

		// Close Key
        RegCloseKey(hkRegistry);
    }
}

// Check for update
void TaskBarIcon::OnMenuUpdate(wxCommandEvent&)
{
	checkUpdate();
}



// Create the Taskbar Menu
wxMenu *TaskBarIcon::CreatePopupMenu()
{
	bool autoRun = false;

    wxMenu *menu = new wxMenu();

    menu->Append(PU_RESTORE, "Restore Windows");
    menu->AppendSeparator();


	// Check Auto run Key
	HKEY hkRegistry;

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

	menu->Append(PU_UPDATE, "Check For Update");
	menu->Append(PU_AUTOSTART, "Start With Windows", wxEmptyString, wxITEM_CHECK)->Check(autoRun);
    menu->AppendSeparator();

	menu->Append(PU_EXIT, "Exit");

    return menu;
}


// On doppel left click -> open Menu
void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if (main_dialog != NULL)
	{
		main_dialog->Show(true);
	}

	if (error_dialog != NULL)
	{
		error_dialog->Show(true);
	}

	if (config_dialog != NULL)
	{
		config_dialog->Show(true);
	}
}
