#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

/**
 * -----------------------------------------------------
 * File        main_frame.h
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

#pragma once

// Precomp Header
#include <wx/wxprec.h>

// We need WX
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "notebook.h"


// Event ID's for main frame
enum {
	wxID_ThreadHandled = wxID_HIGHEST + 50,
};


/**
 * Main Frame
 */
class MainFrame : public wxFrame {
private:
	Notebook *notebook;

public:
	MainFrame(const wxString &title);

	// Create the window
	void CreateWindow(bool createInTaskbar = false);

	// Notebook accessor
	Notebook* GetNotebook();

protected:
	void OnThread(wxCommandEvent &event);
	void OnCloseWindow(wxCloseEvent &event);
	void OnMinimizeWindow(wxIconizeEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
