# design.md - 구현 설계 문서 (Phase 인덱스)

> 이 문서는 [REQUIREMENT.md](../../REQUIREMENT.md)의 기능 명세를 실제 코드로 구현하기 위한
> **구현 방식/자료구조/알고리즘 결정 사항**을 기록한다. "무엇을 만들어야 하는가"는
> REQUIREMENT.md를, "어떻게 만들 것인가"는 이 문서 체계(design.md + Phase별 설계 문서)를 따른다.
> AI 작업물 검증 절차는 [test.md](../TEST/test.md) 참조.

## 1. PoC 참조 저장소

각 PoC 항목은 이미 별도 Repository에서 구현·검증되었으며, 아래 Phase 문서들은 해당 Repository를
참조하여 본 프로젝트에 이식·확장한 것이다 (자세한 대응은 [CLAUDE.md - PoC 필수 구성 요소](../../CLAUDE.md#poc-필수-구성-요소) 참조).

- MVC 스켈레톤 코드: [ConsoleMVC-HSJ-0007](https://github.com/sjcome-arch/ConsoleMVC-HSJ-0007)
- 데이터 영속성 처리: [DataPersistence-HSJ-0007](https://github.com/sjcome-arch/DataPersistence-HSJ-0007)
- 데이터 모니터링 Tool: [DataMonitor-HSJ-0007](https://github.com/sjcome-arch/DataMonitor-HSJ-0007)
- Dummy 데이터 생성 Tool: [DummyDataGenerator-HSJ-0007](https://github.com/sjcome-arch/DummyDataGenerator-HSJ-0007)

## 2. Phase 목록

구현은 아래 순서의 Phase로 진행한다. 각 Phase는 별도의 `design_phase_N.md` 문서를 가지며,
목적(Why)/구현 범위(What)/검증 방법(Verify)/리뷰 포인트(Review)를 담는다.

| Phase | 문서 | 대상 요구사항 | 요약 |
|---|---|---|---|
| 1 | [design_phase_1.md](./design_phase_1.md) | 미션1 PoC (MVC/영속성) | 디렉터리 구조, Repository/JSON 영속성 계층 |
| 2 | [design_phase_2.md](./design_phase_2.md) | REQUIREMENT.md 5.2 | 시료 관리 (등록/조회/검색) |
| 3 | [design_phase_3.md](./design_phase_3.md) | REQUIREMENT.md 5.3 | 시료 주문 + 주문 접수 큐(FIFO) |
| 4 | [design_phase_4.md](./design_phase_4.md) | REQUIREMENT.md 5.4 | 주문 승인/거절 (재고 분기) |
| 5 | [design_phase_5.md](./design_phase_5.md) | REQUIREMENT.md 5.6 | 생산 라인, 실 생산량/수율 처리 |
| 6 | [design_phase_6.md](./design_phase_6.md) | REQUIREMENT.md 5.7 | 출고 처리 |
| 7 | [design_phase_7.md](./design_phase_7.md) | REQUIREMENT.md 5.5 | 모니터링 |
| 8 | [design_phase_8.md](./design_phase_8.md) | 미션1 PoC (모니터링/Dummy 생성) | 데이터 모니터링·Dummy 데이터 생성 Tool 이식 |

## 3. Phase별 개발 문서 체계

- 기능을 한 번에 크게 구현하면 AI 결과물의 품질이 떨어지고 사람이 리뷰하기도 어려워진다.
  따라서 기능을 **Phase 단위**로 쪼개어 진행한다 (`[CRA_AI] Day1_6_Agentic Engineering.pdf`
  "Phase 설계"/"PLAN.md" 내용 참고).
- Phase별로 아래 2개 문서를 사용한다.
  - `docs/PLAN/phaseN.md`: 해당 Phase의 목적(Why), 수행 작업(What), 검증 방법(Verify),
    리뷰 포인트(Review)를 기록한다. **Phase 완료 후에는 삭제해도 되는 임시 문서**이다
    (구현이 끝나면 코드 자체가 진실의 원천이 되므로 문서의 역할은 끝난다).
  - `docs/DESIGN/design_phase_N.md`: 해당 Phase의 구체적 설계(클래스/함수 시그니처, 변경될 파일
    목록 등)를 기록한다. 완료 후에도 참고용으로 남겨도 무방하다.
- Phase 진행 순서:
  1. PLAN 작성 (AI) → 사람 검토
  2. 설계 문서(`design_phase_N.md`) 작성 (AI) → 사람 검토
  3. 구현 지시
  4. **Verify** ([test.md](../TEST/test.md) 참조)
  5. 사람 리뷰 → 커밋 (커밋 전 리뷰 절차는 [CLAUDE.md](../../CLAUDE.md#커밋-전-리뷰-절차) 참조)
- 위 표의 Phase 순서는 권장 순서이며, 선행 Phase가 끝나지 않아도 설계 문서 자체는 미리 작성해
  둘 수 있다. 단, 구현 순서는 선행 관계(예: Phase 1 없이 Phase 2 구현 불가)를 지킨다.

## 4. 미션 2 항목별 대응

| 주안점 | 대응 방안 |
|---|---|
| CLAUDE.md, PRD.md 등 문서 관리 | `REQUIREMENT.md`(PRD 역할) / `CLAUDE.md`(에이전트 가이드) / `docs/DESIGN/design.md` + `design_phase_N.md`(구현 설계) / `docs/TEST/test.md`(검증 절차)로 문서 체계를 구성하고, 요구사항·설계 변경 시마다 즉시 갱신한다. |
| Harness 도입 | `GoogleTest`/`gMock`(NuGet 패키지)로 별도 `Tests` vcxproj를 솔루션에 추가해 `msbuild` 한 번으로 전체 테스트가 실행되도록 한다(확정 근거는 [6.4](#64-테스트-하니스-googletestgmock) 참조). AI 작업 지시 시 "개발 → Verify → Human Review" 순서를 지키는 구체적 절차는 [test.md](../TEST/test.md) 참조. |
| Test | Model 계층(상태 전이, 실 생산량/수율 계산, FIFO 큐 동작)부터 우선 테스트하고, Repository CRUD, Controller 분기(재고 충분/부족)까지 확장한다. 테스트 파일은 `Tests/Model/*.cpp`, `Tests/Repository/*.cpp`로 대상 계층별로 분리한다. TDD로 진행할 경우 RED(테스트 작성)–GREEN(최소 구현)–REVIEW(사람 검토) 사이클을 따른다. |
| Clean Code | 코딩 컨벤션([CLAUDE.md - 코딩 컨벤션 및 구현 규칙](../../CLAUDE.md#코딩-컨벤션-및-구현-규칙))을 지키고, 함수/클래스 단위를 작게 유지한다(PR 100라인 이내). |
| Commit 이력 | 기능 단위로 `[feature]/[fix]/[refactor]/[test]/[docs]/[chore]` 접두사를 사용해 커밋하며, 커밋 전 항상 사용자 리뷰를 받는다([CLAUDE.md - 커밋 전 리뷰 절차](../../CLAUDE.md#커밋-전-리뷰-절차)). |

## 5. 향후 구현 결정 사항 (TBD)

아래 항목은 구현을 진행하며 결정되는 대로 이 문서에 추가한다.

- 시료 ID 채번 규칙 (예: `S-NNN`)

## 6. 결정된 사항

### 6.1 주문번호(Order Number) 채번 규칙

- **형식**: `ORD-NNNNNN` (6자리, 0 패딩 전역 순차 증가, 예: `ORD-000001`, `ORD-000002`, ...).
  시료 ID(`productSpecId`)와는 무관한, `Order`만의 고유 식별자다.
- **채번 주체**: `OrderRepository`가 새 주문 생성 시 발급한다. 지금까지 발급된 최대 일련번호를
  별도로 추적하여 `+1`한 값을 새 주문번호로 부여하고, 이미 삭제된 주문의 번호는 재사용하지 않는다
  (단순히 `findAll().size() + 1`로 계산하면 삭제 후 번호가 겹칠 수 있으므로 피한다).
- **정렬 특성**: 고정 자릿수(6자리) 덕분에 문자열 오름차순 정렬이 곧 발급 순서(=생성 순서)와
  일치한다. 이 특성은 프로그램 시작 시 `WaitingApprovalQueue`/`ProductionLine`을 파일 데이터만으로
  재구성(rebuild)할 때 사용된다 ([design_phase_1.md - 3.1](./design_phase_1.md#31-메모리-큐와-파일-동기화-원칙) 참조).

### 6.2 Dummy 데이터 생성 Tool 노출 방식

- 숨김 메뉴나 별도 실행 파일이 아니라, **메인 메뉴에 일반 항목으로 노출**한다. 참조 PoC
  저장소는 독립 실행 파일 구조였지만, 이 프로젝트에서는 메인 메뉴에서 바로 접근할 수 있게
  구성한다.
- 메뉴 예시(정확한 번호/문구는 구현 시 자유롭게 조정 가능, [design_phase_8.md - 2](./design_phase_8.md#2-dummy-데이터-생성-tool) 참조):
  ```
  [1] 시료 관리
  [2] 시료 주문
  [3] 주문 승인/거절
  [4] 모니터링
  [5] 출고 처리
  [6] 생산 라인
  [7] Dummy 데이터 생성
  [0] 종료
  ```
- 데이터 모니터링 Tool은 노출 방식 자체가 해당 없다 — 6.3 참조(별도 메뉴를 만들지 않는다).

### 6.3 데이터 모니터링 Tool 구현 방식

- **별도 실행 파일도, 숨김 메뉴도 만들지 않는다.** "저장된 데이터 상태를 콘솔에서 실시간 조회"
  라는 PoC 요구사항은 메인 애플리케이션에 이미 있는 조회 메뉴들을 재사용해서 충족한다:
  - 접수 큐 스냅샷 → 주문 승인/거절의 "접수된 주문 목록" (Phase 4)
  - 생산 큐 스냅샷 → 생산 라인의 "대기 주문 확인" (Phase 5)
  - 시료별 재고/상태별 주문 건수 → 모니터링 (Phase 7)
  - 시료 전체 목록 → 시료 관리의 "시료 조회" (Phase 2)
- 별도 프로세스가 아니므로 `reload()` 같은 재조회 장치도 필요 없다 — 메인 애플리케이션이 Repository
  인스턴스를 그대로 공유하기 때문이다(자세한 근거는 [design_phase_8.md - 1](./design_phase_8.md#1-데이터-모니터링-tool) 참조).
- Dummy 데이터 생성 Tool(6.2)과 달리 새 메뉴 항목이 추가되지 않는다는 점에 유의한다.

### 6.4 테스트 하니스 (GoogleTest/gMock)

- **프레임워크**: `GoogleTest` + `gMock`. `Catch2`는 사용하지 않는다.
- **의존성 확보 방식**: **NuGet 패키지**(예: `Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn`)로
  설치한다. vcpkg는 도입하지 않는다(`vcpkg.json` 매니페스트 불필요).
- **빌드 통합**: `SampleOrderSystem-HSJ-0007.slnx`에 **별도 `Tests` vcxproj**를 추가한다. CTest는
  사용하지 않는다. `msbuild SampleOrderSystem-HSJ-0007.slnx /p:Configuration=Debug /p:Platform=x64`
  한 번으로 본 프로젝트와 `Tests` 프로젝트가 함께 빌드되고, 테스트 실행 파일이 생성된다.
- **gMock 활용**: `test_phase_3.md`/`test_phase_4.md`/`test_phase_6.md`에 있는 "저장이 먼저
  일어나는지(순서 검증)" 류의 테스트 케이스는 `ProductSpecRepository`/`OrderRepository`를 gMock
  `MOCK_METHOD`로 목(mock) 객체화하고, `testing::InSequence` 또는 `EXPECT_CALL(...).Times(1)`의
  호출 순서 지정으로 검증한다(예: `orderRepository.update()`가 `productionLine.enqueue()`보다
  먼저 호출되는지).
  - 이를 위해 `ProductSpecRepository`/`OrderRepository`는 순수 클래스가 아니라 **인터페이스
    (추상 기반 클래스)를 두고 실제 구현을 상속**하는 구조로 설계한다(테스트에서는 gMock 목
    클래스로 대체). 인터페이스 분리 세부사항은 `design_phase_1.md`에 반영이 필요하면 별도로
    다룬다.
- 테스트 파일 네이밍은 GoogleTest 관례에 따라 `TEST(TestSuiteName, TestName)` 또는
  `TEST_F(Fixture, TestName)`을 사용하고, `test_phase_N.md`에 정리된 테스트 이름(PascalCase)을
  `TestName`으로 그대로 사용한다.
