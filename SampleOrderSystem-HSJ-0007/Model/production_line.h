#pragma once

#include <queue>
#include <vector>

#include "Model/order.h"

// 생산 라인(단일 라인) + 생산 큐(FIFO).
// Phase 1 범위에서는 큐 골격만 제공한다. 실 생산량/수율 계산, tick 기반 완료 판정 등
// 실제 생산 로직은 design_phase_5.md에서 구현한다.
class ProductionLine {
public:
    explicit ProductionLine(const std::vector<Order>& producingOrdersInFifoOrder);

    void enqueue(const Order& order);
    std::vector<Order> snapshot() const;
    bool isRunning() const;
    size_t size() const;

private:
    std::queue<Order> queue_;
};
