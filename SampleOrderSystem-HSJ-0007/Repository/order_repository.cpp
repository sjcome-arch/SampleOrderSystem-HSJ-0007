#include "Repository/order_repository.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "Repository/json_value.h"

namespace {

double timeToEpochMillis(const Time& t) {
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count());
}

Time epochMillisToTime(double millis) {
    return Time(std::chrono::milliseconds(static_cast<long long>(millis)));
}

Json::Value toJson(const Order& order) {
    Json::Value json = Json::Value::makeObject();
    json["orderId"] = Json::Value(order.orderId);
    json["productSpecId"] = Json::Value(order.productSpecId);
    json["customerName"] = Json::Value(order.customerName);
    json["quantity"] = Json::Value(order.quantity);
    json["status"] = Json::Value(orderStatusToString(order.status));
    json["availableStockAtApproval"] = Json::Value(order.availableStockAtApproval);
    json["shortageQuantity"] = Json::Value(order.shortageQuantity);
    json["actualProductionQuantity"] = Json::Value(order.actualProductionQuantity);
    json["totalProductionTime"] = Json::Value(order.totalProductionTime);
    if (order.productionStartedAt.has_value()) {
        json["productionStartedAt"] = Json::Value(timeToEpochMillis(*order.productionStartedAt));
    }
    if (order.releasedAt.has_value()) {
        json["releasedAt"] = Json::Value(timeToEpochMillis(*order.releasedAt));
    }
    return json;
}

Order fromJson(const Json::Value& json) {
    Order order;
    if (auto* v = json.find("orderId")) order.orderId = v->asString();
    if (auto* v = json.find("productSpecId")) order.productSpecId = v->asString();
    if (auto* v = json.find("customerName")) order.customerName = v->asString();
    if (auto* v = json.find("quantity")) order.quantity = static_cast<int>(v->asNumber());
    if (auto* v = json.find("status")) order.status = orderStatusFromString(v->asString());
    if (auto* v = json.find("availableStockAtApproval"))
        order.availableStockAtApproval = static_cast<int>(v->asNumber());
    if (auto* v = json.find("shortageQuantity")) order.shortageQuantity = static_cast<int>(v->asNumber());
    if (auto* v = json.find("actualProductionQuantity"))
        order.actualProductionQuantity = static_cast<int>(v->asNumber());
    if (auto* v = json.find("totalProductionTime")) order.totalProductionTime = v->asNumber();
    if (auto* v = json.find("productionStartedAt")) order.productionStartedAt = epochMillisToTime(v->asNumber());
    if (auto* v = json.find("releasedAt")) order.releasedAt = epochMillisToTime(v->asNumber());
    return order;
}

long long parseSequence(const std::string& orderId) {
    // "ORD-NNNNNN" 형식에서 숫자 부분만 추출한다.
    const std::string prefix = "ORD-";
    if (orderId.rfind(prefix, 0) != 0) {
        return 0;
    }
    try {
        return std::stoll(orderId.substr(prefix.size()));
    } catch (...) {
        return 0;
    }
}

}  // namespace

OrderRepository::OrderRepository(std::string filePath) : filePath_(std::move(filePath)) { load(); }

void OrderRepository::load() {
    items_.clear();
    nextSequence_ = 1;
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
        Order order = fromJson(root[i]);
        nextSequence_ = std::max(nextSequence_, parseSequence(order.orderId) + 1);
        items_.push_back(std::move(order));
    }
}

void OrderRepository::save() const {
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

void OrderRepository::reload() { load(); }

std::string OrderRepository::issueOrderId() {
    std::ostringstream oss;
    oss << "ORD-" << std::setw(6) << std::setfill('0') << nextSequence_;
    ++nextSequence_;
    return oss.str();
}

std::vector<Order> OrderRepository::findAll() const { return items_; }

std::optional<Order> OrderRepository::findById(const std::string& orderId) const {
    for (const auto& item : items_) {
        if (item.orderId == orderId) {
            return item;
        }
    }
    return std::nullopt;
}

std::vector<Order> OrderRepository::findByStatus(OrderStatus status) const {
    std::vector<Order> result;
    for (const auto& item : items_) {
        if (item.status == status) {
            result.push_back(item);
        }
    }
    return result;
}

Order OrderRepository::add(Order order) {
    order.orderId = issueOrderId();
    items_.push_back(order);
    save();
    return order;
}

void OrderRepository::update(const Order& order) {
    for (auto& item : items_) {
        if (item.orderId == order.orderId) {
            item = order;
            save();
            return;
        }
    }
}
