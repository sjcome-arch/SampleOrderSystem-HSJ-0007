# test_phase_8.md - Phase 8: 데이터 모니터링 재사용 / Dummy 데이터 생성 Tool 테스트 계획

> 대상 설계: [design_phase_8.md](../DESIGN/design_phase_8.md)
> 테스트 파일: `Tests/Controller/dummy_data_generator_controller_test.cpp`

## 1. 테스트 대상

- Dummy 데이터 생성 Tool의 Repository 재사용 및 고정 시드 재현성
- 데이터 모니터링 요구사항이 기존 메뉴(시료 조회/모니터링/접수된 주문 목록/대기 주문 확인) 재사용만으로 충족되는지

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `DummyGeneratorCreatesSpecsViaRepositoryAdd` | 빈 저장소 | 시료 N개 생성 실행 | `ProductSpecRepository::add`를 통해 N건이 `data/product_specs.json`에 반영됨 |
| 2 | `DummyGeneratorCreatesOrdersViaRepositoryAdd` | 등록된 시료 존재 | 주문 M개 생성 실행 | `OrderRepository::add`를 통해 M건이 `data/orders.json`에 반영됨 |
| 3 | `DummyGeneratorFixedSeedProducesIdenticalResultsAcrossRuns` | 빈 저장소, 동일 N/M | 두 번 연속 생성(사이에 초기화) | 생성된 시료명/평균생산시간/수율/초기재고/고객명/수량이 완전히 동일 |
| 4 | `DummyGeneratorYieldWithinRequiredRange` | 시료 생성 | 결과 확인 | 모든 생성된 수율이 `0.7~0.99` 범위 내 |
| 5 | `DummyGeneratorProductionTimeWithinRequiredRange` | 시료 생성 | 결과 확인 | 모든 평균 생산시간이 `0.1~1.0` 범위 내 |
| 6 | `DummyGeneratorOrderIdsRemainSequentialAfterRun` | 기존 주문 K건 존재 | 주문 M개 추가 생성 | 새로 생성된 주문번호가 기존 최대값 이후로 순차 이어짐(design.md 6.1) |
| 7 | `MonitoringMenusShowDataImmediatelyAfterGenerationNoReloadNeeded` | Dummy 데이터 생성 직후 | 시료 조회/모니터링/접수된 주문 목록/대기 주문 확인 메뉴 진입 | 별도 새로고침 없이 방금 생성된 데이터가 바로 보임 |
| 8 | `NoSeparateDataMonitorControllerExists` | - | 코드 리뷰/정적 확인 | 별도의 `DataMonitorController`/전용 실행 파일이 존재하지 않음(기존 메뉴만 재사용) |

## 3. Safety Test (선택)

- 생성 개수(N/M)에 0을 입력했을 때 아무것도 생성되지 않고 정상 종료되는지.

## 4. 참고

- Dummy 데이터 생성 Tool의 메뉴 노출 방식은 [design.md - 6.2](../DESIGN/design.md#62-dummy-데이터-생성-tool-노출-방식), 데이터 모니터링 재사용 근거는 [design.md - 6.3](../DESIGN/design.md#63-데이터-모니터링-tool-구현-방식) 참조.
