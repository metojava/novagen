#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <novagen/generator.hpp>

using namespace novagen;

class GeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_output_dir_ = "test_output_" + std::to_string(::testing::UnitTest::GetInstance()->current_test_info()->index());
        fs::create_directories(test_output_dir_);
    }

    void TearDown() override {
        fs::remove_all(test_output_dir_);
    }

    std::string test_output_dir_;
};

TEST_F(GeneratorTest, GenerateSimpleProject) {
    String sql = R"(
        CREATE TABLE users (
            id INT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            email VARCHAR(255) UNIQUE
        );
    )";

    Generator generator("test_project", "app", test_output_dir_);
    auto tables = parse_tables(sql);

    ASSERT_TRUE(generator.generate(tables));

    fs::path project_path = fs::path(test_output_dir_) / "test_project";
    EXPECT_TRUE(fs::exists(project_path / "CMakeLists.txt"));
    EXPECT_TRUE(fs::exists(project_path / "include" / "app" / "models.h"));
    EXPECT_TRUE(fs::exists(project_path / "src" / "models.cpp"));
    EXPECT_TRUE(fs::exists(project_path / "src" / "main.cpp"));
}

TEST_F(GeneratorTest, GenerateMultipleTables) {
    String sql = R"(
        CREATE TABLE users (
            id INT PRIMARY KEY,
            name VARCHAR(100)
        );

        CREATE TABLE posts (
            id INT PRIMARY KEY,
            user_id INT,
            title VARCHAR(200),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
    )";

    Generator generator("multi_test", "app", test_output_dir_);
    auto tables = parse_tables(sql);

    ASSERT_TRUE(generator.generate(tables));

    fs::path project_path = fs::path(test_output_dir_) / "multi_test";
    EXPECT_TRUE(fs::exists(project_path / "include" / "app" / "database.h"));

    std::ifstream models_h(project_path / "include" / "app" / "models.h");
    std::string content((std::istreambuf_iterator<char>(models_h)),
                        std::istreambuf_iterator<char>());
    models_h.close();

    EXPECT_NE(content.find("struct User"), std::string::npos);
    EXPECT_NE(content.find("struct Post"), std::string::npos);
}

TEST_F(GeneratorTest, GenerateFromSql) {
    String sql = R"(
        CREATE TABLE categories (
            id INT PRIMARY KEY,
            name VARCHAR(100)
        );
    )";

    Generator generator("sql_test", "app", test_output_dir_);
    ASSERT_TRUE(generator.generate_from_sql(sql));
}

TEST_F(GeneratorTest, CppTypesInGeneratedCode) {
    String sql = R"(
        CREATE TABLE products (
            id INT PRIMARY KEY,
            price DOUBLE,
            in_stock BOOLEAN,
            description TEXT
        );
    )";

    Generator generator("types_test", "app", test_output_dir_);
    auto tables = parse_tables(sql);
    ASSERT_TRUE(generator.generate(tables));

    fs::path models_h = fs::path(test_output_dir_) / "types_test" / "include" / "app" / "models.h";
    std::ifstream file(models_h);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    EXPECT_NE(content.find("int32_t"), std::string::npos);
    EXPECT_NE(content.find("double"), std::string::npos);
    EXPECT_NE(content.find("bool"), std::string::npos);
    EXPECT_NE(content.find("std::string"), std::string::npos);
}

TEST_F(GeneratorTest, GitignoreGenerated) {
    String sql = "CREATE TABLE t (id INT PRIMARY KEY);";

    Generator generator("gitignore_test", "app", test_output_dir_);
    auto tables = parse_tables(sql);
    ASSERT_TRUE(generator.generate(tables));

    fs::path gitignore = fs::path(test_output_dir_) / "gitignore_test" / ".gitignore";
    EXPECT_TRUE(fs::exists(gitignore));
}

TEST_F(GeneratorTest, ReadmeGenerated) {
    String sql = "CREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(100));";

    Generator generator("readme_test", "app", test_output_dir_);
    auto tables = parse_tables(sql);
    ASSERT_TRUE(generator.generate(tables));

    fs::path readme = fs::path(test_output_dir_) / "readme_test" / "README.md";
    EXPECT_TRUE(fs::exists(readme));

    std::ifstream file(readme);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    EXPECT_NE(content.find("api/users"), std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
