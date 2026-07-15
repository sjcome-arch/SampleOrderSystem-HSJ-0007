#include "Controller/production_line_controller.h"

ProductionLineController::ProductionLineController(ProductionLine& productionLine, ProductionLineView& view)
    : productionLine_(productionLine), view_(view) {}

void ProductionLineController::run() { view_.showPlaceholder(); }
