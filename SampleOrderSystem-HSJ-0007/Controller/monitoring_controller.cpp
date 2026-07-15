#include "Controller/monitoring_controller.h"

#include "Model/order_status.h"

MonitoringController::MonitoringController(IProductSpecRepository& productSpecRepository,
                                            IOrderRepository& orderRepository, MonitoringView& view)
    : productSpecRepository_(productSpecRepository), orderRepository_(orderRepository), view_(view) {}

OrderStatusSummary MonitoringController::orderStatusSummary() const {
    OrderStatusSummary summary;
    summary.reserved = static_cast<int>(orderRepository_.findByStatus(OrderStatus::RESERVED).size());
    summary.confirmed = static_cast<int>(orderRepository_.findByStatus(OrderStatus::CONFIRMED).size());
    summary.producing = static_cast<int>(orderRepository_.findByStatus(OrderStatus::PRODUCING).size());
    summary.released = static_cast<int>(orderRepository_.findByStatus(OrderStatus::RELEASED).size());
    return summary;
}

std::vector<StockStatusRow> MonitoringController::stockStatus() const {
    std::vector<StockStatusRow> rows;
    for (const auto& spec : productSpecRepository_.findAll()) {
        StockStatusRow row;
        row.productSpecId = spec.productSpecId;
        row.name = spec.name;
        row.stock = spec.stock;
        row.availableStock = spec.availableStock;
        // 판정 순서(design_phase_7.md 2.1): stock==0 최우선 고갈, 그 다음 availableStock==0 부족.
        if (spec.stock == 0) {
            row.level = StockLevel::Depleted;
        } else if (spec.availableStock == 0) {
            row.level = StockLevel::Low;
        } else {
            row.level = StockLevel::Sufficient;
        }
        rows.push_back(row);
    }
    return rows;
}

void MonitoringController::run() {
    view_.showOrderStatusSummary(orderStatusSummary());
    view_.showStockStatus(stockStatus());
}
