/**
 * -----------------------------------------------------
 * File        curl_util.cpp
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
#include "curl_util.h"
#include "calladmin-client.h"

#include <curl/curl.h>
#include <sstream>

#ifdef __WXMSW__
// Memory leak detection for debugging 
#include <wx/msw/msvcrt.h>
#endif


CurlThread::~CurlThread() {
	if (GetThread()) {
		GetThread()->Delete();
	}

	// Wait until thread is finished
	while (true) {
		if (!GetThread()) {
			break;
		}

		wxThread::This()->Sleep(1);
	}
}


// Curl Thread started
// TODO: LOCK fuer app ende
wxThread::ExitCode CurlThread::Entry() {
	// Maybe it's already killed?
	if (!GetThread()->TestDestroy()) {
		// CurlThreadData
		CurlThreadData *data;

		// Response
		std::ostringstream stream;

		// Init Curl
		CURL *curl = curl_easy_init();

		if (curl != NULL) {
			// Error
			char ebuf[CURL_ERROR_SIZE];

			// Configurate Curl
			curl_easy_setopt(curl, CURLOPT_URL, this->page.mb_str().data());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ebuf);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, caGetConfig()->GetTimeout() * 2);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, caGetConfig()->GetTimeout());
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);

			// Perform Curl
			CURLcode res = curl_easy_perform(curl);

			// Everything good :)
			if (res == CURLE_OK) {
				long responseCode;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

				if (responseCode != 200) {
					data = new CurlThreadData(this->callbackFunction, stream.str(), "Invalid Response Code: " + wxString() << responseCode, this->extra);
				} else {
					data = new CurlThreadData(this->callbackFunction, stream.str(), "", this->extra);
				}
			} else {
				// Error ):
				data = new CurlThreadData(this->callbackFunction, stream.str(), ebuf, this->extra);
			}

			// Clean Curl
			curl_easy_cleanup(curl);

			// Add Event Handler
			caGetMainFrame()->GetEventHandler()->CallAfter(&CallAdmin::OnCurlThread, data);

			return (wxThread::ExitCode)0;
		}

		data = new CurlThreadData(this->callbackFunction, "", "", this->extra);

		// Call event
		caGetMainFrame()->GetEventHandler()->CallAfter(&CallAdmin::OnCurlThread, data);
	}

	return (wxThread::ExitCode)0;
}


// Curl receive data -> write to buffer
size_t CurlWriteData(void *buffer, size_t size, size_t nmemb, void *userp) {
	std::ostringstream *data = (std::ostringstream*) userp;

	if (buffer != NULL) {
		size_t count = size * nmemb;

		data->write((char*)buffer, count);

		return count;
	}

	return (size_t)-1;
}