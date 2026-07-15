# test_phase_5.md - Phase 5: 생산 라인 테스트 계획

> 대상 설계: [design_phase_5.md](../DESIGN/design_phase_5.md)
> 테스트 파일: `Tests/Model/production_line_test.cpp`

## 1. 테스트 대상

- `ceil(부족분/수율)` 실 생산량 공식
- `ProductionLine`의 FIFO 처리, `stock`/`availableStock` 갱신, `tick()` 완료 판정
- 완료 예정 시간 누적 계산과 `Time`/`duration` 타입 변환

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `ActualProductionQuantityFormulaRoundsUpCorrectly` | 부족분 50, 수율 0.5 | 계산 | `actualProductionQuantity == 100` |
| 2 | `ActualProductionQuantityFormulaHandlesNonDivisibleYield` | 부족분 1, 수율 0.92 | 계산 | `actualProductionQuantity == 2` |
| 3 | `ProductionLineEnqueueSetsProductionStartedAtWhenQueueWasEmpty` | 빈 큐 | `enqueue(order, t0)` | `order.productionStartedAt == t0` |
| 4 | `ProductionLineEnqueueDoesNotSetStartedAtWhenQueueNotEmpty` | 이미 처리 중인 주문 1건 | 새 주문 `enqueue` | 새 주문의 `productionStartedAt`은 비어있음(`std::nullopt`) |
| 5 | `ProductionLineCompleteCurrentUpdatesStockByFullActualQuantity` | 부족분 50, 수율 0.5(`actualProductionQuantity=100`) | `completeCurrent()` | `stock += 100` (부족분 50이 아니라 전체) |
| 6 | `ProductionLineCompleteCurrentUpdatesAvailableStockBySurplusOnly` | 위와 동일 | `completeCurrent()` | `availableStock += (100-50)=50` |
| 7 | `ProductionLineCompleteCurrentTransitionsOrderToConfirmed` | `PRODUCING` 주문 | `completeCurrent()` | `status == CONFIRMED`, 큐에서 `pop` |
| 8 | `ProductionLineCompleteCurrentSetsNextOrderAsNewFront` | 대기 주문 2건 이상 | 첫 번째 완료 | 두 번째 주문이 새 front가 되고 `productionStartedAt`이 그 시점으로 채워짐 |
| 9 | `ProductionLineFIFOProcessesInEnqueueOrder` | 주문 X, Y를 순서대로 enqueue | 순차적으로 완료 처리 | X가 먼저 완료되고, Y는 그 이후에만 완료 가능 |
| 10 | `ExpectedCompletionTimeFrontEqualsStartedAtPlusTotalTime` | `productionStartedAt=t0`, `totalProductionTime=30`(분) | 완료 예정 시간 계산 | `t0 + 30분`과 일치 |
| 11 | `ExpectedCompletionTimeSecondInQueueAccumulatesPreviousTime` | front `totalProductionTime=30`, 두 번째 항목 `totalProductionTime=20` | 두 번째 항목의 완료 예정 시간 계산 | `t0 + 30분 + 20분` |
| 12 | `TickNotYetDueDoesNotCompleteOrder` | `productionStartedAt=t0`, `totalProductionTime=30` | `tick(t0 + 29분)` | 상태 변화 없음(`PRODUCING` 유지) |
| 13 | `TickExactlyDueCompletesOrder` | 위와 동일 | `tick(t0 + 30분)` | `CONFIRMED`로 전환 |
| 14 | `TickMultipleOverdueOrdersCompletesAllInOneCall` | 대기 중 2건 모두 완료 시각을 지난 상태 | `tick(now)` 1회 호출 | 두 건 모두 `CONFIRMED`로 전환(while 루프 동작) |
| 15 | `ProductionLineIsEmptyReturnsWaitingState` | 큐가 비어있음 | `state()` 호출 | `WAITING` 반환 |

## 3. 테스트 시 시간 처리 원칙

- `test.md` 4절("시간 의존 로직 테스트 원칙")에 따라, `system_clock::now()`를 호출하지 않고
  `Time t0 = std::chrono::system_clock::time_point{} + std::chrono::hours(1);`처럼 고정된
  `Time` 리터럴을 `enqueue`/`tick`의 파라미터로 직접 전달한다.

## 4. Safety Test (선택)

- 생산 큐가 비어 있는 상태에서 `currentOrder()`를 호출했을 때의 동작(예외/기본값 정책 확정 필요).

## 5. 참고

- 재고 갱신 공식의 근거는 [design_phase_5.md - 3](../DESIGN/design_phase_5.md#3-실-생산량과-수율-처리-방식), 완료 예정 시간 계산은 [4.2](../DESIGN/design_phase_5.md#42-완료-예정-시간-계산), tick은 [4.3](../DESIGN/design_phase_5.md#43-완료-판정-시점-tick) 참조.
