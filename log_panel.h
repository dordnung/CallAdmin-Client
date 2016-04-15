#ifndef LOG_PANEL_H
#define LOG_PANEL_H

/**
 * -----------------------------------------------------
 * File        log_panel.h
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

#define LOG_PANEL_PAGE 3
#define LOG_PANEL_LABEL "Logging"


enum LogPanelColumns {
	LogPanelColumn_Level,
	LogPanelColumn_Time,
	LogPanelColumn_Log
};

class LogPanel : public Panel, public wxScrolledWindow {
private:
	wxChoice *logLevel;
	wxListCtrl *logBox;

public:
	LogPanel() : logLevel(NULL), logBox(NULL) {};

	// Load controls
	bool InitPanel();

	int GetPage() {
		return LOG_PANEL_PAGE;
	}

	wxString GetLabel() {
		return LOG_PANEL_LABEL;
	}

	wxPanel *GetPanel() {
		return this;
	}

	// Add Action to the logBox
	void AddLog(wxString log, LogLevel logLevel);

protected:
	void OnSelectOrFocus(wxListEvent &event);
	void OnLogLevelUpdate(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif
