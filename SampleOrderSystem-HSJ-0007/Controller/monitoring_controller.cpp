#include "Controller/monitoring_controller.h"

MonitoringController::MonitoringController(IProductSpecRepository& productSpecRepository,
                                            IOrderRepository& orderRepository, MonitoringView& view)
    : productSpecRepository_(productSpecRepository), orderRepository_(orderRepository), view_(view) {}

void MonitoringController::run() { view_.showPlaceholder(); }
