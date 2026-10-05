#include <iostream>
#include "dataCollector.h"
#include "UserClasses.h"
#include "SessionManager.h"
#include "UI.h"
#include "systemLogger.h"

using namespace std;

int main() {
	ui appUI;
	SessionManager session;
	systemLogger logger;
	logger.logStartup();
	appUI.displaywelcomescreen();
	while (true) {
		
		int choice ;
		

		choice = appUI.displaymainmenu();
		

		switch (choice) {
		case 1:
			appUI.userlogin(session);
			break;
		case 2:
			appUI.userSignUp();
			break;
		case 3:
			cout << "About Us:\n The application was made by [ deboo , Mazen  , Ahmed , Mohamed]" << endl;
			break;
		case 4:
			cout << "Exiting the application. Goodbye!" << endl;
			logger.logInfo("Application exited by user.");
			return 0;
		default:
			cout << "Invalid choice. Please try again." << endl;
		}
	}
}