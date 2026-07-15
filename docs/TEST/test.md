# test.md - Test 및 Verify 프로세스

> AI(Coding Agent)가 작성한 코드를 사람이 리뷰하기 전에 거쳐야 하는 자동/반자동 검증 단계를 정의한다.
> 참고: `[CRA_AI] Day2_2_Test, Verify, Report, PR.pdf`의 Verify Harness 개념을 이 프로젝트에 맞게 구체화했다.
> [CLAUDE.md](../../CLAUDE.md), [design.md](../DESIGN/design.md)에서 참조된다.

## 1. 왜 Verify가 필요한가

- Harness란 Agent가 장시간 실행되어도 올바른 임무를 수행하도록 만드는, Agent와 직접적으로
  연관된 안전장치다.
- 제약 없이 맡기면 AI는 테스트를 돌리지 않고도 "완료했다"고 보고하거나, 심지어 실패하는
  테스트를 자신에게 유리하게 고쳐서 통과시키는 경우가 있다.
- 따라서 **"개발 → Verify → Human Review"** 순서를 강제한다. Verify는 사람이 리뷰하기 전에
  AI 결과물이 최소한의 기준을 통과했는지 AI 스스로 검증하는 단계다.

## 2. Verify 3단계

### 2.1 Test Verify (동작 검증)

- 대상: 새로 작성/수정된 Unit Test 전체
- 방법: `msbuild`로 Tests 프로젝트를 빌드한 뒤 테스트를 실행하고, 전부 PASS하는지 확인한다.
- Phase 개발을 지시할 때는 "Phase1 개발해"가 아니라 **"Phase1 개발하고 Verify까지 수행해"**
  라고 지시한다.
- 구체적인 테스트 케이스 목록(테스트 이름, Given/When/Then, 대상 파일)은 Phase별
  `test_phase_N.md` 문서에 정리되어 있다. 구현 전에 해당 문서를 먼저 참조해 테스트를 작성한다.

  | Phase | 문서 | 대상 |
  |---|---|---|
  | 1 | [test_phase_1.md](./test_phase_1.md) | Repository CRUD, 큐 재구성 |
  | 2 | [test_phase_2.md](./test_phase_2.md) | 시료 관리 (등록/조회/검색) |
  | 3 | [test_phase_3.md](./test_phase_3.md) | 시료 주문 + 주문 접수 큐 |
  | 4 | [test_phase_4.md](./test_phase_4.md) | 주문 승인/거절 (stock/availableStock 분기) |
  | 5 | [test_phase_5.md](./test_phase_5.md) | 생산 라인, 실 생산량/수율, tick 완료 판정 |
  | 6 | [test_phase_6.md](./test_phase_6.md) | 출고 처리 (stock 차감) |
  | 7 | [test_phase_7.md](./test_phase_7.md) | 모니터링 (여유/부족/고갈 판정) |
  | 8 | [test_phase_8.md](./test_phase_8.md) | Dummy 데이터 생성 Tool, 데이터 모니터링 재사용 |

### 2.2 Compliance Verify (요구사항 충족 검증)

- 대상: 해당 Phase의 `docs/PLAN/phaseN.md`(또는 5.3에서 참조한 phase 설계 문서)에 명시된
  지시사항
- 방법: AI가 PLAN 문서의 각 항목과 실제 구현/테스트 결과를 하나씩 대조하여 체크리스트로
  보고하게 한다.
- 목적: "지시한 것보다 AI가 더 많이/다르게 구현하는 문제"를 사전에 차단한다.

### 2.3 Document Consistency Verify (문서 정합성 검증)

- 대상: `REQUIREMENT.md` / `docs/DESIGN/design.md` / `CLAUDE.md` / Phase별 PLAN·design 문서
- 방법: 코드를 생성하기 전에, 위 문서들 사이에 상충되는 지시나 모호한 부분이 없는지 AI가
  먼저 점검하고 보고하게 한다.
- 이 프로젝트는 요구사항 문서(REQUIREMENT.md)가 자주 갱신되어 왔으므로 특히 중요하다.

## 3. Safety Test (공격적 테스트) — 선택적으로 적용

- 목적: 개발자 본인이 만든 "정상 케이스" 테스트만으로는 놓치는 결함을 찾기 위함이다.
- 예시:
  - 존재하지 않는 시료 ID로 시료 주문
  - 주문 수량 0 또는 음수 입력
  - 승인 대기 주문이 없는 상태에서 승인/거절 시도
  - 재고가 정확히 0인 시료에 대한 출고 시도
  - 생산 큐가 비어 있는(`WAITING`) 상태에서 대기 주문 조회
- **주의**: 이런 공격적 테스트는 특정 구현 방식에 강하게 의존해 리팩토링 내성이 낮을 수 있다.
  정식 회귀 테스트(`Tests/`, git 포함)와 분리하여 스크래치 경로(예: `/tmp` 또는 임시 브랜치)
  에서만 실행하고, 실제로 발견된 결함에 대해서만 정식 테스트로 옮겨 담는다.

## 4. 시간 의존 로직 테스트 원칙

- 생산 라인의 완료 판정(`ProductionLine::tick`, `design_phase_5.md` 4.3 참조)처럼 "현재 시각"에
  의존하는 로직은, 실제 시계(`std::chrono::system_clock::now()`)를 테스트 코드에서 직접 호출하지
  않는다. 대신 해당 로직이 시각을 항상 **파라미터(`Time now`)로 전달받도록** 설계하고, 테스트는
  임의로 만든 고정 `Time` 값을 그 파라미터에 넣어 검증한다.
- 이렇게 하면 별도의 Mock/Fake Clock 클래스나 인터페이스 없이도, "30분 뒤" 같은 시나리오를
  실제로 기다리지 않고 즉시 재현·검증할 수 있다.
- 실제 앱(Debug/Release 공통)에서는 Controller가 `system_clock::now()`를 호출해 그 값을
  넘겨주고, 테스트 코드만 고정 값을 넘긴다는 점에서 프로덕션 코드 경로는 완전히 동일하게
  유지된다(별도 시간 주입 기능은 Release 앱에 두지 않는다).
- 구체적인 시그니처와 테스트 예시는 [design_phase_5.md - 4.3 완료 판정 시점](../DESIGN/design_phase_5.md#43-완료-판정-시점-tick) 참조.

## 5. 개발 지시 → Verify → Review 플로우

```
PLAN (AI와 협업, docs/PLAN/phaseN.md)
   │
   ▼
AI Action (구현)
   │
   ▼
Test Verify ──▶ Compliance Verify ──▶ Document Consistency Verify
   │
   ▼
Human Review (사람 최종 검토 → 커밋 전 리뷰 절차 적용)
```

## 6. Fail 발생 시 대응

Verify 단계에서 실패가 발견되면, 사람이 바로 "고쳐줘"라고 지시하지 않는다. 아래 순서를 따른다.

1. 재현 가능한 최소 테스트를 확보한다.
2. AI와 대화하며 실패 원인을 정확히 분석한다.
3. 수정 방향을 사람이 검토하고 합의한다 (필요 시 간단히 문서화).
4. 합의된 방향으로 수정 후, Test Verify부터 다시 수행한다.
