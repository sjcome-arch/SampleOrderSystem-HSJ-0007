#pragma once

#include <string>

// ProductSpecController와 ProductSpecView가 공유하는 입출력 데이터 구조.
// View가 ProductSpecController 전체를 include하지 않도록 별도 헤더로 분리한다.

// 시료 등록 입력값 (콘솔에서 수집된 값을 담는 순수 데이터 구조).
struct RegisterProductSpecInput {
    std::string productSpecId;
    std::string name;
    double avgProductionTime = 0.0;
    double yield = 0.0;
    int stock = 0;
};

enum class RegisterProductSpecResult {
    Success,
    InvalidYield,
    InvalidProductionTime,
    DuplicateId,
    Cancelled,
};
