# design_phase_8.md - Phase 8: 데이터 모니터링 Tool / Dummy 데이터 생성 Tool 이식

> [design.md](./design.md)의 Phase 목록 중 8번째 문서(PoC 이식 Phase).
> 선행 Phase: [design_phase_1.md](./design_phase_1.md) (Repository/영속성 계층이 먼저 있어야 재사용 가능)

- 대상 요구사항: [REQUIREMENT.md 7. 미션 및 제출 관련](../../REQUIREMENT.md#7-미션-및-제출-관련-참고)
  (미션1 PoC "데이터 모니터링 Tool", "Dummy 데이터 생성 Tool" 항목)

## 1. 데이터 모니터링 Tool

- 참조 PoC: [DataMonitor-HSJ-0007](https://github.com/sjcome-arch/DataMonitor-HSJ-0007)
- 별도 콘솔 프로그램이 아니라, 메인 메뉴에 숨김 관리자 메뉴(예: `[9] 데이터 모니터링`)로 추가하거나,
  같은 Repository 계층을 재사용하는 별도의 작은 실행 파일(`tools/DataMonitor`)로 구성한다.
  둘 중 하나를 택하되, **Repository 계층을 그대로 재사용**하는 것이 핵심 조건이다.
- 기능: `data/product_specs.json`, `data/orders.json`을 즉시 다시 읽어(reload) 아래를 콘솔에 표시한다.
  - 전체 시료 목록(재고 포함), 전체 주문 목록(상태 포함), 상태별 주문 건수, 생산 큐/접수 큐 스냅샷
- 이 도구는 데이터를 변경하지 않는 **읽기 전용**이어야 하며, 실행 중인 메인 애플리케이션과 동시에
  띄워도 파일을 다시 읽어들이는 방식으로 최신 상태를 반영한다(PoC의 `reload()` 패턴 재사용).
- **생산 큐/접수 큐 스냅샷 재구성**: 이 도구는 메인 애플리케이션의 `WaitingApprovalQueue`/
  `ProductionLine` 메모리 객체에 접근할 수 없으므로, FIFO 순서를 파일 데이터만으로 재구성해야
  한다. `OrderRepository::findByStatus`로 `RESERVED`(접수 큐)/`PRODUCING`(생산 큐) 주문을 조회한
  뒤, **주문번호(`ORD-NNNNNN`) 오름차순으로 정렬**하면 실제 큐 순서와 일치한다 — 주문번호가
  생성 순서대로 순차 채번되고([design.md - 6.1 주문번호 채번 규칙](./design.md#61-주문번호order-number-채번-규칙)
  참조), 승인 처리(Phase 4)가 `WaitingApprovalQueue`에서 항상 FIFO로만 진행되어 두 큐 모두
  임의 순서로 뒤섞이지 않기 때문이다.

## 2. Dummy 데이터 생성 Tool

- 참조 PoC: [DummyDataGenerator-HSJ-0007](https://github.com/sjcome-arch/DummyDataGenerator-HSJ-0007)
- `tools/DummyDataGenerator`로 별도 실행 파일을 구성하거나, 메인 메뉴 숨김 항목으로 제공한다.
- **반드시 `ProductSpecRepository`/`OrderRepository`의 CRUD(add)를 통해서만 데이터를 추가**한다
  (임시 저장소나 별도 파일 포맷을 새로 만들지 않는다 — CLAUDE.md의 PoC 항목 주의사항과 동일).
- 생성 항목 예시:
  - 시료 N개: 시료명(랜덤 조합), 평균 생산시간(0.1~1.0 분 사이 난수), 수율(0.7~0.99 난수), 초기 재고
  - 주문 M개: 등록된 시료 중 무작위 선택, 고객명(더미 목록에서 무작위), 수량(난수), 상태는 `RESERVED`로
    생성 후 필요 시 임의로 승인/거절/생산/출고까지 진행시켜 상태 분포를 다양화
- 생성 개수는 실행 시 인자 또는 프롬프트 입력으로 지정 가능하게 한다.

## 3. 검증 방법 (Verify)

- Dummy 데이터 생성 Tool 실행 후 생성된 항목이 `data/product_specs.json`/`data/orders.json`에 실제로
  저장되는지 확인한다.
- 데이터 모니터링 Tool을 Dummy 데이터 생성 직후 실행(또는 새로고침)해 방금 생성된 데이터가
  즉시 반영되는지 확인한다.

## 4. 리뷰 포인트 (Review)

- 두 도구가 본 프로젝트의 Repository를 그대로 재사용하고 있는지(별도 JSON 로직을 새로 만들지 않았는지).
