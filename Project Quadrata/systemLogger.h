#pragma once
#include <iostream>
#include <fstream>
#include "dataCollector.h"
using namespace std;
class systemLogger {
public:
	systemLogger();
	void logStartup();
	void logInfo(const string& message);
	void logError(const string& message);
	void logWarning(const string& message);
	void logSessionInfo(int userID, const string& token);
	void logInventoryChange(const string& action, const string& itemId);
};