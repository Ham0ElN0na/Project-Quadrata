#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

// ── Receipt Generator ─────────────────────────────────────────────────────────
// Generates a formatted receipt for a booking.

struct ReceiptItem {
    string description;
    double amount;
    bool   isDiscount; // true = shown as negative
};

class ReceiptGenerator {
private:
    int            receiptId;
    string         customerName;
    string         customerEmail;
    vector<ReceiptItem> items;

public:
    ReceiptGenerator(const string& name, const string& email = "") {
        customerName  = name;
        customerEmail = email;
        srand(static_cast<unsigned>(time(0)));
        receiptId = 10000000 + rand() % 89999999;
    }

    void addItem(const string& desc, double amount, bool isDiscount = false) {
        items.push_back({desc, amount, isDiscount});
    }

    double getTotal() const {
        double total = 0;
        for (auto& i : items) total += i.isDiscount ? -i.amount : i.amount;
        return total;
    }

    void print() const {
        const int W = 50;
        string line(W, '-');
        string dline(W, '=');

        cout << "\n" << dline << "\n";
        cout << setw((W + 16) / 2) << "QUADRATA TRAVEL RECEIPT" << "\n";
        cout << dline << "\n";
        cout << "Receipt ID  : #" << receiptId << "\n";
        cout << "Customer    : " << customerName << "\n";
        if (!customerEmail.empty())
            cout << "Email       : " << customerEmail << "\n";

        // Date/time
        time_t now = time(0);
        char buf[32];
        struct tm t;
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%d %b %Y  %H:%M", &t);
        cout << "Date        : " << buf << "\n";
        cout << line << "\n";

        for (auto& item : items) {
            string prefix = item.isDiscount ? "  - " : "    ";
            cout << left << setw(36) << (prefix + item.description)
                 << right << setw(10)
                 << (item.isDiscount ? "-" : " ") << "$"
                 << fixed << setprecision(2) << item.amount << "\n";
        }

        cout << line << "\n";
        cout << left << setw(36) << "    TOTAL"
             << right << setw(10) << " $"
             << fixed << setprecision(2) << getTotal() << "\n";
        cout << dline << "\n";
        cout << "  Thank you for choosing Quadrata!\n";
        cout << "  Have a wonderful trip.\n";
        cout << dline << "\n\n";
    }

    // Returns receipt as a string (for GUI use)
    string toString() const {
        const int W = 50;
        string line(W, '-');
        string dline(W, '=');
        ostringstream ss;

        ss << "\n" << dline << "\n";
        ss << "         QUADRATA TRAVEL RECEIPT\n";
        ss << dline << "\n";
        ss << "Receipt ID  : #" << receiptId << "\n";
        ss << "Customer    : " << customerName << "\n";
        if (!customerEmail.empty())
            ss << "Email       : " << customerEmail << "\n";

        time_t now = time(0);
        char buf[32];
        struct tm t;
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%d %b %Y  %H:%M", &t);
        ss << "Date        : " << buf << "\n";
        ss << line << "\n";

        for (auto& item : items) {
            string prefix = item.isDiscount ? "  - " : "    ";
            ss << left << setw(36) << (prefix + item.description)
               << right << setw(10)
               << (item.isDiscount ? "-" : " ") << "$"
               << fixed << setprecision(2) << item.amount << "\n";
        }

        ss << line << "\n";
        ss << left << setw(36) << "    TOTAL"
           << right << setw(10) << " $"
           << fixed << setprecision(2) << getTotal() << "\n";
        ss << dline << "\n";
        ss << "  Thank you for choosing Quadrata!\n";
        ss << "  Have a wonderful trip.\n";
        ss << dline << "\n";
        return ss.str();
    }
};
