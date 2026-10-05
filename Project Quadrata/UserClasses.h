#ifndef USERCLASSES_H
#define USERCLASSES_H

#include <string>
#include <vector>
using namespace std;

class Admin {
private:
	string employeeID;
	string password;
	string department;
	int adminLevel;
public:
	Admin(string id);
	Admin(string id, string passkey, string dept, int level);
	void setpassword(string pass);
	string getpassword();
	int getadminLevel();
	string getdepartment();
	string getemployeeID();
};

class Customer {
private:
	string Username;
	string name;
	string email;
	string password;
	double balance;
	int age = 0;
	int points;
	vector<string> paymentHistory;
public:
	Customer(string n);
	Customer(string n, string e, double b, string id , string pass, int a);
	Customer(string n, string e, double b, string id, string pass, int a,int l);
	void setAge(int a);
	void setUserName(string user);
	void setpassword(string pass);
	void setEmail(string e);
	void addBalance(double amount);
	void setName(string n);
	int getAge();
	string getUserName();
	string getpassword();
	string getName();
	string getEmail();
	double getaccountBalance();
	int getloyaltyPoints();
	void addLoyaltyPoints(int points);
	void subtractloyaltyPoints(int pointsToSubtract);
	void addPayment(double amount);
};

#endif
