# test_phase_2.md - Phase 2: 시료 관리 테스트 계획

> 대상 설계: [design_phase_2.md](../DESIGN/design_phase_2.md)
> 테스트 파일: `Tests/Model/product_spec_test.cpp`, `Tests/Controller/product_spec_controller_test.cpp`

## 1. 테스트 대상

- `ProductSpec` 등록 시 `availableStock` 초기화 규칙
- `ProductSpecController`의 등록/조회/검색 입력 검증
- `ProductSpecRepository::findByName`의 부분 일치/대소문자 무시 검색

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `ProductSpec_Register_InitializesAvailableStockEqualToStock` | 초기 재고 50으로 등록 | 등록 완료 | `stock == 50 && availableStock == 50` |
| 2 | `ProductSpecController_Register_RejectsYieldOutOfRange` | 수율 0 또는 1.5 입력 | 등록 시도 | 재입력 요청, 등록되지 않음 |
| 3 | `ProductSpecController_Register_RejectsNonPositiveProductionTime` | 평균 생산시간 0 또는 음수 입력 | 등록 시도 | 재입력 요청 |
| 4 | `ProductSpecController_Register_RejectsDuplicateId` | 이미 등록된 시료 ID | 같은 ID로 재등록 시도 | 등록 거부, 재입력 요청 |
| 5 | `ProductSpecController_Register_CancelOnNoConfirm` | 등록 입력 완료 후 확인 단계에서 N 선택 | - | 등록되지 않고 메뉴로 복귀 |
| 6 | `ProductSpecRepository_FindByName_PartialMatchCaseInsensitive` | 시료명 "SampleA" 등록 | `findByName("samplea")` 또는 `findByName("ample")` 호출 | 해당 시료가 검색 결과에 포함 |
| 7 | `ProductSpecController_Search_ShowsEmptyResultMessage` | 등록된 시료 없음 | 임의 키워드로 검색 | "검색 결과 없음" 안내 출력 |
| 8 | `ProductSpecRepository_Persistence_SurvivesRestart` | 시료 등록 후 프로세스 재시작 시뮬레이션 | `findAll()` 재조회 | 등록했던 시료가 동일하게 조회됨 |

## 3. Safety Test (선택)

- 등록되지 않은 시료 ID로 조회/검색 시 예외 없이 빈 결과 처리되는지.

## 4. 참고

- `stock`/`availableStock` 구분과 등록 시 초기화 규칙의 근거는 [design_phase_2.md - 2.1](../DESIGN/design_phase_2.md#21-productspec-모델-modelproduct_spech--cpp) 참조.
