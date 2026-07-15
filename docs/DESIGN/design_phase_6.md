# design_phase_6.md - Phase 6: 출고 처리

> [design.md](./design.md)의 Phase 목록 중 6번째 문서.
> 선행 Phase: [design_phase_5.md](./design_phase_5.md) (생산 완료 시 CONFIRMED 전환)

- 대상 요구사항: [REQUIREMENT.md 5.7 출고 처리](../../REQUIREMENT.md#57-출고-처리)

## 1. 구현 목표 (Why)

- `CONFIRMED` 상태(즉시 확정 또는 생산 완료)에 도달한 주문을 실제 출고(`RELEASED`)로 마무리하는,
  주문 상태 머신의 마지막 전이를 구현하는 것이 목표다. 이 Phase가 끝나야 하나의 주문이
  `RESERVED → ... → RELEASED`까지 완결된 흐름을 갖게 된다.

## 2. 구현 범위

- `CONFIRMED` 상태 주문 목록 출력(번호/주문번호/고객/시료/수량) 후 출고할 번호 입력받기
- 출고 처리: 상태를 `RELEASED`로 전환하고 처리 결과(주문번호/출고수량/처리일시/상태 변경) 출력
- 출고 처리 완료 후 최신 `CONFIRMED` 목록을 다시 출력(방금 출고된 주문 제외)

### 2.1 큐에서 제거하는 절차가 없는 이유

- 출고 처리는 `WaitingApprovalQueue`나 `ProductionLine`의 큐에서 항목을 제거(dequeue/pop)하는
  절차를 포함하지 않는다. `CONFIRMED` 상태에 도달한 시점에 이미 모든 큐에서 빠져나온 상태이기
  때문이다.
  - 재고 충분으로 즉시 `CONFIRMED`가 된 주문: 애초에 어떤 큐에도 들어간 적이 없다
    (`WaitingApprovalQueue`에서 `dequeue()`만 되고, 생산 큐에는 등록되지 않는다).
  - 재고 부족으로 `PRODUCING`을 거쳐 `CONFIRMED`가 된 주문: `ProductionLine::completeCurrent()`가
    실행되는 시점에 이미 `productionQueue_.pop()`으로 큐에서 제거된다
    ([design_phase_5.md - 4.1 `ProductionLine` 클래스](./design_phase_5.md#41-productionline-클래스-modelproduction_lineh--cpp) 참조).
- 즉 `CONFIRMED` 상태의 주문은 정의상 "큐 밖"에 있는 일반 레코드이고, 본 Phase는
  `OrderRepository::findByStatus(CONFIRMED)`로 조회한 뒤 상태를 `RELEASED`로 바꾸고
  `OrderRepository::update()`로 저장하기만 하면 된다.

## 3. 검증 방법 (Verify)

- 출고 처리 후 해당 주문이 `RELEASED`로 바뀌고, 갱신된 출고 가능 목록에서 제외되는지 확인한다.
- `PRODUCING` 상태 주문은 출고 대상 목록에 나타나지 않는지 확인한다(Phase 5에서 `CONFIRMED`로
  전환되어야만 출고 가능).

## 4. 리뷰 포인트 (Review)

- 상태명이 `RELEASED`로 일관되게 사용되는지 (REQUIREMENT.md 원문의 `RELEASE` 표기 혼용 이슈 없이).
