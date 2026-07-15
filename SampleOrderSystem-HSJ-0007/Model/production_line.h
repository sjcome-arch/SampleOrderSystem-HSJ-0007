#pragma once

#include <queue>
#include <vector>

#include "Model/order.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"

enum class LineState { WAITING, RUNNING };

// 생산 라인(단일 라인) + 생산 큐(FIFO) (REQUIREMENT.md 5.6, design_phase_5.md).
// 재고 부족으로 PRODUCING 상태가 된 주문을 접수 순서대로 처리해 CONFIRMED로 전환하고 재고를 반영한다.
class ProductionLine {
public:
    ProductionLine(const std::vector<Order>& producingOrdersInFifoOrder,
                   IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository);

    // 승인(재고 부족) 시 호출. 큐가 비어 있었다면 이 주문이 새 front가 되어 productionStartedAt을
    // now로 채우고 저장한다.
    void enqueue(Order order, Time now);

    bool isEmpty() const;
    LineState state() const;
    // 현재 처리 중(front) 주문 조회. isEmpty()가 false일 때만 호출한다.
    const Order& currentOrder() const;
    // 생산 완료 처리: stock(실 생산량 전체)/availableStock(여분만) 반영 후 주문 상태를 CONFIRMED로
    // 저장하고 pop한다. 다음 주문이 새 front가 되면 productionStartedAt을 now로 채우고 저장한다.
    void completeCurrent(Time now);
    // "대기 주문 확인"용, FIFO 순서(front→back) 그대로 반환.
    std::vector<Order> snapshot() const;
    // front의 완료 예정 시각이 now를 지난 동안 completeCurrent(now)를 반복 호출한다.
    void tick(Time now);

    // snapshot()과 동일한 순서로, 각 주문의 완료 예정 시각(누적 계산, design_phase_5.md 4.2)을 반환한다.
    std::vector<Time> expectedCompletions() const;

private:
    void startFrontIfNeeded(Time now);

    std::queue<Order> productionQueue_;
    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
};
