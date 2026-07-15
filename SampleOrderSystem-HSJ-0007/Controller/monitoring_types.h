#pragma once

#include <string>

// MonitoringController와 MonitoringView가 공유하는 입출력 데이터 구조.

// 상태별 주문 건수 (REJECTED 제외, REQUIREMENT.md 5.5).
struct OrderStatusSummary {
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int released = 0;
};

enum class StockLevel {
    Sufficient,  // 여유
    Low,         // 부족
    Depleted,    // 고갈
};

// 시료별 재고 현황 한 줄.
struct StockStatusRow {
    std::string productSpecId;
    std::string name;
    int stock = 0;
    int availableStock = 0;
    StockLevel level = StockLevel::Sufficient;
};
