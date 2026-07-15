#pragma once

#include <string>

// 시료(Sample) 마스터 데이터. 등록된 시료만 주문 가능하다(REQUIREMENT.md 5.2).
class ProductSpec {
public:
    std::string productSpecId;
    std::string name;
    double avgProductionTime = 0.0;  // 평균 생산시간(분), 양수
    double yield = 1.0;              // 수율, 0 초과 ~ 1 이하
    int stock = 0;                   // 실제 재고(물리적 수량)
    int availableStock = 0;          // 가용 재고(아직 CONFIRMED 주문에 배정되지 않은 수량)
};
