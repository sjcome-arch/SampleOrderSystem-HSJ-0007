#include "View/order_view.h"

#include <iostream>

int OrderView::promptMenuChoice() const {
    std::cout << "\n[시료 주문]\n1. 주문 접수\n0. 뒤로가기\n선택: ";
    int choice = 0;
    std::cin >> choice;
    return choice;
}

ReserveOrderInput OrderView::promptReserveInput() const {
    ReserveOrderInput input;
    std::cout << "시료 ID: ";
    std::cin >> input.productSpecId;
    std::cout << "고객명: ";
    std::cin >> input.customerName;
    std::cout << "주문 수량: ";
    std::cin >> input.quantity;
    return input;
}

bool OrderView::promptConfirm(const ReserveOrderInput& input) const {
    std::cout << "시료ID=" << input.productSpecId << ", 고객명=" << input.customerName
              << ", 수량=" << input.quantity << " 로 주문하시겠습니까? (Y/N): ";
    char answer = 'N';
    std::cin >> answer;
    return answer == 'Y' || answer == 'y';
}

void OrderView::showReserveResult(ReserveOrderResult result) const {
    switch (result) {
        case ReserveOrderResult::Success:
            std::cout << "주문이 접수되었습니다(RESERVED)." << std::endl;
            break;
        case ReserveOrderResult::ProductSpecNotFound:
            std::cout << "등록되지 않은 시료 ID입니다. 다시 입력하세요." << std::endl;
            break;
        case ReserveOrderResult::InvalidQuantity:
            std::cout << "주문 수량은 1 이상이어야 합니다. 다시 입력하세요." << std::endl;
            break;
        case ReserveOrderResult::Cancelled:
            std::cout << "주문이 취소되었습니다." << std::endl;
            break;
    }
}

void OrderView::showWaitingApprovalList(const std::vector<Order>& orders) const {
    std::cout << "\n[접수된 주문 목록 (RESERVED, FIFO 순)]\n";
    if (orders.empty()) {
        std::cout << "접수된 주문이 없습니다." << std::endl;
        return;
    }
    for (const Order& order : orders) {
        std::cout << "주문번호=" << order.orderId << ", 시료ID=" << order.productSpecId
                   << ", 고객명=" << order.customerName << ", 수량=" << order.quantity << std::endl;
    }
}

void OrderView::showNoOrderToProcess() const {
    std::cout << "처리할 주문이 없습니다(접수 큐가 비어 있습니다)." << std::endl;
}

void OrderView::showStockInquiry(const Order& order, const ProductSpec& spec) const {
    std::cout << "\n[승인 대기 주문]\n주문번호=" << order.orderId << ", 시료ID=" << order.productSpecId
               << ", 시료명=" << spec.name << ", 주문수량=" << order.quantity
               << ", 가용재고=" << spec.availableStock << std::endl;
}

bool OrderView::promptApproveOrReject() const {
    std::cout << "승인하시겠습니까? (Y: 승인 / N: 거절): ";
    char answer = 'N';
    std::cin >> answer;
    return answer == 'Y' || answer == 'y';
}

void OrderView::showApproveResult(const ApproveNextResult& result) const {
    if (result.queueEmpty) {
        std::cout << "승인할 주문이 없습니다." << std::endl;
        return;
    }
    if (result.order.status == OrderStatus::CONFIRMED) {
        std::cout << "주문 " << result.order.orderId << "이(가) 승인되었습니다(CONFIRMED)." << std::endl;
    } else {
        std::cout << "주문 " << result.order.orderId << "이(가) 승인되었으나 재고가 부족해 생산 큐에 등록되었습니다"
                  << "(PRODUCING). 실 생산량=" << result.order.actualProductionQuantity
                  << ", 총 생산시간=" << result.order.totalProductionTime << std::endl;
    }
}

void OrderView::showRejectResult(const RejectNextResult& result) const {
    if (result.queueEmpty) {
        std::cout << "거절할 주문이 없습니다." << std::endl;
        return;
    }
    std::cout << "주문 " << result.order.orderId << "이(가) 거절되었습니다(REJECTED)." << std::endl;
}
