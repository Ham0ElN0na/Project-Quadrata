#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "UI.h"
#include "SessionManager.h"
#include "dataCollector.h"
#include "authenticationService.h"
#include "systemLogger.h"
#include "Inventory.h"
#include "InventorySearchEngine.h"
#include "PaymentGatewayMock.h"
#include "BookingProcessor.h"
#include "PricingEngine&Discount&Cancel.h"
#include "ReceiptGenerator.h"
#include "InventorySearchEngine.h"
#include "PaymentGatewayMock.h"
#include "BookingProcessor.h"
#include "PricingEngine&Discount&Cancel.h"

using json = nlohmann::json;
using namespace std;

void ui::displaywelcomescreen() {
    system("cls");
    cout << "\n\n\n";
    cout << "\t\t                                 " << endl;
    cout << "\t\t* *" << endl;
    cout << "\t\t WELCOME TO QAURDRATA        " << endl;
    cout << "\t\t" << endl;
    cout << "\t\t                             " << endl;
}

int ui::displaymainmenu() {
    cout << "                                        " << endl;
    cout << "  1. Login\n  2. Sign Up\n  3. About Us\n   4. Exit" << endl;
    cout << "                                        " << endl;
    cout << "Enter choice: ";
    int c; cin >> c;
    return c;
}

void ui::userlogin( SessionManager& session) {
	systemLogger logger;
	logger.logInfo("User attempted to log in.");
    system("cls");
    cout << "                 LOGIN SCREEN               " << endl;
	AuthenticationService authService;
	dataCollector dc;
    string id , username;
    bool loginSuccess = false;
    string password, type;
    cout << " Enter account type (1 for admin / 2 for customer): " << endl;
    while (true) {
        cin >> type;
        if (type == "1" || type == "2") {
            break;
        } else {
            cout << "Invalid account type. Please enter '1' for admin or '2' for customer: ";
        }
	}
	dc.selectFilePath("Users.json");
    if (type == "1") {
        cout << "\n ENTER  ID: " << endl;
        cin >> id;
        cout << " ENTER PASSWORD  : " << endl;
        cin >> password;
        loginSuccess = authService.checkPassword(password, Admin(id), dc);
        if (loginSuccess) {
            cout << "Welcome back, " << id << "! Logging in as admin " << endl;
            session.login(1, id);
            // admin menu
            while (true) {
                cout << "Admin Menu:\n 1. List Inventory\n 2. Add Item\n 3. Remove Item\n 4. Update Item\n 5. Set Session Duration\n 6. Logout\n 7. Exit\nEnter choice: ";
                int achoice; cin >> achoice;
                if (achoice == 1) {
                    // read inventory file and print
                    ifstream invf("Inventory.json");
                    nlohmann::json j;
                    if (invf) invf >> j;
                    invf.close();
                    if (!j.contains("Inventory")) { cout << "No inventory file or empty." << endl; }
                    else {
                        for (size_t idx = 0; idx < j["Inventory"].size(); ++idx) {
                            if (idx == 0) {
								cout << "Current Inventory:\n";
                            }
                            auto& it = j["Inventory"][idx];
                            string pid = it.value("id", "");
                            string ptype = it.value("type", "");
                            string pdest = it.value("destination", "");
                            double pprice = it.value("price", 0.0);
                            int pavail = it.value("available", 0);
                            cout << pid << ": " << ptype << " to " << pdest << " - $" << pprice << " [" << pavail  << " available]\n";
							cout << "-----------------------------------" << endl;
                        }
                    }
                }
                else if (achoice == 2) {
                    string iid, itype, idest; double iprice; int iavail;
                    cout << "Enter ID: "; cin >> iid;
                    cout << "Enter type: "; cin >> itype;
                    cout << "Enter destination: "; cin >> idest;
                    cout << "Enter price: ";
                    while (!(cin >> iprice)) {
                        cout << "Invalid price. Enter price: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    cout << "Available? (y/n): ";
                    cin >> iavail;
                    dc.addInventoryItem(iid, itype, idest, iprice, iavail);
					logger.logInfo("Admin " + id + " added inventory item " + iid);
                }
                else if (achoice == 3) {
                    string iid; cout << "Enter ID to remove: "; cin >> iid;
                    dc.removeInventoryItem(iid);
					logger.logInfo("Admin " + id + " removed inventory item " + iid);
                }
                else if (achoice == 4) {
                    string iid, itype, idest; double iprice; int iavail;
                    cout << "Enter ID to update: "; cin >> iid;
                    cout << "Enter new type: "; cin >> itype;
                    cout << "Enter new destination: "; cin >> idest;
                    cout << "Enter new price: ";
                    while (!(cin >> iprice)) {
                        cout << "Invalid price. Enter new price: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    cout << "Available : ";
                    cin >> iavail;
                    dc.updateInventoryItemDetails(iid, itype, idest, iprice, iavail );
					logger.logInfo("Admin " + id + " updated inventory item " + iid);
                }
                else if (achoice == 5) {
                    cout << "enter session Duration in seconds: ";
                    int duration;
                    cin >> duration;
                    session.setSessionDuration(duration);
                }
                else if (achoice == 6) {
                    session.logout();
                    break;
                }
                else if (achoice == 7) {
                    exit(0);
                }
                
                else {
                    cout << "Invalid choice." << endl;
                }
            }
        }
        else {
			logger.logWarning("Admin with ID ( " + id + " ) failed to log in.");
            return;
        }
    }
    else {
        cout << "\n ENTER  ID: " << endl;
        cin >> username;
        cout << " ENTER PASSWORD  : " << endl;
        cin >> password;
        loginSuccess = authService.checkPassword(password, Customer(username), dc);
        if (loginSuccess) {
            cout << "Welcome back, " << username << "! Logging in as customer " << endl;
            session.login(2, username);
            vector<InventoryItem> items;
            {
                ifstream invf("Inventory.json");
                json j;
                if (invf) invf >> j;
                invf.close();
                if (j.contains("Inventory")) {
                    for (auto& it : j["Inventory"]) {
                        string pid = it.value("id", "");
                        string ptype = it.value("type", "");
                        string pdest = it.value("destination", "");
                        double pprice = it.value("price", 0.0);
                        int pavail = it.value("available", 0);
                        items.push_back(InventoryItem(pid, ptype, pdest, pprice, pavail));
                    }
                }
            }
            InventorySearchEngine searcher;
            PaymentGatewayMock pg;
            Customer cust = dc.getCustomerDataFromFile(username);
            while (true) {
                cout << "Customer Menu:\n 1. Search\n 2. View Profile\n 3. Logout\nEnter choice: ";
                int choice; cin >> choice;
                if (choice == 1) {
                    string dest;
                    double minP, maxP;
                    cout << "Enter destination: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, dest);
                    cout << "Enter min price: "; cin >> minP;
                    cout << "Enter max price: "; cin >> maxP;
                    auto results = searcher.search(items, dest, minP, maxP);
                    cout << "Found " << results.size() << " results\n";
                    if (!results.empty()) {
                        for (size_t i = 0; i < results.size(); ++i) {
                            cout << i + 1 << ") " << results[i].getType() << " to " << results[i].getDestination() << " - $" << results[i].getPrice() << " [" << results[i].getNumber() << " available]\n";
                        }
                        cout << "Select item number to book or 0 to cancel: "; int sel; cin >> sel;
                        if (sel > 0 && sel <= (int)results.size()) {
                            auto chosen = results[sel - 1];
                            double finalPrice = chosen.getPrice();

                            // ── Coupon code ───────────────────────────────
                            cout << "\nDo you have a coupon code? (y/n): ";
                            char hasCoupon; cin >> hasCoupon;
                            string couponDiscount = "";
                            if (hasCoupon == 'y' || hasCoupon == 'Y') {
                                cout << "Enter coupon code: ";
                                string code; cin >> code;
                                if (code == "AMMM4") {
                                    finalPrice -= finalPrice * 0.15;
                                    couponDiscount = "Coupon AMMM4 (15% off)";
                                    cout << "Coupon applied! 15% discount.\n";
                                } else if (code == "MAMM4") {
                                    finalPrice -= finalPrice * 0.05;
                                    couponDiscount = "Coupon MAMM4 (5% off)";
                                    cout << "Coupon applied! 5% discount.\n";
                                } else {
                                    cout << "Invalid coupon code. No discount applied.\n";
                                }
                            }

                            // ── Loyalty points ────────────────────────────
                            int loyaltyPts = cust.getloyaltyPoints();
                            string loyaltyDiscount = "";
                            double loyaltyValue = 0.0;
                            if (loyaltyPts > 1000) {
                                cout << "\nYou have " << loyaltyPts << " loyalty points. Use them? (y/n): ";
                                char useLoyalty; cin >> useLoyalty;
                                if (useLoyalty == 'y' || useLoyalty == 'Y') {
                                    loyaltyValue = loyaltyPts / 10.0;
                                    finalPrice -= loyaltyValue;
                                    if (finalPrice < 0) finalPrice = 0;
                                    loyaltyDiscount = "Loyalty points (" + to_string(loyaltyPts) + " pts)";
                                    cust.subtractloyaltyPoints(loyaltyPts);
                                    cout << "Loyalty discount of $" << loyaltyValue << " applied.\n";
                                }
                            }

                            cout << "\nFinal price: $" << finalPrice << "\n";
                            cout << "Confirm booking " << chosen.getType() << " to " << chosen.getDestination() << " for $" << finalPrice << "? (y/n): ";
                            char ans; cin >> ans;
                            if (ans == 'y' || ans == 'Y') {
                                if (cust.getaccountBalance() < finalPrice) {
                                    cout << "Insufficient balance. Booking failed.\n";
                                } else {
                                    // Deduct balance manually (bypass BookingProcessor price prompts)
                                    cust.addPayment(finalPrice);
                                    static int bookingCounter = 5000;
                                    string bookingId = to_string(bookingCounter++);
                                    cust.addLoyaltyPoints(static_cast<int>(finalPrice));
                                    dc.updateCustomerRecord(cust);
                                    dc.appendUserHistory(cust.getUserName(), string("Booked ") + chosen.getType() + " to " + chosen.getDestination() + " for $" + to_string(finalPrice));
                                    dc.updateInventoryAvailability(chosen.getId(), chosen.getNumber() - 1);

                                    // ── Receipt ───────────────────────────
                                    Customer freshCust = dc.getCustomerDataFromFile(cust.getUserName());
                                    string displayName = freshCust.getName().empty() ? cust.getUserName() : freshCust.getName();
                                    ReceiptGenerator receipt(displayName, freshCust.getEmail());
                                    receipt.addItem(chosen.getType() + " to " + chosen.getDestination(), chosen.getPrice());
                                    if (!couponDiscount.empty()) {
                                        double couponAmt = chosen.getPrice() - (loyaltyDiscount.empty() ? finalPrice : finalPrice + loyaltyValue);
                                        receipt.addItem(couponDiscount, couponAmt, true);
                                    }
                                    if (!loyaltyDiscount.empty())
                                        receipt.addItem(loyaltyDiscount, loyaltyValue, true);
                                    receipt.print();
                                    cout << "Booking ID: " << bookingId << "\n";
                                }
                            }
                        }
                    }
                }
                else if (choice == 2) {
                    Customer custData = dc.getCustomerDataFromFile(cust.getUserName());
                    if (!custData.getUserName().empty()) {
                        cout << "Customer Profile:\n";
						cout << "Username: " << custData.getUserName() << "\n";
                        cout << "Name: " << custData.getName() << "\n";
                        cout << "Email: " << custData.getEmail() << "\n";
                        cout << "Balance: $" << custData.getaccountBalance() << "\n";
                        cout << "Loyalty Points: " << custData.getloyaltyPoints() << "\n";
                    }
                    else {
                        cout << "Failed to retrieve customer profile." << endl;
                    }
                    int profileChoice;
                    bool inProfileMenu = true;
                    while (inProfileMenu)
                    {
                        cout << "1 to return to customer menu\n2 to add funds\n3 cancel booking\nchoose: ";
                        cin >> profileChoice;
                        switch (profileChoice)
                        {
                        case 1:
                            inProfileMenu = false;
                            break;
                        case 2:
                        {
                            double amount;
                            cout << "Enter amount to add: ";
                            cin >> amount;
                            custData.addBalance(amount);
                            dc.updateCustomerBalance(custData);
                            break;
                        }
                        case 3:
                        {
                            cancellationService cancelService(custData.getaccountBalance(), system_clock::now(), makeTime(2024, 6, 1));
                            custData.addBalance(cancelService.applyCancellation());
                            dc.updateCustomerBalance(custData);
                            break;
                        }
                        default:
                            cout << "Invalid choice." << endl;
                        }
                    }
                }
                else if (choice == 3) {
                    session.logout();
                    break;
                }
                else {
                    cout << "Invalid choice." << endl;
                }
            }
        }
        else {
            cout << "Login failed. Please check your credentials." << endl;
            return;
        }
    }

    
     
}

void ui::userSignUp() {
    system("cls");
    AuthenticationService authService;
	string type , check;
    cout << "                                                        " << endl;
    cout << "              SIGN UP SCREEEN                             " << endl;
    cout << "                                                                           " << endl;
	cout << " Enter account type (1 for admin / 2 for customer): " << endl;
	cin >> type;
    if (type == "1") {
		cout << "Admin registration selected." << endl;
        cout << "Admin registration require additional verification steps.\nPlease enter the required code" << endl;
		cin >> check;
		if (check != "adminReJion") {
            cout << "Verification failed. Returning to main menu." << endl;
            return;
        }
        authService.adminRegister();
    }
    else if (type == "2") {
        cout << "Customer registration selected." << endl;
        authService.customerRegister();
	}
    cout << " press any key to go back to main menu    ";
    system("pause>0");
}

