#pragma once

#include <string>
#include <vector>

#include "Controller/product_spec_types.h"
#include "Repository/i_product_spec_repository.h"
#include "View/product_spec_view.h"

// 시료 관리 메뉴 (REQUIREMENT.md 5.2).
class ProductSpecController {
public:
    ProductSpecController(IProductSpecRepository& repository, ProductSpecView& view);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    RegisterProductSpecResult registerSpec(const RegisterProductSpecInput& input, bool confirm);
    std::vector<ProductSpec> listAll() const;
    std::vector<ProductSpec> search(const std::string& keyword) const;

private:
    IProductSpecRepository& repository_;
    ProductSpecView& view_;
};
