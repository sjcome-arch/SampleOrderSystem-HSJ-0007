#pragma once

#include <vector>

#include "Controller/monitoring_types.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/monitoring_view.h"

// 모니터링 메뉴 (REQUIREMENT.md 5.5). Repository 조회만 수행하는 읽기 전용 현황판이다.
class MonitoringController {
public:
    MonitoringController(IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository,
                          MonitoringView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    OrderStatusSummary orderStatusSummary() const;
    std::vector<StockStatusRow> stockStatus() const;

private:
    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
    MonitoringView& view_;
};
