#include "View/dummy_data_generator_view.h"

#include <iostream>

int DummyDataGeneratorView::promptSpecCount() const {
    std::cout << "\n[Dummy 데이터 생성]\n생성할 시료 개수(N): ";
    int count = 0;
    std::cin >> count;
    return count;
}

int DummyDataGeneratorView::promptOrderCount() const {
    std::cout << "생성할 주문 개수(M): ";
    int count = 0;
    std::cin >> count;
    return count;
}

void DummyDataGeneratorView::showSummary(const DummyDataGenerationSummary& summary) const {
    std::cout << "생성 완료: 시료 " << summary.specCount << "건, 주문 " << summary.orderCount << "건 생성됨."
               << std::endl;
}
