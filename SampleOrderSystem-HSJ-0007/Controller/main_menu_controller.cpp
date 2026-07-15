#include "Controller/main_menu_controller.h"

MainMenuController::MainMenuController(MainMenuView& view, ProductSpecController& productSpecController,
                                        OrderController& orderController, MonitoringController& monitoringController,
                                        ProductionLineController& productionLineController,
                                        ReleaseController& releaseController,
                                        DummyDataGeneratorController& dummyDataGeneratorController)
    : view_(view),
      productSpecController_(productSpecController),
      orderController_(orderController),
      monitoringController_(monitoringController),
      productionLineController_(productionLineController),
      releaseController_(releaseController),
      dummyDataGeneratorController_(dummyDataGeneratorController) {}

void MainMenuController::run() { view_.showPlaceholder(); }
