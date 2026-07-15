#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "Model/order_status.h"

using Time = std::chrono::system_clock::time_point;

// 주문(Order). 필드 정의는 design_phase_1.md 3.2 참조.
class Order {
public:
    std::string orderId;         // 주문번호 (ORD-NNNNNN, design.md 6.1)
    std::string productSpecId;   // 시료 ID
    std::string customerName;    // 고객명
    int quantity = 0;            // 주문 수량
    OrderStatus status = OrderStatus::RESERVED;

    // 승인 처리(재고 부족) 시점에만 채워짐 (design_phase_4.md 참조)
    int availableStockAtApproval = 0;
    int shortageQuantity = 0;
    int actualProductionQuantity = 0;
    double totalProductionTime = 0.0;
    std::optional<Time> productionStartedAt;

    // 출고 처리 시점에만 채워짐 (design_phase_6.md 참조)
    std::optional<Time> releasedAt;
};
