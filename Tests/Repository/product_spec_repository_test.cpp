#include <filesystem>
#include <fstream>
#include <string>

#include "gtest/gtest.h"

#include "Model/product_spec.h"
#include "Repository/product_spec_repository.h"

namespace {

std::filesystem::path tempFilePath(const std::string& testName) {
    return std::filesystem::temp_directory_path() / ("sos_test_" + testName + ".json");
}

void resetFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}

ProductSpec makeProductSpec(const std::string& id) {
    ProductSpec spec;
    spec.productSpecId = id;
    spec.name = "Test Spec " + id;
    spec.avgProductionTime = 10.0;
    spec.yield = 0.9;
    spec.stock = 10;
    spec.availableStock = 10;
    return spec;
}

}  // namespace

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryAddPersistsToFile) {
    auto path = tempFilePath("add_persists");
    resetFile(path);

    ProductSpecRepository repo(path.string());
    repo.add(makeProductSpec("S-001"));

    EXPECT_EQ(repo.findAll().size(), 1u);
    ASSERT_TRUE(std::filesystem::exists(path));
    EXPECT_GT(std::filesystem::file_size(path), 0u);

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryFindByIdReturnsNulloptWhenMissing) {
    auto path = tempFilePath("find_missing");
    resetFile(path);

    ProductSpecRepository repo(path.string());

    EXPECT_EQ(repo.findById("S-NOPE"), std::nullopt);

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryUpdateOverwritesExistingRecord) {
    auto path = tempFilePath("update_overwrites");
    resetFile(path);

    ProductSpecRepository repo(path.string());
    repo.add(makeProductSpec("S-001"));

    ProductSpec updated = makeProductSpec("S-001");
    updated.stock = 99;
    updated.availableStock = 99;
    repo.update(updated);

    auto found = repo.findById("S-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->stock, 99);
    EXPECT_EQ(found->availableStock, 99);

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryRemoveDeletesRecord) {
    auto path = tempFilePath("remove_deletes");
    resetFile(path);

    ProductSpecRepository repo(path.string());
    repo.add(makeProductSpec("S-001"));
    ASSERT_EQ(repo.findAll().size(), 1u);

    repo.remove("S-001");

    EXPECT_TRUE(repo.findAll().empty());

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryFindByNamePartialMatchCaseInsensitive) {
    auto path = tempFilePath("find_by_name");
    resetFile(path);

    ProductSpecRepository repo(path.string());
    ProductSpec spec = makeProductSpec("S-301");
    spec.name = "SampleA";
    repo.add(spec);

    EXPECT_EQ(repo.findByName("samplea").size(), 1u);
    EXPECT_EQ(repo.findByName("ample").size(), 1u);
    EXPECT_TRUE(repo.findByName("nomatch").empty());

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryPersistenceSurvivesRestart) {
    auto path = tempFilePath("persistence_restart");
    resetFile(path);

    {
        ProductSpecRepository repo(path.string());
        repo.add(makeProductSpec("S-302"));
    }

    ProductSpecRepository restarted(path.string());
    EXPECT_EQ(restarted.findAll().size(), 1u);
    EXPECT_TRUE(restarted.findById("S-302").has_value());

    resetFile(path);
}

TEST(ProductSpecRepositoryTest, ProductSpecRepositoryReloadReflectsExternalFileChange) {
    auto path = tempFilePath("reload_reflects");
    resetFile(path);

    ProductSpecRepository repoA(path.string());
    ProductSpecRepository repoB(path.string());

    repoA.add(makeProductSpec("S-001"));

    EXPECT_TRUE(repoB.findAll().empty());

    repoB.reload();

    EXPECT_EQ(repoB.findAll().size(), 1u);

    resetFile(path);
}
