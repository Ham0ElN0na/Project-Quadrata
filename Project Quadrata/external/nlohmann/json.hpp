// Minimal lightweight stub of nlohmann::json to allow project to compile.
// This is NOT a full implementation. It provides only the limited API used by
// the project source files (contains, operator[], size, push_back, dump, array(), object(), value, iteration, basic assignment).

#ifndef MINIMAL_NLOHMANN_JSON_HPP
#define MINIMAL_NLOHMANN_JSON_HPP

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace nlohmann {

class json {
public:
	using object_t = std::map<std::string, json>;
	using array_t = std::vector<json>;
	using value_t = std::variant<std::nullptr_t, bool, double, std::string, array_t, object_t>;

private:
	value_t m_value;

public:
	json() noexcept : m_value(nullptr) {}
	json(std::nullptr_t) noexcept : m_value(nullptr) {}
	json(bool b) : m_value(b) {}
	json(int i) : m_value(static_cast<double>(i)) {}
	json(double d) : m_value(d) {}
	json(const char* s) : m_value(std::string(s)) {}
	json(const std::string& s) : m_value(s) {}
	json(const array_t& a) : m_value(a) {}
	json(const object_t& o) : m_value(o) {}
	// construct object from initializer list of pairs: { {"key", value}, ... }
	json(std::initializer_list<std::pair<const std::string, json>> init) : m_value(object_t()) { for (auto &p : init) std::get<object_t>(m_value)[p.first] = p.second; }
	// construct array from initializer list of json values
	json(std::initializer_list<json> init) : m_value(array_t()) { for (auto &v : init) std::get<array_t>(m_value).push_back(v); }

	static json object() { return json(object_t{}); }
	static json array() { return json(array_t{}); }

	bool is_object() const { return std::holds_alternative<object_t>(m_value); }
	bool is_array() const { return std::holds_alternative<array_t>(m_value); }
	bool is_string() const { return std::holds_alternative<std::string>(m_value); }
	bool is_number() const { return std::holds_alternative<double>(m_value); }

	bool contains(const std::string& key) const {
		if (!is_object()) return false;
		const auto& obj = std::get<object_t>(m_value);
		return obj.find(key) != obj.end();
	}

	// operator[] for object access (creates if missing)
	json& operator[](const std::string& key) {
		if (!is_object()) m_value = object_t{};
		auto& obj = std::get<object_t>(m_value);
		return obj[key];
	}

	const json& operator[](const std::string& key) const {
		static json nulljson;
		if (!is_object()) return nulljson;
		const auto& obj = std::get<object_t>(m_value);
		auto it = obj.find(key);
		if (it == obj.end()) return nulljson;
		return it->second;
	}

	// operator[] for array index
	json& operator[](size_t idx) {
		if (!is_array()) m_value = array_t{};
		auto& arr = std::get<array_t>(m_value);
		if (idx >= arr.size()) arr.resize(idx + 1);
		return arr[idx];
	}

	const json& operator[](size_t idx) const {
		static json nulljson;
		if (!is_array()) return nulljson;
		const auto& arr = std::get<array_t>(m_value);
		if (idx >= arr.size()) return nulljson;
		return arr[idx];
	}

	size_t size() const {
		if (is_array()) return std::get<array_t>(m_value).size();
		if (is_object()) return std::get<object_t>(m_value).size();
		return 0;
	}

	// value(key, default)
	std::string value(const std::string& key, const std::string& def) const {
		if (!is_object()) return def;
		const auto& obj = std::get<object_t>(m_value);
		auto it = obj.find(key);
		if (it == obj.end()) return def;
		if (it->second.is_string()) return std::get<std::string>(it->second.m_value);
		if (it->second.is_number()) {
			std::ostringstream ss; ss << std::get<double>(it->second.m_value);
			return ss.str();
		}
		return def;
	}

	double value(const std::string& key, double def) const {
		if (!is_object()) return def;
		const auto& obj = std::get<object_t>(m_value);
		auto it = obj.find(key);
		if (it == obj.end()) return def;
		if (it->second.is_number()) return std::get<double>(it->second.m_value);
		if (it->second.is_string()) {
			try { return std::stod(std::get<std::string>(it->second.m_value)); } catch(...) { return def; }
		}
		return def;
	}

	int value(const std::string& key, int def) const { return static_cast<int>(value(key, static_cast<double>(def))); }

	void push_back(const json& v) {
		if (!is_array()) m_value = array_t{};
		std::get<array_t>(m_value).push_back(v);
	}

	json& at(const std::string& key) { return operator[](key); }

	// assignment operators
	json& operator=(const std::string& s) { m_value = s; return *this; }
	json& operator=(const char* s) { m_value = std::string(s); return *this; }
	json& operator=(double d) { m_value = d; return *this; }
	json& operator=(int i) { m_value = static_cast<double>(i); return *this; }
	json& operator=(bool b) { m_value = b; return *this; }
	json& operator=(const array_t& a) { m_value = a; return *this; }
	json& operator=(const object_t& o) { m_value = o; return *this; }
	json& operator=(std::initializer_list<std::pair<const std::string, json>> init) { m_value = object_t(init.begin(), init.end()); return *this; }

	// equality with string
	bool operator==(const std::string& other) const {
		return is_string() && std::get<std::string>(m_value) == other;
	}

	// conversions to primitive types to mimic nlohmann::json implicit conversions
	operator std::string() const {
		if (is_string()) return std::get<std::string>(m_value);
		if (is_number()) {
			std::ostringstream ss; ss << std::get<double>(m_value); return ss.str();
		}
		return dump();
	}

	operator double() const {
		if (is_number()) return std::get<double>(m_value);
		if (is_string()) {
			try { return std::stod(std::get<std::string>(m_value)); } catch(...) { return 0.0; }
		}
		if (std::holds_alternative<bool>(m_value)) return std::get<bool>(m_value) ? 1.0 : 0.0;
		return 0.0;
	}

	operator int() const { return static_cast<int>(static_cast<double>(*this)); }

	// iteration support for arrays
	using iterator = array_t::iterator;
	using const_iterator = array_t::const_iterator;
	iterator begin() { if (!is_array()) m_value = array_t{}; return std::get<array_t>(m_value).begin(); }
	iterator end() { if (!is_array()) m_value = array_t{}; return std::get<array_t>(m_value).end(); }
	const_iterator begin() const { static array_t empty; if (!is_array()) return empty.end(); return std::get<array_t>(m_value).begin(); }
	const_iterator end() const { static array_t empty; if (!is_array()) return empty.end(); return std::get<array_t>(m_value).end(); }

	// dump (simple serializer)
	std::string dump(int indent = -1) const {
		std::ostringstream oss;
		dump_impl(oss, indent, 0);
		return oss.str();
	}

private:
	void dump_impl(std::ostringstream& oss, int indent, int level) const {
		if (std::holds_alternative<std::nullptr_t>(m_value)) {
			oss << "null";
		} else if (std::holds_alternative<bool>(m_value)) {
			oss << (std::get<bool>(m_value) ? "true" : "false");
		} else if (std::holds_alternative<double>(m_value)) {
			oss << std::get<double>(m_value);
		} else if (std::holds_alternative<std::string>(m_value)) {
			oss << '"' << escape_string(std::get<std::string>(m_value)) << '"';
		} else if (std::holds_alternative<array_t>(m_value)) {
			const auto& arr = std::get<array_t>(m_value);
			oss << '[';
			bool first = true;
			for (const auto& el : arr) {
				if (!first) oss << ',';
				if (indent >= 0) oss << '\n' << std::string((level+1)*indent, ' ');
				el.dump_impl(oss, indent, level+1);
				first = false;
			}
			if (indent >= 0 && !arr.empty()) oss << '\n' << std::string(level*indent, ' ');
			oss << ']';
		} else if (std::holds_alternative<object_t>(m_value)) {
			const auto& obj = std::get<object_t>(m_value);
			oss << '{';
			bool first = true;
			for (const auto& [k, v] : obj) {
				if (!first) oss << ',';
				if (indent >= 0) oss << '\n' << std::string((level+1)*indent, ' ');
				oss << '"' << escape_string(k) << '": ';
				v.dump_impl(oss, indent, level+1);
				first = false;
			}
			if (indent >= 0 && !obj.empty()) oss << '\n' << std::string(level*indent, ' ');
			oss << '}';
		}
	}

	static std::string escape_string(const std::string& s) {
		std::string out;
		for (char c : s) {
			switch (c) {
				case '"': out += "\\\""; break;
				case '\\': out += "\\\\"; break;
				case '\b': out += "\\b"; break;
				case '\f': out += "\\f"; break;
				case '\n': out += "\\n"; break;
				case '\r': out += "\\r"; break;
				case '\t': out += "\\t"; break;
				default: out += c; break;
			}
		}
		return out;
	}

public:
	// simple parser: we don't aim to fully parse JSON; we will try to detect arrays/objects
	friend std::istream& operator>>(std::istream& is, json& j) {
		std::ostringstream ss;
		ss << is.rdbuf();
		std::string s = ss.str();
		j = json::object();

		auto skip_ws = [&](size_t &p){ while (p < s.size() && (s[p]==' '||s[p]=='\n'||s[p]=='\r'||s[p]=='\t')) ++p; };

		auto find_matching_brace = [&](size_t start)->size_t {
			if (start >= s.size() || s[start] != '{') return std::string::npos;
			int depth = 0;
			for (size_t k = start; k < s.size(); ++k) {
				if (s[k] == '{') ++depth;
				else if (s[k] == '}') {
					--depth;
					if (depth == 0) return k;
				}
			}
			return std::string::npos;
		};

		auto parse_value_string = [&](const std::string &src, size_t pos)->std::string {
			// pos points to '"'
			size_t st = pos;
			if (st >= src.size() || src[st] != '"') return std::string();
			size_t en = src.find('"', st+1);
			if (en == std::string::npos) return std::string();
			return src.substr(st+1, en - st - 1);
		};

		auto parse_simple_value = [&](const std::string &src, size_t pos)->std::string {
			// parse until comma or closing brace
			size_t i = pos;
			while (i < src.size() && src[i] != ',' && src[i] != '}' && src[i] != ']') ++i;
			std::string token = src.substr(pos, i - pos);
			// trim
			size_t a = 0; while (a < token.size() && isspace((unsigned char)token[a])) ++a;
			size_t b = token.size(); while (b> a && isspace((unsigned char)token[b-1])) --b;
			return token.substr(a, b-a);
		};

		auto parse_section = [&](const std::string &name){
			size_t p = s.find('"' + name + '"');
			if (p == std::string::npos) return;
			size_t brace = s.find('{', p);
			if (brace == std::string::npos) return;
			size_t end_brace = find_matching_brace(brace);
			if (end_brace == std::string::npos) return;
			size_t i = brace + 1;
			skip_ws(i);
			// iterate entries: "key": { ... },
			while (i < end_brace) {
				skip_ws(i);
				if (i >= end_brace) break;
				if (s[i] == '}') break;
				// find name
				if (s[i] != '"') break;
				size_t name_start = i;
				size_t name_end = s.find('"', name_start+1);
				if (name_end == std::string::npos || name_end+1 >= s.size()) break;
				std::string key = s.substr(name_start+1, name_end - name_start -1);
				// find colon
				size_t colon = s.find(':', name_end);
				if (colon == std::string::npos) break;
				// find the next non-ws char
				size_t vpos = colon+1; skip_ws(vpos);
				if (vpos >= s.size()) break;
				if (s[vpos] == '{') {
					size_t inner_start = vpos;
					size_t inner_end = find_matching_brace(inner_start);
					if (inner_end == std::string::npos) break;
					std::string inner = s.substr(inner_start, inner_end - inner_start + 1);
					// build object
					json obj = json::object();
					// list of known keys we care about
					const std::vector<std::string> keys = {"Password","Email","Balance","name","Age","LoyaltyPoints","AdminLevel","Department","Username","History"};
					for (const auto &kname : keys) {
						size_t kp = inner.find('"' + kname + '"');
						if (kp == std::string::npos) continue;
						size_t kcolon = inner.find(':', kp);
						if (kcolon == std::string::npos) continue;
						size_t valpos = kcolon+1; while (valpos < inner.size() && isspace((unsigned char)inner[valpos])) ++valpos;
						if (valpos >= inner.size()) continue;
						if (inner[valpos] == '"') {
							std::string v = parse_value_string(inner, valpos);
							obj[kname] = v;
						} else if (inner[valpos] == '[') {
							// simple: treat as empty array
							obj[kname] = json::array();
						} else {
							// number or bare value
							std::string tok = parse_simple_value(inner, valpos);
							// remove trailing commas or spaces
							// try to detect integer/number
							bool is_num = false; for (char ch: tok) if ((ch>='0'&&ch<='9')||ch=='.'||ch=='-'||ch=='+') { is_num=true; break; }
							if (is_num) {
								try {
									double num = std::stod(tok);
									// if integer
									int inum = static_cast<int>(num);
									if (std::fabs(num - inum) < 1e-9) obj[kname] = inum; else obj[kname] = num;
								} catch(...) { obj[kname] = tok; }
							} else {
								// fallback: string trimmed of quotes
								// remove possible quotes
								size_t q1 = tok.find('"');
								if (q1 != std::string::npos) {
									std::string v = parse_value_string(tok, q1);
									obj[kname] = v;
								} else obj[kname] = tok;
							}
						}
					}
					j[name][key] = obj;
					i = inner_end + 1;
					// skip comma
					size_t comma = s.find(',', i);
					if (comma == std::string::npos || comma > end_brace) { i = end_brace; break; }
					i = comma + 1;
				} else {
					// value not an object, skip
					size_t nextcomma = s.find(',', vpos);
					if (nextcomma == std::string::npos || nextcomma > end_brace) { i = end_brace; break; }
					i = nextcomma + 1;
				}
				skip_ws(i);
			}
		};

		parse_section("Clients");
		parse_section("Admins");
		// keep Inventory as empty array if present
		// parse Inventory array of objects into j["Inventory"]
		size_t invpos = s.find('"' + std::string("Inventory") + '"');
		if (invpos != std::string::npos) {
			size_t bracket = s.find('[', invpos);
			if (bracket != std::string::npos) {
				// find matching closing bracket
				auto find_matching_bracket = [&](size_t start)->size_t {
					if (start >= s.size() || s[start] != '[') return std::string::npos;
					int depth = 0;
					for (size_t k = start; k < s.size(); ++k) {
						if (s[k] == '[') ++depth;
						else if (s[k] == ']') { --depth; if (depth == 0) return k; }
					}
					return std::string::npos;
				};
				size_t endbr = find_matching_bracket(bracket);
				if (endbr != std::string::npos) {
					size_t p = bracket + 1;
					j["Inventory"] = json::array();
					while (p < endbr) {
						// find next object
						skip_ws(p);
						if (p >= endbr) break;
						size_t objstart = s.find('{', p);
						if (objstart == std::string::npos || objstart >= endbr) break;
						size_t objend = find_matching_brace(objstart);
						if (objend == std::string::npos || objend > endbr) break;
						std::string inner = s.substr(objstart, objend - objstart + 1);
						json item = json::object();
						const std::vector<std::string> ikeys = {"id","type","destination","price","available"};
						for (const auto &kname : ikeys) {
							size_t kp = inner.find('"' + kname + '"');
							if (kp == std::string::npos) continue;
							size_t kcolon = inner.find(':', kp);
							if (kcolon == std::string::npos) continue;
							size_t valpos = kcolon + 1; while (valpos < inner.size() && isspace((unsigned char)inner[valpos])) ++valpos;
							if (valpos >= inner.size()) continue;
							if (inner[valpos] == '"') {
								std::string v = parse_value_string(inner, valpos);
								item[kname] = v;
							} else {
								std::string tok = parse_simple_value(inner, valpos);
								bool is_num = false; for (char ch: tok) if ((ch>='0'&&ch<='9')||ch=='.'||ch=='-'||ch=='+') { is_num=true; break; }
								if (is_num) {
									try { double num = std::stod(tok); int inum = static_cast<int>(num); if (std::fabs(num - inum) < 1e-9) item[kname] = inum; else item[kname] = num; } catch(...) { item[kname] = tok; }
								} else item[kname] = tok;
							}
						}
						j["Inventory"].push_back(item);
						p = objend + 1;
					}
				}
			}
		}
		return is;
	}
};

} // namespace nlohmann

#endif // MINIMAL_NLOHMANN_JSON_HPP
