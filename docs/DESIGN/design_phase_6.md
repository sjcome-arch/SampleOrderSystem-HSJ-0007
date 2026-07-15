# design_phase_6.md - Phase 6: 출고 처리

> [design.md](./design.md)의 Phase 목록 중 6번째 문서.
> 선행 Phase: [design_phase_5.md](./design_phase_5.md) (생산 완료 시 CONFIRMED 전환)

- 대상 요구사항: [REQUIREMENT.md 5.7 출고 처리](../../REQUIREMENT.md#57-출고-처리)

## 1. 구현 범위

- `CONFIRMED` 상태 주문 목록 출력(번호/주문번호/고객/시료/수량) 후 출고할 번호 입력받기
- 출고 처리: 상태를 `RELEASED`로 전환하고 처리 결과(주문번호/출고수량/처리일시/상태 변경) 출력
- 출고 처리 완료 후 최신 `CONFIRMED` 목록을 다시 출력(방금 출고된 주문 제외)

## 2. 검증 방법 (Verify)

- 출고 처리 후 해당 주문이 `RELEASED`로 바뀌고, 갱신된 출고 가능 목록에서 제외되는지 확인한다.
- `PRODUCING` 상태 주문은 출고 대상 목록에 나타나지 않는지 확인한다(Phase 5에서 `CONFIRMED`로
  전환되어야만 출고 가능).

## 3. 리뷰 포인트 (Review)

- 상태명이 `RELEASED`로 일관되게 사용되는지 (REQUIREMENT.md 원문의 `RELEASE` 표기 혼용 이슈 없이).
