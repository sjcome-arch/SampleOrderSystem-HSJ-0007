#pragma once

#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/dummy_data_generator_view.h"

// Dummy 데이터 생성 메뉴 (design_phase_8.md 참조). 실제 메뉴 흐름은 Phase 8에서 구현한다.
class DummyDataGeneratorController {
public:
    DummyDataGeneratorController(IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository,
                                  DummyDataGeneratorView& view);

    void run();

private:
    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
    DummyDataGeneratorView& view_;
};
