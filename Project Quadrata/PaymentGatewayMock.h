#ifndef PAYMENTGATEWAYMOCK_H
#define PAYMENTGATEWAYMOCK_H

#include <string>
using namespace std;

class PaymentGatewayMock {
public:
	bool validateCardFormat(const string& cardNumber);
	bool charge(double amount);
};

#endif // PAYMENTGATEWAYMOCK_H
