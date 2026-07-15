#include "View/production_line_view.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Controller/production_line_controller.h"

namespace {

std::string formatTime(const Time& t) {
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    std::tm tm{};
    localtime_s(&tm, &tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

}  // namespace

int ProductionLineView::promptMenuChoice() const {
    std::cout << "\n[생산 라인]\n1. 대기 주문 확인\n2. 생산 현황\n0. 뒤로가기\n선택: ";
    int choice = 0;
    std::cin >> choice;
    return choice;
}

void ProductionLineView::showWaitingQueue(const std::vector<ProductionQueueRow>& rows) const {
    std::cout << "\n[생산 큐 대기 목록 (FIFO 순)]\n";
    if (rows.empty()) {
        std::cout << "대기 중인 생산 주문이 없습니다." << std::endl;
        return;
    }
    for (const auto& row : rows) {
        std::cout << row.sequence << ". 주문번호=" << row.order.orderId << ", 시료명=" << row.spec.name
                   << ", 주문량=" << row.order.quantity << ", 부족분=" << row.order.shortageQuantity
                   << ", 실생산량=" << row.order.actualProductionQuantity
                   << ", 완료예정=" << formatTime(row.expectedCompletion) << std::endl;
    }
}

void ProductionLineView::showCurrentProduction(const CurrentProductionInfo& info) const {
    if (!info.isRunning) {
        std::cout << "\n[생산 현황] WAITING - 현재 처리 중인 주문 없음" << std::endl;
        return;
    }
    std::cout << "\n[생산 현황] RUNNING\n"
               << "주문번호: " << info.order.orderId << "\n"
               << "주문량: " << info.order.quantity << "\n"
               << "주문 시 재고: " << info.order.availableStockAtApproval << "\n"
               << "부족수량: " << info.order.shortageQuantity << "\n"
               << "실생산량: " << info.order.actualProductionQuantity << "\n"
               << "수율: " << info.spec.yield << "\n"
               << "생산시간: " << info.order.totalProductionTime << "\n"
               << "완료예정: " << formatTime(info.expectedCompletion) << std::endl;
}
