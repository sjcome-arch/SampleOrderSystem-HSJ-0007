#pragma once

#include <string>
#include <vector>

#include "Repository/i_product_spec_repository.h"

// JSON 파일(data/product_specs.json 기본값) 기반 ProductSpec CRUD 저장소.
class ProductSpecRepository : public IProductSpecRepository {
public:
    explicit ProductSpecRepository(std::string filePath);

    std::vector<ProductSpec> findAll() const override;
    std::optional<ProductSpec> findById(const std::string& productSpecId) const override;
    std::vector<ProductSpec> findByName(const std::string& keyword) const override;
    void add(const ProductSpec& spec) override;
    void update(const ProductSpec& spec) override;
    void remove(const std::string& productSpecId) override;
    void reload() override;

private:
    std::string filePath_;
    std::vector<ProductSpec> items_;

    void load();
    void save() const;
};
