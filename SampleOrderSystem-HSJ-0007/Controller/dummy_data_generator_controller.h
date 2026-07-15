#pragma once

#include <random>

#include "Controller/dummy_data_generator_types.h"
#include "Controller/order_controller.h"
#include "Controller/release_controller.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/dummy_data_generator_view.h"

// Dummy 데이터 생성 메뉴 (design_phase_8.md 2). 시료/주문은 반드시 Repository CRUD(add)를 통해서만
// 추가하며, 주문의 승인/거절/출고는 기존 OrderController/ReleaseController 로직을 재사용해
// WaitingApprovalQueue/ProductionLine과의 일관성을 유지한다(design_phase_8.md 검증 절 참조).
class DummyDataGeneratorController {
public:
    DummyDataGeneratorController(IProductSpecRepository& productSpecRepository, IOrderRepository& orderRepository,
                                  OrderController& orderController, ReleaseController& releaseController,
                                  DummyDataGeneratorView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상). 매 호출마다 고정 시드로 재현 가능한 결과를 낸다.
    DummyDataGenerationSummary generate(int specCount, int orderCount);

private:
    int generateProductSpecs(int count, std::mt19937& rng);
    int generateOrders(int count, std::mt19937& rng);

    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
    OrderController& orderController_;
    ReleaseController& releaseController_;
    DummyDataGeneratorView& view_;
};
