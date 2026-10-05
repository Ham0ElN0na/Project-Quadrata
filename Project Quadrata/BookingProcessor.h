#ifndef BOOKINGPROCESSOR_H
#define BOOKINGPROCESSOR_H

#include "Inventory.h"
#include "PaymentGatewayMock.h"

class BookingProcessor {
public:
	bool bookItem(vector<InventoryItem>& items, const string& itemId, Customer &customer, Booking &outBooking, PaymentGatewayMock &pg);
};

#endif // BOOKINGPROCESSOR_H
