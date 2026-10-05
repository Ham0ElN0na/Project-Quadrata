#include "BookingProcessor.h"
#include "PricingEngine&Discount&Cancel.h"
#include <sstream>
#include <iostream>
#include "systemLogger.h"
#include "dataCollector.h"
using namespace std;

bool BookingProcessor::bookItem(vector<InventoryItem>& items, const string& itemId, Customer &customer, Booking &outBooking, PaymentGatewayMock &pg) {
	systemLogger logger;
	for (auto &it : items) {
		if (it.getId() == itemId) {
			if (it.getNumber() <= 0) return false;
			int day, month, year;
			double price = it.getPrice();
			cout << "Enter travel date (day month year): ";
			cin >> day >> month >> year;
			time_t now = time(0);
			struct tm timeinfo = {};
			localtime_s(&timeinfo, &now);
			tm* currenttime = &timeinfo;
			while (day < 1 || day > 31 || month < 1 || month > 12 || year < currenttime->tm_year + 1900) {
				cout << "Invalid date. Please enter a valid date (day month year): ";
				cin >> day >> month >> year;
			}
			auto registration = system_clock::now();
			auto travel = makeTime(year, month, day);
			DiscountCalculator discount(price,registration,travel,customer.getUserName(),customer.getAge(),customer.getloyaltyPoints());
			price = discount.finalPrice();
			price = discount.priceWithDiscounts(price, customer);
			cout << "Final price after discounts and taxes: $" << price << endl;
			cout << "confirm booking? (y/n): ";
			char confirm;
			cin >> confirm;
			if (confirm != 'y' && confirm != 'Y') return false;
			if (customer.getaccountBalance() < price) return false;
			customer.addPayment(price);
			static int bookingCounter = 5000;
			stringstream ss; ss << bookingCounter++;
			outBooking = Booking(ss.str(), itemId, customer.getUserName(), price);
			customer.addLoyaltyPoints(static_cast<int>(price));
			logger.logInventoryChange("Booked", itemId);
			return true;
		}
	}
	logger.logError("Attempted to book item with ID " + itemId + " but it was not found in inventory.");
	return false;
}
