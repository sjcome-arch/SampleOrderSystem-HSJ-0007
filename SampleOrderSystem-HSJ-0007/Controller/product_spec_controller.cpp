#include "Controller/product_spec_controller.h"

ProductSpecController::ProductSpecController(IProductSpecRepository& repository, ProductSpecView& view)
    : repository_(repository), view_(view) {}

RegisterProductSpecResult ProductSpecController::registerSpec(const RegisterProductSpecInput& input,
                                                                bool confirm) {
    if (!(input.yield > 0.0 && input.yield <= 1.0)) {
        return RegisterProductSpecResult::InvalidYield;
    }
    if (!(input.avgProductionTime > 0.0)) {
        return RegisterProductSpecResult::InvalidProductionTime;
    }
    if (repository_.findById(input.productSpecId).has_value()) {
        return RegisterProductSpecResult::DuplicateId;
    }
    if (!confirm) {
        return RegisterProductSpecResult::Cancelled;
    }

    ProductSpec spec;
    spec.productSpecId = input.productSpecId;
    spec.name = input.name;
    spec.avgProductionTime = input.avgProductionTime;
    spec.yield = input.yield;
    spec.stock = input.stock;
    spec.availableStock = input.stock;
    repository_.add(spec);
    return RegisterProductSpecResult::Success;
}

std::vector<ProductSpec> ProductSpecController::listAll() const { return repository_.findAll(); }

std::vector<ProductSpec> ProductSpecController::search(const std::string& keyword) const {
    return repository_.findByName(keyword);
}

void ProductSpecController::run() {
    for (;;) {
        int choice = view_.promptMenuChoice();
        if (choice == 1) {
            RegisterProductSpecInput input = view_.promptRegisterInput();
            RegisterProductSpecResult result = RegisterProductSpecResult::Cancelled;
            while (true) {
                if (!(input.yield > 0.0 && input.yield <= 1.0)) {
                    view_.showRegisterResult(RegisterProductSpecResult::InvalidYield);
                    input = view_.promptRegisterInput();
                    continue;
                }
                if (!(input.avgProductionTime > 0.0)) {
                    view_.showRegisterResult(RegisterProductSpecResult::InvalidProductionTime);
                    input = view_.promptRegisterInput();
                    continue;
                }
                if (repository_.findById(input.productSpecId).has_value()) {
                    view_.showRegisterResult(RegisterProductSpecResult::DuplicateId);
                    input = view_.promptRegisterInput();
                    continue;
                }
                break;
            }
            bool confirm = view_.promptConfirm(input);
            result = registerSpec(input, confirm);
            view_.showRegisterResult(result);
        } else if (choice == 2) {
            view_.showList(listAll());
        } else if (choice == 3) {
            std::string keyword = view_.promptSearchKeyword();
            std::vector<ProductSpec> results = search(keyword);
            view_.showSearchResults(results);
        } else {
            return;
        }
    }
}
