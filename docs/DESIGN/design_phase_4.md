# design_phase_4.md - Phase 4: 주문 승인/거절

> [design.md](./design.md)의 Phase 목록 중 4번째 문서.
> 선행 Phase: [design_phase_3.md](./design_phase_3.md) (주문 접수 큐)

- 대상 요구사항: [REQUIREMENT.md 5.4 주문 승인/거절](../../REQUIREMENT.md#54-주문-승인거절)

## 1. 구현 범위

- `ReservationQueue`에서 FIFO 순서로 주문을 조회/처리한다 ([design_phase_3.md](./design_phase_3.md) 참조).
- 승인 처리 분기:
  - 재고 파악 출력 → 재고 충분 시 `CONFIRMED` 전환
  - 재고 파악 출력 → 재고 부족 시 `PRODUCING` 전환 + 생산 큐([design_phase_5.md](./design_phase_5.md) 참조)에 등록
- 거절 처리: 즉시 `REJECTED` 전환 (정상 흐름에서 제외, 모니터링에서도 제외)
- 처리 결과 출력 (주문번호, 상태 변경 내역, 재고 부족 시 실 생산량/예상 생산 시간 포함)

## 2. 검증 방법 (Verify)

- 재고가 충분한 시료 주문을 승인하면 즉시 `CONFIRMED`가 되는지 확인한다.
- 재고가 부족한 시료 주문을 승인하면 `PRODUCING`으로 전환되고 생산 큐에 등록되는지 확인한다.
- 거절한 주문이 이후 모니터링(Phase 7)의 상태별 집계에서 제외되는지 확인한다.

## 3. 리뷰 포인트 (Review)

- 재고 비교 로직(주문 수량 vs 현재 재고)이 정확한지, 경계값(정확히 재고와 동일한 수량)을 올바르게
  "재고 충분"으로 처리하는지.
- 승인/거절 처리 후 해당 주문이 주문 접수 큐에서 제거되는지.
