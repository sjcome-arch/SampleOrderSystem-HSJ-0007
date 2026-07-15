#include "Controller/dummy_data_generator_controller.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

#include "Model/order_status.h"

namespace {

constexpr unsigned int kFixedSeed = 12345;  // design_phase_8.md 2.1: 실행할 때마다 동일한 결과를 내기 위한 고정 시드.

const std::vector<std::string>& baseNames() {
    static const std::vector<std::string> names = {"Alpha", "Beta", "Gamma", "Delta", "Epsilon",
                                                     "Zeta",  "Eta",  "Theta", "Iota",  "Kappa"};
    return names;
}

const std::vector<std::string>& customerNames() {
    static const std::vector<std::string> names = {"Acme Labs",  "Nova Fab",   "Quantum R&D", "Silverline",
                                                     "Bluepeak",   "Cedar Semi", "Riverstone",  "Northgate"};
    return names;
}

}  // namespace

DummyDataGeneratorController::DummyDataGeneratorController(IProductSpecRepository& productSpecRepository,
                                                             IOrderRepository& orderRepository,
                                                             OrderController& orderController,
                                                             ReleaseController& releaseController,
                                                             DummyDataGeneratorView& view)
    : productSpecRepository_(productSpecRepository),
      orderRepository_(orderRepository),
      orderController_(orderController),
      releaseController_(releaseController),
      view_(view) {}

int DummyDataGeneratorController::generateProductSpecs(int count, std::mt19937& rng) {
    std::uniform_int_distribution<int> baseNameDist(0, static_cast<int>(baseNames().size()) - 1);
    std::uniform_int_distribution<int> suffixDist(1, 999);
    std::uniform_real_distribution<double> productionTimeDist(0.1, 1.0);
    std::uniform_real_distribution<double> yieldDist(0.7, 0.99);
    std::uniform_int_distribution<int> stockDist(0, 200);

    int sequence = static_cast<int>(productSpecRepository_.findAll().size()) + 1;
    int generated = 0;
    for (int i = 0; i < count; ++i) {
        // 시료 ID 채번 규칙은 design.md "5. 향후 구현 결정 사항(TBD)"의 예시(S-NNN)를 따른다.
        std::string id;
        do {
            id = "S-" + std::to_string(sequence++);
        } while (productSpecRepository_.findById(id).has_value());

        ProductSpec spec;
        spec.productSpecId = id;
        spec.name = baseNames()[baseNameDist(rng)] + "-" + std::to_string(suffixDist(rng));
        spec.avgProductionTime = productionTimeDist(rng);
        spec.yield = yieldDist(rng);
        spec.stock = stockDist(rng);
        spec.availableStock = spec.stock;

        productSpecRepository_.add(spec);
        ++generated;
    }
    return generated;
}

int DummyDataGeneratorController::generateOrders(int count, std::mt19937& rng) {
    auto specs = productSpecRepository_.findAll();
    if (specs.empty()) {
        return 0;
    }

    std::uniform_int_distribution<size_t> specDist(0, specs.size() - 1);
    std::uniform_int_distribution<int> quantityDist(1, 30);
    std::uniform_int_distribution<int> customerDist(0, static_cast<int>(customerNames().size()) - 1);
    std::uniform_int_distribution<int> outcomeDist(0, 3);  // 0=RESERVED 유지, 1=거절, 2=승인, 3=승인 후 출고까지

    std::vector<int> outcomes(count);
    for (int& outcome : outcomes) {
        outcome = outcomeDist(rng);
    }
    // 접수 큐는 FIFO이므로, RESERVED로 남길 주문(0)을 맨 뒤로 몰아 생성해야 앞서 생성한 주문의
    // 승인/거절 처리(approveNext/rejectNext)가 항상 자기 자신을 대상으로 하게 된다.
    std::stable_partition(outcomes.begin(), outcomes.end(), [](int outcome) { return outcome != 0; });

    int generated = 0;
    for (int outcome : outcomes) {
        const ProductSpec& spec = specs[specDist(rng)];

        ReserveOrderInput input;
        input.productSpecId = spec.productSpecId;
        input.customerName = customerNames()[customerDist(rng)];
        input.quantity = quantityDist(rng);

        if (orderController_.reserveOrder(input, true) != ReserveOrderResult::Success) {
            continue;
        }
        ++generated;

        if (outcome == 0) {
            continue;  // RESERVED로 남김
        }
        if (outcome == 1) {
            orderController_.rejectNext();
            continue;
        }

        ApproveNextResult approveResult = orderController_.approveNext();
        if (outcome == 3 && !approveResult.queueEmpty && approveResult.order.status == OrderStatus::CONFIRMED) {
            releaseController_.release(approveResult.order.orderId, std::chrono::system_clock::now());
        }
    }
    return generated;
}

DummyDataGenerationSummary DummyDataGeneratorController::generate(int specCount, int orderCount) {
    std::mt19937 rng(kFixedSeed);

    DummyDataGenerationSummary summary;
    summary.specCount = generateProductSpecs(specCount, rng);
    summary.orderCount = generateOrders(orderCount, rng);
    return summary;
}

void DummyDataGeneratorController::run() {
    int specCount = view_.promptSpecCount();
    int orderCount = view_.promptOrderCount();
    view_.showSummary(generate(specCount, orderCount));
}
