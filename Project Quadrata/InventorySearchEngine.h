#ifndef INVENTORYSEARCHENGINE_H
#define INVENTORYSEARCHENGINE_H

#include "Inventory.h"
#include <vector>
#include <string>

using namespace std;

class InventorySearchEngine {
public:
	vector<InventoryItem> search(const vector<InventoryItem>& items, string destination, double minPrice, double maxPrice);
};

#endif // INVENTORYSEARCHENGINE_H
