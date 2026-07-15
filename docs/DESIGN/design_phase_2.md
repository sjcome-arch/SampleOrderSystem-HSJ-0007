# design_phase_2.md - Phase 2: 시료 관리

> [design.md](./design.md)의 Phase 목록 중 2번째 문서.
> 선행 Phase: [design_phase_1.md](./design_phase_1.md) (Repository/MVC 기반 구조)

- 대상 요구사항: [REQUIREMENT.md 5.2 시료 관리](../../REQUIREMENT.md#52-시료-관리)

## 1. 구현 목표 (Why)

- 시료(ProductSpec)는 이 시스템의 가장 기본 단위이며, 등록된 시료만 이후 주문(Phase 3)의 대상이
  될 수 있다. 따라서 Phase 2는 주문·생산·출고 등 뒤따르는 모든 Phase가 참조할 **시료 마스터
  데이터**를 등록·조회·검색할 수 있게 만드는 것을 목표로 한다.
- 또한 Phase 2는 [design_phase_1.md](./design_phase_1.md)에서 설계한 JSON 영속성 계층
  (`ProductSpecRepository`)이 실제 데이터로 동작하는 첫 Phase이다. 즉 "등록한 시료가 프로그램을
  껐다 켜도 그대로 남아있는지"를 이 단계에서 처음으로 검증하게 된다.

## 2. 구현 범위

### 2.1 `ProductSpec` 모델 (`Model/product_spec.h` / `.cpp`)

```cpp
class ProductSpec {
public:
    std::string productSpecId;  // 시료 ID (채번 규칙은 design.md 5. TBD 참조)
    std::string name;           // 시료명
    double avgProductionTime;   // 평균 생산시간(분), 양수
    double yield;                // 수율, 0 초과 ~ 1 이하
    int stock;                   // 실제 재고 수량(물리적 창고 수량). 생산 완료 시에만 증가하고,
                                  // 출고(RELEASED) 시에만 감소한다(design_phase_6.md 참조). 승인/확정
                                  // 시점에는 변하지 않는다.
    int availableStock;           // 가용 재고: stock 중 아직 어떤 CONFIRMED 주문에도 배정되지 않은
                                  // 수량. 등록 시 stock과 같은 값으로 초기화한다. 승인 처리(Phase 4)와
                                  // 생산 완료(Phase 5)마다 갱신되며, 재고 충분/부족 판단과 모니터링의
                                  // 여유/부족 판정(design_phase_7.md 참조)에 stock 대신 이 값을 쓴다.
};
```

- `stock`과 `availableStock`은 서로 다른 개념이다. `stock`은 "창고에 실제로 있는 물리적 수량"이고,
  `availableStock`은 "그중 아직 어떤 주문에도 배정되지 않아 새 주문에 쓸 수 있는 수량"이다.
  두 필드가 갱신되는 시점과 정확한 계산 방법은 [design_phase_4.md - 2.1](./design_phase_4.md#21-승인재고-부족-시-실-생산량-계산-저장-생산-큐-등록)과
  [design_phase_5.md - 3](./design_phase_5.md#3-실-생산량과-수율-처리-방식)에서 구체적인 예시와 함께 정의한다.
- 시료 등록 시 `availableStock`은 항상 `stock`과 동일한 값으로 초기화한다(아직 아무 주문도 없으므로
  전량 가용).

### 2.2 `ProductSpecController` 메뉴 흐름 (`Controller/product_spec_controller.h` / `.cpp`)

- **등록**: 시료ID/시료명/평균생산시간/수율/초기재고 입력 → 입력값 검증(2.3 참조) → 확인(Y/N)
  → Y 선택 시 `ProductSpecRepository::add` 호출 → 등록 결과 출력. N 선택 시 등록 취소, 메뉴로 복귀.
- **조회**: `ProductSpecRepository::findAll` 결과를 표 형태(ID/시료명/평균생산시간/수율/재고)로 출력.
- **검색**: 시료명 키워드 입력 → `ProductSpecRepository::findByName(keyword)` 호출 → 결과 출력
  (없으면 "검색 결과 없음" 안내, REQUIREMENT.md 5.0 공통 사항).
- **뒤로가기**: 상위(메인) 메뉴로 복귀.

### 2.3 입력 검증 규칙

- 수율(`yield`)은 `0 < yield <= 1` 범위를 벗어나면 재입력을 요청한다.
- 평균 생산시간(`avgProductionTime`)은 양수가 아니면 재입력을 요청한다.
- 초기 재고(`stock`)는 음수가 아니면 통과(0 허용).
- 시료ID가 이미 등록된 값이면 등록을 거부하고 재입력을 요청한다(`findById`로 중복 확인).

### 2.4 검색 구현

- `ProductSpecRepository::findByName(const std::string& keyword) const`은 시료명 부분 일치
  (대소문자 무시)로 매칭되는 항목을 모두 반환한다 (Repository 인터페이스는
  [design_phase_1.md](./design_phase_1.md) 참조).
  - REQUIREMENT.md 5.2는 "시료명 등 속성으로 검색"이라고만 명시하며, 부분 일치/대소문자 무시
    여부는 규정하지 않는다. 이 두 가지는 요구사항이 아니라 본 문서에서 정한 구현 결정이다.

## 3. 검증 방법 (Verify)

- 시료 등록 후 `data/product_specs.json`에 반영되는지 확인한다.
- 등록되지 않은 시료 ID로 검색 시 빈 목록/오류 안내가 출력되는지 확인한다(REQUIREMENT.md 5.0 공통 사항).
- 시료를 등록한 뒤 프로그램을 종료하고 재실행했을 때, `data/product_specs.json` 파일이 삭제되지
  않고 그대로 남아 있으며 등록했던 시료가 조회/검색 메뉴에 동일하게 나타나는지 확인한다
  (Phase 1에서 설계한 JSON 영속성이 실제로 재실행 간 데이터를 유지하는지 검증).

## 4. 리뷰 포인트 (Review)

- 시료 등록 시 입력 검증(수율 0~1 범위, 평균 생산시간 양수 등)이 있는지.
- View 계층이 Repository를 직접 호출하지 않고 Controller를 거치는지.
