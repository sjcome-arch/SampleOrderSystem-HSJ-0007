#include "Controller/production_line_controller.h"

#include <chrono>

ProductionLineController::ProductionLineController(ProductionLine& productionLine,
                                                     IProductSpecRepository& productSpecRepository,
                                                     ProductionLineView& view)
    : productionLine_(productionLine), productSpecRepository_(productSpecRepository), view_(view) {}

std::vector<ProductionQueueRow> ProductionLineController::waitingQueue() const {
    std::vector<Order> orders = productionLine_.snapshot();
    std::vector<Time> expectedCompletions = productionLine_.expectedCompletions();

    std::vector<ProductionQueueRow> rows;
    rows.reserve(orders.size());
    for (size_t i = 0; i < orders.size(); ++i) {
        ProductionQueueRow row;
        row.sequence = static_cast<int>(i + 1);
        row.order = orders[i];
        row.spec = productSpecRepository_.findById(orders[i].productSpecId).value();
        row.expectedCompletion = expectedCompletions[i];
        rows.push_back(row);
    }
    return rows;
}

CurrentProductionInfo ProductionLineController::currentProduction() const {
    CurrentProductionInfo info;
    if (productionLine_.isEmpty()) {
        info.isRunning = false;
        return info;
    }
    info.isRunning = true;
    info.order = productionLine_.currentOrder();
    info.spec = productSpecRepository_.findById(info.order.productSpecId).value();
    info.expectedCompletion = productionLine_.expectedCompletions().front();
    return info;
}

void ProductionLineController::run() {
    productionLine_.tick(std::chrono::system_clock::now());

    int choice = view_.promptMenuChoice();
    if (choice == 1) {
        view_.showWaitingQueue(waitingQueue());
    } else if (choice == 2) {
        view_.showCurrentProduction(currentProduction());
    }
}
