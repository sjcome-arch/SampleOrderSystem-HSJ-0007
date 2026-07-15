#pragma once

#include <string>
#include <vector>

#include "Repository/i_product_spec_repository.h"
#include "View/product_spec_view.h"

// 시료 등록 입력값 (콘솔에서 수집된 값을 담는 순수 데이터 구조).
struct RegisterProductSpecInput {
    std::string productSpecId;
    std::string name;
    double avgProductionTime = 0.0;
    double yield = 0.0;
    int stock = 0;
};

enum class RegisterProductSpecResult {
    Success,
    InvalidYield,
    InvalidProductionTime,
    DuplicateId,
    Cancelled,
};

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
