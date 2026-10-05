#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>
#include "UserClasses.h"
using namespace std;

class InventoryItem {
private:
	string id;
	string type;
	string destination;
	double price;
	int number;
public:
	InventoryItem(string id, string type, string destination, double price, int n);
	string getId() const;
	string getType() const;
	string getDestination() const;
	double getPrice() const;
	int getNumber() const;
	void setNumber(int n);
	void setPrice(double p);
	void setDestination(string d);
	void setType(string t);
};

class Booking {
private:
	string bookingId;
	string itemId;
	string userId;
	double amount;
public:
	Booking(string bookingId, string itemId, string userId, double amount);
	string getBookingId() const;
	string getItemId() const;
	string getUserId() const;
	double getAmount() const;
	void setBookingId(string id);
	void setItemId(string id);
	void setUserId(string id);
	void setAmount(double a);
};


#endif
