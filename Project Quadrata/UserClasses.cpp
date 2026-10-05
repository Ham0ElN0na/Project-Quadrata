#include <iostream>
#include <string>
#include <vector>
#include "UserClasses.h"
using namespace std;

Customer::Customer(string n)
	: Username(n), email(""), password("000"), balance(0.0), points(0), name(""), age(0)
{
}

Customer::Customer(string n, string e, double b ,string id , string pass, int a)
    : Username(n) , email(e), password(pass), balance(b), points(0) , name(id), age(a)
{ 
}

Customer::Customer(string n, string e, double b, string id, string pass, int a, int l)
    : Username(n), email(e), password(pass), balance(b), points(l), name(id), age(a)
{
}

void Customer::setAge(int a){
    age = a;
}

int Customer::getAge(){
    return age;
}

void Customer::setUserName(string user){
    Username = user;
}
void Customer::setEmail(string e){
    email = e;
}
void Customer::addBalance(double amount){
    balance += amount;
}
void Customer::setpassword(string pass){
    password = pass;
}
void Customer::setName(string n){
    name = n;
}
string Customer::getUserName(){
    return Username;
}
string Customer::getpassword(){
    return password;
}

string Customer::getName(){
    return name;
}

string Customer::getEmail(){
    return email;
}

double Customer::getaccountBalance(){
    return balance;
}

int Customer::getloyaltyPoints(){
    return points;
}

void Customer::addLoyaltyPoints(int pts){
    points += pts;
}

void Customer::subtractloyaltyPoints(int pointsToSubtract){
    if (pointsToSubtract > points) {
        cout << "Not enough loyalty points to subtract." << endl;
        return;
    }
    points -= pointsToSubtract;
}

void Customer::addPayment(double amount){
    balance -= amount;
    paymentHistory.push_back("Paid $" + to_string(amount));
    points += static_cast<int>(amount)/10;
}

Admin::Admin(string id)
    : employeeID(id), password(""), department(""), adminLevel(0)
{
}

Admin::Admin(string id, string passkey, string dept, int level )
	: employeeID(id), department(dept), adminLevel(level), password(passkey)
{
}

void Admin::setpassword(string pass){
    password = pass;
}

string Admin::getpassword(){
    return password;
}

int Admin::getadminLevel(){
    return adminLevel;
}

string Admin::getemployeeID(){
    return employeeID;
}

string Admin::getdepartment(){
    return department;
}