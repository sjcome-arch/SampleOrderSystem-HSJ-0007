#include "Controller/release_controller.h"

ReleaseController::ReleaseController(IOrderRepository& orderRepository, ReleaseView& view)
    : orderRepository_(orderRepository), view_(view) {}

void ReleaseController::run() { view_.showPlaceholder(); }
