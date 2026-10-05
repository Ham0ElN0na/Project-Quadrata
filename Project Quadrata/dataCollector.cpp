#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>
#include "UserClasses.h"
#include "dataCollector.h"
#include "systemLogger.h"

using json = nlohmann::json;
using namespace std;

// -- Safe JSON helpers ---------------------------------------------------------
// Read: open, read all, close, parse. Returns empty object on any error.
static json safeRead(const string& path) {
    ifstream f(path);
    if (!f.is_open()) return json::object();
    ostringstream ss;
    ss << f.rdbuf();
    f.close();
    string content = ss.str();
    if (content.empty()) return json::object();
    try {
        return json::parse(content);
    } catch (...) {
        return json::object();
    }
}

// Write: write to a temp file first, then rename over the target (atomic).
// This prevents corruption if the process crashes mid-write.
static bool safeWrite(const string& path, const json& j) {
    string tmp = path + ".tmp";
    {
        ofstream f(tmp, ios::out | ios::trunc);
        if (!f.is_open()) return false;
        f << j.dump(4);
        f.close();
    }
    // Remove old file and rename temp into place
    remove(path.c_str());
    return rename(tmp.c_str(), path.c_str()) == 0;
}

// -- dataCollector -------------------------------------------------------------

dataCollector::dataCollector() {
    filename = "unknown";
}

dataCollector::dataCollector(string name) {
    filename = name;
}

void dataCollector::selectFilePath(string name) {
    filename = name;
}

void dataCollector::saveDataInFile(string data) {
    ifstream checkfile(filename);
    if (!checkfile) {
        cout << "File could not be open" << endl;
    } else {
        ofstream openfile(filename, ios::app);
        openfile << data;
        openfile.close();
        cout << "data was saved" << endl;
    }
    checkfile.close();
}

string dataCollector::getPassWordFromFile(Admin data) {
    systemLogger logger;
    json j = safeRead(filename);
    if (j.is_null() || !j.contains("Admins")) {
        logger.logError("Failed to read file: " + filename);
        return "";
    }
    if (j["Admins"].contains(data.getemployeeID())) {
        logger.logInfo("Password retrieved for admin: " + data.getemployeeID());
        return j["Admins"][data.getemployeeID()]["Password"];
    }
    cout << "Admin with ID ( " << data.getemployeeID() << " ) not found." << endl;
    logger.logError("Admin with ID ( " + data.getemployeeID() + " ) not found in file: " + filename);
    return "";
}

string dataCollector::getPassWordFromFile(Customer data) {
    systemLogger logger;
    json j = safeRead(filename);
    if (j.is_null() || !j.contains("Clients")) {
        logger.logError("Failed to read file: " + filename);
        return "";
    }
    if (j["Clients"].contains(data.getUserName())) {
        logger.logInfo("Password retrieved for customer: " + data.getUserName());
        return j["Clients"][data.getUserName()]["Password"];
    }
    cout << "Client with userName ( " << data.getUserName() << " ) not found." << endl;
    logger.logError("Client with userName ( " + data.getUserName() + " ) not found in file: " + filename);
    return "";
}

void dataCollector::saveDataInFile(Admin data, dataCollector dc) {
    systemLogger logger;
    json j = safeRead(filename);
    if (!j.contains("Admins")) j["Admins"] = json::object();
    if (j["Admins"].contains(data.getemployeeID())) {
        cout << "Admin with ID ( " << data.getemployeeID() << " ) already exists." << endl;
        logger.logError("Couldn't save admin with ID ( " + data.getemployeeID() + " ), already exists.");
        return;
    }
    json adminObj = json::object();
    adminObj["Username"]   = data.getemployeeID();
    adminObj["Password"]   = data.getpassword();
    adminObj["Department"] = data.getdepartment();
    adminObj["AdminLevel"] = data.getadminLevel();
    j["Admins"][data.getemployeeID()] = adminObj;
    if (safeWrite(filename, j)) {
        logger.logInfo("Admin with ID ( " + data.getemployeeID() + " ) was saved successfully.");
        cout << "data was saved" << endl;
    } else {
        cerr << "Failed to write file: " << filename << endl;
    }
}

void dataCollector::saveDataInFile(Customer data, dataCollector dc) {
    systemLogger logger;
    json j = safeRead(filename);
    if (!j.contains("Clients")) j["Clients"] = json::object();
    if (j["Clients"].contains(data.getUserName())) {
        cout << "Client with userName ( " << data.getUserName() << " ) already exists." << endl;
        logger.logError("Couldn't save client with userName ( " + data.getUserName() + " ), already exists.");
        return;
    }
    json clientObj = json::object();
    clientObj["name"]          = data.getName();
    clientObj["Password"]      = data.getpassword();
    clientObj["Balance"]       = data.getaccountBalance();
    clientObj["Email"]         = data.getEmail();
    clientObj["LoyaltyPoints"] = data.getloyaltyPoints();
    clientObj["Age"]           = data.getAge();
    clientObj["History"]       = json::array();
    j["Clients"][data.getUserName()] = clientObj;
    if (safeWrite(filename, j)) {
        logger.logInfo("Client with userName ( " + data.getUserName() + " ) was saved successfully.");
        cout << "data was saved" << endl;
    } else {
        cerr << "Failed to write file: " << filename << endl;
    }
}

void dataCollector::createFile(string name) {
    systemLogger logger;
    if (name.empty()) {
        cerr << "File name cannot be empty." << endl;
        logger.logError("Attempted to create a file with an empty name.");
        return;
    }
    if (name.find(".json") == string::npos && name.find(".txt") == string::npos) {
        cerr << "File name must end with .json or .txt" << endl;
        logger.logError("Attempted to create a file with an invalid extension: " + name);
        return;
    }
    ifstream checkfile(name);
    if (checkfile) {
        cerr << "This file exists" << endl;
        checkfile.close();
    } else {
        checkfile.close();
        ofstream openfile(name);
        openfile.close();
        cout << "A file with the name of ( " << name << " ) was created." << endl;
        logger.logInfo("File created: " + name);
        cout << "do you want to access it now?(y/n)" << endl;
        string answer;
        cin >> answer;
        while (answer != "N" && answer != "n" && answer != "y" && answer != "Y") {
            cout << answer << " is not an option choose y/n";
            cin >> answer;
        }
        if (answer == "y" || answer == "Y") {
            filename = name;
            cout << "The file that you are using now is " << name << endl;
            logger.logInfo("File set to: " + name);
        } else {
            cout << "The file that you are using now is " << filename << endl;
            logger.logInfo("File set to: " + filename);
        }
    }
}

Customer dataCollector::getCustomerDataFromFile(string username) {
    systemLogger logger;
    json j = safeRead(filename);
    if (j.is_null() || !j.contains("Clients")) {
        logger.logError("Failed to read file: " + filename);
        return Customer("");
    }
    if (j["Clients"].contains(username)) {
        string email    = j["Clients"][username].value("Email", "");
        double balance  = j["Clients"][username].value("Balance", 0.0);
        string name     = j["Clients"][username].value("name", "");
        string password = j["Clients"][username].value("Password", "");
        int    age      = j["Clients"][username].value("Age", 0);
        int    points   = j["Clients"][username].value("LoyaltyPoints", 0);
        logger.logInfo("Customer data retrieved for userName: " + username);
        return Customer(username, email, balance, name, password, age, points);
    }
    cout << "Client with userName ( " << username << " ) not found." << endl;
    logger.logError("Client with userName ( " + username + " ) not found in file: " + filename);
    return Customer("");
}

Admin dataCollector::getAdminDataFromFile(string id) {
    systemLogger logger;
    json j = safeRead(filename);
    if (j.is_null() || !j.contains("Admins")) {
        logger.logError("Failed to read file: " + filename);
        return Admin("");
    }
    if (j["Admins"].contains(id)) {
        string password   = j["Admins"][id].value("Password", "");
        string department = j["Admins"][id].value("Department", "");
        int    adminLevel = j["Admins"][id].value("AdminLevel", 1);
        logger.logInfo("Admin data retrieved for ID: " + id);
        return Admin(id, password, department, adminLevel);
    }
    cout << "Admin with ID ( " << id << " ) not found." << endl;
    logger.logError("Admin with ID ( " + id + " ) not found in file: " + filename);
    return Admin("");
}

void dataCollector::updateCustomerRecord(Customer& data) {
    json j = safeRead(filename);
    if (!j.contains("Clients"))                          j["Clients"] = json::object();
    if (!j["Clients"].contains(data.getUserName()))      j["Clients"][data.getUserName()] = json::object();
    auto& c = j["Clients"][data.getUserName()];
    if (!data.getName().empty())  c["name"]          = data.getName();
    c["Balance"]       = data.getaccountBalance();
    if (!data.getEmail().empty()) c["Email"]          = data.getEmail();
    c["LoyaltyPoints"] = data.getloyaltyPoints();
    safeWrite(filename, j);
}

void dataCollector::updateCustomerBalance(Customer& data) {
    json j = safeRead(filename);
    if (!j.contains("Clients"))                          j["Clients"] = json::object();
    if (!j["Clients"].contains(data.getUserName()))      j["Clients"][data.getUserName()] = json::object();
    j["Clients"][data.getUserName()]["Balance"] = data.getaccountBalance();
    safeWrite(filename, j);
}

void dataCollector::appendUserHistory(const string& userID, const string& action) {
    json j = safeRead(filename);
    if (!j.contains("Clients"))               j["Clients"] = json::object();
    if (!j["Clients"].contains(userID))       return;
    if (!j["Clients"][userID].contains("History"))
        j["Clients"][userID]["History"] = json::array();
    json entry = {{"Action", action}};
    j["Clients"][userID]["History"].push_back(entry);
    safeWrite(filename, j);
}

void dataCollector::updateInventoryAvailability(const string& itemId, int available) {
    string invFile = "Inventory.json";
    json j = safeRead(invFile);
    if (!j.contains("Inventory")) return;
    for (auto& it : j["Inventory"]) {
        if (it["id"] == itemId) { it["available"] = available; break; }
    }
    safeWrite(invFile, j);
}

void dataCollector::addInventoryItem(const string& id, const string& type,
                                     const string& destination, double price, int available) {
    string invFile = "Inventory.json";
    json j = safeRead(invFile);
    if (!j.contains("Inventory")) j["Inventory"] = json::array();
    json item = json::object();
    item["id"]          = id;
    item["type"]        = type;
    item["destination"] = destination;
    item["price"]       = price;
    item["available"]   = available;
    j["Inventory"].push_back(item);
    safeWrite(invFile, j);
}

void dataCollector::removeInventoryItem(const string& id) {
    string invFile = "Inventory.json";
    json j = safeRead(invFile);
    if (!j.contains("Inventory")) return;
    json arr = json::array();
    for (auto& it : j["Inventory"])
        if (it["id"] != id) arr.push_back(it);
    j["Inventory"] = arr;
    safeWrite(invFile, j);
}

void dataCollector::updateInventoryItemDetails(const string& id, const string& type,
                                               const string& destination, double price, int available) {
    string invFile = "Inventory.json";
    json j = safeRead(invFile);
    if (!j.contains("Inventory")) return;
    for (auto& it : j["Inventory"]) {
        if (it["id"] == id) {
            it["type"]        = type;
            it["destination"] = destination;
            it["price"]       = price;
            it["available"]   = available;
            break;
        }
    }
    safeWrite(invFile, j);
}
