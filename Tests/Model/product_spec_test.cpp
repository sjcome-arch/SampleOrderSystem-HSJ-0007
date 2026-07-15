#include <filesystem>
#include <string>

#include "gtest/gtest.h"

#include "Controller/product_spec_controller.h"
#include "Repository/product_spec_repository.h"
#include "View/product_spec_view.h"

namespace {

std::filesystem::path tempFilePath(const std::string& testName) {
    return std::filesystem::temp_directory_path() / ("sos_test_model_" + testName + ".json");
}

void resetFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

}  // namespace

TEST(ProductSpecTest, ProductSpecRegisterInitializesAvailableStockEqualToStock) {
    auto path = tempFilePath("register_available_stock");
    resetFile(path);

    ProductSpecRepository repository(path.string());
    ProductSpecView view;
    ProductSpecController controller(repository, view);

    RegisterProductSpecInput input;
    input.productSpecId = "S-100";
    input.name = "Sample100";
    input.avgProductionTime = 5.0;
    input.yield = 0.8;
    input.stock = 50;

    ASSERT_EQ(controller.registerSpec(input, true), RegisterProductSpecResult::Success);

    auto found = repository.findById("S-100");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->stock, 50);
    EXPECT_EQ(found->availableStock, 50);

    resetFile(path);
}
