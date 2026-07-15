#include "Controller/product_spec_controller.h"

ProductSpecController::ProductSpecController(IProductSpecRepository& repository, ProductSpecView& view)
    : repository_(repository), view_(view) {}

void ProductSpecController::run() { view_.showPlaceholder(); }
