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

### 생산 큐 스케줄링 (FIFO)

- 대상 요구사항: [REQUIREMENT.md 5.6 생산 라인](../../REQUIREMENT.md#56-생산-라인)
- 생산 큐는 단일 생산 라인이 처리할 `PRODUCING` 주문을 담는 큐이며, 접수(등록)된 순서대로
  처리한다(FIFO). 임의 순서로 새치기하여 먼저 생산되는 주문은 없다.
- "대기 주문 확인" 메뉴는 이 큐의 현재 스냅샷을 접수 순서 그대로(맨 앞 = 현재 처리 중 또는
  다음 처리 대상) 출력한다.
- 큐 자료구조는 언어 표준 큐(예: `std::queue`)를 사용하고, 생산 완료 시 `pop`하여 다음 주문을
  꺼낸다.

## 2. 실 생산량과 수율 처리 방식

- 대상 요구사항: [REQUIREMENT.md 5.6 생산 라인](../../REQUIREMENT.md#56-생산-라인)
- 실 생산량은 `ceil(부족분 / 수율)`로 계산하며, 이 값은 수율을 반영해 **미리 여유분까지 포함한 수치**이다.
- 따라서 생산이 완료되면 실 생산량 전체를 정상 양산품으로 간주하여 재고에 더한다.
  생산 완료 시점에 수율을 다시 적용하여 불량품을 걸러내는 추가 시뮬레이션(랜덤 손실 등)은 하지 않는다.
  - 예: 부족분 50, 수율 0.5 → 실 생산량 = `ceil(50/0.5)` = 100개, 생산 완료 시 100개 모두 재고로 반영
- 즉 수율은 "생산 계획 수량 산정"에만 사용되고, "생산 결과의 품질 판정"에는 사용되지 않는다.
- 수율이 1 미만이면 실 생산량이 부족분보다 많아지며(위 예시에서 100 > 50), 이 초과분(여분)도
  전체 재고에 합산한다. 즉 재고 갱신 시 "부족분만큼만" 더하는 것이 아니라 "실 생산량 전체"를 더해야 한다.
  - 예시 기준: 재고 += 100 (부족분 50이 아니라 실 생산량 100 전체)
  - 여분(100 - 50 = 50)은 해당 주문에는 사용되지 않고 다른 주문을 위한 재고로 남는다.

## 3. 프로젝트 구현 시나리오 (미션 1·2 대응)

- 대상 요구사항: [REQUIREMENT.md 7. 미션 및 제출 관련](../../REQUIREMENT.md#7-미션-및-제출-관련-참고)
- 본 Repository(SampleOrderSystem)는 미션2(프로젝트 개발) 산출물이지만, PDF 원문의 미션1 PoC 4개
  항목([CLAUDE.md - PoC 필수 구성 요소](../../CLAUDE.md#poc-필수-구성-요소) 참조)도 함께 충족해야
  하므로, 아래와 같이 하나의 구현 시나리오로 통합하여 진행한다.

### 3.1 디렉터리 구조 (MVC 스켈레톤 코드)

```
SampleOrderSystem-HSJ-0007/           # vcxproj 루트
├── Model/            # 도메인 엔티티 + 상태 전이 로직 (View/Controller 비의존)
│   ├── Sample.h / .cpp            # 시료 ID/시료명/평균생산시간/수율/재고
│   ├── Order.h / .cpp             # 주문번호/시료ID/고객명/수량/상태(RESERVED 등)
│   ├── OrderStatus.h              # enum class OrderStatus { RESERVED, REJECTED, ... }
│   ├── ReservationQueue.h / .cpp  # 주문 접수 큐 (1. 참조)
│   └── ProductionLine.h / .cpp    # 생산 큐 + 생산 라인 상태(RUNNING/WAITING)
├── Repository/       # 데이터 영속성 계층 (Model만 참조, View/Controller 비의존)
│   ├── SampleRepository.h / .cpp  # 시료 CRUD
│   └── OrderRepository.h / .cpp   # 주문 CRUD
├── View/             # 콘솔 입출력 전담 (Model을 직접 변경하지 않음)
│   ├── MainMenuView.h / .cpp
│   ├── SampleView.h / .cpp
│   ├── OrderView.h / .cpp
│   ├── MonitoringView.h / .cpp
│   ├── ProductionLineView.h / .cpp
│   └── ReleaseView.h / .cpp
├── Controller/       # 사용자 입력 처리, Model 조작, View 갱신 지시
│   ├── SampleController.h / .cpp
│   ├── OrderController.h / .cpp
│   ├── MonitoringController.h / .cpp
│   ├── ProductionLineController.h / .cpp
│   └── ReleaseController.h / .cpp
└── Main.cpp          # 진입점: Repository 로드 → Controller/View 연결 → 메인 메뉴 루프
```

- 계층 간 의존 방향은 `Controller → View`, `Controller → Model/Repository` 단방향으로 고정한다.
  View와 Model은 서로를 직접 참조하지 않는다.
- 각 메뉴(5.1~5.7)는 하나의 Controller에 대응시켜 책임을 명확히 분리한다.

### 3.2 데이터 영속성 처리 (파일/JSON/DB 중 선택 + CRUD)

- **선택 방식**: JSON 파일 기반 영속성. `nlohmann/json` 라이브러리를 vcpkg로 추가한다
  (`vcpkg install nlohmann-json` 후 vcxproj에 include/lib 경로 연결).
  - 이유: DB 서버 설치 없이도 사람이 읽을 수 있는 포맷으로 CRUD/모니터링/Dummy 데이터 생성 도구가
    동일한 파일을 공유하기 쉽다.
- **저장 파일**: `data/samples.json`, `data/orders.json` (실행 파일 기준 상대 경로).
- **Repository 인터페이스** (예시):
  ```cpp
  class SampleRepository {
  public:
      std::vector<Sample> findAll() const;
      std::optional<Sample> findById(const std::string& sampleId) const;
      std::vector<Sample> findByName(const std::string& keyword) const; // 시료 검색
      void add(const Sample& sample);                 // Create
      void update(const Sample& sample);               // Update (재고 변경 등)
      void remove(const std::string& sampleId);         // Delete
  private:
      void load();  // data/samples.json → 메모리
      void save();  // 메모리 → data/samples.json
  };
  ```
  `OrderRepository`도 동일한 패턴(findAll/findById/findByStatus/add/update)으로 구성한다.
- Model 계층(`ReservationQueue`, `ProductionLine`)은 Repository를 통해서만 데이터를 읽고 쓰며,
  파일 포맷을 직접 알지 못하도록 한다(Repository가 직렬화/역직렬화를 캡슐화).

### 3.3 데이터 모니터링 Tool

- 별도 콘솔 프로그램이 아니라, 메인 메뉴에 숨김 관리자 메뉴(예: `[9] 데이터 모니터링`)로 추가하거나,
  같은 Repository 계층을 재사용하는 별도의 작은 실행 파일(`tools/DataMonitor`)로 구성한다.
  둘 중 하나를 택하되, **Repository 계층을 그대로 재사용**하는 것이 핵심 조건이다.
- 기능: `data/samples.json`, `data/orders.json`을 즉시 다시 읽어(reload) 아래를 콘솔에 표시한다.
  - 전체 시료 목록(재고 포함), 전체 주문 목록(상태 포함), 상태별 주문 건수, 생산 큐/접수 큐 스냅샷
- 이 도구는 데이터를 변경하지 않는 **읽기 전용**이어야 하며, 실행 중인 메인 애플리케이션과 동시에
  띄워도 파일을 다시 읽어들이는 방식으로 최신 상태를 반영한다.

### 3.4 Dummy 데이터 생성 Tool

- `tools/DummyDataGenerator`로 별도 실행 파일을 구성하거나, 메인 메뉴 숨김 항목으로 제공한다.
- **반드시 `SampleRepository`/`OrderRepository`의 CRUD(add)를 통해서만 데이터를 추가**한다
  (임시 저장소나 별도 파일 포맷을 새로 만들지 않는다 — CLAUDE.md의 PoC 항목 주의사항과 동일).
- 생성 항목 예시:
  - 시료 N개: 시료명(랜덤 조합), 평균 생산시간(0.1~1.0 분 사이 난수), 수율(0.7~0.99 난수), 초기 재고
  - 주문 M개: 등록된 시료 중 무작위 선택, 고객명(더미 목록에서 무작위), 수량(난수), 상태는 `RESERVED`로
    생성 후 필요 시 임의로 승인/거절/생산/출고까지 진행시켜 상태 분포를 다양화
- 생성 개수는 실행 시 인자 또는 프롬프트 입력으로 지정 가능하게 한다.

### 3.5 미션 2 항목별 대응

| 주안점 | 대응 방안 |
|---|---|
| CLAUDE.md, PRD.md 등 문서 관리 | `REQUIREMENT.md`(PRD 역할) / `CLAUDE.md`(에이전트 가이드) / `docs/DESIGN/design.md`(구현 설계, 본 문서)로 문서 체계를 구성하고, 요구사항·설계 변경 시마다 즉시 갱신한다. |
| Harness 도입 | 단위 테스트를 실제로 실행/집계할 수 있는 테스트 하니스를 구성한다. vcpkg로 `Catch2`(또는 `GoogleTest`)를 추가하고, 별도 `Tests` vcxproj(또는 CTest 연동)를 솔루션에 추가해 `msbuild` 한 번으로 전체 테스트가 실행되도록 한다. |
| Test | Model 계층(상태 전이, 실 생산량/수율 계산, FIFO 큐 동작)부터 우선 테스트하고, Repository CRUD, Controller 분기(재고 충분/부족)까지 확장한다. 테스트 파일은 `Tests/Model/*.cpp`, `Tests/Repository/*.cpp`로 대상 계층별로 분리한다. |
| Clean Code | 코딩 컨벤션([CLAUDE.md - 코딩 컨벤션 및 구현 규칙](../../CLAUDE.md#코딩-컨벤션-및-구현-규칙))을 지키고, 함수/클래스 단위를 작게 유지한다(PR 100라인 이내). |
| Commit 이력 | 기능 단위로 `[feature]/[fix]/[refactor]/[test]/[docs]/[chore]` 접두사를 사용해 커밋하며, 커밋 전 항상 사용자 리뷰를 받는다([CLAUDE.md - 커밋 전 리뷰 절차](../../CLAUDE.md#커밋-전-리뷰-절차)). |

## 4. 향후 구현 결정 사항 (TBD)

아래 항목은 구현을 진행하며 결정되는 대로 이 문서에 추가한다.

- 주문 ID / 시료 ID 채번 규칙 (예: `ORD-YYYYMMDD-NNNN`, `S-NNN`)
- 데이터 모니터링 Tool / Dummy 데이터 생성 Tool을 "숨김 메뉴"로 둘지 "별도 실행 파일"로 둘지 최종 확정
- vcpkg 도입 여부 및 매니페스트(`vcpkg.json`) 구성
