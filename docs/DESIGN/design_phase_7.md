# design_phase_7.md - Phase 7: 모니터링

> [design.md](./design.md)의 Phase 목록 중 7번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md) ~ [design_phase_6.md](./design_phase_6.md)
> (모든 상태 전이가 구현된 이후에 집계 대상이 확정된다)

- 대상 요구사항: [REQUIREMENT.md 5.5 모니터링](../../REQUIREMENT.md#55-모니터링)

## 1. 구현 목표 (Why)

- Phase 1~6에서 구현된 주문 상태 전이와 재고 변화를 관리자가 실시간으로 한눈에 파악할 수 있는
  **읽기 전용** 현황판을 제공하는 것이 목표다. 데이터를 변경하지 않고 Repository 조회만으로
  상태별 집계와 재고 여유/부족/고갈을 보여준다.

## 2. 구현 범위

- 상태별 주문 현황: `RESERVED`/`CONFIRMED`/`PRODUCING`/`RELEASED` 건수 집계 (`REJECTED` 제외)
- 재고 현황: 시료별로 시료명, `stock`(물리적 재고), `availableStock`(가용 재고), 상태(여유/부족/고갈)를
  함께 출력한다.
  - REQUIREMENT.md 5.5는 "재고(수량)" 하나만 언급하지만, `stock`만 보여주면 "재고는 100인데 왜
    부족이지?" 같은 혼란이 생길 수 있어(2.1의 판정 기준 참조) `availableStock`도 함께 보여주기로
    한다(요구사항 문구를 넘어선 구현 결정).
  - 여유: 주문 대비 재고 충분, 부족: 주문 대비 재고 수량 부족, 고갈: 재고 수량 0

### 2.1 여유/부족/고갈 판정 기준

- REQUIREMENT.md의 "주문 대비 재고"는 `ProductSpec.stock`(물리적 재고)이 아니라
  `ProductSpec.availableStock`(가용 재고)을 기준으로 판정한다 — `availableStock`이 이미 "재고 중
  기존 `CONFIRMED` 주문에 배정되고 남은 양"을 뜻하기 때문이다(필드 구분은
  [design_phase_2.md - 2.1](./design_phase_2.md#21-productspec-모델-modelproduct_spech--cpp) 참조).
- 판정 순서 (REQUIREMENT.md 5.5의 "고갈: 재고 수량이 0"은 물리적 재고 기준이므로 최우선 적용):
  1. `stock == 0` → **고갈**
  2. `availableStock == 0` (하지만 `stock > 0`) → **부족** — 물리적 재고는 있지만 전부 기존
     확정 주문에 배정되어, 새 주문에는 즉시 내줄 수 있는 몫이 없다는 뜻이다.
  3. 그 외(`availableStock > 0`) → **여유** — 기존 확정 주문에 배정하고도 남은 가용 재고가 있다.
- 예시: [design_phase_4.md - 2.2 예시 시나리오](./design_phase_4.md#22-예시-시나리오-stockavailablestock-추적)에서
  두 번째 주문까지 처리된 시점(`stock=100`, `availableStock=40`)은 "여유"이고, 세 번째 주문이
  승인되어 `availableStock=0`이 된 시점은(`stock=100`이라도) "부족"이다.

### 2.2 사용하는 자료구조

- `WaitingApprovalQueue`/`ProductionLine`의 메모리 큐를 직접 순회하지 않는다. 모니터링에 필요한
  정보(상태별 건수, 재고 여유/부족/고갈)는 FIFO 순서와 무관하므로,
  `OrderRepository::findAll()`/`findByStatus(...)`와 `ProductSpecRepository::findAll()`로 조회한
  결과만 집계하면 된다. 큐 내부 구조에 접근하면 View/Controller가 Model의 내부 자료구조에
  결합되어 계층 분리 원칙([design_phase_1.md](./design_phase_1.md))에 어긋난다.
- Phase 7은 메인 애플리케이션 안의 메뉴(`MonitoringController`)로, Phase 3~6이 이미 쓰고 있는
  `OrderRepository`/`ProductSpecRepository` **공유 인스턴스**를 그대로 사용한다. "저장 우선" 원칙
  ([design_phase_1.md - 3.1](./design_phase_1.md#31-메모리-큐와-파일-동기화-원칙))에 따라 상태 변경마다
  파일에 즉시 반영되므로, 같은 프로세스 안에서 Repository의 메모리 상태는 항상 파일과 일치하는
  최신 상태다. 따라서 조회 전에 `reload()`를 호출할 필요가 없다.
- 반면 [design_phase_8.md](./design_phase_8.md)의 데이터 모니터링 Tool은 **별도 프로세스**라서
  메인 프로세스의 Repository 인스턴스를 공유할 수 없고, 그래서 그쪽만 `reload()`로 파일을 다시
  읽어야 한다 — Phase 7과 Phase 8은 이 점에서 다르다.

## 3. 검증 방법 (Verify)

- `REJECTED` 주문이 상태별 집계에 포함되지 않는지 확인한다.
- 재고 0인 시료가 "고갈"로, 주문 대비 부족한 시료가 "부족"으로 정확히 분류되는지 확인한다.
- 재고 현황 출력에 `stock`과 `availableStock`이 각각 올바른 값으로 함께 표시되는지 확인한다
  (예: `stock=100`, `availableStock=0`인 시료가 "부족"으로 표시되며 두 수치가 화면에 다 보이는지).

## 4. 리뷰 포인트 (Review)

- 모니터링은 Repository의 `findAll`/`findByStatus`만 읽고, 어떤 데이터도 변경하지 않는지
  (읽기 전용이어야 함).
- `WaitingApprovalQueue`/`ProductionLine`의 내부 큐에 직접 접근하는 코드가 없는지.
