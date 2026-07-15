#include "gtest/gtest.h"

#include "Model/waiting_approval_queue.h"

namespace {

Order makeOrder(const std::string& id) {
    Order order;
    order.orderId = id;
    order.status = OrderStatus::RESERVED;
    return order;
}

}  // namespace

TEST(WaitingApprovalQueueTest, WaitingApprovalQueueEnqueuePreservesInsertionOrder) {
    WaitingApprovalQueue queue({});
    queue.enqueue(makeOrder("ORD-000001"));
    queue.enqueue(makeOrder("ORD-000002"));
    queue.enqueue(makeOrder("ORD-000003"));

    auto snapshot = queue.snapshot();
    ASSERT_EQ(snapshot.size(), 3u);
    EXPECT_EQ(snapshot[0].orderId, "ORD-000001");
    EXPECT_EQ(snapshot[1].orderId, "ORD-000002");
    EXPECT_EQ(snapshot[2].orderId, "ORD-000003");
}

TEST(WaitingApprovalQueueTest, WaitingApprovalQueueDequeueRemovesFrontOnly) {
    WaitingApprovalQueue queue({});
    queue.enqueue(makeOrder("ORD-000001"));
    queue.enqueue(makeOrder("ORD-000002"));
    queue.enqueue(makeOrder("ORD-000003"));

    auto dequeued = queue.dequeue();
    ASSERT_TRUE(dequeued.has_value());
    EXPECT_EQ(dequeued->orderId, "ORD-000001");

    auto snapshot = queue.snapshot();
    ASSERT_EQ(snapshot.size(), 2u);
    EXPECT_EQ(snapshot[0].orderId, "ORD-000002");
    EXPECT_EQ(snapshot[1].orderId, "ORD-000003");
}

TEST(WaitingApprovalQueueTest, WaitingApprovalQueueIsEmptyTrueWhenNoOrders) {
    WaitingApprovalQueue queue({});
    EXPECT_TRUE(queue.empty());
}

TEST(WaitingApprovalQueueTest, WaitingApprovalQueueRebuildFromRepositoryMatchesOrderNumberAscending) {
    std::vector<Order> reserved = {makeOrder("ORD-000001"), makeOrder("ORD-000002"), makeOrder("ORD-000003")};
    WaitingApprovalQueue rebuilt(reserved);

    auto snapshot = rebuilt.snapshot();
    ASSERT_EQ(snapshot.size(), 3u);
    EXPECT_EQ(snapshot[0].orderId, "ORD-000001");
    EXPECT_EQ(snapshot[1].orderId, "ORD-000002");
    EXPECT_EQ(snapshot[2].orderId, "ORD-000003");
}
