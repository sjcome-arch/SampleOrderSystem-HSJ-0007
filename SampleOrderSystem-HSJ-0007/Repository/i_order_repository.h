#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Model/order.h"
#include "Model/order_status.h"

// gMock으로 대체 가능하도록 구현과 분리한 인터페이스 (design.md 6.4).
class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual std::vector<Order> findAll() const = 0;
    virtual std::optional<Order> findById(const std::string& orderId) const = 0;
    virtual std::vector<Order> findByStatus(OrderStatus status) const = 0;
    // order.orderId는 무시하고 새 주문번호(ORD-NNNNNN)를 채번해 저장한 뒤,
    // 채번된 주문번호가 반영된 Order를 반환한다.
    virtual Order add(Order order) = 0;
    virtual void update(const Order& order) = 0;
    virtual void reload() = 0;
};
