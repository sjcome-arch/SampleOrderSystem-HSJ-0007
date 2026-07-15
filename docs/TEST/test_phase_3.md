# test_phase_3.md - Phase 3: 시료 주문 및 주문 접수 큐 테스트 계획

> 대상 설계: [design_phase_3.md](../DESIGN/design_phase_3.md)
> 테스트 파일: `Tests/Model/waiting_approval_queue_test.cpp`, `Tests/Controller/order_controller_test.cpp`

## 1. 테스트 대상

- `WaitingApprovalQueue`의 FIFO 동작(`enqueue`/`front`/`dequeue`/`isEmpty`/`snapshot`)
- `OrderController`의 시료 주문 메뉴 흐름과 저장 순서(파일 우선 → 큐 반영)

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `WaitingApprovalQueue_Enqueue_PreservesInsertionOrder` | 주문 A, B, C를 순서대로 enqueue | `snapshot()` 호출 | [A, B, C] 순서로 반환 |
| 2 | `WaitingApprovalQueue_Dequeue_RemovesFrontOnly` | A, B, C가 쌓인 큐 | `dequeue()` 1회 호출 | `front()`가 B, `snapshot()`은 [B, C] |
| 3 | `WaitingApprovalQueue_IsEmpty_TrueWhenNoOrders` | 빈 큐 | `isEmpty()` 호출 | `true` |
| 4 | `OrderController_Reserve_CreatesOrderWithReservedStatus` | 등록된 시료 존재 | 시료 ID/고객명/수량 입력 → 확인(Y) | `Order.status == RESERVED`, `OrderRepository`에 저장됨 |
| 5 | `OrderController_Reserve_SavesBeforeEnqueue` | 위와 동일 | 주문 생성 확정 | `OrderRepository::add` 호출이 `WaitingApprovalQueue::enqueue`보다 먼저 실행됨(순서 검증용 spy/mock) |
| 6 | `OrderController_Reserve_CancelOnNoConfirm` | 입력 완료 후 확인 단계에서 N 선택 | - | 주문이 생성되지 않고, 큐에도 추가되지 않음 |
| 7 | `WaitingApprovalQueue_RebuildFromRepository_MatchesOrderNumberAscending` | RESERVED 주문 3건이 파일에 저장된 상태(재시작 시뮬레이션) | `findByStatus(RESERVED)`를 주문번호 오름차순 정렬 후 enqueue | 재구성된 큐 순서가 원래 접수 순서와 동일 |

## 3. Safety Test (선택)

- 존재하지 않는 시료 ID로 주문 시도 시 안내 문구가 출력되고 `Order`가 생성되지 않는지.
- 주문 수량 0 또는 음수 입력 시 재입력을 요청하는지.

## 4. 참고

- 생산 큐(Phase 5)와 별개의 자료구조임을 확인하는 테스트는 `test_phase_5.md`의 통합 테스트에서 함께 다룬다.
