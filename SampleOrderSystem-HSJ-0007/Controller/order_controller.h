#pragma once

#include <optional>
#include <string>
#include <utility>

#include "Model/order.h"
#include "Model/production_line.h"
#include "Model/waiting_approval_queue.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/order_view.h"

// 시료 주문 접수 입력값.
struct ReserveOrderInput {
    std::string productSpecId;
    std::string customerName;
    int quantity = 0;
};

enum class ReserveOrderResult {
    Success,
    ProductSpecNotFound,
    InvalidQuantity,
    Cancelled,
};

// 승인 처리 결과. queueEmpty가 true면 접수 큐가 비어 있어 처리할 주문이 없었음을 뜻한다.
struct ApproveNextResult {
    bool queueEmpty = true;
    Order order;
};

// 거절 처리 결과. queueEmpty가 true면 접수 큐가 비어 있어 처리할 주문이 없었음을 뜻한다.
struct RejectNextResult {
    bool queueEmpty = true;
    Order order;
};

// 시료 주문 접수 + 주문 승인/거절 메뉴 (REQUIREMENT.md 5.3, 5.4).
class OrderController {
public:
    OrderController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                     WaitingApprovalQueue& waitingApprovalQueue, ProductionLine& productionLine, OrderView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    ReserveOrderResult reserveOrder(const ReserveOrderInput& input, bool confirm);
    std::optional<std::pair<Order, ProductSpec>> peekNextForApproval() const;
    ApproveNextResult approveNext();
    RejectNextResult rejectNext();

private:
    IOrderRepository& orderRepository_;
    IProductSpecRepository& productSpecRepository_;
    WaitingApprovalQueue& waitingApprovalQueue_;
    ProductionLine& productionLine_;
    OrderView& view_;
};
