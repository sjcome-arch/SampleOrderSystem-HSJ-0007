# CLAUDE.md

이 파일은 이 저장소에서 작업하는 Claude Code에게 제공되는 가이드입니다.

## 프로젝트 개요

"반도체 시료 생산주문관리 시스템" (SampleOrderSystem) — 가상의 반도체 회사 S-Semi의
시료(Sample) 생산 및 주문 관리를 위한 **콘솔 기반** 애플리케이션.

전체 요구사항은 [REQUIREMENT.md](./REQUIREMENT.md)를, 구현 방식(자료구조/알고리즘 등 상세 설계)은
[design.md](./docs/DESIGN/design.md)를 참조한다. 원본 과제 PDF: `[CRA_AI] Day3_개인과제_반도체시료관리.pdf`.

- 요구사항(무엇을 만들 것인가)이 바뀌면 `REQUIREMENT.md`를, 구현 결정(어떻게 만들 것인가)이 바뀌면
  `docs/DESIGN/design.md`를 업데이트한다.

## 기술 스택 및 빌드

- 언어: C++20
- 빌드 시스템: Visual Studio (`SampleOrderSystem-HSJ-0007.slnx`, MSBuild `.vcxproj`)
- 플랫폼: Win32 / x64, Debug / Release 구성
- 애플리케이션 유형: 콘솔 애플리케이션 (Console Subsystem)

빌드 명령 (MSBuild, Developer PowerShell 기준):
```
msbuild SampleOrderSystem-HSJ-0007.slnx /p:Configuration=Debug /p:Platform=x64
```

## 도메인 핵심 규칙

- **시료(Sample)**: 시스템의 기본 단위. 속성 = 시료 ID, 시료명, 평균 생산시간, 수율.
  등록된 시료만 주문 가능.
- **주문(Order) 상태 머신**:
  `RESERVED → (승인/거절)`
  - 승인 + 재고 충분 → `CONFIRMED`
  - 승인 + 재고 부족 → `PRODUCING` (생산 큐에 자동 등록하고 FIFO로 처리한다)
  - 거절 → `REJECTED` (정상 흐름 제외, 모니터링에서도 제외)
  - `PRODUCING` → 생산 완료 시 → `CONFIRMED`
  - `CONFIRMED` → 출고 처리 시 → `RELEASED`
- **주문 접수 큐**: `RESERVED` 주문은 별도 큐에 적재되며, 승인/거절 처리는 **FIFO** 순서로 진행한다.
  생산 큐와는 별개의 큐이다 (자세한 내용은 [design.md](./docs/DESIGN/design.md) 참조).
- **생산 라인**: 단일 라인, 시료 하나씩 생산. 생산 큐는 **FIFO**.
  - 실 생산량 = `ceil(부족분 / 수율)`
  - 총 생산 시간 = `평균 생산시간 * 실 생산량`
- **재고 상태 분류** (모니터링용): 여유 / 부족(주문 대비 부족) / 고갈(재고 0)
- 주문 상태명 표기 불일치(`RELEASE` vs `RELEASED`)는 `RELEASED`로 통일해서 구현한다.

## 아키텍처 지침

- **MVC 구조**를 따른다 (Model / View / Controller 역할 분리). 각 레이어의 책임을 섞지 않는다.
  - Model: 시료, 주문, 생산 큐 등 도메인 엔티티 및 상태 전이 로직
  - View: 콘솔 입출력, 메뉴 렌더링 (예시 UI 화면은 참고용이며 자유롭게 구성 가능)
  - Controller: 사용자 입력을 받아 Model을 조작하고 View를 갱신
- 데이터 영속성(파일/JSON/DB 등)이 필요하면 Model과 분리된 별도 저장소(Repository) 계층으로 둔다.

## PoC 필수 구성 요소

본 프로젝트는 아래 4가지 PoC 항목을 충족해야 한다 (원본 과제의 [미션1] PoC 개발 항목 기준).

| 항목 | 요구 내용 |
|---|---|
| MVC 스켈레톤 코드 | Model / Controller / View 패키지(디렉터리) 구조와 역할 분리를 완성한다. 각 계층은 서로의 책임을 침범하지 않는다. |
| 데이터 영속성 처리 | 선택한 방식(파일, JSON, DB 등)으로 데이터를 저장·불러오는 구조를 구현한다. CRUD(생성/조회/수정/삭제)를 모두 포함한다. |
| 데이터 모니터링 Tool | 현재 저장된 데이터 상태를 콘솔에서 실시간으로 조회할 수 있는 관리자 도구를 제공한다. |
| Dummy 데이터 생성 Tool | 테스트를 위한 Dummy Data를 생성하는 도구를 제공한다. 생성된 Dummy Data는 연결된 DB(영속성 저장소)에 실제로 추가되어야 한다. |

- 이 항목들은 코드 리뷰/평가 기준이므로, 구현 시 위 4가지가 모두 실제로 동작하는지 확인한다.
- MVC 구조 세부 지침은 위 "아키텍처 지침" 절을 따른다.
- "데이터 영속성 처리"와 "Dummy 데이터 생성 Tool"은 동일한 저장소(Repository) 계층을 공유해야 하며,
  Dummy 데이터 생성 도구가 별도의 임시 저장소를 사용하지 않도록 주의한다.

## 작업 방식 원칙

1. **문서 관리**: 요구사항 변경/설계 결정이 생기면 `REQUIREMENT.md` 또는 이 파일을 최신 상태로 유지한다.
2. **Test 우선**: 상태 전이(승인/거절/생산/출고), 수율 기반 생산량 계산, FIFO 큐 동작에는
   반드시 단위 테스트를 작성한다.
3. **Clean Code**: 짧고 명확한 함수, 불필요한 주석 지양, 도메인 용어(RESERVED, CONFIRMED 등)를
   코드 전반에서 일관되게 사용한다.
4. **Commit 이력**: 기능 단위로 의미 있는 커밋을 남긴다 (요구사항에서 평가 항목으로 명시됨).
5. 예시 UI 화면(PDF)은 이해를 돕기 위한 참고 자료일 뿐이며, 실제 화면 레이아웃/문구는 자유롭게 결정한다.

## 코딩 컨벤션 및 구현 규칙

- **명명 규칙**: 함수와 변수명은 `camelCase`로, 파일명은 `snake_case`로 작성한다.

## 커밋 메시지 작성 규칙

- **포맷**: `[Prefix] 구현 내용 요약 및 상세 명시`
- **Prefix 종류**:
  - `[feature]`: 새로운 기능 추가 및 해당 기능의 unit test 포함 시
  - `[fix]`: 버그 수정
  - `[refactor]`: 코드 리팩토링 (기능적 변화 없음)
  - `[test]`: 테스트 코드만 추가/수정
  - `[docs]`: 문서 수정
  - `[chore]`: 빌드 시스템 등 코드 외적인 수정
- **예시**: `[feature] 사용자 인증 함수 추가 및 유효성 검사 unit test 구현`

## 참고

- 과제 진행 시 사용 모델은 Sonnet (Effort: Medium)으로 제한됨 (평가 조건).
