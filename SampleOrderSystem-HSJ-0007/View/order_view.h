#pragma once

#include <vector>

#include "Controller/order_types.h"
#include "Model/order.h"
#include "Model/product_spec.h"

// 시료 주문 입력·확인 및 주문 승인/거절 화면 출력 (REQUIREMENT.md 5.3, 5.4).
class OrderView {
public:
    int promptMenuChoice() const;
    ReserveOrderInput promptReserveInput() const;
    bool promptConfirm(const ReserveOrderInput& input) const;
    void showReserveResult(ReserveOrderResult result) const;

    void showWaitingApprovalList(const std::vector<Order>& orders) const;
    void showNoOrderToProcess() const;
    void showStockInquiry(const Order& order, const ProductSpec& spec) const;
    bool promptApproveOrReject() const;
    void showApproveResult(const ApproveNextResult& result) const;
    void showRejectResult(const RejectNextResult& result) const;
};
