#pragma once
#include <string>
#include "dataCollector.h"
using namespace std;

class AuthenticationService {
	string test;
public:
	AuthenticationService();
    bool checkPassword(const string& password, const Admin& data, dataCollector& dc);
    bool checkPassword(const string& password, const Customer& data, dataCollector& dc);
	void adminRegister();
	void customerRegister();
};