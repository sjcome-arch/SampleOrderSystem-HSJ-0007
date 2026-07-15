# design_phase_2.md - Phase 2: 시료 관리

> [design.md](./design.md)의 Phase 목록 중 2번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md) (Repository/MVC 기반 구조)

- 대상 요구사항: [REQUIREMENT.md 5.2 시료 관리](../../REQUIREMENT.md#52-시료-관리)

## 1. 구현 범위

- `Sample` 모델: 시료 ID, 시료명, 평균 생산시간, 수율, 재고(현재 수량)
- `SampleController` + `SampleRepository`를 이용한 등록/조회/검색/뒤로가기 메뉴 구현
  (Repository 인터페이스는 [design_phase_1.md](./design_phase_1.md) 참조)
- 시료 검색은 시료명 부분 일치(대소문자 무시) 기준으로 `findByName`을 구현한다.

## 2. 검증 방법 (Verify)

- 시료 등록 후 `data/samples.json`에 반영되는지 확인한다.
- 등록되지 않은 시료 ID로 검색 시 빈 목록/오류 안내가 출력되는지 확인한다(REQUIREMENT.md 5.0 공통 사항).

## 3. 리뷰 포인트 (Review)

- 시료 등록 시 입력 검증(수율 0~1 범위, 평균 생산시간 양수 등)이 있는지.
- View 계층이 Repository를 직접 호출하지 않고 Controller를 거치는지.
