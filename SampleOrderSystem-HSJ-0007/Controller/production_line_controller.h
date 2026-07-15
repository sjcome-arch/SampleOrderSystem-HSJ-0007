#pragma once

#include <vector>

#include "Model/order.h"
#include "Model/product_spec.h"
#include "Model/production_line.h"
#include "Repository/i_product_spec_repository.h"
#include "View/production_line_view.h"

// "대기 주문 확인" 한 줄(순번, 주문, 시료 정보, 완료 예정 시간).
struct ProductionQueueRow {
    int sequence = 0;
    Order order;
    ProductSpec spec;
    Time expectedCompletion;
};

// "생산 현황" 조회 결과. isRunning이 false면 나머지 필드는 의미 없음(현재 처리 중인 주문 없음).
struct CurrentProductionInfo {
    bool isRunning = false;
    Order order;
    ProductSpec spec;
    Time expectedCompletion;
};

// 생산 라인 메뉴 (REQUIREMENT.md 5.6): 대기 주문 확인 + 현재 생산 현황 조회.
class ProductionLineController {
public:
    ProductionLineController(ProductionLine& productionLine, IProductSpecRepository& productSpecRepository,
                              ProductionLineView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    std::vector<ProductionQueueRow> waitingQueue() const;
    CurrentProductionInfo currentProduction() const;

private:
    ProductionLine& productionLine_;
    IProductSpecRepository& productSpecRepository_;
    ProductionLineView& view_;
};
