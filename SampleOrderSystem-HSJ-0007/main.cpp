#include <algorithm>
#include <windows.h>

#include "Controller/dummy_data_generator_controller.h"
#include "Controller/main_menu_controller.h"
#include "Controller/monitoring_controller.h"
#include "Controller/order_controller.h"
#include "Controller/product_spec_controller.h"
#include "Controller/production_line_controller.h"
#include "Controller/release_controller.h"
#include "Model/production_line.h"
#include "Model/waiting_approval_queue.h"
#include "Repository/order_repository.h"
#include "Repository/product_spec_repository.h"
#include "View/dummy_data_generator_view.h"
#include "View/main_menu_view.h"
#include "View/monitoring_view.h"
#include "View/order_view.h"
#include "View/product_spec_view.h"
#include "View/production_line_view.h"
#include "View/release_view.h"

namespace {

std::vector<Order> sortedByOrderId(std::vector<Order> orders) {
    std::sort(orders.begin(), orders.end(), [](const Order& a, const Order& b) { return a.orderId < b.orderId; });
    return orders;
}

}  // namespace

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    ProductSpecRepository productSpecRepository("data/product_specs.json");
    OrderRepository orderRepository("data/orders.json");

    // 시작 시 재구성 (design_phase_1.md 3.1)
    WaitingApprovalQueue waitingApprovalQueue(sortedByOrderId(orderRepository.findByStatus(OrderStatus::RESERVED)));
    ProductionLine productionLine(sortedByOrderId(orderRepository.findByStatus(OrderStatus::PRODUCING)));

    MainMenuView mainMenuView;
    ProductSpecView productSpecView;
    OrderView orderView;
    MonitoringView monitoringView;
    ProductionLineView productionLineView;
    ReleaseView releaseView;
    DummyDataGeneratorView dummyDataGeneratorView;

    ProductSpecController productSpecController(productSpecRepository, productSpecView);
    OrderController orderController(orderRepository, productSpecRepository, waitingApprovalQueue, productionLine,
                                     orderView);
    MonitoringController monitoringController(productSpecRepository, orderRepository, monitoringView);
    ProductionLineController productionLineController(productionLine, productionLineView);
    ReleaseController releaseController(orderRepository, releaseView);
    DummyDataGeneratorController dummyDataGeneratorController(productSpecRepository, orderRepository,
                                                                dummyDataGeneratorView);

    MainMenuController mainMenuController(mainMenuView, productSpecController, orderController, monitoringController,
                                           productionLineController, releaseController,
                                           dummyDataGeneratorController);
    mainMenuController.run();

    return 0;
}
