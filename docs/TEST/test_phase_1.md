# test_phase_1.md - Phase 1: 프로젝트 기반 구조 테스트 계획

> 대상 설계: [design_phase_1.md](../DESIGN/design_phase_1.md)
> 테스트 파일: `Tests/Repository/product_spec_repository_test.cpp`, `Tests/Repository/order_repository_test.cpp`

## 1. 테스트 대상

- `ProductSpecRepository`, `OrderRepository`의 CRUD(`add`/`findAll`/`findById`/`update`/`remove`/`reload`)
- `WaitingApprovalQueue`/`ProductionLine`의 "저장 우선 + 시작 시 재구성" 원칙 (design_phase_1.md 3.1)

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `ProductSpecRepository_Add_PersistsToFile` | 빈 저장소 | `add(spec)` 호출 | `findAll()`에 1건 반영, `data/product_specs.json` 파일에도 반영 |
| 2 | `ProductSpecRepository_FindById_ReturnsNulloptWhenMissing` | 빈 저장소 | 존재하지 않는 ID로 `findById` | `std::nullopt` 반환 |
| 3 | `ProductSpecRepository_Update_OverwritesExistingRecord` | 등록된 시료 1건 | `stock`을 바꿔 `update` 호출 | `findById` 결과에 변경된 값 반영 |
| 4 | `ProductSpecRepository_Remove_DeletesRecord` | 등록된 시료 1건 | `remove(id)` 호출 | `findAll()`에서 사라짐 |
| 5 | `ProductSpecRepository_Reload_ReflectsExternalFileChange` | 저장소 A로 add 후, 별도 인스턴스 B가 같은 파일을 읽음 | B에서 `reload()` 호출 | B의 `findAll()`에도 A가 추가한 레코드가 보임 |
| 6 | `OrderRepository_FindByStatus_FiltersCorrectly` | `RESERVED`/`CONFIRMED` 주문 각 1건씩 등록 | `findByStatus(RESERVED)` 호출 | `RESERVED` 주문만 반환 |
| 7 | `Repository_RestartAfterProcessExit_KeepsData` | 시료/주문 등록 후 Repository 인스턴스를 새로 생성(재시작 시뮬레이션) | 새 인스턴스로 `findAll()` | 이전에 등록한 데이터가 그대로 남아있음 |
| 8 | `QueueRebuild_OnStartup_MatchesShutdownOrder` | `RESERVED` 주문 3건을 순서대로 생성 후 종료 시뮬레이션 | 새 `WaitingApprovalQueue`를 `findByStatus(RESERVED)` 오름차순 정렬로 재구성 | `snapshot()` 순서가 종료 전 큐 순서와 동일 |

## 3. Safety Test (선택)

- 존재하지 않는 파일 경로에서 Repository를 처음 생성했을 때 빈 목록으로 정상 시작하는지(파일이 없을 때 예외 없이 신규 생성).

## 4. 참고

- 콘솔 UTF-8 출력(`/utf-8`, `SetConsoleOutputCP`)은 자동화된 유닛 테스트보다 수동 확인(Compliance Verify) 대상이다.
