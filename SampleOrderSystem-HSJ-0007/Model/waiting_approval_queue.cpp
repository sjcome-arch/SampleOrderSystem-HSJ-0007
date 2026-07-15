#include "Model/waiting_approval_queue.h"

WaitingApprovalQueue::WaitingApprovalQueue(const std::vector<Order>& reservedOrdersInFifoOrder) {
    for (const auto& order : reservedOrdersInFifoOrder) {
        queue_.push(order);
    }
}

void WaitingApprovalQueue::enqueue(const Order& order) { queue_.push(order); }

std::optional<Order> WaitingApprovalQueue::dequeue() {
    if (queue_.empty()) {
        return std::nullopt;
    }
    Order front = queue_.front();
    queue_.pop();
    return front;
}

std::vector<Order> WaitingApprovalQueue::snapshot() const {
    std::vector<Order> result;
    std::queue<Order> copy = queue_;
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

bool WaitingApprovalQueue::empty() const { return queue_.empty(); }

size_t WaitingApprovalQueue::size() const { return queue_.size(); }
