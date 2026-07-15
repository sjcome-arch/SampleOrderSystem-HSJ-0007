#pragma once

#include "Model/order.h"

// MainMenuController와 MainMenuView가 공유하는 입출력 데이터 구조.

// 메인 메뉴 진입 시 표시하는 시스템 현황 (REQUIREMENT.md 5.1).
struct SystemStatus {
    Time now;
    int registeredSpecCount = 0;
    int totalStock = 0;
    int totalOrderCount = 0;
    int productionWaitingCount = 0;
};
