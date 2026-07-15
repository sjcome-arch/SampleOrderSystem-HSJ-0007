#include "View/release_view.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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

void ReleaseView::showReleasableList(const std::vector<ReleasableOrderRow>& rows) const {
    std::cout << "\n[출고 가능 목록 (CONFIRMED)]\n";
    if (rows.empty()) {
        std::cout << "출고 가능한 주문이 없습니다." << std::endl;
        return;
    }
    for (const auto& row : rows) {
        std::cout << row.sequence << ". 주문번호=" << row.order.orderId << ", 고객명=" << row.order.customerName
                   << ", 시료명=" << row.spec.name << ", 수량=" << row.order.quantity << std::endl;
    }
}

int ReleaseView::promptSelection() const {
    std::cout << "출고할 번호를 입력하세요 (0: 뒤로가기): ";
    int selection = 0;
    std::cin >> selection;
    return selection;
}

void ReleaseView::showInvalidSelection() const { std::cout << "잘못된 값이 입력되었습니다." << std::endl; }

void ReleaseView::showReleaseResult(const ReleaseResult& result) const {
    if (result.outcome != ReleaseOutcome::Success) {
        std::cout << "출고 처리에 실패했습니다." << std::endl;
        return;
    }
    std::cout << "\n[출고 완료]\n주문번호=" << result.order.orderId << ", 출고수량=" << result.order.quantity
               << ", 처리일시=" << formatTime(*result.order.releasedAt) << ", 상태=CONFIRMED -> RELEASED"
               << std::endl;
}
