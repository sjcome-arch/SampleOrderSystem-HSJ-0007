# design_phase_4.md - Phase 4: 주문 승인/거절

> [design.md](./design.md)의 Phase 목록 중 4번째 문서.
> 선행 Phase: [design_phase_3.md](./design_phase_3.md) (주문 접수 큐)

- 대상 요구사항: [REQUIREMENT.md 5.4 주문 승인/거절](../../REQUIREMENT.md#54-주문-승인거절)

## 1. 구현 목표 (Why)

- 생산 담당자가 주문 접수 큐(Phase 3)에 쌓인 `RESERVED` 주문에 대해 승인 또는 거절을 직접
  선택할 수 있게 하는 것이 목표다. 승인을 선택한 경우에 한해, 재고 충분 여부에 따라 즉시
  확정(`CONFIRMED`)할지 생산 큐(Phase 5)로 넘길지(`PRODUCING`)가 자동으로 갈린다 — 재고 상황이
  승인/거절 자체를 결정하는 것은 아니며, 거절은 재고와 무관하게 담당자의 선택으로 즉시
  `REJECTED`로 전환된다.

## 2. 구현 범위

- `WaitingApprovalQueue`에서 FIFO 순서로 주문을 조회/처리한다 ([design_phase_3.md](./design_phase_3.md) 참조).
- 승인 처리 분기는 **`ProductSpec.stock`(물리적 재고)이 아니라 `ProductSpec.availableStock`(가용
  재고)을 기준으로 판단한다** (두 필드의 구분은 [design_phase_2.md - 2.1](./design_phase_2.md#21-productspec-모델-modelproduct_spech--cpp) 참조):
  - 재고 파악 출력 → `availableStock >= 주문 수량`이면 충분 → `CONFIRMED` 전환,
    `availableStock -= 주문 수량` → 즉시 `ProductSpecRepository::update`와
    `OrderRepository::update(order)`로 저장
  - 재고 파악 출력 → `availableStock < 주문 수량`이면 부족 → `PRODUCING` 전환 + 생산 큐
    ([design_phase_5.md](./design_phase_5.md) 참조)에 등록 (2.1 참조)
- 거절 처리: 즉시 `REJECTED` 전환 → 즉시 `OrderRepository::update(order)`로 저장 (정상 흐름에서
  제외, 모니터링에서도 제외). `availableStock`은 건드리지 않는다.
- 처리 결과 출력 (주문번호, 상태 변경 내역, 재고 부족 시 실 생산량/예상 생산 시간 포함)

### 2.1 승인(재고 부족) 시 실 생산량 계산, 저장, 생산 큐 등록

- REQUIREMENT.md 5.4의 "처리 결과 출력"은 재고 부족으로 승인된 주문에 대해 **그 시점에** 실
  생산량과 예상 생산 시간을 함께 출력하도록 요구한다. 따라서 실 생산량 계산은 생산 큐가 나중에
  처리할 때가 아니라, **승인 처리(본 Phase)의 그 시점에 완료**되어야 한다.
- 계산 절차 (공식 자체는 [design_phase_5.md - 실 생산량과 수율 처리 방식](./design_phase_5.md#3-실-생산량과-수율-처리-방식) 참조):
  1. `availableStockAtApproval` = 승인 시점의 `ProductSpec.availableStock` 값을 그대로 기록
  2. `shortageQuantity` = 주문 수량 - `availableStockAtApproval`
  3. **`availableStock`을 0으로 만든다** — 부족 판정이 났다는 것은 가용 재고 전부가 이미 이
     주문에 배정되었다는 뜻이다(2.2 예시의 세 번째 주문에서 40 → 0이 되는 과정 참조).
     `stock`(물리적 재고)은 이 시점에 건드리지 않는다.
  4. `ProductSpecRepository`에서 해당 시료의 수율/평균 생산시간을 조회
  5. `actualProductionQuantity` = `ceil(shortageQuantity / 수율)`, `totalProductionTime` = 평균
     생산시간 * `actualProductionQuantity` (design_phase_5.md의 공식을 그대로 사용)
  6. 위 필드(`availableStockAtApproval`/`shortageQuantity`/`actualProductionQuantity`/`totalProductionTime`)와
     `status = PRODUCING`을 `Order`에 채운다(필드 정의는
     [design_phase_1.md - 3.2 `Order` 클래스](./design_phase_1.md#32-order-클래스-필드-정의-modelorderh--cpp) 참조).
- **동기화 시점**: 위 필드를 채운 직후, **다른 어떤 처리보다도 먼저** `ProductSpecRepository::update`
  (availableStock 반영)와 `OrderRepository::update(order)`를 호출해 파일에 즉시 반영한다. 저장이
  성공한 뒤에야 화면에 결과를 출력하고, `ProductionLine::enqueue(order)`를 호출해 메모리 큐에
  반영한다. 배치로 모아 저장하거나 생산 완료 시점까지 미루지 않는다(저장 순서 원칙은
  [design_phase_1.md - 3.1](./design_phase_1.md#31-메모리-큐와-파일-동기화-원칙) 참조).
- **Phase 5는 이 값을 재계산하지 않고 그대로 사용**하여 FIFO 순서 처리·완료 시 재고 반영·예상
  완료 시간 누적만 담당한다(계산 시점과 처리 시점의 역할 분리는
  [design_phase_5.md - 구현 목표](./design_phase_5.md#1-구현-목표-why) 참조). 생산 완료 시
  `availableStock`을 어떻게 되돌리는지는 [design_phase_5.md - 3](./design_phase_5.md#3-실-생산량과-수율-처리-방식) 참조.

### 2.2 예시 시나리오 (`stock`/`availableStock` 추적)

시료 A의 초기 상태: `stock = 100`, `availableStock = 100`.

| 순서 | 이벤트 | 판단 | 결과 | `stock` | `availableStock` |
|---|---|---|---|---|---|
| 1 | A 30개 주문 승인 | `availableStock(100) >= 30` → 충분 | `CONFIRMED`, `availableStock -= 30` | 100 | 70 |
| 2 | A 30개 주문 승인 | `availableStock(70) >= 30` → 충분 | `CONFIRMED`, `availableStock -= 30` | 100 | 40 |
| (이 시점 모니터링) | - | - | - | **100** | **40** |
| 3 | A 100개 주문 승인 | `availableStock(40) < 100` → 부족 | `shortageQuantity = 100 - 40 = 60`, `availableStock = 0`, `PRODUCING` 등록 | 100 | 0 |
| 3 생산 완료 | (수율 적용, 예: `actualProductionQuantity = 100`) | - | `stock += 100` (전체), `availableStock += (100 - 60) = 40`(잉여만), 주문3 `CONFIRMED` | 200 | 40 |

- 1~2번째 주문은 이미 있던 재고로 즉시 처리되므로 `stock`은 그대로고 `availableStock`만 줄어든다.
  이 시점에 모니터링을 보면 `stock`은 여전히 100이지만, 실제로 새 주문에 배정 가능한 양은 40뿐이다
  — 그래서 `availableStock`이라는 별도 필드가 필요하다.
- 3번째 주문은 `availableStock`이 40뿐이라 60개가 부족하고, 그 60개만큼만 생산에 들어간다(전체
  100개를 다시 생산하지 않는다).
- 생산이 끝나면(예시에서 수율에 따라 100개가 만들어졌다고 가정) `stock`에는 생산량 전체(100)가
  더해지고, 그중 주문3이 필요로 했던 60개는 주문3에 적용되어 소진되고, 나머지 잉여(40개)만
  `availableStock`으로 남아 다음 주문에 쓰일 수 있다.

## 3. 검증 방법 (Verify)

- 재고가 충분한 시료 주문을 승인하면 즉시 `CONFIRMED`가 되고 `availableStock`이 주문 수량만큼
  줄어드는지 확인한다(`stock` 자체는 변하지 않아야 한다).
- 재고가 부족한 시료 주문을 승인하면 `PRODUCING`으로 전환되고 생산 큐에 등록되며,
  `availableStock`이 0이 되는지 확인한다.
- 거절한 주문이 이후 모니터링(Phase 7)의 상태별 집계에서 제외되는지 확인한다.
- 재고 부족으로 승인 처리한 결과 화면에 실 생산량과 예상 생산 시간이 **승인 즉시** 출력되는지
  확인한다(생산 큐 처리 시점까지 계산이 미뤄지지 않는지).
- 2.2 예시 시나리오를 그대로 재현했을 때 각 단계의 `stock`/`availableStock` 값이 표와 일치하는지
  확인한다.

## 4. 리뷰 포인트 (Review)

- 재고 비교 로직이 `stock`이 아니라 `availableStock` 기준인지, 경계값(정확히 `availableStock`과
  동일한 수량)을 올바르게 "충분"으로 처리하는지.
- 승인/거절 처리 후 해당 주문이 주문 접수 큐에서 제거되는지.
- 실 생산량/총 생산 시간 계산이 승인 시점(본 Phase)에서 한 번만 이루어지고, Phase 5에서 중복
  재계산되지 않는지.
- 재고 충분으로 즉시 `CONFIRMED` 처리할 때 `stock`을 실수로 함께 줄이고 있지 않은지(감소는
  출고 시점에만 일어나야 한다, [design_phase_6.md](./design_phase_6.md) 참조).
