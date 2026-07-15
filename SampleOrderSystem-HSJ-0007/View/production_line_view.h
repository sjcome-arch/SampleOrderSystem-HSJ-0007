#pragma once

#include <vector>

struct ProductionQueueRow;
struct CurrentProductionInfo;

// 생산 라인 상태(RUNNING/WAITING), 대기 큐 목록 출력 (REQUIREMENT.md 5.6).
class ProductionLineView {
public:
    int promptMenuChoice() const;
    void showWaitingQueue(const std::vector<ProductionQueueRow>& rows) const;
    void showCurrentProduction(const CurrentProductionInfo& info) const;
};
