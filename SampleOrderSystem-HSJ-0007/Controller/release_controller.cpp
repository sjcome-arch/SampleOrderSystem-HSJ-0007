#include "Controller/release_controller.h"

#include <chrono>

#include "Model/order_status.h"

ReleaseController::ReleaseController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                                      ReleaseView& view)
    : orderRepository_(orderRepository), productSpecRepository_(productSpecRepository), view_(view) {}

std::vector<ReleasableOrderRow> ReleaseController::releasableList() const {
    std::vector<Order> orders = orderRepository_.findByStatus(OrderStatus::CONFIRMED);

    std::vector<ReleasableOrderRow> rows;
    rows.reserve(orders.size());
    for (const Order& order : orders) {
        // 시료가 삭제되어 조회되지 않으면 출고 가능 목록에서 제외한다(크래시 방지).
        auto specOpt = productSpecRepository_.findById(order.productSpecId);
        if (!specOpt.has_value()) {
            continue;
        }
        ReleasableOrderRow row;
        row.sequence = static_cast<int>(rows.size() + 1);
        row.order = order;
        row.spec = *specOpt;
        rows.push_back(row);
    }
    return rows;
}

ReleaseResult ReleaseController::release(const std::string& orderId, Time now) {
    auto orderOpt = orderRepository_.findById(orderId);
    if (!orderOpt.has_value() || orderOpt->status != OrderStatus::CONFIRMED) {
        return ReleaseResult{ReleaseOutcome::InvalidSelection, Order{}};
    }
    auto specOpt = productSpecRepository_.findById(orderOpt->productSpecId);
    if (!specOpt.has_value()) {
        return ReleaseResult{ReleaseOutcome::InvalidSelection, Order{}};
    }

    // 저장 우선 원칙(design_phase_1.md 3.1): stock 차감을 먼저 저장한 뒤 주문 상태를 저장한다.
    ProductSpec spec = *specOpt;
    spec.stock -= orderOpt->quantity;
    productSpecRepository_.update(spec);

    Order order = *orderOpt;
    order.status = OrderStatus::RELEASED;
    order.releasedAt = now;
    orderRepository_.update(order);

    return ReleaseResult{ReleaseOutcome::Success, order};
}

void ReleaseController::run() {
    auto rows = releasableList();
    view_.showReleasableList(rows);
    if (rows.empty()) {
        return;
    }

    int selection = view_.promptSelection();
    if (selection == 0) {
        return;
    }
    if (selection < 1 || selection > static_cast<int>(rows.size())) {
        view_.showInvalidSelection();
        return;
    }

    ReleaseResult result = release(rows[selection - 1].order.orderId, std::chrono::system_clock::now());
    view_.showReleaseResult(result);
    view_.showReleasableList(releasableList());
}
