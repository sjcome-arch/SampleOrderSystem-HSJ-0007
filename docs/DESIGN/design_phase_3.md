# design_phase_3.md - Phase 3: 시료 주문 및 주문 접수 큐

> [design.md](./design.md)의 Phase 목록 중 3번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md), [design_phase_2.md](./design_phase_2.md)

- 대상 요구사항: [REQUIREMENT.md 5.3 시료 주문](../../REQUIREMENT.md#53-시료-주문)

## 1. 주문 접수 큐 (Reservation Queue)

- 고객 주문이 생성되어 `RESERVED` 상태가 되면, 별도의 **주문 접수 큐**에 적재한다.
- 생산 담당자가 주문 승인/거절을 처리할 때는 이 큐에서 **FIFO(선입선출)** 순서로 꺼내어 처리한다
  (승인/거절 처리 로직 자체는 [design_phase_4.md](./design_phase_4.md) 참조).
  - 즉, "접수된 주문 목록"은 큐의 현재 스냅샷을 순서대로 보여주는 것이며,
    임의의 순서로 승인/거절 처리를 허용하지 않는다(먼저 접수된 주문부터 처리).
- 승인 또는 거절이 완료된 주문은 큐에서 제거(dequeue)된다.

### 주의: 생산 큐(Production Queue)와의 구분

- REQUIREMENT.md 5.6에 정의된 **생산 큐**([design_phase_5.md](./design_phase_5.md) 참조)는
  `PRODUCING` 상태로 전환된 주문들이 실제 생산 라인에서 처리되기를 기다리는 별도의 FIFO 큐이다.
- 즉 이 프로젝트에는 **두 개의 독립된 FIFO 큐**가 존재한다.

| 큐 이름 | 대상 상태 | 처리 주체 | 처리 결과 |
|---|---|---|---|
| 주문 접수 큐 (Reservation Queue) | `RESERVED` | 생산 담당자 (승인/거절) | `CONFIRMED` / `PRODUCING` / `REJECTED` |
| 생산 큐 (Production Queue) | `PRODUCING` | 생산 라인 | `CONFIRMED` (생산 완료) |

두 큐를 혼동하지 않도록 클래스/변수 명명 시 `reservationQueue`, `productionQueue`처럼 구분한다.

## 2. 구현 범위

- `ReservationQueue`: `std::queue<Order>` 기반, push(예약 접수)/front(다음 처리 대상 조회)/pop(승인·거절 완료 시)
- `OrderController`의 시료 주문 메뉴: 입력 → 입력 내용 확인(Y/N) → 확정 시 `RESERVED` 생성 후
  `ReservationQueue`에 적재 → 주문 완료 정보 출력 (REQUIREMENT.md 5.3 참조)

## 3. 검증 방법 (Verify)

- 여러 건을 연속 예약했을 때 접수된 주문 목록(5.4에서 조회)이 입력 순서와 동일하게 나오는지 확인한다.
- 입력 취소(N) 선택 시 주문이 생성되지 않는지 확인한다.

## 4. 리뷰 포인트 (Review)

- 주문 접수 큐와 생산 큐가 서로 다른 자료구조 인스턴스로 분리되어 있는지.
- 주문 담당자 단계(본 Phase)에서 `REJECTED`/`CONFIRMED` 등 승인 관련 상태로 직접 전환하는 코드가
  없는지(그 책임은 생산 담당자 단계인 Phase 4에 있어야 한다).
