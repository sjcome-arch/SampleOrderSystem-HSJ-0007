# design_phase_1.md - Phase 1: 프로젝트 기반 구조 (MVC 스켈레톤 + 데이터 영속성)

> [design.md](./design.md)의 Phase 목록 중 1번째 문서. 이후 모든 Phase가 이 구조 위에서 진행된다.
> 참조 PoC: [ConsoleMVC-HSJ-0007](https://github.com/sjcome-arch/ConsoleMVC-HSJ-0007),
> [DataPersistence-HSJ-0007](https://github.com/sjcome-arch/DataPersistence-HSJ-0007)

- 대상 요구사항: [REQUIREMENT.md 7. 미션 및 제출 관련](../../REQUIREMENT.md#7-미션-및-제출-관련-참고)
  (미션1 PoC "MVC 스켈레톤 코드", "데이터 영속성 처리" 항목)

## 1. 구현 목표 (Why)

- Phase 1은 이후 모든 Phase가 그 위에서 개발될 **기반 구조**를 만드는 것이 목표다. 도메인 로직을
  구현하기 전에 Model/Repository/View/Controller의 책임 분리와, 데이터를 파일에 저장·복원하는
  영속성 계층부터 확정해야 뒤따르는 Phase(시료 관리, 주문 등)가 흔들림 없이 진행된다.
- 동시에 미션1 PoC 항목 중 "MVC 스켈레톤 코드"와 "데이터 영속성 처리" 두 가지를 본 프로젝트에
  이식·확장하여 충족시키는 것도 이 Phase의 목표다.

## 2. 디렉터리 구조 (MVC 스켈레톤 코드)

```
SampleOrderSystem-HSJ-0007/           # vcxproj 루트
├── Model/            # 도메인 엔티티 + 상태 전이 로직 (View/Controller 비의존)
│   ├── product_spec.h / .cpp       # class ProductSpec: 시료 ID/시료명/평균생산시간/수율/재고
│   ├── order.h / .cpp              # class Order: 주문번호/시료ID/고객명/수량/상태(RESERVED 등)
│   ├── order_status.h              # enum class OrderStatus { RESERVED, REJECTED, ... }
│   ├── waiting_approval_queue.h / .cpp  # class WaitingApprovalQueue: 주문 접수 큐 (design_phase_3.md 참조)
│   └── production_line.h / .cpp    # class ProductionLine: 생산 큐 + 생산 라인 상태(RUNNING/WAITING, design_phase_5.md 참조)
├── Repository/       # 데이터 영속성 계층 (Model만 참조, View/Controller 비의존)
│   ├── product_spec_repository.h / .cpp  # class ProductSpecRepository: 시료 CRUD
│   └── order_repository.h / .cpp   # class OrderRepository: 주문 CRUD
├── View/             # 콘솔 입출력 전담 (Model을 직접 변경하지 않음)
│   ├── main_menu_view.h / .cpp       # class MainMenuView: 메인 메뉴 목록/시스템 현황 출력 (REQUIREMENT.md 5.1)
│   ├── product_spec_view.h / .cpp    # class ProductSpecView: 시료 등록/조회/검색 화면 출력 (REQUIREMENT.md 5.2)
│   ├── order_view.h / .cpp           # class OrderView: 시료 주문 입력·확인 및 주문 승인/거절 화면 출력 (REQUIREMENT.md 5.3, 5.4)
│   ├── monitoring_view.h / .cpp      # class MonitoringView: 상태별 주문 현황, 시료별 재고 현황 출력 (REQUIREMENT.md 5.5)
│   ├── production_line_view.h / .cpp # class ProductionLineView: 생산 라인 상태(RUNNING/WAITING), 대기 큐 목록 출력 (REQUIREMENT.md 5.6)
│   └── release_view.h / .cpp         # class ReleaseView: 출고 가능 목록/출고 처리 결과 출력 (REQUIREMENT.md 5.7)
├── Controller/       # 사용자 입력 처리, Model 조작, View 갱신 지시
│   ├── main_menu_controller.h / .cpp # class MainMenuController: 메뉴 표시→입력→하위 Controller 디스패치 (5.1)
│   ├── product_spec_controller.h / .cpp   # class ProductSpecController
│   ├── order_controller.h / .cpp          # class OrderController
│   ├── monitoring_controller.h / .cpp     # class MonitoringController
│   ├── production_line_controller.h / .cpp # class ProductionLineController
│   └── release_controller.h / .cpp        # class ReleaseController
└── main.cpp          # 진입점: Repository/하위 Controller 생성 → MainMenuController.run() 호출
```

- 파일명은 `snake_case`, 클래스명은 `PascalCase`를 사용한다 (Google C++ Style Guide 관례,
  [CLAUDE.md - 코딩 컨벤션 및 구현 규칙](../../CLAUDE.md#코딩-컨벤션-및-구현-규칙) 참조).
  예: `class ProductSpec` → `product_spec.h` / `product_spec.cpp`.

- 계층 간 의존 방향은 `Controller → View`, `Controller → Model/Repository` 단방향으로 고정한다.
  View와 Model은 서로를 직접 참조하지 않는다.
- 각 메뉴(REQUIREMENT.md 5.1~5.7)는 하나의 Controller에 대응시켜 책임을 명확히 분리한다.
- [ConsoleMVC-HSJ-0007](https://github.com/sjcome-arch/ConsoleMVC-HSJ-0007) PoC의 Model/View/Controller
  분리 구조를 그대로 이식하고, 도메인 엔티티만 Item → ProductSpec/Order로 교체한다.

### 계층별 파일명 목록

트리 구조의 파일명을 계층(Model/Repository/View/Controller) 기준으로 모아 정리한다.
파일명은 `snake_case`, 클래스명은 `PascalCase`를 사용한다(위 규칙 참조).

| 계층 | 파일명(.h / .cpp) | 클래스명 |
|---|---|---|
| Model | `product_spec.h` / `.cpp`<br>`order.h` / `.cpp`<br>`order_status.h`<br>`waiting_approval_queue.h` / `.cpp`<br>`production_line.h` / `.cpp` | `ProductSpec`<br>`Order`<br>`OrderStatus` (enum class)<br>`WaitingApprovalQueue`<br>`ProductionLine` |
| Repository | `product_spec_repository.h` / `.cpp`<br>`order_repository.h` / `.cpp` | `ProductSpecRepository`<br>`OrderRepository` |
| View | `main_menu_view.h` / `.cpp`<br>`product_spec_view.h` / `.cpp`<br>`order_view.h` / `.cpp`<br>`monitoring_view.h` / `.cpp`<br>`production_line_view.h` / `.cpp`<br>`release_view.h` / `.cpp` | `MainMenuView`<br>`ProductSpecView`<br>`OrderView`<br>`MonitoringView`<br>`ProductionLineView`<br>`ReleaseView` |
| Controller | `main_menu_controller.h` / `.cpp`<br>`product_spec_controller.h` / `.cpp`<br>`order_controller.h` / `.cpp`<br>`monitoring_controller.h` / `.cpp`<br>`production_line_controller.h` / `.cpp`<br>`release_controller.h` / `.cpp` | `MainMenuController`<br>`ProductSpecController`<br>`OrderController`<br>`MonitoringController`<br>`ProductionLineController`<br>`ReleaseController` |
| (진입점) | `main.cpp` | - |

- Model 계층에 `main_menu` 관련 파일이 없는 이유는 아래 "기능/엔티티별 4계층 대응표"의
  메인 메뉴(5.1) 행 설명을 참조한다 (자체 도메인 엔티티 없이 하위 Controller 디스패치만 담당).

### 기능/엔티티별 4계층 대응표

트리 구조만으로는 "Order 하나"가 Model/Repository/View/Controller 어디에 흩어져 있는지 한눈에
보기 어렵다. 아래 표로 기능(REQUIREMENT.md 메뉴) 단위 대응 관계를 명시한다.

클래스명과 파일명(`snake_case`)을 함께 표기한다.

| 기능(메뉴) | Model | Repository | View | Controller |
|---|---|---|---|---|
| 메인 메뉴 (5.1) | - | - | `MainMenuView`<br>(`main_menu_view.h` / `.cpp`) | `MainMenuController`<br>(`main_menu_controller.h` / `.cpp`, 하위 Controller 디스패치 전담) |
| 시료 관리 (5.2) | `ProductSpec`<br>(`product_spec.h` / `.cpp`) | `ProductSpecRepository`<br>(`product_spec_repository.h` / `.cpp`) | `ProductSpecView`<br>(`product_spec_view.h` / `.cpp`) | `ProductSpecController`<br>(`product_spec_controller.h` / `.cpp`) |
| 시료 주문 / 주문 승인·거절 (5.3, 5.4) | `Order`, `OrderStatus`, `WaitingApprovalQueue`<br>(`order.h` / `.cpp`, `order_status.h`, `waiting_approval_queue.h` / `.cpp`) | `OrderRepository`<br>(`order_repository.h` / `.cpp`) | `OrderView`<br>(`order_view.h` / `.cpp`) | `OrderController`<br>(`order_controller.h` / `.cpp`) |
| 모니터링 (5.5) | - (엔티티 없음, 조회 전용) | `ProductSpecRepository`, `OrderRepository`<br>(`product_spec_repository.h` / `.cpp`, `order_repository.h` / `.cpp`) | `MonitoringView`<br>(`monitoring_view.h` / `.cpp`) | `MonitoringController`<br>(`monitoring_controller.h` / `.cpp`) |
| 생산 라인 (5.6) | `ProductionLine`<br>(`production_line.h` / `.cpp`) | - (Model이 메모리에서 직접 관리, `ProductSpecRepository`로 재고만 반영) | `ProductionLineView`<br>(`production_line_view.h` / `.cpp`) | `ProductionLineController`<br>(`production_line_controller.h` / `.cpp`) |
| 출고 처리 (5.7) | - (엔티티 없음, `Order` 상태만 변경) | `OrderRepository`<br>(`order_repository.h` / `.cpp`) | `ReleaseView`<br>(`release_view.h` / `.cpp`) | `ReleaseController`<br>(`release_controller.h` / `.cpp`) |

- "메인 메뉴"와 "출고 처리"/"모니터링"처럼 Model 칸이 비어 있는 행은 정상이다. 해당 기능이
  하나의 엔티티를 소유하지 않고, 기존 엔티티(Order 등)를 조회·조합하기만 하기 때문이다.
  이런 경우까지 억지로 빈 Model 클래스를 만들지 않는다.

## 3. 데이터 영속성 처리 (파일/JSON/DB 중 선택 + CRUD)

- **선택 방식**: JSON 파일 기반 영속성. [DataPersistence-HSJ-0007](https://github.com/sjcome-arch/DataPersistence-HSJ-0007)
  PoC에서 검증한 자체 구현 JSON 라이브러리(외부 의존성 없음, `Json::Value` parse/dump)를 그대로 이식한다.
  - 이유: DB 서버 설치나 vcpkg 등 외부 패키지 매니저 설정 없이도, 사람이 읽을 수 있는 포맷으로
    CRUD/모니터링/Dummy 데이터 생성 도구가 동일한 파일을 공유하기 쉽다.
- **저장 파일**: `data/product_specs.json`, `data/orders.json` (실행 파일 기준 상대 경로).
- **Repository 인터페이스** (예시):
  ```cpp
  class ProductSpecRepository {
  public:
      std::vector<ProductSpec> findAll() const;
      std::optional<ProductSpec> findById(const std::string& productSpecId) const;
      std::vector<ProductSpec> findByName(const std::string& keyword) const; // 시료 검색
      void add(const ProductSpec& spec);                 // Create
      void update(const ProductSpec& spec);               // Update (재고 변경 등)
      void remove(const std::string& productSpecId);         // Delete
      void reload();                                    // 파일을 다시 읽음 (모니터링 도구용)
  private:
      void load();  // data/product_specs.json → 메모리
      void save();  // 메모리 → data/product_specs.json
  };
  ```
  `OrderRepository`도 동일한 패턴(findAll/findById/findByStatus/add/update/reload)으로 구성한다.
- Model 계층(`WaitingApprovalQueue`, `ProductionLine`)은 Repository를 통해서만 데이터를 읽고 쓰며,
  파일 포맷을 직접 알지 못하도록 한다(Repository가 직렬화/역직렬화를 캡슐화).

### 3.1 메모리 큐와 파일 동기화 원칙

`WaitingApprovalQueue`(Phase 3)와 `ProductionLine`(Phase 5)은 내부에 `std::queue<Order>`를 들고
빠르게 front/pop 처리를 하면서도, 파일(Repository)과 항상 재구성 가능한 관계를 유지해야 한다.
이를 위해 아래 두 원칙을 따른다.

- **저장 순서(쓰기 시 파일 우선)**: 큐 상태를 바꾸는 모든 연산(주문 접수, 승인 시 생산 큐 등록,
  생산 완료 등)은 **`OrderRepository`(및 필요 시 `ProductSpecRepository`)에 먼저 저장해 성공을
  확인한 뒤에만** 메모리 큐(`enqueue`/`dequeue` 등)를 갱신한다. 저장 도중 프로그램이 종료되어도
  메모리는 휘발되므로, "파일에 반영된 것 = 실제로 일어난 일"이라는 관계가 항상 유지된다.
- **시작 시 재구성(rebuild on startup)**: `main.cpp`에서 `WaitingApprovalQueue`/`ProductionLine`을
  생성할 때는 빈 큐로 시작하지 않고, `OrderRepository::findByStatus(RESERVED)` /
  `findByStatus(PRODUCING)` 결과를 **주문번호(`ORD-NNNNNN`) 오름차순**으로 정렬해 그 순서대로
  `enqueue`하여 채운다. 주문번호가 생성 순서대로 순차 채번되고([design.md - 6.1 주문번호
  채번 규칙](./design.md#61-주문번호order-number-채번-규칙)), 큐가 "뒤에서만 push, 앞에서만 pop"하는
  불변식을 지키기 때문에, 이 재구성 결과는 항상 실제 큐 상태와 동일하다.

이 두 원칙 덕분에 런타임 중에는 메모리 큐로 빠르게 처리하면서도, 파일이 항상 유일한 진실
소스(single source of truth) 역할을 하여 둘 사이의 싱크가 어긋나지 않는다.

## 4. 검증 방법 (Verify)

- 빌드: `msbuild SampleOrderSystem-HSJ-0007.slnx /p:Configuration=Debug /p:Platform=x64`가 경고 없이 성공한다.
- Repository 단위 테스트: add → findAll → update → remove 시나리오가 파일에 정확히 반영되는지 확인한다
  (자세한 절차는 [test.md](../TEST/test.md) 참조).
- 콘솔 한글 출력이 깨지지 않는지 확인한다(`/utf-8` 컴파일 옵션 + `SetConsoleOutputCP(CP_UTF8)` 적용 여부).
- 접수/생산 큐에 항목이 있는 상태에서 프로그램을 종료 후 재실행했을 때, 재구성된 큐의 순서가
  종료 전과 동일한지 확인한다(3.1 시작 시 재구성 원칙 검증).

## 5. 리뷰 포인트 (Review)

- Model/View/Controller/Repository 4계층의 의존 방향이 설계대로 지켜졌는지.
- 큐 상태를 바꾸는 모든 지점에서 Repository 저장이 메모리 큐 갱신보다 먼저 일어나는지(3.1 저장
  순서 원칙).
- ProductSpec/Order 엔티티 필드가 REQUIREMENT.md 5.2/5.3과 일치하는지.
