# test_phase_4.md - Phase 4: 주문 승인/거절 테스트 계획

> 대상 설계: [design_phase_4.md](../DESIGN/design_phase_4.md)
> 테스트 파일: `Tests/Controller/order_controller_approval_test.cpp`

## 1. 테스트 대상

- `availableStock` 기준 승인 분기(충분/부족), 거절 처리
- 재고 부족 시 실 생산량/총 생산 시간 계산과 저장 순서
- 2.2 예시 시나리오(재고 100, 주문 30/30/100) 재현

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `ApproveSufficientAvailableStockConfirmsImmediately` | `availableStock=100`, 주문 수량 30 | 승인 처리 | `status==CONFIRMED`, `availableStock==70`, `stock`은 불변(100) |
| 2 | `ApproveExactBoundaryAvailableStockTreatedAsSufficient` | `availableStock==주문 수량` (경계값) | 승인 처리 | `CONFIRMED` (부족 아님) |
| 3 | `ApproveInsufficientAvailableStockTransitionsToProducing` | `availableStock=40`, 주문 수량 100 | 승인 처리 | `status==PRODUCING`, `shortageQuantity==60`, `availableStock==0`, `stock` 불변 |
| 4 | `ApproveInsufficientComputesActualProductionQuantityAndTime` | `shortageQuantity=60`, 수율 0.6, 평균생산시간 2분 | 승인 처리 | `actualProductionQuantity==ceil(60/0.6)==100`, `totalProductionTime==200` |
| 5 | `ApproveInsufficientPersistsBeforeEnqueue` | 재고 부족 케이스 | 승인 처리 | `ProductSpecRepository::update`/`OrderRepository::update` 호출이 `ProductionLine::enqueue`보다 먼저 실행됨 |
| 6 | `ApproveInsufficientOutputsActualProductionQuantityImmediately` | 재고 부족 케이스 | 승인 처리 결과 화면 출력 | 실 생산량/예상 생산 시간이 승인 즉시 화면에 표시됨(생산 완료까지 지연되지 않음) |
| 7 | `RejectOrderTransitionsToRejectedAndKeepsAvailableStock` | `RESERVED` 주문 | 거절 처리 | `status==REJECTED`, `availableStock` 불변 |
| 8 | `ApproveRejectRemovesOrderFromWaitingApprovalQueue` | 접수 큐에 주문 존재 | 승인 또는 거절 처리 | 처리된 주문이 `WaitingApprovalQueue`에서 제거됨(dequeue) |
| 9 | `ExampleScenario100Stock3030100Orders` | `stock=100`, `availableStock=100` | 주문 30→승인, 주문 30→승인, 주문 100→승인 | 순서대로 `(stock,availableStock)` = (100,70) → (100,40) → (100,0), `shortageQuantity==60` |
| 10 | `StockInquiryDisplayShowsBothStockAndAvailableStock` | 임의의 시료/주문 | 재고 파악 출력 | 화면에 `stock`과 `availableStock` 값이 모두 표시됨 |

## 3. Safety Test (선택)

- 접수 큐가 비어 있는 상태에서 승인/거절 시도 시 안내 문구 출력 여부.

## 4. 참고

- 예시 시나리오 수치의 원본 출처는 [design_phase_4.md - 2.2](../DESIGN/design_phase_4.md#22-예시-시나리오-stockavailablestock-추적).
- 실 생산량 공식(`ceil`)의 세부 단위 테스트는 `test_phase_5.md`에서 중복 없이 다룬다(여기서는 승인 흐름 통합 테스트에 집중).
