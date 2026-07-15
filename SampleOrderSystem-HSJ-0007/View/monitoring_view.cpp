#include "View/monitoring_view.h"

#include <iostream>
#include <string>

namespace {

std::string levelLabel(StockLevel level) {
    switch (level) {
        case StockLevel::Sufficient:
            return "여유";
        case StockLevel::Low:
            return "부족";
        case StockLevel::Depleted:
            return "고갈";
    }
    return "";
}

}  // namespace

void MonitoringView::showOrderStatusSummary(const OrderStatusSummary& summary) const {
    std::cout << "\n[상태별 주문 현황]\n";
    std::cout << "RESERVED=" << summary.reserved << ", CONFIRMED=" << summary.confirmed
               << ", PRODUCING=" << summary.producing << ", RELEASED=" << summary.released << std::endl;
}

void MonitoringView::showStockStatus(const std::vector<StockStatusRow>& rows) const {
    std::cout << "\n[시료별 재고 현황]\n";
    if (rows.empty()) {
        std::cout << "등록된 시료가 없습니다." << std::endl;
        return;
    }
    for (const auto& row : rows) {
        std::cout << "시료명=" << row.name << ", 재고=" << row.stock << ", 가용재고=" << row.availableStock
                   << ", 상태=" << levelLabel(row.level) << std::endl;
    }
}
