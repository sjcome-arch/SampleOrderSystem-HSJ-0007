#pragma once

#include "Repository/i_product_spec_repository.h"
#include "View/product_spec_view.h"

// 시료 관리 메뉴 (REQUIREMENT.md 5.2). 실제 메뉴 흐름은 Phase 2에서 구현한다.
class ProductSpecController {
public:
    ProductSpecController(IProductSpecRepository& repository, ProductSpecView& view);

    void run();

private:
    IProductSpecRepository& repository_;
    ProductSpecView& view_;
};
