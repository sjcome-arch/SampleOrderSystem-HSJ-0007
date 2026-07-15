#pragma once

#include "Model/waiting_approval_queue.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/order_view.h"

// 시료 주문 접수 + 주문 승인/거절 메뉴 (REQUIREMENT.md 5.3, 5.4). 실제 메뉴 흐름은 Phase 3~4에서 구현한다.
class OrderController {
public:
    OrderController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                     WaitingApprovalQueue& waitingApprovalQueue, OrderView& view);

    void run();

private:
    IOrderRepository& orderRepository_;
    IProductSpecRepository& productSpecRepository_;
    WaitingApprovalQueue& waitingApprovalQueue_;
    OrderView& view_;
};
