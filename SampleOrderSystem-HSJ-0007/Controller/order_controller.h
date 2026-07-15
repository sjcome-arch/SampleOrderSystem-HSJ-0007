#pragma once

#include <optional>
#include <string>
#include <utility>

#include "Controller/order_types.h"
#include "Model/order.h"
#include "Model/production_line.h"
#include "Model/waiting_approval_queue.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/order_view.h"

// 시료 주문 접수 + 주문 승인/거절 메뉴 (REQUIREMENT.md 5.3, 5.4).
class OrderController {
public:
    OrderController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                     WaitingApprovalQueue& waitingApprovalQueue, ProductionLine& productionLine, OrderView& view);

    // 메인 메뉴 [2] 시료 주문: 예약 입력 → 확인 → 결과 출력.
    void runReserve();
    // 메인 메뉴 [3] 주문 승인/거절: 접수 목록 표시 → 다음 건 재고 파악 → 승인/거절 처리.
    void runApproval();

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
