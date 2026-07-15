#pragma once

#include "Repository/i_order_repository.h"
#include "View/release_view.h"

// 출고 처리 메뉴 (REQUIREMENT.md 5.7). 실제 메뉴 흐름은 Phase 6에서 구현한다.
class ReleaseController {
public:
    ReleaseController(IOrderRepository& orderRepository, ReleaseView& view);

    void run();

private:
    IOrderRepository& orderRepository_;
    ReleaseView& view_;
};
