#pragma once

#include "Controller/main_menu_types.h"

// 메인 메뉴 목록/시스템 현황 출력 (REQUIREMENT.md 5.1).
class MainMenuView {
public:
    void showSystemStatus(const SystemStatus& status) const;
    int promptMenuChoice() const;
    void showInvalidChoice() const;
};
