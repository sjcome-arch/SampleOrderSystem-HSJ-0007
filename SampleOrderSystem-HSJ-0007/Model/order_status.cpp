#include "Model/order_status.h"

#include <stdexcept>

std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED: return "RESERVED";
        case OrderStatus::REJECTED: return "REJECTED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::RELEASED: return "RELEASED";
    }
    throw std::invalid_argument("orderStatusToString: unknown OrderStatus");
}

OrderStatus orderStatusFromString(const std::string& text) {
    if (text == "RESERVED") return OrderStatus::RESERVED;
    if (text == "REJECTED") return OrderStatus::REJECTED;
    if (text == "PRODUCING") return OrderStatus::PRODUCING;
    if (text == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (text == "RELEASED") return OrderStatus::RELEASED;
    throw std::invalid_argument("orderStatusFromString: unknown status '" + text + "'");
}
