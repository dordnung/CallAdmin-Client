/**
 * -----------------------------------------------------
 * File        call_panel.cpp
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
#include "call_panel.h"
#include "calladmin-client.h"

#include <wx/xrc/xmlres.h>

#ifdef __WXMSW__
	// Memory leak detection for debugging 
	#include <wx/msw/msvcrt.h>
#endif


// Events for Log Panel
wxBEGIN_EVENT_TABLE(CallPanel, wxScrolledWindow)
EVT_LIST_ITEM_ACTIVATED(XRCID("callBox"), CallPanel::OnBoxClick)
wxEND_EVENT_TABLE()


// Load controls
bool CallPanel::InitPanel() {
	if (!wxXmlResource::Get()->LoadObject(this, caGetNotebook()->GetWindow(), "callPanel", "wxScrolledWindow")) {
		wxMessageBox("Error: Couldn't find XRCID callPanel", "Error on creating CallAdmin", wxOK | wxCENTRE | wxICON_ERROR);

		return false;
	}

	// Box for all Calls and autosize finished column
	FIND_OR_FAIL(this->callBox, XRCCTRL(*this, "callBox", wxListCtrl), "callBox");

	// Remember default header size of the columns
	for (int i = 0; i < this->callBox->GetColumnCount(); i++) {
		// Add default width to the width list
		this->columnHeaderWidths.push_back(this->callBox->GetColumnWidth(i));
	}

	return true;
}


// Update Call List
void CallPanel::UpdateCalls() {
	// First clear calls
	this->callBox->DeleteAllItems();

	for (wxVector<CallDialog *>::iterator callDialog = caGetCallDialogs()->begin(); callDialog != caGetCallDialogs()->end(); ++callDialog) {
		CallDialog *currentDialog = *callDialog;

		AppendCall(currentDialog->IsHandled(), currentDialog->GetTime(), currentDialog->GetServer());
	}

	// Autosize columns
	if (this->callBox->GetItemCount() > 0) {
		// Use content width for time and server if there is some content
		this->callBox->SetColumnWidth(CallPanelColumns::CallPanelColumn_Time, wxLIST_AUTOSIZE);
		this->callBox->SetColumnWidth(CallPanelColumns::CallPanelColumn_Server, wxLIST_AUTOSIZE);
	}
	else {
		// Use header width for time and server if there is no content
		this->callBox->SetColumnWidth(CallPanelColumns::CallPanelColumn_Time, this->columnHeaderWidths.at(CallPanelColumns::CallPanelColumn_Time));
		this->callBox->SetColumnWidth(CallPanelColumns::CallPanelColumn_Server, this->columnHeaderWidths.at(CallPanelColumns::CallPanelColumn_Server));
	}

	// Static size for finished column
	this->callBox->SetColumnWidth(CallPanelColumns::CallPanelColumn_Finished, this->columnHeaderWidths.at(CallPanelColumns::CallPanelColumn_Finished));
}


// Append call to the callbox
void CallPanel::AppendCall(bool finished, wxString time, wxString server) {
	long item = this->callBox->InsertItem(0, "call");

	this->callBox->SetItem(item, CallPanelColumns::CallPanelColumn_Time, time);
	this->callBox->SetItem(item, CallPanelColumns::CallPanelColumn_Server, wxString::FromUTF8(server));
	this->callBox->SetItem(item, CallPanelColumns::CallPanelColumn_Finished, finished ? wxString::FromUTF8("\xE2\x9C\x94") : wxString::FromUTF8("\xE2\x9C\x96"));

	this->callBox->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}


// The call is now handled
void CallPanel::SetHandled(int item) {
	this->callBox->SetItem(item, 0, wxString::FromUTF8("\xE2\x9C\x94"));

	caGetCallDialogs()->at(item)->SetFinish();
	caGetCallDialogs()->at(item)->GetTakeoverButton()->Enable(false);
}


// Window Event -> Open Call
void CallPanel::OnBoxClick(wxListEvent &event) {
	int selection = event.GetIndex();

	caGetCallDialogs()->at(selection)->Show(true);
	caGetCallDialogs()->at(selection)->Iconize(false);
	caGetCallDialogs()->at(selection)->Raise();
}
