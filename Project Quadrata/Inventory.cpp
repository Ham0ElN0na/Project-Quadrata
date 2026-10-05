#include "Inventory.h"
#include "UserClasses.h"
#include <sstream>
#include <algorithm>
#include <cctype>

InventoryItem::InventoryItem(string id, string type, string destination, double price, int n)
	: id(id), type(type), destination(destination), price(price), number(n) {
}

string InventoryItem::getId() const { return id; }
string InventoryItem::getType() const { return type; }
string InventoryItem::getDestination() const { return destination; }
double InventoryItem::getPrice() const { return price; }
int InventoryItem::getNumber() const { return number; }

void InventoryItem::setNumber(int n) { number = n; }
void InventoryItem::setPrice(double p) { price = p; }
void InventoryItem::setDestination(string d) { destination = d; }
void InventoryItem::setType(string t) { type = t; }

Booking::Booking(string bookingId, string itemId, string userId, double amount)
	: bookingId(bookingId), itemId(itemId), userId(userId), amount(amount) {
}

string Booking::getBookingId() const { return bookingId; }
string Booking::getItemId() const { return itemId; }
string Booking::getUserId() const { return userId; }
double Booking::getAmount() const { return amount; }

void Booking::setBookingId(string id) { bookingId = id; }
void Booking::setItemId(string id) { itemId = id; }
void Booking::setUserId(string id) { userId = id; }
void Booking::setAmount(double a) { amount = a; }

