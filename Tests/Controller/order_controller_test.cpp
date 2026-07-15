#include <filesystem>
#include <string>

#include "gtest/gtest.h"

#include "Controller/order_controller.h"
#include "Model/production_line.h"
#include "Model/waiting_approval_queue.h"
#include "Repository/order_repository.h"
#include "Repository/product_spec_repository.h"
#include "View/order_view.h"

namespace {

std::filesystem::path tempFilePath(const std::string& testName) {
    return std::filesystem::temp_directory_path() / ("sos_test_ordercontroller_" + testName + ".json");
}

void resetFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

ProductSpec makeProductSpec(const std::string& id) {
    ProductSpec spec;
    spec.productSpecId = id;
    spec.name = "SampleA";
    spec.avgProductionTime = 10.0;
    spec.yield = 0.9;
    spec.stock = 100;
    spec.availableStock = 100;
    return spec;
}

// OrderRepository::add 호출 시점에 WaitingApprovalQueue가 아직 비어 있는지 확인해
// "파일 저장이 큐 반영보다 먼저" 원칙을 검증하는 스파이 저장소.
class SaveOrderTrackingRepository : public IOrderRepository {
public:
    SaveOrderTrackingRepository(OrderRepository& delegate, const WaitingApprovalQueue& queue)
        : delegate_(delegate), queue_(queue) {}

    std::vector<Order> findAll() const override { return delegate_.findAll(); }
    std::optional<Order> findById(const std::string& orderId) const override { return delegate_.findById(orderId); }
    std::vector<Order> findByStatus(OrderStatus status) const override { return delegate_.findByStatus(status); }
    Order add(Order order) override {
        queueWasEmptyOnSave = queue_.empty();
        return delegate_.add(std::move(order));
    }
    void update(const Order& order) override { delegate_.update(order); }
    void reload() override { delegate_.reload(); }

    bool queueWasEmptyOnSave = false;

private:
    OrderRepository& delegate_;
    const WaitingApprovalQueue& queue_;
};

}  // namespace

TEST(OrderControllerTest, OrderControllerReserveCreatesOrderWithReservedStatus) {
    auto specPath = tempFilePath("reserve_creates_spec");
    auto orderPath = tempFilePath("reserve_creates_order");
    resetFile(specPath);
    resetFile(orderPath);

    ProductSpecRepository specRepository(specPath.string());
    specRepository.add(makeProductSpec("S-401"));
    OrderRepository orderRepository(orderPath.string());
    WaitingApprovalQueue queue({});
    ProductionLine productionLine({}, specRepository, orderRepository);
    OrderView view;
    OrderController controller(orderRepository, specRepository, queue, productionLine, view);

    ReserveOrderInput input;
    input.productSpecId = "S-401";
    input.customerName = "Customer A";
    input.quantity = 10;

    ASSERT_EQ(controller.reserveOrder(input, true), ReserveOrderResult::Success);

    auto orders = orderRepository.findByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(orders.size(), 1u);
    EXPECT_EQ(orders[0].status, OrderStatus::RESERVED);
    EXPECT_FALSE(queue.empty());

    resetFile(specPath);
    resetFile(orderPath);
}

TEST(OrderControllerTest, OrderControllerReserveSavesBeforeEnqueue) {
    auto specPath = tempFilePath("reserve_save_order_spec");
    auto orderPath = tempFilePath("reserve_save_order_order");
    resetFile(specPath);
    resetFile(orderPath);

    ProductSpecRepository specRepository(specPath.string());
    specRepository.add(makeProductSpec("S-402"));
    OrderRepository orderRepository(orderPath.string());
    WaitingApprovalQueue queue({});
    ProductionLine productionLine({}, specRepository, orderRepository);
    SaveOrderTrackingRepository trackingRepository(orderRepository, queue);
    OrderView view;
    OrderController controller(trackingRepository, specRepository, queue, productionLine, view);

    ReserveOrderInput input;
    input.productSpecId = "S-402";
    input.customerName = "Customer B";
    input.quantity = 5;

    ASSERT_EQ(controller.reserveOrder(input, true), ReserveOrderResult::Success);
    EXPECT_TRUE(trackingRepository.queueWasEmptyOnSave);

    resetFile(specPath);
    resetFile(orderPath);
}

TEST(OrderControllerTest, OrderControllerReserveCancelOnNoConfirm) {
    auto specPath = tempFilePath("reserve_cancel_spec");
    auto orderPath = tempFilePath("reserve_cancel_order");
    resetFile(specPath);
    resetFile(orderPath);

    ProductSpecRepository specRepository(specPath.string());
    specRepository.add(makeProductSpec("S-403"));
    OrderRepository orderRepository(orderPath.string());
    WaitingApprovalQueue queue({});
    ProductionLine productionLine({}, specRepository, orderRepository);
    OrderView view;
    OrderController controller(orderRepository, specRepository, queue, productionLine, view);

    ReserveOrderInput input;
    input.productSpecId = "S-403";
    input.customerName = "Customer C";
    input.quantity = 5;

    EXPECT_EQ(controller.reserveOrder(input, false), ReserveOrderResult::Cancelled);
    EXPECT_TRUE(orderRepository.findAll().empty());
    EXPECT_TRUE(queue.empty());

    resetFile(specPath);
    resetFile(orderPath);
}

TEST(OrderControllerTest, OrderControllerReserveRejectsUnknownProductSpec) {
    auto specPath = tempFilePath("reserve_unknown_spec");
    auto orderPath = tempFilePath("reserve_unknown_order");
    resetFile(specPath);
    resetFile(orderPath);

    ProductSpecRepository specRepository(specPath.string());
    OrderRepository orderRepository(orderPath.string());
    WaitingApprovalQueue queue({});
    ProductionLine productionLine({}, specRepository, orderRepository);
    OrderView view;
    OrderController controller(orderRepository, specRepository, queue, productionLine, view);

    ReserveOrderInput input;
    input.productSpecId = "S-NOPE";
    input.customerName = "Customer D";
    input.quantity = 5;

    EXPECT_EQ(controller.reserveOrder(input, true), ReserveOrderResult::ProductSpecNotFound);
    EXPECT_TRUE(orderRepository.findAll().empty());

    resetFile(specPath);
    resetFile(orderPath);
}

TEST(OrderControllerTest, OrderControllerReserveRejectsNonPositiveQuantity) {
    auto specPath = tempFilePath("reserve_bad_qty_spec");
    auto orderPath = tempFilePath("reserve_bad_qty_order");
    resetFile(specPath);
    resetFile(orderPath);

    ProductSpecRepository specRepository(specPath.string());
    specRepository.add(makeProductSpec("S-404"));
    OrderRepository orderRepository(orderPath.string());
    WaitingApprovalQueue queue({});
    ProductionLine productionLine({}, specRepository, orderRepository);
    OrderView view;
    OrderController controller(orderRepository, specRepository, queue, productionLine, view);

    ReserveOrderInput input;
    input.productSpecId = "S-404";
    input.customerName = "Customer E";
    input.quantity = 0;

    EXPECT_EQ(controller.reserveOrder(input, true), ReserveOrderResult::InvalidQuantity);
    EXPECT_TRUE(orderRepository.findAll().empty());

    resetFile(specPath);
    resetFile(orderPath);
}
