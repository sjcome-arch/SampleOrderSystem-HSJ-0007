#pragma once

#include "Model/order.h"
#include "Model/product_spec.h"

// ReleaseController와 ReleaseView가 공유하는 입출력 데이터 구조.

// "출고 가능 목록" 한 줄(순번, 주문, 시료 정보).
struct ReleasableOrderRow {
    int sequence = 0;
    Order order;
    ProductSpec spec;
};

enum class ReleaseOutcome {
    Success,
    InvalidSelection,
};

// 출고 처리 결과. Success일 때만 order가 RELEASED로 갱신된 최신 값이다.
struct ReleaseResult {
    ReleaseOutcome outcome = ReleaseOutcome::InvalidSelection;
    Order order;
};
