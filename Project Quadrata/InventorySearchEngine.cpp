#include "InventorySearchEngine.h"
#include <algorithm>
#include <cctype>

vector<InventoryItem> InventorySearchEngine::search(const vector<InventoryItem>& items, string destination, double minPrice, double maxPrice) {
	vector<InventoryItem> results;
	string destLower = destination;
	transform(destLower.begin(), destLower.end(), destLower.begin(), [](unsigned char c){ return std::tolower(c); });
	for (const auto &it : items) {
		if (it.getNumber() <= 0) continue;
		string itemDest = it.getDestination();
		transform(itemDest.begin(), itemDest.end(), itemDest.begin(), [](unsigned char c){ return std::tolower(c); });
		if (!destLower.empty() && itemDest.find(destLower) == string::npos) continue;
		if (it.getPrice() < minPrice || it.getPrice() > maxPrice) continue;
		results.push_back(it);
	}
	return results;
}
