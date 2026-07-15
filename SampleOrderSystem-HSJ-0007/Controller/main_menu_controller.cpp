#include "Controller/main_menu_controller.h"

#include <chrono>

MainMenuController::MainMenuController(MainMenuView& view, IProductSpecRepository& productSpecRepository,
                                        IOrderRepository& orderRepository, ProductionLine& productionLine,
                                        ProductSpecController& productSpecController,
                                        OrderController& orderController, MonitoringController& monitoringController,
                                        ProductionLineController& productionLineController,
                                        ReleaseController& releaseController,
                                        DummyDataGeneratorController& dummyDataGeneratorController)
    : view_(view),
      productSpecRepository_(productSpecRepository),
      orderRepository_(orderRepository),
      productionLine_(productionLine),
      productSpecController_(productSpecController),
      orderController_(orderController),
      monitoringController_(monitoringController),
      productionLineController_(productionLineController),
      releaseController_(releaseController),
      dummyDataGeneratorController_(dummyDataGeneratorController) {}

SystemStatus MainMenuController::systemStatus() const {
    SystemStatus status;
    status.now = std::chrono::system_clock::now();

    auto specs = productSpecRepository_.findAll();
    status.registeredSpecCount = static_cast<int>(specs.size());
    for (const auto& spec : specs) {
        status.totalStock += spec.stock;
    }

    status.totalOrderCount = static_cast<int>(orderRepository_.findAll().size());
    status.productionWaitingCount = static_cast<int>(productionLine_.snapshot().size());
    return status;
}

void MainMenuController::run() {
    for (;;) {
        // 지연 평가 완료 판정(design_phase_5.md 4.3): 화면 갱신 시점마다 tick으로 완료 주문을 반영한다.
        productionLine_.tick(std::chrono::system_clock::now());
        view_.showSystemStatus(systemStatus());

        int choice = view_.promptMenuChoice();
        switch (choice) {
            case 1:
                productSpecController_.run();
                break;
            case 2:
                orderController_.runReserve();
                break;
            case 3:
                orderController_.runApproval();
                break;
            case 4:
                monitoringController_.run();
                break;
            case 5:
                releaseController_.run();
                break;
            case 6:
                productionLineController_.run();
                break;
            case 7:
                dummyDataGeneratorController_.run();
                break;
            case 0:
                return;
            default:
                view_.showInvalidChoice();
                break;
        }
    }
}
