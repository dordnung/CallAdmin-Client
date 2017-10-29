#ifndef LOG_H
#define LOG_H

/**
 * -----------------------------------------------------
 * File        log.h
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


// Precomp Header
#include <wx/wxprec.h>


// We need WX
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <wx/notebook.h>



// Log Panel Class
class LogPanel: public wxPanel
{
private:
	wxListBox* logBox;

public:
	LogPanel(wxNotebook* note);

	void addLog(wxString log);
};


// Log a Action
void LogAction(wxString action);


#endif
