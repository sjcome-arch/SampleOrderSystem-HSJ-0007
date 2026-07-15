#include "Controller/order_controller.h"

OrderController::OrderController(IOrderRepository& orderRepository, IProductSpecRepository& productSpecRepository,
                                  WaitingApprovalQueue& waitingApprovalQueue, OrderView& view)
    : orderRepository_(orderRepository),
      productSpecRepository_(productSpecRepository),
      waitingApprovalQueue_(waitingApprovalQueue),
      view_(view) {}

void OrderController::run() { view_.showPlaceholder(); }
