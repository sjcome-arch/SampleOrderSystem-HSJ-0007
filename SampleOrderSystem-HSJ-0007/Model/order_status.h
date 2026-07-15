#pragma once

#include <string>

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASED,
};

std::string orderStatusToString(OrderStatus status);
OrderStatus orderStatusFromString(const std::string& text);
