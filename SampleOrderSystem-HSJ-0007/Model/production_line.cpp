#include "Model/production_line.h"

ProductionLine::ProductionLine(const std::vector<Order>& producingOrdersInFifoOrder) {
    for (const auto& order : producingOrdersInFifoOrder) {
        queue_.push(order);
    }
}

void ProductionLine::enqueue(const Order& order) { queue_.push(order); }

std::vector<Order> ProductionLine::snapshot() const {
    std::vector<Order> result;
    std::queue<Order> copy = queue_;
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

bool ProductionLine::isRunning() const { return !queue_.empty(); }

size_t ProductionLine::size() const { return queue_.size(); }
