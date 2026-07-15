#pragma once

#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/monitoring_view.h"

// 모니터링 메뉴 (REQUIREMENT.md 5.5). 실제 메뉴 흐름은 Phase 7에서 구현한다.
class MonitoringController {
public:
    MonitoringController(IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository,
                          MonitoringView& view);

    void run();

private:
    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
    MonitoringView& view_;
};
