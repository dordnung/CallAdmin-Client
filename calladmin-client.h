/**
 * -----------------------------------------------------
 * File        calladmin-client.h
 * Authors     Impact, David <popoklopsi> Ordnung
 * License     GPLv3
 * Web         http://gugyclan.eu, http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * CallAdmin Header File
 * Copyright (C) 2013 Impact, David <popoklopsi> Ordnung
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

// App Class
class CallAdmin : public wxApp
{
public:
	// Where all began :)
    virtual bool OnInit();

	// Command line arguments
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

	// Start in Taskbar?
	static bool start_taskbar;
};

// Taskbar Class
class TaskBarIcon : public wxTaskBarIcon
{
public:
    TaskBarIcon() {}

	// Taskbar Events
    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuUpdate(wxCommandEvent&);
	void OnMenuAutoStart(wxCommandEvent&);

    virtual wxMenu *CreatePopupMenu();

    DECLARE_EVENT_TABLE()
};

// Timer Class
class Timer : public wxTimer
{
private:
	int attempts;

public:
	Timer() : wxTimer(this, 1) {}

	void run(int milliSecs);
	void update(wxTimerEvent&);
	
	int getAttempts() {return attempts;}
	void setAttempts(int attempt) {attempts = attempt;}

	DECLARE_EVENT_TABLE()
};

// Main Dialog Class
class MainDialog: public wxDialog
{
private:
	wxCheckBox* store;
	wxCheckBox* available;

	wxListBox* callBox;

public:
	MainDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title) {}

	void createWindow(bool reconnect, wxString error, bool taskbar = false);

	bool isAvailable() {return (available != NULL && available->IsChecked());}
	bool wantStore() {return (store != NULL && !store->IsChecked());}

	void updateCall();

protected:
    void OnExit(wxCommandEvent& event);
    void OnHide(wxCommandEvent& event);
    void OnReconnect(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	void OnBoxClick(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

// Error Dialog Class
class ErrorDialog: public wxDialog
{
public:
    ErrorDialog(const wxString& title, int error, wxString errorMessage, wxString type);

protected:
    void OnHide(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

// Config Dialog Class
class ConfigDialog: public wxDialog
{
private:
	wxSpinCtrl* stepSlider;
	wxSpinCtrl* timeoutSlider;
	wxSpinCtrl* attemptsSlider;
	wxTextCtrl* pageText;
	wxTextCtrl* keyText;

public:
    ConfigDialog(const wxString& title);

protected:
    void OnSet(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

// Call Dialog Class
class CallDialog: public wxDialog
{
private:
	// We need information about a call ;)
	wxString fullIP;
	wxString serverName;
	wxString target;
	wxString targetID;
	wxString client;
	wxString clientID;
	wxString reason;
	wxString boxText;
	std::string reportedAt;

	int ID;

public:
	CallDialog(const wxString& title) : wxDialog(NULL, wxID_ANY, title) {};

	// Privat -> set Methods
	void setIP(wxString IP) {fullIP = IP;}
	void setName(wxString name) {serverName = name;}
	void setTarget(wxString name) {target = name;}
	void setTargetID(wxString ID) {targetID = ID;}
	void setClient(wxString name) {client = name;}
	void setClientID (wxString ID) {clientID = ID;}
	void setReason (wxString name) {reason = name;}

	void setID(int num) {ID = num;}
	void setTime(std::string time) {reportedAt = time;}
	void setBoxText(wxString text) {boxText = text;}

	int getTime() const 
	{
		int timestamp;
		std::stringstream timeS(reportedAt);

		timeS >> timestamp;

		return timestamp;
	}

	wxString getTargetID() const {return targetID;}
	wxString getClientID() const {return clientID;}
	wxString getServer() {return serverName;}
	wxString getBoxText() {return boxText;}

	void startCall();

	// Operator overloadings
	friend bool operator==(const CallDialog& x, const CallDialog& y) { return (x.getTime() == y.getTime() && x.getClientID() == y.getClientID() && y.getTargetID() == y.getTargetID()); }
	friend bool operator!=(const CallDialog& x, const CallDialog& y) { return !(x == y);}

protected:
	void OnConnect(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

void checkUpdate();

void createReconnect(wxString error);
void createError(int errorCode, wxString error, wxString type);
void exitProgramm();

bool parseConfig();

std::wstring s2ws(wxString s);
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
     {wxCMD_LINE_SWITCH, "taskbar", "taskbar", "Move GUI to taskbar on Start"},
     {wxCMD_LINE_NONE}
};

// Declare the app
DECLARE_APP(CallAdmin)
