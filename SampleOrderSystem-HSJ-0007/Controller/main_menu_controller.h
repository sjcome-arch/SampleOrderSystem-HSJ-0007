#pragma once

#include "Controller/dummy_data_generator_controller.h"
#include "Controller/main_menu_types.h"
#include "Controller/monitoring_controller.h"
#include "Controller/order_controller.h"
#include "Controller/product_spec_controller.h"
#include "Controller/production_line_controller.h"
#include "Controller/release_controller.h"
#include "Model/production_line.h"
#include "Repository/i_order_repository.h"
#include "Repository/i_product_spec_repository.h"
#include "View/main_menu_view.h"

// 메뉴 표시 → 입력 → 하위 Controller 디스패치 (REQUIREMENT.md 5.1).
class MainMenuController {
public:
    MainMenuController(MainMenuView& view, IProductSpecRepository& productSpecRepository,
                        IOrderRepository& orderRepository, ProductionLine& productionLine,
                        ProductSpecController& productSpecController, OrderController& orderController,
                        MonitoringController& monitoringController,
                        ProductionLineController& productionLineController, ReleaseController& releaseController,
                        DummyDataGeneratorController& dummyDataGeneratorController);

    void run();

    // 콘솔 I/O와 분리된 순수 로직 (단위 테스트 대상).
    SystemStatus systemStatus() const;

private:
    MainMenuView& view_;
    IProductSpecRepository& productSpecRepository_;
    IOrderRepository& orderRepository_;
    ProductionLine& productionLine_;
    ProductSpecController& productSpecController_;
    OrderController& orderController_;
    MonitoringController& monitoringController_;
    ProductionLineController& productionLineController_;
    ReleaseController& releaseController_;
    DummyDataGeneratorController& dummyDataGeneratorController_;
};
