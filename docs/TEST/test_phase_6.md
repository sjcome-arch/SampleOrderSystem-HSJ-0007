# test_phase_6.md - Phase 6: 출고 처리 테스트 계획

> 대상 설계: [design_phase_6.md](../DESIGN/design_phase_6.md)
> 테스트 파일: `Tests/Controller/release_controller_test.cpp`

## 1. 테스트 대상

- 출고 처리에 따른 상태 전이(`CONFIRMED → RELEASED`)와 `stock` 차감
- `availableStock`이 출고 시점에 변하지 않음을 검증

## 2. 테스트 케이스

| # | 테스트 이름 | Given | When | Then |
|---|---|---|---|---|
| 1 | `ReleaseConfirmedOrderTransitionsToReleased` | `CONFIRMED` 주문 1건 | 출고 처리 | `status == RELEASED`, `releasedAt`이 채워짐 |
| 2 | `ReleaseDecreasesStockByOrderQuantity` | `stock=100`, 출고 대상 주문 수량 30 | 출고 처리 | `stock == 70` |
| 3 | `ReleaseDoesNotChangeAvailableStock` | `availableStock=40` | 출고 처리 | `availableStock == 40` (불변) |
| 4 | `ReleaseRemovesOrderFromConfirmedList` | `CONFIRMED` 주문 목록에 포함 | 출고 처리 완료 | 갱신된 목록에서 해당 주문 제외 |
| 5 | `ReleasePersistsStockBeforeOrderStatus` | 임의 주문 | 출고 처리 | `ProductSpecRepository::update`가 `OrderRepository::update`보다 먼저(또는 동일 트랜잭션 순서로) 호출됨 |
| 6 | `ReleaseListExcludesProducingOrders` | `PRODUCING` 주문 존재 | 출고 가능 목록 조회 | 해당 주문이 목록에 나타나지 않음 |
| 7 | `ReleaseListExcludesRejectedOrders` | `REJECTED` 주문 존재 | 출고 가능 목록 조회 | 해당 주문이 목록에 나타나지 않음 |
| 8 | `OrderStatusUsesReleasedNotRelease` | 출고 처리 완료 | 상태 문자열 확인 | `"RELEASED"`로 일관 표기 (`"RELEASE"` 아님) |

## 3. Safety Test (선택)

- 출고 가능 목록이 빈 상태에서 출고 메뉴 진입 시 안내 문구 출력 여부.
- 잘못된 번호(목록 범위 밖) 입력 시 재입력 요청 여부.

## 4. 참고

- 전체 시나리오(승인→생산→출고)를 잇는 통합 테스트는 [test_phase_4.md](./test_phase_4.md) #9 예시 시나리오의 연장선에서 작성해도 된다(주문 1/2/3을 모두 출고까지 진행시켜 최종 `stock == availableStock` 불변식 확인).
