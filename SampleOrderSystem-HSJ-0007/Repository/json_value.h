#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Json {

enum class Type { Null, Bool, Number, String, Array, Object };

// 외부 의존성 없는 최소 JSON 값 타입.
// object는 삽입 순서를 보존하기 위해 (key, value) 벡터로 저장한다.
class Value {
public:
    Value();
    Value(bool value);
    Value(double value);
    Value(int value);
    Value(const std::string& value);
    Value(const char* value);

    static Value makeArray();
    static Value makeObject();
    static Value parse(const std::string& text);

    Type type() const { return type_; }
    bool isNull() const { return type_ == Type::Null; }

    bool asBool() const;
    double asNumber() const;
    std::string asString() const;

    // Object
    Value& operator[](const std::string& key);
    const Value* find(const std::string& key) const;

    // Array
    void push_back(Value value);
    size_t size() const;
    Value& operator[](size_t index);
    const Value& operator[](size_t index) const;
    const std::vector<Value>& items() const { return array_; }

    std::string dump() const;

private:
    Type type_ = Type::Null;
    bool bool_ = false;
    double number_ = 0.0;
    std::string string_;
    std::vector<Value> array_;
    std::vector<std::pair<std::string, Value>> object_;

    void dumpTo(std::string& out) const;
};

}  // namespace Json
