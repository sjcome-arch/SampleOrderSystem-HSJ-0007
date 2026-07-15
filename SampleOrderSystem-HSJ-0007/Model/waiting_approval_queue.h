#pragma once

#include <optional>
#include <queue>
#include <vector>

#include "Model/order.h"

// 주문 접수 큐 (RESERVED 상태 주문의 FIFO 승인/거절 대기열).
// 생성 시 전달되는 목록으로 큐를 재구성한다 (design_phase_1.md 3.1 "시작 시 재구성" 원칙).
// 호출자는 OrderRepository::findByStatus(RESERVED) 결과를 주문번호 오름차순으로 정렬해 전달한다.
class WaitingApprovalQueue {
public:
    explicit WaitingApprovalQueue(const std::vector<Order>& reservedOrdersInFifoOrder);

    void enqueue(const Order& order);
    std::optional<Order> dequeue();
    std::vector<Order> snapshot() const;
    bool empty() const;
    size_t size() const;

private:
    std::queue<Order> queue_;
};
