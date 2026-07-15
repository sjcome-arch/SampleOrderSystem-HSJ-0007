# design_phase_5.md - Phase 5: 생산 라인

> [design.md](./design.md)의 Phase 목록 중 5번째 문서.
> 선행 Phase: [design_phase_4.md](./design_phase_4.md) (주문 승인 시 생산 큐 등록)

- 대상 요구사항: [REQUIREMENT.md 5.6 생산 라인](../../REQUIREMENT.md#56-생산-라인)

## 1. 구현 목표 (Why)

- 재고 부족으로 생산 큐에 등록된 `PRODUCING` 주문을, 단일 생산 라인이 FIFO 순서에 따라 실제로
  처리·완료(`CONFIRMED`로 전환 + 재고 반영)하는 것이 목표다. Phase 4에서 갈라진 "재고 부족" 분기가
  실제로 재고를 채워 넣는 마무리 단계다.
- **역할 분리**: 실 생산량/총 생산 시간 계산(`ceil(부족분/수율)` 등, 아래 3절 공식)은 승인
  시점([design_phase_4.md - 승인(재고 부족) 시 실 생산량 계산 및 생산 큐 등록](./design_phase_4.md#21-승인재고-부족-시-실-생산량-계산-및-생산-큐-등록))에
  이미 끝나 있다. 본 Phase는 그 계산 결과를 **재계산 없이 그대로** 사용해 FIFO 처리·재고 반영·
  예상 완료 시간 누적만 담당한다. 아래 3절의 공식 설명은 "언제 계산하는지"가 아니라 "그 계산이
  무엇인지"를 정의하는 공용 참조 자료다.

## 2. 생산 큐 스케줄링 (FIFO)

- 생산 큐는 단일 생산 라인이 처리할 `PRODUCING` 주문을 담는 큐이며, 접수(등록)된 순서대로
  처리한다(FIFO). 임의 순서로 새치기하여 먼저 생산되는 주문은 없다.
- "대기 주문 확인" 메뉴는 이 큐의 현재 스냅샷을 접수 순서 그대로(맨 앞 = 현재 처리 중 또는
  다음 처리 대상) 출력한다.
- 큐 자료구조는 언어 표준 큐(예: `std::queue`)를 사용하고, 생산 완료 시 `pop`하여 다음 주문을
  꺼낸다.
- 생산 큐와 주문 접수 큐(Phase 3)는 서로 다른 자료구조임에 유의한다.

## 3. 실 생산량과 수율 처리 방식

- 실 생산량은 `ceil(부족분 / 수율)`로 계산하며, 이 값은 수율을 반영해 **미리 여유분까지 포함한 수치**이다.
- 따라서 생산이 완료되면 실 생산량 전체를 정상 양산품으로 간주하여 재고에 더한다.
  생산 완료 시점에 수율을 다시 적용하여 불량품을 걸러내는 추가 시뮬레이션(랜덤 손실 등)은 하지 않는다.
  - 예: 부족분 50, 수율 0.5 → 실 생산량 = `ceil(50/0.5)` = 100개, 생산 완료 시 100개 모두 재고로 반영
- 즉 수율은 "생산 계획 수량 산정"에만 사용되고, "생산 결과의 품질 판정"에는 사용되지 않는다.
- 수율이 1 미만이면 실 생산량이 부족분보다 많아지며(위 예시에서 100 > 50), 이 초과분(여분)도
  전체 재고에 합산한다. 즉 재고 갱신 시 "부족분만큼만" 더하는 것이 아니라 "실 생산량 전체"를 더해야 한다.
  - 예시 기준: 재고 += 100 (부족분 50이 아니라 실 생산량 100 전체)
  - 여분(100 - 50 = 50)은 해당 주문에는 사용되지 않고 다른 주문을 위한 재고로 남는다.
- 생산 완료 시 주문 상태 `PRODUCING` → `CONFIRMED` 로 전환한다.

## 4. 구현 범위

### 4.1 `ProductionLine` 클래스 (`Model/production_line.h` / `.cpp`)

```cpp
class ProductionLine {
public:
    void enqueue(const Order& order);       // 승인(재고 부족) 시 호출, Phase 4에서 이미 계산된
                                             // 실 생산량/총 생산 시간을 담은 Order를 그대로 받는다
    bool isEmpty() const;
    LineState state() const;                // isEmpty() ? WAITING : RUNNING
    const Order& currentOrder() const;      // 현재 처리 중(front) 주문 조회
    void completeCurrent();                 // 생산 완료: PRODUCING → CONFIRMED, 재고 반영, pop
    std::vector<Order> snapshot() const;    // "대기 주문 확인"용, FIFO 순서(front→back) 그대로 반환

private:
    std::queue<Order> productionQueue_;
    ProductSpecRepository& productSpecRepository_;  // completeCurrent 시 재고 반영(update)
    OrderRepository& orderRepository_;               // 상태 변경 영속화(update)
};
```

- 생산 라인은 **1개**만 존재한다 (단일 라인, 생산 큐도 하나). 인스턴스는 프로그램 시작 시
  `main.cpp`에서 단 하나만 생성해 `OrderController`(Phase 4, 승인 시 `enqueue` 호출)와
  `ProductionLineController`(Phase 5, 조회/`completeCurrent` 호출)에 공유 주입한다
  ([design_phase_1.md - 의존 방향](./design_phase_1.md) 참조. `WaitingApprovalQueue`와 동일한 패턴).
- `completeCurrent()`는 **먼저** `productSpecRepository_.update(...)`(재고 반영)와
  `orderRepository_.update(...)`(상태 변경 저장)를 호출해 파일에 반영하고, **저장이 성공한
  뒤에만** `productionQueue_.pop()`한다(저장 순서 원칙은
  [design_phase_1.md - 3.1 메모리 큐와 파일 동기화 원칙](./design_phase_1.md#31-메모리-큐와-파일-동기화-원칙) 참조).
- 마찬가지로 `enqueue()`도 호출자(Phase 4)가 `OrderRepository::update`로 먼저 저장한 뒤 호출하는
  것을 전제로 한다.
- 프로그램 시작 시 `main.cpp`는 `OrderRepository::findByStatus(PRODUCING)`을 주문번호 오름차순
  정렬해 그 순서대로 `enqueue`하여 `productionQueue_`를 재구성한다(같은 3.1 참조). 즉 재시작해도
  생산 큐는 항상 종료 직전과 동일한 순서로 복원된다.
- front가 새로 정해질 때(생산 시작 시점, `enqueue()`로 빈 큐에 처음 들어올 때 또는
  `completeCurrent()`로 다음 항목이 front가 될 때) 그 `Order`의 `productionStartedAt`을 현재
  시각으로 채우고 저장한다.

### 4.2 완료 예정 시간 계산

- `Order`에 완료 예정 시간을 필드로 저장하지 않고, 조회(대기 주문 확인/현재 처리 중 조회) 시점에
  아래 방식으로 계산한다 (필드 정의는 [design_phase_1.md - 3.2 `Order` 클래스](./design_phase_1.md#32-order-클래스-필드-정의-modelorderh--cpp) 참조).
- 계산 방법: front부터 순서대로 `totalProductionTime`을 누적하되, 기준점(base)은 front의
  `productionStartedAt`이다.
  - front(현재 처리 중) 주문: 완료 예정 시간 = `productionStartedAt` + `totalProductionTime`
  - front보다 뒤에 있는 i번째 대기 주문: 완료 예정 시간 = front의 완료 예정 시간 +
    (front와 자신 사이에 있는 모든 주문의 `totalProductionTime` 합) + 자신의 `totalProductionTime`
  - 즉 REQUIREMENT.md 5.6의 "자신보다 먼저 대기 중인 주문들의 예상 생산 시간까지 누적"을,
    `productionStartedAt`이라는 절대 시각 기준점 위에서 누적합으로 구현한다.
- 큐는 뒤에서만 추가되고 앞에서만 제거되므로, 이미 큐에 있는 항목들의 완료 예정 시간은 새 주문이
  추가되어도 바뀌지 않는다 — 따라서 저장하지 않고 매번 계산해도 항상 같은 결과를 준다.

## 5. 검증 방법 (Verify)

- 부족분/수율 조합별 실 생산량 계산 단위 테스트: `ceil(부족분/수율)` 공식이 정확히 구현되었는지
  (예: 부족분 50, 수율 0.5 → 100; 부족분 1, 수율 0.92 → 2).
- 생산 큐에 여러 건이 쌓였을 때 FIFO 순서대로 처리되는지, 대기 목록의 예상 완료 시간이 누적
  계산되는지 확인한다.
- 생산 큐가 비어 있을 때 `WAITING` 상태와 "현재 처리 중인 주문 없음" 안내가 출력되는지 확인한다.
- 여러 건이 대기 중일 때, 뒤쪽 대기 항목의 완료 예정 시간이 `productionStartedAt` 기준으로
  앞선 모든 주문의 총 생산 시간을 정확히 누적한 값과 같은지 확인한다.

## 6. 리뷰 포인트 (Review)

- 생산 완료 후 재고 갱신이 "부족분"이 아니라 "실 생산량 전체"로 되어 있는지(가장 흔한 실수 지점).
- 생산 큐와 주문 접수 큐(Phase 3)가 실수로 하나의 자료구조로 합쳐지지 않았는지.
