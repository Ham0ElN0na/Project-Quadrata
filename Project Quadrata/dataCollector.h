#pragma once
#include <string>
#include "UserClasses.h"
using namespace std;

class dataCollector {
private:
    string filename;
public:
    dataCollector();
    dataCollector(string name);
    void createFile(string name);
    void saveDataInFile(string data);
    void selectFilePath(string name);
    void saveDataInFile(Admin data, dataCollector dc);
    void saveDataInFile(Customer data, dataCollector dc);
    string getPassWordFromFile(Admin data);
    string getPassWordFromFile(Customer data);
    Customer getCustomerDataFromFile(string name);
	Admin getAdminDataFromFile(string id);
    void updateCustomerRecord(Customer& data);
    void appendUserHistory(const string& userID, const string& action);
    void updateInventoryAvailability(const string& itemId, int available);
    void addInventoryItem(const string& id, const string& type, const string& destination, double price, int available);
    void removeInventoryItem(const string& id);
    void updateInventoryItemDetails(const string& id, const string& type, const string& destination, double price, int available);
    void updateCustomerBalance(Customer& data);
};
