#pragma once

#include "../Include/Common.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <sstream>
#include <fstream>

namespace APLG {

/**
 * @brief JSON value type
 * 
 * Supports null, boolean, number, string, array, and object types.
 */
class JsonValue {
public:
    using Null = std::monostate;
    using Bool = bool;
    using Number = float64;
    using String = std::string;
    using Array = std::vector<JsonValue>;
    using Object = std::unordered_map<std::string, JsonValue>;
    
private:
    std::variant<Null, Bool, Number, String, Array, Object> m_value;
    
public:
    JsonValue() : m_value(Null{}) {}
    JsonValue(Bool value) : m_value(value) {}
    JsonValue(Number value) : m_value(value) {}
    JsonValue(int32 value) : m_value(static_cast<Number>(value)) {}
    JsonValue(int64 value) : m_value(static_cast<Number>(value)) {}
    JsonValue(const String& value) : m_value(value) {}
    JsonValue(const char* value) : m_value(String(value)) {}
    JsonValue(const Array& value) : m_value(value) {}
    JsonValue(const Object& value) : m_value(value) {}
    
    // Type checking
    bool isNull() const { return std::holds_alternative<Null>(m_value); }
    bool isBool() const { return std::holds_alternative<Bool>(m_value); }
    bool isNumber() const { return std::holds_alternative<Number>(m_value); }
    bool isString() const { return std::holds_alternative<String>(m_value); }
    bool isArray() const { return std::holds_alternative<Array>(m_value); }
    bool isObject() const { return std::holds_alternative<Object>(m_value); }
    
    // Value getters
    Bool getBool() const {
        if (!isBool()) return false;
        return std::get<Bool>(m_value);
    }
    Number getNumber() const {
        if (!isNumber()) return 0.0;
        return std::get<Number>(m_value);
    }
    String getString() const {
        if (!isString()) return "";
        return std::get<String>(m_value);
    }
    Array getArray() const {
        if (!isArray()) return Array{};
        return std::get<Array>(m_value);
    }
    Object getObject() const {
        if (!isObject()) return Object{};
        return std::get<Object>(m_value);
    }
    
    // Array access
    JsonValue& operator[](size_t index) {
        if (!isArray()) {
            m_value = Array{};
        }
        auto& arr = std::get<Array>(m_value);
        if (index >= arr.size()) {
            arr.resize(index + 1);
        }
        return arr[index];
    }
    
    const JsonValue& operator[](size_t index) const {
        static JsonValue nullValue;
        if (!isArray()) return nullValue;
        auto& arr = std::get<Array>(m_value);
        if (index >= arr.size()) return nullValue;
        return arr[index];
    }
    
    // Object access
    JsonValue& operator[](const std::string& key) {
        if (!isObject()) {
            m_value = Object{};
        }
        return std::get<Object>(m_value)[key];
    }
    
    const JsonValue& operator[](const std::string& key) const {
        static JsonValue nullValue;
        if (!isObject()) return nullValue;
        auto& obj = std::get<Object>(m_value);
        auto it = obj.find(key);
        return (it != obj.end()) ? it->second : nullValue;
    }
    
    bool hasKey(const std::string& key) const {
        if (!isObject()) return false;
        auto& obj = std::get<Object>(m_value);
        return obj.find(key) != obj.end();
    }
    
    // Serialization
    std::string serialize(int indent = 0) const {
        std::stringstream ss;
        serializeImpl(ss, indent);
        return ss.str();
    }
    
private:
    void serializeImpl(std::stringstream& ss, int indent) const {
        std::string indentStr(indent * 2, ' ');
        
        if (isNull()) {
            ss << "null";
        } else if (isBool()) {
            ss << (getBool() ? "true" : "false");
        } else if (isNumber()) {
            ss << getNumber();
        } else if (isString()) {
            ss << "\"" << getString() << "\"";
        } else if (isArray()) {
            ss << "[";
            auto arr = getArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) ss << ", ";
                arr[i].serializeImpl(ss, indent);
            }
            ss << "]";
        } else if (isObject()) {
            ss << "{\n";
            auto obj = getObject();
            bool first = true;
            for (const auto& [key, value] : obj) {
                if (!first) ss << ",\n";
                ss << indentStr << "  \"" << key << "\": ";
                value.serializeImpl(ss, indent + 1);
                first = false;
            }
            ss << "\n" << indentStr << "}";
        }
    }
};

/**
 * @brief Simple JSON parser
 * 
 * Parses JSON strings into JsonValue objects.
 * Supports basic JSON syntax.
 */
class JsonParser {
public:
    static JsonValue parse(const std::string& json) {
        JsonParser parser(json);
        return parser.parseValue();
    }
    
    static JsonValue parseFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return JsonValue();
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }
    
private:
    JsonParser(const std::string& json) : m_json(json), m_pos(0) {}
    
    char peek() const {
        if (m_pos < m_json.length()) {
            return m_json[m_pos];
        }
        return '\0';
    }
    
    char consume() {
        if (m_pos < m_json.length()) {
            return m_json[m_pos++];
        }
        return '\0';
    }
    
    void skipWhitespace() {
        while (m_pos < m_json.length() && std::isspace(peek())) {
            m_pos++;
        }
    }
    
    JsonValue parseValue() {
        skipWhitespace();
        char c = peek();
        
        if (c == 'n') return parseNull();
        if (c == 't' || c == 'f') return parseBool();
        if (c == '"') return parseString();
        if (c == '[') return parseArray();
        if (c == '{') return parseObject();
        if (c == '-' || std::isdigit(c)) return parseNumber();
        
        return JsonValue();
    }
    
    JsonValue parseNull() {
        consume(); // 'n'
        consume(); // 'u'
        consume(); // 'l'
        consume(); // 'l'
        return JsonValue();
    }
    
    JsonValue parseBool() {
        if (peek() == 't') {
            consume(); consume(); consume(); consume(); // "true"
            return JsonValue(true);
        } else {
            consume(); consume(); consume(); consume(); consume(); // "false"
            return JsonValue(false);
        }
    }
    
    JsonValue parseNumber() {
        size_t start = m_pos;
        if (peek() == '-') consume();
        
        while (m_pos < m_json.length() && std::isdigit(peek())) {
            consume();
        }
        
        if (peek() == '.') {
            consume();
            while (m_pos < m_json.length() && std::isdigit(peek())) {
                consume();
            }
        }
        
        std::string numStr = m_json.substr(start, m_pos - start);
        return JsonValue(std::stod(numStr));
    }
    
    JsonValue parseString() {
        consume(); // '"'
        std::string result;
        
        while (peek() != '"' && m_pos < m_json.length()) {
            char c = consume();
            if (c == '\\') {
                c = consume();
                if (c == 'n') c = '\n';
                else if (c == 't') c = '\t';
                else if (c == 'r') c = '\r';
            }
            result += c;
        }
        
        consume(); // closing '"'
        return JsonValue(result);
    }
    
    JsonValue parseArray() {
        consume(); // '['
        JsonValue::Array arr;
        
        skipWhitespace();
        if (peek() == ']') {
            consume();
            return JsonValue(arr);
        }
        
        do {
            skipWhitespace();
            arr.push_back(parseValue());
            skipWhitespace();
        } while (peek() == ',' && consume());
        
        consume(); // ']'
        return JsonValue(arr);
    }
    
    JsonValue parseObject() {
        consume(); // '{'
        JsonValue::Object obj;
        
        skipWhitespace();
        if (peek() == '}') {
            consume();
            return JsonValue(obj);
        }
        
        do {
            skipWhitespace();
            std::string key = parseString().getString();
            skipWhitespace();
            consume(); // ':'
            skipWhitespace();
            obj[key] = parseValue();
            skipWhitespace();
        } while (peek() == ',' && consume());
        
        consume(); // '}'
        return JsonValue(obj);
    }
    
    const std::string& m_json;
    size_t m_pos;
};

/**
 * @brief JSON file writer
 */
class JsonWriter {
public:
    static bool writeToFile(const std::string& filename, const JsonValue& value) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << value.serialize(2);
        return true;
    }

    static bool writeToFile(const std::string& filename, const std::string& jsonContent) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonContent;
        return true;
    }
};

} // namespace APLG
