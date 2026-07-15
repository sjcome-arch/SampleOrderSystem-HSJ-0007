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

### 2.2 출고 시 `stock` 차감

- `ProductSpec.stock`(물리적 재고)은 **출고(RELEASED) 처리 시점에만 감소**한다. 승인/확정
  (`CONFIRMED`) 시점에는 `stock`을 건드리지 않고 `availableStock`(가용 재고)만 줄어든다는 점은
  이미 [design_phase_4.md - 2.1](./design_phase_4.md#21-승인재고-부족-시-실-생산량-계산-저장-생산-큐-등록)/[design_phase_2.md - 2.1](./design_phase_2.md#21-productspec-모델-modelproduct_spech--cpp)에서
  정했다. 출고는 그 물리적 재고가 실제로 창고에서 나가는 시점이므로, 여기서 비로소 `stock`을
  차감한다.
- 처리 절차: 출고할 주문을 고른 뒤, **먼저** `ProductSpecRepository::update`로
  `stock -= order.quantity`를 반영하고, 이어서 `OrderRepository::update`로 `status = RELEASED`,
  `releasedAt = now`를 저장한다(저장 순서 원칙은 [design_phase_1.md - 3.1](./design_phase_1.md#31-메모리-큐와-파일-동기화-원칙) 참조).
- `availableStock`은 출고 시점에 건드리지 않는다 — 이 주문의 수요는 이미 승인 시점에
  `availableStock`에서 빠졌으므로(또는 부족분이었다면 생산 완료 시점에 반영되었으므로), 출고는
  물리적 재고 이동일 뿐 가용 재고 계산에는 영향이 없다.

## 3. 검증 방법 (Verify)

- 출고 처리 후 해당 주문이 `RELEASED`로 바뀌고, 갱신된 출고 가능 목록에서 제외되는지 확인한다.
- `PRODUCING` 상태 주문은 출고 대상 목록에 나타나지 않는지 확인한다(Phase 5에서 `CONFIRMED`로
  전환되어야만 출고 가능).
- 출고 처리 후 `ProductSpec.stock`이 출고 수량만큼 정확히 감소하는지, `availableStock`은
  변하지 않는지 확인한다.

## 4. 리뷰 포인트 (Review)

- 상태명이 `RELEASED`로 일관되게 사용되는지 (REQUIREMENT.md 원문의 `RELEASE` 표기 혼용 이슈 없이).
- `stock` 차감이 승인/확정 시점이 아니라 출고 시점에만 일어나는지, `availableStock`을 실수로
  함께 건드리고 있지 않은지.
