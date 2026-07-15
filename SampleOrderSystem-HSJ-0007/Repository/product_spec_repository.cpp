#include "Repository/product_spec_repository.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Repository/json_value.h"

namespace {

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

ProductSpec fromJson(const Json::Value& json) {
    ProductSpec spec;
    if (auto* v = json.find("productSpecId")) spec.productSpecId = v->asString();
    if (auto* v = json.find("name")) spec.name = v->asString();
    if (auto* v = json.find("avgProductionTime")) spec.avgProductionTime = v->asNumber();
    if (auto* v = json.find("yield")) spec.yield = v->asNumber();
    if (auto* v = json.find("stock")) spec.stock = static_cast<int>(v->asNumber());
    if (auto* v = json.find("availableStock")) spec.availableStock = static_cast<int>(v->asNumber());
    return spec;
}

Json::Value toJson(const ProductSpec& spec) {
    Json::Value json = Json::Value::makeObject();
    json["productSpecId"] = Json::Value(spec.productSpecId);
    json["name"] = Json::Value(spec.name);
    json["avgProductionTime"] = Json::Value(spec.avgProductionTime);
    json["yield"] = Json::Value(spec.yield);
    json["stock"] = Json::Value(spec.stock);
    json["availableStock"] = Json::Value(spec.availableStock);
    return json;
}

}  // namespace

ProductSpecRepository::ProductSpecRepository(std::string filePath) : filePath_(std::move(filePath)) {
    load();
}

void ProductSpecRepository::load() {
    items_.clear();
    std::ifstream in(filePath_, std::ios::binary);
    if (!in.is_open()) {
        return;  // 파일이 없으면 빈 목록으로 시작 (Safety Test)
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    std::string text = buffer.str();
    if (text.empty()) {
        return;
    }
    Json::Value root = Json::Value::parse(text);
    for (size_t i = 0; i < root.size(); ++i) {
        items_.push_back(fromJson(root[i]));
    }
}

void ProductSpecRepository::save() const {
    std::filesystem::path path(filePath_);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    Json::Value root = Json::Value::makeArray();
    for (const auto& item : items_) {
        root.push_back(toJson(item));
    }
    std::ofstream out(filePath_, std::ios::binary | std::ios::trunc);
    out << root.dump();
}

void ProductSpecRepository::reload() { load(); }

std::vector<ProductSpec> ProductSpecRepository::findAll() const { return items_; }

std::optional<ProductSpec> ProductSpecRepository::findById(const std::string& productSpecId) const {
    for (const auto& item : items_) {
        if (item.productSpecId == productSpecId) {
            return item;
        }
    }
    return std::nullopt;
}

std::vector<ProductSpec> ProductSpecRepository::findByName(const std::string& keyword) const {
    std::vector<ProductSpec> result;
    std::string lowerKeyword = toLower(keyword);
    for (const auto& item : items_) {
        if (toLower(item.name).find(lowerKeyword) != std::string::npos) {
            result.push_back(item);
        }
    }
    return result;
}

void ProductSpecRepository::add(const ProductSpec& spec) {
    items_.push_back(spec);
    save();
}

void ProductSpecRepository::update(const ProductSpec& spec) {
    for (auto& item : items_) {
        if (item.productSpecId == spec.productSpecId) {
            item = spec;
            save();
            return;
        }
    }
}

void ProductSpecRepository::remove(const std::string& productSpecId) {
    items_.erase(std::remove_if(items_.begin(), items_.end(),
                                 [&](const ProductSpec& item) { return item.productSpecId == productSpecId; }),
                 items_.end());
    save();
}
