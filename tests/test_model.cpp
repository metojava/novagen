#include <gtest/gtest.h>
#include <novagen/model.hpp>

using namespace novagen;

TEST(ModelTest, ToPascal) {
    EXPECT_EQ(to_pascal("users"), "Users");
    EXPECT_EQ(to_pascal("user_profiles"), "UserProfiles");
    EXPECT_EQ(to_pascal("order_items"), "OrderItems");
    EXPECT_EQ(to_pascal("my-table"), "MyTable");
}

TEST(ModelTest, ToSnake) {
    EXPECT_EQ(to_snake("Users"), "users");
    EXPECT_EQ(to_snake("UserProfiles"), "user_profiles");
    EXPECT_EQ(to_snake("MyTable"), "my_table");
}

TEST(ModelTest, ToKebab) {
    EXPECT_EQ(to_kebab("users"), "users");
    EXPECT_EQ(to_kebab("UserProfiles"), "user-profiles");
    EXPECT_EQ(to_kebab("my_table"), "my-table");
}

TEST(ModelTest, Pluralize) {
    EXPECT_EQ(pluralize("user"), "users");
    EXPECT_EQ(pluralize("category"), "categories");
    EXPECT_EQ(pluralize("post"), "posts");
    EXPECT_EQ(pluralize("bus"), "buses");
}

TEST(ModelTest, ColumnCppType) {
    Column col;

    col.sql_type = "INT";
    EXPECT_EQ(col.cpp_type(), "int32_t");

    col.sql_type = "BIGINT";
    EXPECT_EQ(col.cpp_type(), "int64_t");

    col.sql_type = "VARCHAR(100)";
    EXPECT_EQ(col.cpp_type(), "std::string");

    col.sql_type = "BOOLEAN";
    EXPECT_EQ(col.cpp_type(), "bool");

    col.sql_type = "FLOAT";
    EXPECT_EQ(col.cpp_type(), "float");

    col.sql_type = "DOUBLE";
    EXPECT_EQ(col.cpp_type(), "double");
}

TEST(ModelTest, ColumnFieldName) {
    Column col;
    col.name = "firstName";
    EXPECT_EQ(col.field_name(), "first_name");

    col.name = "UserID";
    EXPECT_EQ(col.field_name(), "user_id");
}

TEST(ModelTest, ColumnDbType) {
    Column col;

    col.sql_type = "INT";
    EXPECT_EQ(col.db_type(), "INTEGER");

    col.sql_type = "VARCHAR(255)";
    EXPECT_EQ(col.db_type(), "VARCHAR(255)");

    col.sql_type = "TEXT";
    EXPECT_EQ(col.db_type(), "TEXT");

    col.sql_type = "BLOB";
    EXPECT_EQ(col.db_type(), "BLOB");
}

TEST(ModelTest, TableClassName) {
    Table table;
    table.name = "user_profiles";
    EXPECT_EQ(table.class_name(), "UserProfiles");
}

TEST(ModelTest, TableInstanceName) {
    Table table;
    table.name = "UserProfiles";
    EXPECT_EQ(table.instance_name(), "userProfiles");

    table.name = "Users";
    EXPECT_EQ(table.instance_name(), "users");
}

TEST(ModelTest, TablePkColumn) {
    Table table;
    table.columns = {
        {"id", "INT", true, true, false},
        {"name", "VARCHAR(100)", false, false, false}
    };

    const Column* pk = table.pk_column();
    ASSERT_NE(pk, nullptr);
    EXPECT_EQ(pk->name, "id");
    EXPECT_TRUE(pk->primary_key);
}

TEST(ModelTest, TableNonPkColumns) {
    Table table;
    table.columns = {
        {"id", "INT", true, true, false},
        {"name", "VARCHAR(100)", false, false, false},
        {"email", "VARCHAR(255)", false, false, false}
    };

    auto non_pk = table.non_pk_columns();
    EXPECT_EQ(non_pk.size(), 2);
    EXPECT_EQ(non_pk[0].name, "name");
    EXPECT_EQ(non_pk[1].name, "email");
}

TEST(SqlParserTest, ParseSimpleCreateTable) {
    String sql = R"(
        CREATE TABLE users (
            id INT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            email VARCHAR(255) UNIQUE
        );
    )";

    auto tables = parse_tables(sql);
    EXPECT_EQ(tables.size(), 1);
    EXPECT_EQ(tables[0].name, "users");
    EXPECT_EQ(tables[0].columns.size(), 3);
    EXPECT_EQ(tables[0].pk_column()->name, "id");
}

TEST(SqlParserTest, ParseMultipleTables) {
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

    auto tables = parse_tables(sql);
    EXPECT_EQ(tables.size(), 2);
    EXPECT_EQ(tables[0].name, "users");
    EXPECT_EQ(tables[1].name, "posts");
    EXPECT_EQ(tables[1].foreign_keys.size(), 1);
    EXPECT_EQ(tables[1].foreign_keys[0].ref_table, "users");
}

TEST(SqlParserTest, ParseAutoIncrement) {
    String sql = R"(
        CREATE TABLE products (
            id INT PRIMARY KEY IDENTITY(1,1),
            name VARCHAR(100)
        );
    )";

    auto tables = parse_tables(sql);
    EXPECT_EQ(tables[0].columns[0].auto_increment, true);
}

TEST(SqlParserTest, ParseDefaultValues) {
    String sql = R"(
        CREATE TABLE settings (
            id INT PRIMARY KEY,
            value VARCHAR(100) DEFAULT 'default',
            active BOOLEAN DEFAULT 1
        );
    )";

    auto tables = parse_tables(sql);
    EXPECT_EQ(tables[0].columns[1].default_value, "'default'");
}

TEST(SqlParserTest, ForeignKeyReference) {
    String sql = R"(
        CREATE TABLE orders (
            id INT PRIMARY KEY,
            user_id INT NOT NULL,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
    )";

    auto tables = parse_tables(sql);
    EXPECT_EQ(tables[0].foreign_keys.size(), 1);
    EXPECT_EQ(tables[0].foreign_keys[0].column, "user_id");
    EXPECT_EQ(tables[0].foreign_keys[0].ref_table, "users");
    EXPECT_EQ(tables[0].foreign_keys[0].ref_column, "id");
}

TEST(SqlParserTest, ForeignKeyRefClass) {
    ForeignKey fk;
    fk.ref_table = "user_profiles";
    EXPECT_EQ(fk.ref_class(), "UserProfiles");
}

TEST(SqlParserTest, ForeignKeyRefInstance) {
    ForeignKey fk;
    fk.column = "user_id";
    EXPECT_EQ(fk.ref_instance(), "user");

    fk.column = "profile_id";
    EXPECT_EQ(fk.ref_instance(), "profile");
}
