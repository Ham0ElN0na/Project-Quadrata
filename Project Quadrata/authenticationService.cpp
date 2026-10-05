#include <iostream>
#include "dataCollector.h"
#include "authenticationService.h"

using namespace std;

AuthenticationService::AuthenticationService() {
	test = "test";
}

bool AuthenticationService::checkPassword(const string& password,const Admin& data , dataCollector& dc) {
    string storedPassword = dc.getPassWordFromFile(data);
    if (storedPassword == password) {
        cout << "Password is correct." << endl;
		return true;
    } else if (storedPassword.empty()) {
        return false;
    }
    else {
        cout << "Password is incorrect." << endl;
		return false;
    }
}

bool AuthenticationService::checkPassword(const string& password, const Customer& data, dataCollector& dc) {
    string storedPassword = dc.getPassWordFromFile(data);
    if (storedPassword == password) {
        cout << "Password is correct." << endl;
        return true;
    }
    else {
        cout << "Password is incorrect." << endl;
        return false;
    }
}
void AuthenticationService::adminRegister() {
    string id, passkey, dept;
    int level = 1;
    cout << "Enter employee ID: ";
    cin >> id;
    cout << "Enter password: ";
    cin >> passkey;
    cout << "Enter department: ";
    cin >> dept;
    Admin newAdmin(id, passkey, dept, level);
    dataCollector dc;
    dc.selectFilePath("Users.json");
    dc.saveDataInFile(newAdmin, dc);
}

void AuthenticationService::customerRegister() {
    string username, email, name, passkey;
    double balance = 0.0;
    cout << "Enter Username: ";
    cin >> username;
	cout << "Enter password: ";
    cin >> passkey;
    cout << "Enter email: ";
    cin >> email;
	cout << "Enter Name: ";
	cin.ignore();
	getline(cin, name);
    int age;
    cout << "Enter age: ";
    cin >> age;
    Customer newCustomer(username, email, balance, name, passkey, age);
    dataCollector dc;
    dc.selectFilePath("Users.json");
    dc.saveDataInFile(newCustomer, dc);
}