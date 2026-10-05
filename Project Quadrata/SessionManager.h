#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H
#include <string>
#include <ctime>
using namespace std;
class SessionManager {
private:
    int activeUserID;
    time_t loginTime;
    bool active;
	int sessionToken;
	int sessionDuration; 
	string activeUserName;
    void watchSession(); 

public:
    SessionManager();
	void setStartup();
    int getStartup() const;
    void setSessionDuration(int duration);
    void login(int userID, const string& userName);
    int logout();
    void printInfo() const;
    bool isActive() const;
    long long getSessionToken() const;
};

#endif