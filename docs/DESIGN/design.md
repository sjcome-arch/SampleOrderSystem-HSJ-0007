# design.md - 구현 설계 문서

> 이 문서는 [REQUIREMENT.md](../../REQUIREMENT.md)의 기능 명세를 실제 코드로 구현하기 위한
> **구현 방식/자료구조/알고리즘 결정 사항**을 기록한다. "무엇을 만들어야 하는가"는
> REQUIREMENT.md를, "어떻게 만들 것인가"는 이 문서를 따른다.

## 1. 주문 접수 큐 (Reservation Queue)

- 대상 요구사항: [REQUIREMENT.md 5.3 시료 주문](../../REQUIREMENT.md#53-시료-주문),
  [5.4 주문 승인/거절](../../REQUIREMENT.md#54-주문-승인거절)
- 고객 주문이 생성되어 `RESERVED` 상태가 되면, 별도의 **주문 접수 큐**에 적재한다.
- 생산 담당자가 주문 승인/거절을 처리할 때는 이 큐에서 **FIFO(선입선출)** 순서로 꺼내어 처리한다.
  - 즉, "접수된 주문 목록"은 큐의 현재 스냅샷을 순서대로 보여주는 것이며,
    임의의 순서로 승인/거절 처리를 허용하지 않는다(먼저 접수된 주문부터 처리).
- 승인 또는 거절이 완료된 주문은 큐에서 제거(dequeue)된다.

### 주의: 생산 큐(Production Queue)와의 구분

- REQUIREMENT.md 5.6에 정의된 **생산 큐**는 `PRODUCING` 상태로 전환된 주문들이 실제 생산 라인에서
  처리되기를 기다리는 별도의 FIFO 큐이다.
- 즉 이 프로젝트에는 **두 개의 독립된 FIFO 큐**가 존재한다.

| 큐 이름 | 대상 상태 | 처리 주체 | 처리 결과 |
|---|---|---|---|
| 주문 접수 큐 (Reservation Queue) | `RESERVED` | 생산 담당자 (승인/거절) | `CONFIRMED` / `PRODUCING` / `REJECTED` |
| 생산 큐 (Production Queue) | `PRODUCING` | 생산 라인 | `CONFIRMED` (생산 완료) |

두 큐를 혼동하지 않도록 클래스/변수 명명 시 `reservationQueue`, `productionQueue`처럼 구분한다.

## 2. 향후 구현 결정 사항 (TBD)

아래 항목은 구현을 진행하며 결정되는 대로 이 문서에 추가한다.

- 데이터 영속성 방식 (파일 / JSON / DB 중 선택)
- 주문 ID / 시료 ID 채번 규칙
- 콘솔 UI 렌더링 방식 (메뉴 네비게이션 구조)
