#pragma once

#include <string>
#include <vector>

#include "Controller/release_types.h"
#include "Model/order.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/release_view.h"

// 출고 처리 메뉴 (REQUIREMENT.md 5.7).
class ReleaseController {
public:
    ReleaseController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                       ReleaseView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    std::vector<ReleasableOrderRow> releasableList() const;
    ReleaseResult release(const std::string& orderId, Time now);

private:
    IOrderRepository& orderRepository_;
    IProductSpecRepository& productSpecRepository_;
    ReleaseView& view_;
};
