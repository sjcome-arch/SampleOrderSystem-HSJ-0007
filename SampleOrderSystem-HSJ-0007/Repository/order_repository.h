#pragma once

#include <string>
#include <vector>

#include "Repository/i_order_repository.h"

// JSON 파일(data/orders.json 기본값) 기반 Order CRUD 저장소.
// 주문번호(ORD-NNNNNN) 채번 규칙은 design.md 6.1 참조.
class OrderRepository : public IOrderRepository {
public:
    explicit OrderRepository(std::string filePath);

    std::vector<Order> findAll() const override;
    std::optional<Order> findById(const std::string& orderId) const override;
    std::vector<Order> findByStatus(OrderStatus status) const override;
    Order add(Order order) override;
    void update(const Order& order) override;
    void reload() override;

private:
    std::string filePath_;
    std::vector<Order> items_;
    long long nextSequence_ = 1;

    void load();
    void save() const;
    std::string issueOrderId();
};
