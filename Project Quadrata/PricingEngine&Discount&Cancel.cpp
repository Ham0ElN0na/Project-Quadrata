using namespace std;
#include <iostream>
#include "UserClasses.h"
#include <ctime>
#include "PricingEngine&Discount&Cancel.h"
PricingEngine::PricingEngine(double m,system_clock::time_point r,system_clock::time_point t)
{
    mainPrice = m;
    registrationTime = r;
    travelTime = t;
}
double PricingEngine::getMainPrice() {
    return mainPrice;
}
double PricingEngine::priceWithTax(double price) {
    return price + (price * taxRate);
}
double PricingEngine::finalPrice() {
    double price = priceWithTax(mainPrice);
    auto diff = duration_cast<hours>(travelTime - registrationTime);
    if (diff.count() < 72) {
        price += mainPrice * 0.50;
    }
    else if (diff.count() < 168 && diff.count()>=72) {
        price += mainPrice * 0.05;
    }
    time_t tt = system_clock::to_time_t(travelTime);
    tm timeInfo;
    localtime_s(&timeInfo, &tt);
    int day = timeInfo.tm_wday;
    if (day == 5 || day == 6) {
        price += mainPrice * 0.05;
    }
    return price;
}
system_clock::time_point makeTime(int y, int m, int d, int h, int min)
{
    tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = h;
    t.tm_min = min;
    return system_clock::from_time_t(mktime(&t));
}
DiscountCalculator::DiscountCalculator(double m,system_clock::time_point r,system_clock::time_point t,string n,int a,int l): PricingEngine(m, r, t)
{
    name = n;
    age = a;
    loyaltyPoints = l;
}
double DiscountCalculator::priceWithDiscounts(double price, Customer& customer) {
    string hasPromo, code, loyalty;
    cout << "Do you have promo code? (Y/N): ";
    cin >> hasPromo;
    if (hasPromo == "Y" || hasPromo == "y") {
        cout << "Enter promo code: ";
        cin >> code;
        if (code == "AMMM4") {
            price -= price * 0.15;
        }
        else if (code == "MAMM4") {
            price -= price * 0.05;
        }
    }
    if (age > 6 && age < 25) {
        price -= price * 0.10;
    }
    cout << "Use loyalty points? (Y/N): ";
    cin >> loyalty;
    if ((loyalty == "Y" || loyalty == "y") && loyaltyPoints > 1000) {
        price -= loyaltyPoints / 10.0;
		customer.subtractloyaltyPoints(loyaltyPoints);
    }
    return price;
}
cancellationService::cancellationService(
    double price,
    system_clock::time_point cancelDate,
    system_clock::time_point travel
) {
    bookingPrice = price;
    cancellationDate = cancelDate;
    travelDate = travel;
}
double cancellationService::calculateRefund() {
    auto diff = duration_cast<hours>(travelDate - cancellationDate);
    if (diff.count() >= 168) {
        return bookingPrice;
    }
    else if (diff.count() >= 72) {
        return bookingPrice - (bookingPrice * 0.10);
    }
    else if (diff.count() > 0) {
        return bookingPrice - (bookingPrice * 0.50);
    }
    return 0;
}
double cancellationService::applyCancellation() {
    double refund = calculateRefund();
    return refund;
}
void cancellationService::restoreInventory(int& availableItems) {
    availableItems++;
}
