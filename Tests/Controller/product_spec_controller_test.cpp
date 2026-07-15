#include <filesystem>
#include <string>

#include "gtest/gtest.h"

#include "Controller/product_spec_controller.h"
#include "Repository/product_spec_repository.h"
#include "View/product_spec_view.h"

namespace {

std::filesystem::path tempFilePath(const std::string& testName) {
    return std::filesystem::temp_directory_path() / ("sos_test_pscontroller_" + testName + ".json");
}

void resetFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

RegisterProductSpecInput makeValidInput(const std::string& id) {
    RegisterProductSpecInput input;
    input.productSpecId = id;
    input.name = "SampleA";
    input.avgProductionTime = 10.0;
    input.yield = 0.9;
    input.stock = 30;
    return input;
}

}  // namespace

TEST(ProductSpecControllerTest, ProductSpecControllerRegisterRejectsYieldOutOfRange) {
    auto path = tempFilePath("reject_yield");
    resetFile(path);
    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    RegisterProductSpecInput input = makeValidInput("S-201");
    input.yield = 0.0;
    EXPECT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::InvalidYield);

    input.yield = 1.5;
    EXPECT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::InvalidYield);
    EXPECT_FALSE(repository.findById("S-201").has_value());

    resetFile(path);
}

TEST(ProductSpecControllerTest, ProductSpecControllerRegisterRejectsNonPositiveProductionTime) {
    auto path = tempFilePath("reject_time");
    resetFile(path);
    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    RegisterProductSpecInput input = makeValidInput("S-202");
    input.avgProductionTime = 0.0;
    EXPECT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::InvalidProductionTime);

    input.avgProductionTime = -1.0;
    EXPECT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::InvalidProductionTime);
    EXPECT_FALSE(repository.findById("S-202").has_value());

    resetFile(path);
}

TEST(ProductSpecControllerTest, ProductSpecControllerRegisterRejectsDuplicateId) {
    auto path = tempFilePath("reject_duplicate");
    resetFile(path);
    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    RegisterProductSpecInput input = makeValidInput("S-203");
    ASSERT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::Success);

    RegisterProductSpecInput duplicate = makeValidInput("S-203");
    EXPECT_EQ(controller.registerSpec(duplicate, true), RegisterProductSpecResult::DuplicateId);
    EXPECT_EQ(repository.findAll().size(), 1u);

    resetFile(path);
}

TEST(ProductSpecControllerTest, ProductSpecControllerRegisterCancelOnNoConfirm) {
    auto path = tempFilePath("cancel_no_confirm");
    resetFile(path);
    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    RegisterProductSpecInput input = makeValidInput("S-204");
    EXPECT_EQ(controller.registerSpec(input, false), RegisterProductSpecResult::Cancelled);
    EXPECT_FALSE(repository.findById("S-204").has_value());

    resetFile(path);
}

TEST(ProductSpecControllerTest, ProductSpecControllerSearchShowsEmptyResultMessage) {
    auto path = tempFilePath("search_empty");
    resetFile(path);
    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    EXPECT_TRUE(controller.search("anything").empty());

    resetFile(path);
}
