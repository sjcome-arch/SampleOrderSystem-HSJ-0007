#pragma once

#include <vector>

#include "Controller/monitoring_types.h"

// 상태별 주문 현황, 시료별 재고 현황 출력 (REQUIREMENT.md 5.5).
class MonitoringView {
public:
    void showOrderStatusSummary(const OrderStatusSummary& summary) const;
    void showStockStatus(const std::vector<StockStatusRow>& rows) const;
};
