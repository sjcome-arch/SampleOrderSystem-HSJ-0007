#include "Controller/dummy_data_generator_controller.h"

DummyDataGeneratorController::DummyDataGeneratorController(IProductSpecRepository& productSpecRepository,
                                                             IOrderRepository& orderRepository,
                                                             DummyDataGeneratorView& view)
    : productSpecRepository_(productSpecRepository), orderRepository_(orderRepository), view_(view) {}

void DummyDataGeneratorController::run() { view_.showPlaceholder(); }
