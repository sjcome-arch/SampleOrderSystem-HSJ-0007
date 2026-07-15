#include "Model/production_line.h"

namespace {

Time addMinutes(Time base, double minutes) {
    auto duration = std::chrono::duration<double, std::ratio<60>>(minutes);
    return base + std::chrono::duration_cast<Time::duration>(duration);
}

Time expectedCompletionOf(const Order& order) { return addMinutes(*order.productionStartedAt, order.totalProductionTime); }

}  // namespace

ProductionLine::ProductionLine(const std::vector<Order>& producingOrdersInFifoOrder,
                                IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository)
    : productSpecRepository_(productSpecRepository), orderRepository_(orderRepository) {
    for (const auto& order : producingOrdersInFifoOrder) {
        productionQueue_.push(order);
    }
}

void ProductionLine::startFrontIfNeeded(Time now) {
    if (productionQueue_.empty()) {
        return;
    }
    Order& front = productionQueue_.front();
    if (!front.productionStartedAt.has_value()) {
        front.productionStartedAt = now;
        orderRepository_.update(front);
    }
}

void ProductionLine::enqueue(Order order, Time now) {
    bool wasEmpty = productionQueue_.empty();
    productionQueue_.push(std::move(order));
    if (wasEmpty) {
        startFrontIfNeeded(now);
    }
}

bool ProductionLine::isEmpty() const { return productionQueue_.empty(); }

LineState ProductionLine::state() const { return isEmpty() ? LineState::WAITING : LineState::RUNNING; }

const Order& ProductionLine::currentOrder() const { return productionQueue_.front(); }

void ProductionLine::completeCurrent(Time now) {
    if (productionQueue_.empty()) {
        return;
    }
    Order order = productionQueue_.front();
    ProductSpec spec = productSpecRepository_.findById(order.productSpecId).value();

    spec.stock += order.actualProductionQuantity;
    spec.availableStock += (order.actualProductionQuantity - order.shortageQuantity);
    order.status = OrderStatus::CONFIRMED;

    productSpecRepository_.update(spec);
    orderRepository_.update(order);
    productionQueue_.pop();

    startFrontIfNeeded(now);
}

std::vector<Order> ProductionLine::snapshot() const {
    std::vector<Order> result;
    std::queue<Order> copy = productionQueue_;
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

void ProductionLine::tick(Time now) {
    while (!productionQueue_.empty() && now >= expectedCompletionOf(productionQueue_.front())) {
        completeCurrent(now);
    }
}

std::vector<Time> ProductionLine::expectedCompletions() const {
    std::vector<Time> result;
    if (productionQueue_.empty()) {
        return result;
    }
    std::queue<Order> copy = productionQueue_;
    Time base = *copy.front().productionStartedAt;
    Time::duration cumulative{0};
    while (!copy.empty()) {
        cumulative += std::chrono::duration_cast<Time::duration>(
            std::chrono::duration<double, std::ratio<60>>(copy.front().totalProductionTime));
        result.push_back(base + cumulative);
        copy.pop();
    }
    return result;
}
