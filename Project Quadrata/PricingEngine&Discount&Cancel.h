#ifndef PRICINGENGINE_DISCOUNT_CANCEL_H
#define PRICINGENGINE_DISCOUNT_CANCEL_H
#include <iostream>
#include <string>
#include <chrono>
using namespace std;
using namespace std::chrono;
class PricingEngine {
private:
    const double taxRate = 0.14;
    double mainPrice;
    system_clock::time_point registrationTime;
    system_clock::time_point travelTime;
public:
    PricingEngine(double m,system_clock::time_point r,system_clock::time_point t);
    double getMainPrice();
    double priceWithTax(double price);
    double finalPrice();
};
system_clock::time_point makeTime(int y, int m, int d, int h = 0, int min = 0); 
class DiscountCalculator : public PricingEngine {
private:
    string name;
    int age;
    int loyaltyPoints;
public:
    DiscountCalculator(double m,system_clock::time_point r,system_clock::time_point t,string n,int a,int l);
    double priceWithDiscounts(double price, Customer& customer);
};
class cancellationService {
private:
    double bookingPrice;
    system_clock::time_point cancellationDate;
    system_clock::time_point travelDate;
public:
    cancellationService(
        double price,
        system_clock::time_point cancelDate,
        system_clock::time_point travel
    );
    double calculateRefund();
    double applyCancellation();
    void restoreInventory(int& availableItems);
};


#endif