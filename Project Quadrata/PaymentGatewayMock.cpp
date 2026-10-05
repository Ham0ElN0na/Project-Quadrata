#include "PaymentGatewayMock.h"
#include <cctype>

bool PaymentGatewayMock::validateCardFormat(const string& cardNumber) {
	if (cardNumber.size() < 12 || cardNumber.size() > 19) return false;
	for (char c : cardNumber) if (!isdigit(c)) return false;
	return true;
}

bool PaymentGatewayMock::charge(double amount) {
	(void)amount;
	return true;
}
