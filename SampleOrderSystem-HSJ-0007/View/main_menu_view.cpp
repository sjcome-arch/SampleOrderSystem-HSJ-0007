#include "View/main_menu_view.h"

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

void MainMenuView::showSystemStatus(const SystemStatus& status) const {
    std::cout << "\n=== 반도체 시료 생산주문관리 시스템 ===\n";
    std::cout << "현재시간=" << formatTime(status.now) << ", 등록시료=" << status.registeredSpecCount
               << "종, 총재고=" << status.totalStock << ", 전체주문=" << status.totalOrderCount
               << "건, 생산대기=" << status.productionWaitingCount << "건" << std::endl;
}

int MainMenuView::promptMenuChoice() const {
    std::cout << "\n[1] 시료 관리\n[2] 시료 주문\n[3] 주문 승인/거절\n[4] 모니터링\n[5] 출고 처리\n"
                  "[6] 생산 라인\n[7] Dummy 데이터 생성\n[0] 종료\n선택 > ";
    int choice = -1;
    std::cin >> choice;
    return choice;
}

void MainMenuView::showInvalidChoice() const { std::cout << "잘못된 값이 입력되었습니다." << std::endl; }
