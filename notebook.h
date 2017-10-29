#ifndef NOTEBOOK_H
#define NOTEBOOK_H

/**
 * -----------------------------------------------------
 * File        notebook.h
 * Authors     David Ordnung, Impact
 * License     GPLv3
 * Web         http://dordnung.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung, Impact
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
#include "panel.h"


/**
 * Notebook with its panels
 */
class Notebook {
private:
	wxNotebook *window;
	wxVector<Panel *> panels;

public:
	explicit Notebook(wxNotebook *window) : window(window) {};
	~Notebook();

	// Adds a page to the notebook
	void AddPage(Panel *panel) {
		this->panels.push_back(panel);
	}

	// Create the pages of the notebook
	bool CreatePages();

	// Notebook window accessor
	wxNotebook* GetWindow() {
		return this->window;
	}

	template<typename T>
	T GetPanel(size_t page) {
		return static_cast<T>(panels.at(page));
	}
};

#endif
