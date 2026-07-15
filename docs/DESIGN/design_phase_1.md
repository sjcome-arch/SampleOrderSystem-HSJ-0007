# design_phase_1.md - Phase 1: 프로젝트 기반 구조 (MVC 스켈레톤 + 데이터 영속성)

> [design.md](./design.md)의 Phase 목록 중 1번째 문서. 이후 모든 Phase가 이 구조 위에서 진행된다.
> 참조 PoC: [ConsoleMVC-HSJ-0007](https://github.com/sjcome-arch/ConsoleMVC-HSJ-0007),
> [DataPersistence-HSJ-0007](https://github.com/sjcome-arch/DataPersistence-HSJ-0007)

- 대상 요구사항: [REQUIREMENT.md 7. 미션 및 제출 관련](../../REQUIREMENT.md#7-미션-및-제출-관련-참고)
  (미션1 PoC "MVC 스켈레톤 코드", "데이터 영속성 처리" 항목)

## 1. 디렉터리 구조 (MVC 스켈레톤 코드)

```
SampleOrderSystem-HSJ-0007/           # vcxproj 루트
├── Model/            # 도메인 엔티티 + 상태 전이 로직 (View/Controller 비의존)
│   ├── Sample.h / .cpp            # 시료 ID/시료명/평균생산시간/수율/재고
│   ├── Order.h / .cpp             # 주문번호/시료ID/고객명/수량/상태(RESERVED 등)
│   ├── OrderStatus.h              # enum class OrderStatus { RESERVED, REJECTED, ... }
│   ├── ReservationQueue.h / .cpp  # 주문 접수 큐 (design_phase_3.md 참조)
│   └── ProductionLine.h / .cpp    # 생산 큐 + 생산 라인 상태(RUNNING/WAITING, design_phase_5.md 참조)
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
- 각 메뉴(REQUIREMENT.md 5.1~5.7)는 하나의 Controller에 대응시켜 책임을 명확히 분리한다.
- [ConsoleMVC-HSJ-0007](https://github.com/sjcome-arch/ConsoleMVC-HSJ-0007) PoC의 Model/View/Controller
  분리 구조를 그대로 이식하고, 도메인 엔티티만 Item → Sample/Order로 교체한다.

## 2. 데이터 영속성 처리 (파일/JSON/DB 중 선택 + CRUD)

- **선택 방식**: JSON 파일 기반 영속성. [DataPersistence-HSJ-0007](https://github.com/sjcome-arch/DataPersistence-HSJ-0007)
  PoC에서 검증한 자체 구현 JSON 라이브러리(외부 의존성 없음, `Json::Value` parse/dump)를 그대로 이식한다.
  - 이유: DB 서버 설치나 vcpkg 등 외부 패키지 매니저 설정 없이도, 사람이 읽을 수 있는 포맷으로
    CRUD/모니터링/Dummy 데이터 생성 도구가 동일한 파일을 공유하기 쉽다.
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
      void reload();                                    // 파일을 다시 읽음 (모니터링 도구용)
  private:
      void load();  // data/samples.json → 메모리
      void save();  // 메모리 → data/samples.json
  };
  ```
  `OrderRepository`도 동일한 패턴(findAll/findById/findByStatus/add/update/reload)으로 구성한다.
- Model 계층(`ReservationQueue`, `ProductionLine`)은 Repository를 통해서만 데이터를 읽고 쓰며,
  파일 포맷을 직접 알지 못하도록 한다(Repository가 직렬화/역직렬화를 캡슐화).

## 3. 검증 방법 (Verify)

- 빌드: `msbuild SampleOrderSystem-HSJ-0007.slnx /p:Configuration=Debug /p:Platform=x64`가 경고 없이 성공한다.
- Repository 단위 테스트: add → findAll → update → remove 시나리오가 파일에 정확히 반영되는지 확인한다
  (자세한 절차는 [test.md](../TEST/test.md) 참조).
- 콘솔 한글 출력이 깨지지 않는지 확인한다(`/utf-8` 컴파일 옵션 + `SetConsoleOutputCP(CP_UTF8)` 적용 여부).

## 4. 리뷰 포인트 (Review)

- Model/View/Controller/Repository 4계층의 의존 방향이 설계대로 지켜졌는지.
- Sample/Order 엔티티 필드가 REQUIREMENT.md 5.2/5.3과 일치하는지.
