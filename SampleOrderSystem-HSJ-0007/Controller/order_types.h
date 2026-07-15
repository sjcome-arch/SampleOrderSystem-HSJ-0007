#pragma once

#include <string>

#include "Model/order.h"

// OrderController와 OrderView가 공유하는 입출력 데이터 구조.
// View가 OrderController 전체를 include하지 않도록 별도 헤더로 분리한다.

// 시료 주문 접수 입력값.
struct ReserveOrderInput {
    std::string productSpecId;
    std::string customerName;
    int quantity = 0;
};

enum class ReserveOrderResult {
    Success,
    ProductSpecNotFound,
    InvalidQuantity,
    Cancelled,
};

// 승인 처리 결과. queueEmpty가 true면 접수 큐가 비어 있거나(또는 시료를 찾을 수 없어) 처리할 주문이
// 없었음을 뜻한다.
struct ApproveNextResult {
    bool queueEmpty = true;
    Order order;
};

// 거절 처리 결과. queueEmpty가 true면 접수 큐가 비어 있어 처리할 주문이 없었음을 뜻한다.
struct RejectNextResult {
    bool queueEmpty = true;
    Order order;
};
