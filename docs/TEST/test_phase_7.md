# test_phase_7.md - Phase 7: 모니터링 테스트 계획

> 대상 설계: [design_phase_7.md](../DESIGN/design_phase_7.md)
> 테스트 파일: `Tests/Controller/monitoring_controller_test.cpp`

## 1. 테스트 대상

- 상태별 주문 건수 집계 (`REJECTED` 제외)
- `stock`/`availableStock` 기반 여유/부족/고갈 판정
- 모니터링이 읽기 전용이며 Repository만 사용하는지

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `StatusCount_ExcludesRejectedOrders` | `RESERVED` 2건, `REJECTED` 1건 | 상태별 주문 현황 집계 | `REJECTED`는 집계에 포함되지 않음 |
| 2 | `StatusCount_CountsAllFourValidStatuses` | 각 상태(`RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASED`) 1건씩 | 집계 | 4개 상태 모두 1건으로 카운트 |
| 3 | `InventoryStatus_ZeroStock_ClassifiedAsDepleted` | `stock == 0` | 재고 현황 판정 | "고갈" |
| 4 | `InventoryStatus_StockPositiveButAvailableZero_ClassifiedAsShortage` | `stock=100`, `availableStock=0` | 재고 현황 판정 | "부족" (2.2 예시 시나리오의 세 번째 주문 승인 직후 상태와 동일) |
| 5 | `InventoryStatus_AvailableStockPositive_ClassifiedAsSurplus` | `stock=100`, `availableStock=40` | 재고 현황 판정 | "여유" |
| 6 | `InventoryDisplay_ShowsBothStockAndAvailableStock` | 임의의 시료 | 재고 현황 출력 | 화면에 `stock`과 `availableStock` 값이 함께 표시됨 |
| 7 | `Monitoring_DoesNotMutateAnyData` | 임의 데이터 상태 | 모니터링 메뉴 조회(반복 호출 포함) | 조회 전후로 `data/*.json` 내용이 전혀 변하지 않음 |
| 8 | `Monitoring_UsesRepositoryOnly_NoDirectQueueAccess` | - | 코드 리뷰/정적 확인 | `MonitoringController`/`MonitoringView`가 `WaitingApprovalQueue`/`ProductionLine`을 직접 참조하지 않음 |

## 3. Safety Test (선택)

- 시료/주문이 하나도 없는 초기 상태에서 모니터링 진입 시 0건/빈 목록으로 정상 출력되는지.

## 4. 참고

- 판정 기준의 근거는 [design_phase_7.md - 2.1](../DESIGN/design_phase_7.md#21-여유부족고갈-판정-기준) 참조.
