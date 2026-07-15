#include "Controller/order_controller.h"

#include <chrono>
#include <cmath>

OrderController::OrderController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                                  WaitingApprovalQueue& waitingApprovalQueue, ProductionLine& productionLine,
                                  OrderView& view)
    : orderRepository_(orderRepository),
      productSpecRepository_(productSpecRepository),
      waitingApprovalQueue_(waitingApprovalQueue),
      productionLine_(productionLine),
      view_(view) {}

ReserveOrderResult OrderController::reserveOrder(const ReserveOrderInput& input, bool confirm) {
    if (!productSpecRepository_.findById(input.productSpecId).has_value()) {
        return ReserveOrderResult::ProductSpecNotFound;
    }
    if (input.quantity <= 0) {
        return ReserveOrderResult::InvalidQuantity;
    }
    if (!confirm) {
        return ReserveOrderResult::Cancelled;
    }

    Order order;
    order.productSpecId = input.productSpecId;
    order.customerName = input.customerName;
    order.quantity = input.quantity;
    order.status = OrderStatus::RESERVED;

    // 저장 우선 원칙 (design_phase_1.md 3.1): 파일에 먼저 반영한 뒤 메모리 큐를 갱신한다.
    Order saved = orderRepository_.add(order);
    waitingApprovalQueue_.enqueue(saved);
    return ReserveOrderResult::Success;
}

std::optional<std::pair<Order, ProductSpec>> OrderController::peekNextForApproval() const {
    auto snapshot = waitingApprovalQueue_.snapshot();
    if (snapshot.empty()) {
        return std::nullopt;
    }
    const Order& front = snapshot.front();
    auto spec = productSpecRepository_.findById(front.productSpecId);
    if (!spec.has_value()) {
        return std::nullopt;
    }
    return std::make_pair(front, *spec);
}

ApproveNextResult OrderController::approveNext() {
    auto dequeued = waitingApprovalQueue_.dequeue();
    if (!dequeued.has_value()) {
        return ApproveNextResult{true, Order{}};
    }

    Order order = *dequeued;
    auto specOpt = productSpecRepository_.findById(order.productSpecId);
    ProductSpec spec = specOpt.value();

    if (spec.availableStock >= order.quantity) {
        spec.availableStock -= order.quantity;
        order.status = OrderStatus::CONFIRMED;
        productSpecRepository_.update(spec);
        orderRepository_.update(order);
    } else {
        order.availableStockAtApproval = spec.availableStock;
        order.shortageQuantity = order.quantity - spec.availableStock;
        spec.availableStock = 0;
        order.actualProductionQuantity =
            static_cast<int>(std::ceil(static_cast<double>(order.shortageQuantity) / spec.yield));
        order.totalProductionTime = spec.avgProductionTime * order.actualProductionQuantity;
        order.status = OrderStatus::PRODUCING;

        productSpecRepository_.update(spec);
        orderRepository_.update(order);
        productionLine_.enqueue(order, std::chrono::system_clock::now());
    }

    return ApproveNextResult{false, order};
}

RejectNextResult OrderController::rejectNext() {
    auto dequeued = waitingApprovalQueue_.dequeue();
    if (!dequeued.has_value()) {
        return RejectNextResult{true, Order{}};
    }

    Order order = *dequeued;
    order.status = OrderStatus::REJECTED;
    orderRepository_.update(order);
    return RejectNextResult{false, order};
}

void OrderController::run() {
    int choice = view_.promptMenuChoice();
    if (choice == 1) {
        ReserveOrderInput input = view_.promptReserveInput();
        while (true) {
            if (!productSpecRepository_.findById(input.productSpecId).has_value()) {
                view_.showReserveResult(ReserveOrderResult::ProductSpecNotFound);
                input = view_.promptReserveInput();
                continue;
            }
            if (input.quantity <= 0) {
                view_.showReserveResult(ReserveOrderResult::InvalidQuantity);
                input = view_.promptReserveInput();
                continue;
            }
            break;
        }
        bool confirm = view_.promptConfirm(input);
        ReserveOrderResult result = reserveOrder(input, confirm);
        view_.showReserveResult(result);
    } else if (choice == 2) {
        view_.showWaitingApprovalList(waitingApprovalQueue_.snapshot());
    } else if (choice == 3) {
        auto next = peekNextForApproval();
        if (!next.has_value()) {
            view_.showNoOrderToProcess();
            return;
        }
        view_.showStockInquiry(next->first, next->second);
        if (view_.promptApproveOrReject()) {
            view_.showApproveResult(approveNext());
        } else {
            view_.showRejectResult(rejectNext());
        }
    }
}
