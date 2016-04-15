#ifndef CALL_PANEL_H
#define CALL_PANEL_H

/**
 * -----------------------------------------------------
 * File        call_panel.h
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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "config.h"
#include "panel.h"
#include <wx/listctrl.h>

#define CALL_PANEL_PAGE 1
#define CALL_PANEL_LABEL "Calls"


enum CallPanelColumns {
	CallPanelColumn_Time,
	CallPanelColumn_Server,
	CallPanelColumn_Finished
};

class CallPanel : public Panel, public wxScrolledWindow {
private:
	wxListCtrl *callBox;

	// Default column header widths
	wxVector<int> columnHeaderWidths;

public:
	CallPanel() : callBox(NULL) {};

	// Load controls
	bool InitPanel();

	int GetPage() {
		return CALL_PANEL_PAGE;
	}

	wxString GetLabel() {
		return CALL_PANEL_LABEL;
	}

	wxPanel *GetPanel() {
		return this;
	}

	// Update Call list
	void UpdateCalls();

	// Append call to the callbox
	void AppendCall(bool finished, wxString time, wxString server);

	void SetHandled(int item);

protected:
	void OnBoxClick(wxListEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif
