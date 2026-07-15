#include "Repository/json_value.h"

#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace Json {

Value::Value() : type_(Type::Null) {}
Value::Value(bool value) : type_(Type::Bool), bool_(value) {}
Value::Value(double value) : type_(Type::Number), number_(value) {}
Value::Value(int value) : type_(Type::Number), number_(static_cast<double>(value)) {}
Value::Value(const std::string& value) : type_(Type::String), string_(value) {}
Value::Value(const char* value) : type_(Type::String), string_(value) {}

Value Value::makeArray() {
    Value v;
    v.type_ = Type::Array;
    return v;
}

Value Value::makeObject() {
    Value v;
    v.type_ = Type::Object;
    return v;
}

bool Value::asBool() const { return bool_; }
double Value::asNumber() const { return number_; }
std::string Value::asString() const { return string_; }

Value& Value::operator[](const std::string& key) {
    type_ = Type::Object;
    for (auto& pair : object_) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    object_.emplace_back(key, Value());
    return object_.back().second;
}

const Value* Value::find(const std::string& key) const {
    if (type_ != Type::Object) {
        return nullptr;
    }
    for (const auto& pair : object_) {
        if (pair.first == key) {
            return &pair.second;
        }
    }
    return nullptr;
}

void Value::push_back(Value value) {
    type_ = Type::Array;
    array_.push_back(std::move(value));
}

size_t Value::size() const { return array_.size(); }

Value& Value::operator[](size_t index) { return array_[index]; }
const Value& Value::operator[](size_t index) const { return array_[index]; }

namespace {

void escapeInto(const std::string& s, std::string& out) {
    out += '"';
    for (char c : s) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                out += c;
        }
    }
    out += '"';
}

std::string numberToString(double n) {
    if (n == std::floor(n) && std::abs(n) < 1e15) {
        return std::to_string(static_cast<long long>(n));
    }
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

class Parser {
public:
    explicit Parser(const std::string& text) : text_(text) {}

    Value parse() {
        skipWhitespace();
        Value v = parseValue();
        skipWhitespace();
        return v;
    }

private:
    const std::string& text_;
    size_t pos_ = 0;

    char peek() const {
        if (pos_ >= text_.size()) {
            throw std::runtime_error("Json::parse: unexpected end of input");
        }
        return text_[pos_];
    }

    char next() { return text_[pos_++]; }

    void skipWhitespace() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool consume(char expected) {
        skipWhitespace();
        if (pos_ < text_.size() && text_[pos_] == expected) {
            ++pos_;
            return true;
        }
        return false;
    }

    void expect(char expected) {
        if (!consume(expected)) {
            throw std::runtime_error(std::string("Json::parse: expected '") + expected + "'");
        }
    }

    Value parseValue() {
        skipWhitespace();
        char c = peek();
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return Value(parseString());
        if (c == 't' || c == 'f') return parseBool();
        if (c == 'n') return parseNull();
        return Value(parseNumber());
    }

    Value parseObject() {
        expect('{');
        Value obj = Value::makeObject();
        skipWhitespace();
        if (consume('}')) {
            return obj;
        }
        while (true) {
            skipWhitespace();
            std::string key = parseString();
            expect(':');
            obj[key] = parseValue();
            skipWhitespace();
            if (consume(',')) {
                continue;
            }
            expect('}');
            break;
        }
        return obj;
    }

    Value parseArray() {
        expect('[');
        Value arr = Value::makeArray();
        skipWhitespace();
        if (consume(']')) {
            return arr;
        }
        while (true) {
            arr.push_back(parseValue());
            skipWhitespace();
            if (consume(',')) {
                continue;
            }
            expect(']');
            break;
        }
        return arr;
    }

    std::string parseString() {
        skipWhitespace();
        expect('"');
        std::string out;
        while (true) {
            char c = next();
            if (c == '"') {
                break;
            }
            if (c == '\\') {
                char esc = next();
                switch (esc) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    default: out += esc;
                }
            } else {
                out += c;
            }
        }
        return out;
    }

    double parseNumber() {
        size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        while (pos_ < text_.size() &&
               (std::isdigit(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '.' ||
                text_[pos_] == 'e' || text_[pos_] == 'E' || text_[pos_] == '-' || text_[pos_] == '+')) {
            ++pos_;
        }
        return std::stod(text_.substr(start, pos_ - start));
    }

    Value parseBool() {
        if (text_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return Value(true);
        }
        if (text_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return Value(false);
        }
        throw std::runtime_error("Json::parse: invalid literal");
    }

    Value parseNull() {
        if (text_.compare(pos_, 4, "null") == 0) {
            pos_ += 4;
            return Value();
        }
        throw std::runtime_error("Json::parse: invalid literal");
    }
};

}  // namespace

Value Value::parse(const std::string& text) {
    Parser parser(text);
    return parser.parse();
}

void Value::dumpTo(std::string& out) const {
    switch (type_) {
        case Type::Null:
            out += "null";
            break;
        case Type::Bool:
            out += bool_ ? "true" : "false";
            break;
        case Type::Number:
            out += numberToString(number_);
            break;
        case Type::String:
            escapeInto(string_, out);
            break;
        case Type::Array: {
            out += '[';
            for (size_t i = 0; i < array_.size(); ++i) {
                if (i > 0) out += ',';
                array_[i].dumpTo(out);
            }
            out += ']';
            break;
        }
        case Type::Object: {
            out += '{';
            for (size_t i = 0; i < object_.size(); ++i) {
                if (i > 0) out += ',';
                escapeInto(object_[i].first, out);
                out += ':';
                object_[i].second.dumpTo(out);
            }
            out += '}';
            break;
        }
    }
}

std::string Value::dump() const {
    std::string out;
    dumpTo(out);
    return out;
}

}  // namespace Json
