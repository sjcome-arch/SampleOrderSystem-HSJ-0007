#pragma once

#include "Model/production_line.h"
#include "View/production_line_view.h"

// 생산 라인 메뉴 (REQUIREMENT.md 5.6). 실제 메뉴 흐름은 Phase 5에서 구현한다.
class ProductionLineController {
public:
    ProductionLineController(ProductionLine& productionLine, ProductionLineView& view);

    void run();

private:
    ProductionLine& productionLine_;
    ProductionLineView& view_;
};
