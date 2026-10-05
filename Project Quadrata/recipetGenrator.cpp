#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

class Item {
protected:
    string name;
    double pricePerUnit;

public:
    Item(string n, double p) {
        name = n;
        pricePerUnit = p;
    }

    virtual ~Item() {}

    virtual string getName() { return name; }
    virtual double getTotalPrice() { return pricePerUnit; }
    virtual void printDetails() {
        cout << "- " << name << " : $" << getTotalPrice() << "\n";
    }
};

class Hotel : public Item {
private:
    int nights;

public:
    Hotel(string n, double p, int nghts) : Item(n, p) {
        nights = nghts;
    }

    double getTotalPrice() override {
        return pricePerUnit * nights;
    }

    void printDetails() override {
        cout << "- " << name << " (" << nights << " Nights) : $" << getTotalPrice() << "\n";
    }
};

class Airline : public Item {
private:
    bool isRoundTrip;

public:
    Airline(string n, double p, bool roundTrip) : Item(n, p) {
        isRoundTrip = roundTrip;
    }

    void printDetails() override {
        string tripType = (isRoundTrip) ? "Round Trip" : "One Way";
        cout << "- " << name << " (" << tripType << ") : $" << getTotalPrice() << "\n";
    }
};

class Receipt {
private:
    int receiptId;
    string customerName;
    Item* items[10];
    int itemCount;

public:
    Receipt(string name) {
        customerName = name;
        itemCount = 0;
        receiptId = 10000000 + rand() % 89999999;
    }

    ~Receipt() {
        for (int i = 0; i < itemCount; i++) {
            delete items[i];
        }
    }

    void addBooking(Item* newItem) {
        if (itemCount < 10) {
            items[itemCount] = newItem;
            itemCount++;
        }
        else {
            cout << "Receipt is full!\n";
            delete newItem;
        }
    }

    void receiptGenerator() {
        double total = 0;

        cout << "\n";
        cout << "         RECEIPT DETAILS         \n";
        cout << "\n";
        cout << "Receipt ID: #" << receiptId << "\n";
        cout << "Customer Name: " << customerName << "\n";
        cout << "\n";
        cout << "\n";

        for (int i = 0; i < itemCount; i++) {
            items[i]->printDetails();
            total += items[i]->getTotalPrice();
        }
        cout << "\n";
        cout << "\n";
        cout << "Total: $" << total << "\n";
        cout << "\n";
        cout << "Thank you for choosing us, have a great trip!\n";
        cout << "\n";
        cout << "\n";
    }
};

//int main() {
//    srand(time(0));
//
//    Receipt myReceipt("Moaz Mohamed");
//
//    myReceipt.addBooking(new Airline("Cairo to Dubai Flight", 300.00, true));
//    myReceipt.addBooking(new Hotel("Hilton Dubai Room", 150.00, 3));
//    myReceipt.addBooking(new Airline("Internal Shuttle Flight", 50.00, false));
//
//    myReceipt.receiptGenerator();
//
//    return 0;
//}