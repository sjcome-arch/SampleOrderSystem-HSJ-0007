# design_phase_3.md - Phase 3: 시료 주문 및 주문 접수 큐

> [design.md](./design.md)의 Phase 목록 중 3번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md), [design_phase_2.md](./design_phase_2.md)

- 대상 요구사항: [REQUIREMENT.md 5.3 시료 주문](../../REQUIREMENT.md#53-시료-주문)

## 1. 구현 목표 (Why)

- 고객이 등록된 시료를 실제로 주문할 수 있게 하고, 주문 즉시 승인되지 않도록 **주문 접수 큐**에
  대기시키는 것이 이 Phase의 목표다. 승인/거절 처리(Phase 4)가 접수 순서(FIFO)를 지키려면, 그
  이전 단계인 여기서 큐 자료구조와 `RESERVED` 상태 생성 로직을 먼저 확정해야 한다.

## 2. 주문 접수 큐 (Waiting Approval Queue)

- 고객 주문이 생성되어 `RESERVED` 상태가 되면, 별도의 **주문 접수 큐**에 적재한다.
- 생산 담당자가 주문 승인/거절을 처리할 때는 이 큐에서 **FIFO(선입선출)** 순서로 꺼내어 처리한다
  (승인/거절 처리 로직 자체는 [design_phase_4.md](./design_phase_4.md) 참조).
  - 즉, "접수된 주문 목록"은 큐의 현재 스냅샷을 순서대로 보여주는 것이며,
    임의의 순서로 승인/거절 처리를 허용하지 않는다(먼저 접수된 주문부터 처리).
- 승인 또는 거절이 완료된 주문은 큐에서 제거(dequeue)된다.

### 2.1 `WaitingApprovalQueue` 클래스 (`Model/waiting_approval_queue.h` / `.cpp`)

```cpp
class WaitingApprovalQueue {
public:
    void enqueue(const Order& order);        // 예약 접수 (RESERVED 생성 직후 호출)
    const Order& front() const;              // 다음 처리 대상 조회 (승인/거절 대상)
    void dequeue();                          // 승인/거절 완료 시 큐에서 제거
    bool isEmpty() const;
    std::vector<Order> snapshot() const;     // "접수된 주문 목록" 출력용 (큐 내용을 순서대로 복사)

private:
    std::queue<Order> queue_;
};
```

- `front()`/`dequeue()`는 승인/거절 처리(Phase 4)에서만 호출한다. 본 Phase(Phase 3)는 `enqueue()`만 사용한다.
- `snapshot()`은 `std::queue`를 직접 순회할 수 없으므로, 내부적으로 `std::deque` 등을 사용하거나
  임시 복사본을 순회해 큐 순서 그대로 목록을 반환한다.

### 2.2 시료 주문 메뉴 흐름 (`OrderController` — 예약 생성)

- **입력**: 시료 ID, 고객명, 주문 수량 (REQUIREMENT.md 5.3 참조)
- **입력 내용 확인**: 시료명/고객명/수량을 화면에 표시 → 확인(Y/N)
  - Y: `Order` 생성(상태 `RESERVED`) → `WaitingApprovalQueue::enqueue` → 주문 완료 정보 출력
  - N: 주문 생성 취소, 시료 주문 메뉴로 복귀
- 생성된 `Order`는 `OrderRepository::add`를 통해 영속화한다(큐는 메모리 상 처리 순서만 관리하고,
  실제 데이터는 Repository가 소유).

### 주의: 생산 큐(Production Queue)와의 구분

- REQUIREMENT.md 5.6에 정의된 **생산 큐**([design_phase_5.md](./design_phase_5.md) 참조)는
  `PRODUCING` 상태로 전환된 주문들이 실제 생산 라인에서 처리되기를 기다리는 별도의 FIFO 큐이다.
- 즉 이 프로젝트에는 **두 개의 독립된 FIFO 큐**가 존재한다.

| 큐 이름 | 대상 상태 | 처리 주체 | 처리 결과 |
|---|---|---|---|
| 주문 접수 큐 (Waiting Approval Queue) | `RESERVED` | 생산 담당자 (승인/거절) | `CONFIRMED` / `PRODUCING` / `REJECTED` |
| 생산 큐 (Production Queue) | `PRODUCING` | 생산 라인 | `CONFIRMED` (생산 완료) |

두 큐를 혼동하지 않도록 클래스/변수 명명 시 `waitingApprovalQueue`, `productionQueue`처럼 구분한다.

## 3. 구현 범위

- `WaitingApprovalQueue`: `std::queue<Order>` 기반, push(예약 접수)/front(다음 처리 대상 조회)/pop(승인·거절 완료 시)
- `OrderController`의 시료 주문 메뉴: 입력 → 입력 내용 확인(Y/N) → 확정 시 `RESERVED` 생성 후
  `WaitingApprovalQueue`에 적재 → 주문 완료 정보 출력 (REQUIREMENT.md 5.3 참조)

## 4. 검증 방법 (Verify)

- 여러 건을 연속 예약했을 때 접수된 주문 목록(5.4에서 조회)이 입력 순서와 동일하게 나오는지 확인한다.
- 입력 취소(N) 선택 시 주문이 생성되지 않는지 확인한다.

## 5. 리뷰 포인트 (Review)

- 주문 접수 큐와 생산 큐가 서로 다른 자료구조 인스턴스로 분리되어 있는지.
- 주문 담당자 단계(본 Phase)에서 `REJECTED`/`CONFIRMED` 등 승인 관련 상태로 직접 전환하는 코드가
  없는지(그 책임은 생산 담당자 단계인 Phase 4에 있어야 한다).
