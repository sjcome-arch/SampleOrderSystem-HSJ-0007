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
- 재고 현황: 시료별 현재 재고와 상태(여유/부족/고갈) 표기
  - 여유: 주문 대비 재고 충분, 부족: 주문 대비 재고 수량 부족, 고갈: 재고 수량 0

### 2.1 사용하는 자료구조

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

## 4. 리뷰 포인트 (Review)

- 모니터링은 Repository의 `findAll`/`findByStatus`만 읽고, 어떤 데이터도 변경하지 않는지
  (읽기 전용이어야 함).
- `WaitingApprovalQueue`/`ProductionLine`의 내부 큐에 직접 접근하는 코드가 없는지.
