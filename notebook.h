#ifndef NOTEBOOK_H
#define NOTEBOOK_H

/**
 * -----------------------------------------------------
 * File        notebook.h
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

#include <wx/notebook.h>

#include "main_panel.h"
#include "config_panel.h"
#include "trackers_panel.h"
#include "log_panel.h"
#include "about_panel.h"


/**
 * Notebook with its panels
 */
class Notebook : public wxNotebook {
private:
	MainPanel *mainPanel;
	ConfigPanel *configPanel;
	TrackerPanel *trackersPanel;
	LogPanel *logPanel;
	AboutPanel *aboutPanel;

public:
	Notebook();

	void CreateAndAddPages();

	// Panel accessors
	MainPanel* GetMainPanel() {
		return this->mainPanel;
	}

	ConfigPanel* GetConfigPanel() {
		return this->configPanel;
	}

	TrackerPanel* GetTrackersPanel() {
		return this->trackersPanel;
	}

	LogPanel* GetLogPanel() {
		return this->logPanel;
	}

	AboutPanel* GetAboutPanel() {
		return this->aboutPanel;
	}

protected:
	void OnCloseWindow(wxCloseEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
