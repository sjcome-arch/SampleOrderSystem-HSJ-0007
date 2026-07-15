# design_phase_7.md - Phase 7: 모니터링

> [design.md](./design.md)의 Phase 목록 중 7번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md) ~ [design_phase_6.md](./design_phase_6.md)
> (모든 상태 전이가 구현된 이후에 집계 대상이 확정된다)

- 대상 요구사항: [REQUIREMENT.md 5.5 모니터링](../../REQUIREMENT.md#55-모니터링)

## 1. 구현 범위

- 상태별 주문 현황: `RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASED` 건수 집계 (`REJECTED` 제외)
- 재고 현황: 시료별 현재 재고와 상태(여유/부족/고갈) 표기
  - 여유: 주문 대비 재고 충분, 부족: 주문 대비 재고 수량 부족, 고갈: 재고 수량 0

## 2. 검증 방법 (Verify)

- `REJECTED` 주문이 상태별 집계에 포함되지 않는지 확인한다.
- 재고 0인 시료가 "고갈"로, 주문 대비 부족한 시료가 "부족"으로 정확히 분류되는지 확인한다.

## 3. 리뷰 포인트 (Review)

- 모니터링은 Repository의 `findAll`/`findByStatus`만 읽고, 어떤 데이터도 변경하지 않는지
  (읽기 전용이어야 함).
