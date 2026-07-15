#pragma once

#include "Controller/dummy_data_generator_controller.h"
#include "Controller/monitoring_controller.h"
#include "Controller/order_controller.h"
#include "Controller/product_spec_controller.h"
#include "Controller/production_line_controller.h"
#include "Controller/release_controller.h"
#include "View/main_menu_view.h"

// 메뉴 표시 → 입력 → 하위 Controller 디스패치 (REQUIREMENT.md 5.1).
// 실제 메뉴 선택/디스패치 흐름은 Phase 2 이후 하위 Controller들이 갖춰지면 구현한다.
class MainMenuController {
public:
    MainMenuController(MainMenuView& view, ProductSpecController& productSpecController,
                        OrderController& orderController, MonitoringController& monitoringController,
                        ProductionLineController& productionLineController, ReleaseController& releaseController,
                        DummyDataGeneratorController& dummyDataGeneratorController);

    void run();

private:
    MainMenuView& view_;
    ProductSpecController& productSpecController_;
    OrderController& orderController_;
    MonitoringController& monitoringController_;
    ProductionLineController& productionLineController_;
    ReleaseController& releaseController_;
    DummyDataGeneratorController& dummyDataGeneratorController_;
};
