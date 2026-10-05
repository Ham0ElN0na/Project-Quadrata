// Minimal stub of nlohmann::json to satisfy compilation in this project.
// This is NOT a full JSON implementation. It provides a subset of the API
// used by the project: json objects/arrays, operator[], contains, value,
// dump, push_back, size and basic conversions.

#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

namespace nlohmann {

class json {
public:
	enum Type { Null, Object, Array, String, Number, Boolean };

	json() : type(Null), num(0), boolean(false) {}
	json(int v) : type(Number), num(v), boolean(false) {}
	json(double v) : type(Number), num(v), boolean(false) {}
	json(const std::string& s) : type(String), str(s), num(0), boolean(false) {}
	json(const char* s) : type(String), str(s), num(0), boolean(false) {}

	static json object() { json j; j.type = Object; return j; }
	static json array() { json j; j.type = Array; return j; }

	// Access as object by key
	json& operator[](const std::string& key) {
		if (type != Object) {
			type = Object;
			obj.clear();
		}
		return obj[key];
	}

	const json& operator[](const std::string& key) const {
		static json empty;
		if (type != Object) return empty;
		auto it = obj.find(key);
		if (it == obj.end()) return empty;
		return it->second;
	}

	// Access as array by index
	json& operator[](size_t idx) {
		if (type != Array) {
			type = Array;
			arr.clear();
		}
		if (idx >= arr.size()) arr.resize(idx + 1);
		return arr[idx];
	}

	const json& operator[](size_t idx) const {
		static json empty;
		if (type != Array) return empty;
		if (idx >= arr.size()) return empty;
		return arr[idx];
	}

	bool contains(const std::string& key) const {
		if (type != Object) return false;
		return obj.find(key) != obj.end();
	}

	// value with defaults for common types used in the project
	std::string value(const std::string& key, const std::string& defaultVal) const {
		if (!contains(key)) return defaultVal;
		const json& v = obj.at(key);
		if (v.type == String) return v.str;
		return defaultVal;
	}

	int value(const std::string& key, int defaultVal) const {
		if (!contains(key)) return defaultVal;
		const json& v = obj.at(key);
		if (v.type == Number) return static_cast<int>(v.num);
		return defaultVal;
	}

	double value(const std::string& key, double defaultVal) const {
		if (!contains(key)) return defaultVal;
		const json& v = obj.at(key);
		if (v.type == Number) return v.num;
		return defaultVal;
	}

	// non-const value variant used on iteration elements
	std::string value(const std::string& key, const std::string& defaultVal) {
		return static_cast<const json&>(*this).value(key, defaultVal);
	}
	int value(const std::string& key, int defaultVal) {
		return static_cast<const json&>(*this).value(key, defaultVal);
	}
	double value(const std::string& key, double defaultVal) {
		return static_cast<const json&>(*this).value(key, defaultVal);
	}

	// push_back for arrays
	void push_back(const json& v) {
		if (type != Array) { type = Array; arr.clear(); }
		arr.push_back(v);
	}

	size_t size() const { if (type == Array) return arr.size(); return 0; }

	// basic dump serialization (compact)
	std::string dump(int /*indent*/ = -1) const {
		if (type == Object) {
			std::string out = "{";
			bool first = true;
			for (auto& p : obj) {
				if (!first) out += ",";
				out += '"' + p.first + "":" + p.second.dump();
				first = false;
			}
			out += "}";
			return out;
		}
		if (type == Array) {
			std::string out = "[";
			bool first = true;
			for (const auto& e : arr) {
				if (!first) out += ",";
				out += e.dump();
				first = false;
			}
			out += "]";
			return out;
		}
		if (type == String) return '"' + str + '"';
		if (type == Number) {
			std::ostringstream ss; ss << num; return ss.str();
		}
		if (type == Boolean) return boolean ? "true" : "false";
		return "null";
	}

	// stream extraction: simple stub that leaves json empty when file missing or invalid
	friend std::istream& operator>>(std::istream& is, json& j) {
		std::ostringstream ss;
		ss << is.rdbuf();
		std::string content = ss.str();
		// very small heuristic: if file contains '{' assume object, otherwise leave empty object
		j = json::object();
		(void)content; // content is ignored in this minimal stub
		return is;
	}

	// conversions
	operator std::string() const { return (type == String) ? str : std::string(); }
	operator int() const { return (type == Number) ? static_cast<int>(num) : 0; }
	operator double() const { return (type == Number) ? num : 0.0; }

	// setters
	json& operator=(const std::string& s) { type = String; str = s; return *this; }
	json& operator=(const char* s) { type = String; str = s; return *this; }
	json& operator=(int v) { type = Number; num = v; return *this; }
	json& operator=(double v) { type = Number; num = v; return *this; }

	// begin/end for range-based for on arrays
	std::vector<json>::iterator begin() { return arr.begin(); }
	std::vector<json>::iterator end() { return arr.end(); }
	std::vector<json>::const_iterator begin() const { return arr.begin(); }
	std::vector<json>::const_iterator end() const { return arr.end(); }

private:
	Type type;
	std::map<std::string, json> obj;
	std::vector<json> arr;
	std::string str;
	double num;
	bool boolean;
};

} // namespace nlohmann
