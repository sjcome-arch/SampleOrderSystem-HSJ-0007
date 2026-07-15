#pragma once

#include <string>
#include <vector>

#include "Model/product_spec.h"

enum class RegisterProductSpecResult;
struct RegisterProductSpecInput;

// 시료 등록/조회/검색 화면 출력 (REQUIREMENT.md 5.2).
class ProductSpecView {
public:
    int promptMenuChoice() const;
    RegisterProductSpecInput promptRegisterInput() const;
    bool promptConfirm(const RegisterProductSpecInput& input) const;
    void showRegisterResult(RegisterProductSpecResult result) const;
    void showList(const std::vector<ProductSpec>& specs) const;
    std::string promptSearchKeyword() const;
    void showSearchResults(const std::vector<ProductSpec>& specs) const;
};
