/**
 * -----------------------------------------------------
 * File        curl_util.cpp
 * Authors     David O., Impact
 * License     GPLv3
 * Web         http://popoklopsi.de, http://gugyclan.eu
 * -----------------------------------------------------
 *
 * Copyright (C) 2013 David O., Impact
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

// Include Project
#include "curl_util.h"
#include "calladmin-client.h"

// c++ libs
#include <sstream>

// Curl
#include <curl/curl.h>


// Create and Start
CurlThread::CurlThread(CurlCallback callbackFunction, wxString page, int extra) {
	this->callbackFunction = callbackFunction;
	this->page = page;
	this->extra = extra;
}


// Curl Thread started
wxThread::ExitCode CurlThread::Entry() {
	if (!TestDestroy()) {
		// Event
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_ThreadHandled);

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
				event.SetClientObject(new CurlThreadData(this->callbackFunction, stream.str(), "", this->extra));
			} else {
				// Error ):
				event.SetClientObject(new CurlThreadData(this->callbackFunction, stream.str(), ebuf, this->extra));
			}

			// Clean Curl
			curl_easy_cleanup(curl);

			// Add Event Handler
			caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);

			return (wxThread::ExitCode)0;
		}

		event.SetClientObject(new CurlThreadData(this->callbackFunction, "", "", this->extra));

		// Add Event Handler
		caGetMainFrame()->GetEventHandler()->AddPendingEvent(event);
	}

	return (wxThread::ExitCode)0;
}


// Get Page
void CurlThread::GetPage(CurlCallback callbackFunction, wxString page, int extra) {
	CurlThread *curlThread = new CurlThread(callbackFunction, page, extra);

	curlThread->Create();
	curlThread->Run();
}


// Curl receive data -> write to buffer
size_t CurlWriteData(void *buffer, size_t size, size_t nmemb, void *userp) {
	std::ostringstream *data = (std::ostringstream*) userp;

	if (data != NULL) {
		size_t count = size * nmemb;

		data->write((char*)buffer, count);

		return count;
	}

	return (size_t)-1;
}


CurlThreadData::CurlThreadData(CurlCallback callbackFunction, wxString content, char *error, int extra) {
	this->callbackFunction = callbackFunction;
	this->content = content;
	this->error = error;
	this->extra = extra;
}


CurlCallback CurlThreadData::GetCallbackFunction() {
	return this->callbackFunction;
}


wxString CurlThreadData::GetContent() {
	return this->content;
}


char* CurlThreadData::GetError() {
	return this->error;
}


int CurlThreadData::GetExtra() {
	return this->extra;
}