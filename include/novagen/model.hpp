#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>
#include <memory>
#include <cstdint>

namespace novagen {

using String = std::string;
using Strings = std::vector<String>;
using Int64 = int64_t;
using Int32 = int32_t;

struct Column {
    String name;
    String sql_type;
    bool nullable{true};
    bool primary_key{false};
    bool auto_increment{false};
    bool unique{false};
    std::optional<String> default_value;
    std::optional<Int64> length;
    std::optional<Int64> precision;
    std::optional<Int64> scale;

    String cpp_type() const;
    String field_name() const;
    String db_type() const;
    String getter_name() const;
    String setter_name() const;
};

struct ForeignKey {
    String column;
    String ref_table;
    String ref_column;

    String ref_class() const;
    String ref_instance() const;
};

struct Index {
    std::optional<String> name;
    Strings columns;
    bool unique{false};
};

struct Table {
    String name;
    std::vector<Column> columns;
    std::vector<ForeignKey> foreign_keys;
    std::vector<Index> indexes;

    String class_name() const;
    String instance_name() const;
    String kebab_name() const;
    String snake_name() const;
    String fk_ref(const String& column) const;

    const Column* pk_column() const;
    std::vector<Column> non_pk_columns() const;
};

Strings parse_sql(const String& sql);
std::vector<Table> parse_tables(const String& sql);

String to_pascal(const String& name);
String to_camel(const String& name);
String to_snake(const String& name);
String to_kebab(const String& name);
String pluralize(const String& name);

} // namespace novagen
