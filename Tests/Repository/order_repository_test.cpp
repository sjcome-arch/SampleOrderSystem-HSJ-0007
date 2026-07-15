#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "Model/order.h"
#include "Model/order_status.h"
#include "Model/product_spec.h"
#include "Model/waiting_approval_queue.h"
#include "Repository/order_repository.h"
#include "Repository/product_spec_repository.h"

namespace {

std::filesystem::path tempFilePath(const std::string& testName) {
    return std::filesystem::temp_directory_path() / ("sos_test_" + testName + ".json");
}

void resetFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

Order makeOrder(const std::string& productSpecId, OrderStatus status) {
    Order order;
    order.productSpecId = productSpecId;
    order.customerName = "Test Customer";
    order.quantity = 5;
    order.status = status;
    return order;
}

}  // namespace

TEST(OrderRepositoryTest, OrderRepositoryFindByStatusFiltersCorrectly) {
    auto path = tempFilePath("find_by_status");
    resetFile(path);

    OrderRepository repo(path.string());
    repo.add(makeOrder("S-001", OrderStatus::RESERVED));
    repo.add(makeOrder("S-001", OrderStatus::CONFIRMED));

    auto reserved = repo.findByStatus(OrderStatus::RESERVED);

    ASSERT_EQ(reserved.size(), 1u);
    EXPECT_EQ(reserved[0].status, OrderStatus::RESERVED);

    resetFile(path);
}

TEST(OrderRepositoryTest, RepositoryRestartAfterProcessExitKeepsData) {
    auto specPath = tempFilePath("restart_specs");
    auto orderPath = tempFilePath("restart_orders");
    resetFile(specPath);
    resetFile(orderPath);

    {
        ProductSpecRepository specRepo(specPath.string());
        OrderRepository orderRepo(orderPath.string());

        ProductSpec spec;
        spec.productSpecId = "S-001";
        spec.name = "Test Spec";
        spec.avgProductionTime = 10.0;
        spec.yield = 0.9;
        spec.stock = 10;
        spec.availableStock = 10;
        specRepo.add(spec);

        orderRepo.add(makeOrder("S-001", OrderStatus::RESERVED));
    }

    // 재시작 시뮬레이션: 완전히 새로운 Repository 인스턴스 생성
    ProductSpecRepository restartedSpecRepo(specPath.string());
    OrderRepository restartedOrderRepo(orderPath.string());

    EXPECT_EQ(restartedSpecRepo.findAll().size(), 1u);
    EXPECT_EQ(restartedOrderRepo.findAll().size(), 1u);

    resetFile(specPath);
    resetFile(orderPath);
}

TEST(OrderRepositoryTest, QueueRebuildOnStartupMatchesShutdownOrder) {
    auto path = tempFilePath("queue_rebuild");
    resetFile(path);

    std::vector<std::string> orderIdsInCreationOrder;
    {
        OrderRepository repo(path.string());
        for (int i = 0; i < 3; ++i) {
            Order created = repo.add(makeOrder("S-001", OrderStatus::RESERVED));
            orderIdsInCreationOrder.push_back(created.orderId);
        }
    }

    // 종료 후 재시작 시뮬레이션: 새 Repository + 새 WaitingApprovalQueue로 재구성
    OrderRepository restartedRepo(path.string());
    auto reserved = restartedRepo.findByStatus(OrderStatus::RESERVED);
    std::sort(reserved.begin(), reserved.end(),
              [](const Order& a, const Order& b) { return a.orderId < b.orderId; });

    WaitingApprovalQueue queue(reserved);
    auto snapshot = queue.snapshot();

    ASSERT_EQ(snapshot.size(), orderIdsInCreationOrder.size());
    for (size_t i = 0; i < snapshot.size(); ++i) {
        EXPECT_EQ(snapshot[i].orderId, orderIdsInCreationOrder[i]);
    }

    resetFile(path);
}
