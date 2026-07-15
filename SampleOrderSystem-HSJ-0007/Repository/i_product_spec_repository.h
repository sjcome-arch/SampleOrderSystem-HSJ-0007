#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Model/product_spec.h"

// gMock으로 대체 가능하도록 구현과 분리한 인터페이스 (design.md 6.4).
class IProductSpecRepository {
public:
    virtual ~IProductSpecRepository() = default;

    virtual std::vector<ProductSpec> findAll() const = 0;
    virtual std::optional<ProductSpec> findById(const std::string& productSpecId) const = 0;
    virtual std::vector<ProductSpec> findByName(const std::string& keyword) const = 0;
    virtual void add(const ProductSpec& spec) = 0;
    virtual void update(const ProductSpec& spec) = 0;
    virtual void remove(const std::string& productSpecId) = 0;
    virtual void reload() = 0;
};
