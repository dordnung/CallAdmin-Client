#ifndef CURL_UTIL_H
#define CURL_UTIL_H

/**
 * -----------------------------------------------------
 * File        curl_util.h
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


// Thread for Curl Performances
typedef void(*CurlCallback)(wxString, wxString, int);


class CurlThread : public wxThreadHelper {
private:
	// Callback function
	CurlCallback callbackFunction;

	// Page
	wxString page;

	// Optional Parameter
	int extra;

protected:
	virtual wxThread::ExitCode Entry();

public:
	CurlThread() : callbackFunction(NULL), page(wxString()), extra(0) {}
	~CurlThread();

	void SetCallbackFunction(CurlCallback callbackFunction) {
		this->callbackFunction = callbackFunction;
	}

	void SetPage(wxString page) {
		this->page = page;
	}

	void SetExtra(int extra) {
		this->extra = extra;
	}
};

size_t CurlWriteData(void *buffer, size_t size, size_t nmemb, void *userp);


// Client Data for CurlThread
class CurlThreadData : public wxClientData {
private:
	// Callback function
	CurlCallback callbackFunction;

	// Callback function
	wxString content;

	// Error
	wxString error;

	// Optional Parameter
	int extra;

public:
	CurlThreadData(CurlCallback callbackFunction, wxString content, wxString error, int extra)
		: callbackFunction(callbackFunction), content(content), error(error), extra(extra) {}

	CurlCallback GetCallbackFunction() {
		return this->callbackFunction;
	}

	wxString GetContent() {
		return this->content;
	}

	wxString GetError() {
		return this->error;
	}

	int GetExtra() {
		return this->extra;
	}
};

#endif
