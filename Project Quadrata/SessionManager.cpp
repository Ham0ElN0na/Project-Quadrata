#include "SessionManager.h"
#include "systemLogger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
SessionManager::SessionManager() {
    activeUserID = -1;
	activeUserName = "";
    loginTime = 0;
    active = false;
	sessionDuration = getStartup();
	sessionToken = getSessionToken();
}
void SessionManager::login(int userID, const std::string& userName) {
    activeUserID = userID;
    activeUserName = userName;
    loginTime = static_cast<int>(time(nullptr));
    active = true;
    systemLogger logger;
    logger.logInfo("User " + userName + " logged in. Session token is " + to_string(sessionToken));
    cout << "User " << userName << " logged in.\n";    
    thread(&SessionManager::watchSession, this).detach();
}
int SessionManager::logout() {
    systemLogger logger;
    if (active) {
        active = false;
        activeUserID = -1;
		logger.logInfo("User " + activeUserName + " logged out. Session token was " + to_string(sessionToken));
        cout << "\nUser logged out.\n";
        exit(0);
    }
    return 0;
}

int SessionManager::getStartup() const {
    ifstream sessionFile("session.json");
    nlohmann::json j;
    if (sessionFile.is_open()) {
        sessionFile >> j;
        sessionFile.close();
        return j.value("sessionDuration", 60);
    }
    return 60; 
}

void SessionManager :: setStartup() {
    ofstream sessionFile("session.json");
    nlohmann::json j;
	j["sessionDuration"] = sessionDuration;
    sessionFile << j.dump();
    sessionFile.close();
}


void SessionManager::printInfo() const {
    cout << "Active User ID: " << activeUserID << "\n";
    srand(static_cast<unsigned int>(time(0)));
    cout << "Session Token: Token" << sessionToken << endl;
}
void SessionManager::setSessionDuration(int duration) {
    sessionDuration = duration;
	setStartup();
}

long long SessionManager::getSessionToken() const {
    srand(static_cast<unsigned int>(time(0)));
    return (rand()+1000) % 100000;
}

bool SessionManager::isActive() const {
    return active;
}   
void SessionManager::watchSession() {
	systemLogger logger;
    while (active) {
        time_t now = time(nullptr);
            if (now - loginTime >= sessionDuration) {
            logger.logInfo("Session number " + to_string(sessionToken) + " for user " + activeUserName + " has expired.");
			cout << "\nSession expired. Logging out...\n";
            logout();
            break;
        }
            if (now - loginTime == sessionDuration -30) {
				logger.logWarning("Session number " + to_string(sessionToken) + " for user " + activeUserName + " will expire in 30 seconds.");
            }
        this_thread::sleep_for(chrono::seconds(1));
    }
}
