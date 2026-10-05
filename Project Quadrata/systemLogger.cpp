#include <iostream>
#include "systemLogger.h"
#include "dataCollector.h"
#include <fstream>
using namespace std;

systemLogger::systemLogger() {
}

void systemLogger::logStartup() {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "System started up." << endl;
        logFile.close();
    }
    else
    {
        cout << "Unable to open log file." << endl;
    }
}

void systemLogger::logInfo(const string& message) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[INFO]: " << message << endl;
        logFile.close();
    }
}

void systemLogger::logError(const string& message) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[ERROR]: " << message << endl;
        logFile.close();
    }
}

void systemLogger::logWarning(const string& message) {
	cerr << "\n[WARNING]: " << message << endl;
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[WARNING]: " << message << endl;
        logFile.close();
    }
}
void systemLogger::logSessionInfo(int userID, const string& token) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[SESSION] UserID: " << userID << " session token " << token << endl;
        logFile.close();
    }
}

void systemLogger::logInventoryChange(const string& action, const string& itemId) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[INVENTORY] " << action << " item with ID: " << itemId << endl;
        logFile.close();
    }
}