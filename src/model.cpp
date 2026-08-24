#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <regex>
#include <sstream>
#include <stack>
#include <unordered_map>

#include "model.hpp"

namespace novagen {

inline bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline bool is_identifier_char(char c) {
    return std::isalnum(c) || c == '_';
}

inline String trim(const String& s) {
    size_t start = 0;
    while (start < s.size() && is_space(s[start])) ++start;
    size_t end = s.size();
    while (end > start && is_space(s[end - 1])) --end;
    return s.substr(start, end - start);
}

inline String trim_chars(const String& s, const char* chars) {
    size_t start = 0;
    while (start < s.size() && std::strchr(chars, s[start])) ++start;
    size_t end = s.size();
    while (end > start && std::strchr(chars, s[end - 1])) --end;
    return s.substr(start, end - start);
}

inline String to_lower(const String& s) {
    String result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline String to_upper(const String& s) {
    String result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

String to_pascal(const String& name) {
    String result;
    bool capitalize_next = true;

    for (char c : name) {
        if (c == '_' || c == '-' || c == ' ') {
            capitalize_next = true;
        } else if (capitalize_next) {
            result += static_cast<char>(std::toupper(c));
            capitalize_next = false;
        } else {
            result += static_cast<char>(std::tolower(c));
        }
    }
    if (!result.empty() && std::islower(result[0])) {
        result[0] = static_cast<char>(std::toupper(result[0]));
    }
    return result;
}

String to_camel(const String& name) {
    String result;
    bool capitalize_next = false;
    for (char c : name) {
        if (c == '_' || c == '-' || c == ' ') {
            capitalize_next = true;
        } else if (capitalize_next) {
            result += static_cast<char>(std::toupper(c));
            capitalize_next = false;
        } else {
            result += c;
        }
    }
    if (!result.empty() && std::isupper(result[0])) {
        result[0] = static_cast<char>(std::tolower(result[0]));
    }
    return result;
}

String to_snake(const String& name) {
    String result;
    for (size_t i = 0; i < name.size(); ++i) {
        char c = name[i];
        if (std::isupper(c)) {
            if (i > 0 && result.back() != '_') result += '_';
            result += static_cast<char>(std::tolower(c));
        } else if (c == '-' || c == ' ') {
            if (!result.empty() && result.back() != '_') result += '_';
        } else {
            result += c;
        }
    }
    return result;
}

String to_kebab(const String& name) {
    String snake = to_snake(name);
    String result;
    for (char c : snake) {
        result += (c == '_') ? '-' : c;
    }
    return result;
}

String pluralize(const String& name) {
    if (name.empty()) return name;

    String lower = to_lower(name);
    if (lower.back() == 's') {
        return name;
    }
    if (lower.back() == 'y') {
        return name.substr(0, name.size() - 1) + "ies";
    }
    return name + "s";
}

const std::unordered_map<String, String>& cpp_type_map() {
    static const std::unordered_map<String, String> map = {
        {"int", "int32_t"},
        {"integer", "int32_t"},
        {"smallint", "int16_t"},
        {"tinyint", "int8_t"},
        {"bigint", "int64_t"},
        {"bit", "bool"},
        {"boolean", "bool"},
        {"decimal", "double"},
        {"numeric", "double"},
        {"money", "double"},
        {"smallmoney", "double"},
        {"float", "float"},
        {"double", "double"},
        {"real", "float"},
        {"varchar", "std::string"},
        {"nvarchar", "std::string"},
        {"varchar2", "std::string"},
        {"char", "std::string"},
        {"nchar", "std::string"},
        {"text", "std::string"},
        {"ntext", "std::string"},
        {"clob", "std::string"},
        {"nclob", "std::string"},
        {"long", "std::string"},
        {"raw", "std::vector<uint8_t>"},
        {"long raw", "std::vector<uint8_t>"},
        {"varbinary", "std::vector<uint8_t>"},
        {"binary", "std::vector<uint8_t>"},
        {"image", "std::vector<uint8_t>"},
        {"blob", "std::vector<uint8_t>"},
        {"bfile", "std::vector<uint8_t>"},
        {"datetime", "std::chrono::system_clock::time_point"},
        {"datetime2", "std::chrono::system_clock::time_point"},
        {"datetimeoffset", "std::chrono::system_clock::time_point"},
        {"smalldatetime", "std::chrono::system_clock::time_point"},
        {"date", "std::chrono::year_month_day"},
        {"time", "std::chrono::seconds"},
        {"time with time zone", "std::string"},
        {"time with timezone", "std::string"},
        {"timestamp", "std::chrono::system_clock::time_point"},
        {"timestamp with time zone", "std::string"},
        {"timestamp with timezone", "std::string"},
        {"timestamp(6)", "std::chrono::system_clock::time_point"},
        {"interval year to month", "std::string"},
        {"interval day to second", "std::string"},
        {"uniqueidentifier", "std::string"},
        {"xml", "std::string"},
        {"json", "std::string"},
        {"sql_variant", "std::string"},
        {"geometry", "std::string"},
        {"geography", "std::string"},
        {"sys_refcursor", "void*"},
        {"ref cursor", "void*"},
        {"rowid", "std::string"},
        {"urowid", "std::string"},
        {"number", "double"},
        {"float", "float"},
        {"pls_integer", "int32_t"},
        {"binary_integer", "int32_t"},
        {"pls_byte", "int8_t"},
        {"signed", "int32_t"},
        {"unsigned", "uint32_t"},
        {"simple_integer", "int32_t"},
        {"simple_float", "float"},
        {"simple_double", "double"},
    };
    return map;
}

String Column::cpp_type() const {
    String base = sql_type;
    size_t paren_pos = base.find('(');
    if (paren_pos != String::npos) {
        base = base.substr(0, paren_pos);
    }
    size_t identity_pos = to_lower(base).find("identity");
    if (identity_pos != String::npos) {
        base = trim(base.substr(0, identity_pos));
    }
    base = to_lower(trim(base));

    const auto& map = cpp_type_map();
    auto it = map.find(base);
    if (it != map.end()) {
        return it->second;
    }
    return "std::string";
}

String Column::field_name() const {
    return to_snake(name);
}

String Column::db_type() const {
    String base = to_lower(sql_type);
    size_t paren_pos = base.find('(');
    if (paren_pos != String::npos) {
        base = base.substr(0, paren_pos);
    }
    base = trim(base);

    if (base == "int" || base == "integer" || base == "pls_integer" ||
        base == "binary_integer" || base == "simple_integer") return "INTEGER";
    if (base == "bigint") return "INTEGER";
    if (base == "smallint") return "INTEGER";
    if (base == "tinyint") return "INTEGER";
    if (base == "bit" || base == "boolean" || base == "bool") return "INTEGER";
    if (base == "decimal" || base == "numeric" || base == "money" ||
        base == "smallmoney" || base == "float" || base == "double" ||
        base == "real" || base == "number") {
        return "REAL";
    }
    if (base == "varchar" || base == "nvarchar" || base == "varchar2" ||
        base == "char" || base == "nchar" || base == "text" ||
        base == "ntext" || base == "clob" || base == "nclob" ||
        base == "long" || base == "uniqueidentifier" ||
        base == "xml" || base == "json") {
        if (length.has_value() && length.value() < 4000) {
            return "VARCHAR(" + std::to_string(length.value()) + ")";
        }
        return "TEXT";
    }
    if (base == "varbinary" || base == "binary" || base == "image" ||
        base == "blob" || base == "bfile" || base == "raw" || base == "long raw") {
        return "BLOB";
    }
    if (base == "datetime" || base == "datetime2" || base == "datetimeoffset" ||
        base == "timestamp" || base == "smalldatetime" || base == "date" ||
        base == "time" || base == "timestamp with time zone" ||
        base == "timestamp with timezone" || base == "time with time zone" ||
        base == "time with timezone") {
        return "TEXT";
    }
    if (base == "geometry" || base == "geography" || base == "sql_variant" ||
        base == "rowid" || base == "urowid" || base == "sys_refcursor" ||
        base == "ref cursor") {
        return "TEXT";
    }
    return "TEXT";
}

String Column::getter_name() const {
    return "get_" + field_name() + "()";
}

String Column::setter_name() const {
    return "set_" + field_name() + "()";
}

String ForeignKey::ref_class() const {
    return to_pascal(ref_table);
}

String ForeignKey::ref_instance() const {
    String n = to_lower(column);
    n = std::regex_replace(n, std::regex(R"(_?id$)", std::regex_constants::icase), "");
    n = std::regex_replace(n, std::regex(R"(_?by$)", std::regex_constants::icase), "");
    return to_snake(n);
}

String Table::class_name() const {
    return to_pascal(name);
}

String Table::instance_name() const {
    String cn = class_name();
    if (cn.empty()) return cn;
    cn[0] = static_cast<char>(std::tolower(cn[0]));
    return cn;
}

String Table::kebab_name() const {
    return to_kebab(name);
}

String Table::snake_name() const {
    return to_snake(name);
}

String Table::fk_ref(const String& column) const {
    for (const auto& fk : foreign_keys) {
        if (to_lower(trim_chars(fk.column, "\"'`[]")) == to_lower(trim_chars(column, "\"'`[]"))) {
            String ref = fk.ref_table;
            bool quote = !std::regex_match(ref, std::regex(R"([\w]+)"));
            return (quote ? "\"" + ref + "\"" : ref) + "." + fk.ref_column;
        }
    }
    return "";
}

const Column* Table::pk_column() const {
    for (const auto& col : columns) {
        if (col.primary_key) {
            return &col;
        }
    }
    return nullptr;
}

std::vector<Column> Table::non_pk_columns() const {
    std::vector<Column> result;
    for (const auto& col : columns) {
        if (!col.primary_key) {
            result.push_back(col);
        }
    }
    return result;
}

std::vector<String> split_statements(const String& sql) {
    std::vector<String> stmts;
    String sql_copy = sql;
    sql_copy = std::regex_replace(sql_copy, std::regex(R"(--.*)"), "");
    sql_copy = std::regex_replace(sql_copy, std::regex(R"(/\*.*?\*/)"), "");

    std::vector<String> result;
    std::stringstream buf;
    int depth = 0;
    bool in_string = false;
    char str_char = 0;

    for (size_t i = 0; i < sql_copy.size(); ++i) {
        char c = sql_copy[i];

        if (in_string) {
            buf << c;
            if (c == str_char) {
                in_string = false;
            }
            continue;
        }

        if (c == '\'' || c == '"') {
            in_string = true;
            str_char = c;
            buf << c;
            continue;
        }

        if (c == '(') {
            depth++;
            buf << c;
            continue;
        }
        if (c == ')') {
            depth--;
            buf << c;
            continue;
        }

        if (c == ';' && depth == 0) {
            String s = trim(buf.str());
            if (!s.empty()) {
                result.push_back(s);
            }
            buf.str("");
            buf.clear();
            continue;
        }

        if (c == 'G' && i + 1 < sql_copy.size() &&
            to_upper(sql_copy.substr(i, 2)) == "GO" && depth == 0) {
            String s = trim(buf.str());
            if (!s.empty()) {
                result.push_back(s);
            }
            buf.str("");
            buf.clear();
            i++;
            continue;
        }

        buf << c;
    }

    String s = trim(buf.str());
    if (!s.empty()) {
        result.push_back(s);
    }

    return result;
}

std::vector<String> split_column_lines(const String& body) {
    std::vector<String> lines;
    std::stringstream buf;
    int depth = 0;
    bool in_string = false;

    for (char c : body) {
        if (in_string) {
            buf << c;
            if (c == '\'') {
                in_string = false;
            }
            continue;
        }

        if (c == '\'') {
            in_string = true;
            buf << c;
            continue;
        }

        if (c == '(') {
            depth++;
            buf << c;
            continue;
        }
        if (c == ')') {
            depth--;
            buf << c;
            continue;
        }

        if (c == ',' && depth == 0) {
            String s = trim(buf.str());
            if (!s.empty()) {
                lines.push_back(s);
            }
            buf.str("");
            buf.clear();
            continue;
        }

        buf << c;
    }

    String rest = trim(buf.str());
    if (!rest.empty()) {
        lines.push_back(rest);
    }

    return lines;
}

std::vector<String> extract_paren_names(const String& s) {
    std::vector<String> result;
    std::regex re(R"(\(([^)]+)\))");
    std::smatch match;
    if (std::regex_search(s, match, re)) {
        String inner = match[1].str();
        std::stringstream ss(inner);
        String item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            item = std::regex_replace(item, std::regex(R"(")"), "");
            item = std::regex_replace(item, std::regex(R"(`)"), "");
            item = std::regex_replace(item, std::regex(R"(\[)"), "");
            item = std::regex_replace(item, std::regex(R"(\])"), "");
            item = trim(item);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
    }
    return result;
}

std::optional<ForeignKey> parse_fk(const String& line) {
    std::regex col_re(R"(FOREIGN\s+KEY\s*\(\s*([^)]+?)\s*\))",
                      std::regex_constants::icase);
    std::regex ref_re(R"(REFERENCES\s+(?:["`\[]?\w+["`\]]?\.)?["`\[]?(\w+(?:\s+\w+)?)["`\]]?\s*\(\s*([^)]+?)\s*\))",
                      std::regex_constants::icase);

    std::smatch col_match;
    std::smatch ref_match;

    if (std::regex_search(line, col_match, col_re) &&
        std::regex_search(line, ref_match, ref_re)) {

        auto clean = [](const String& s) {
            String result = trim(s);
            result = std::regex_replace(result, std::regex(R"(")"), "");
            result = std::regex_replace(result, std::regex(R"(`)"), "");
            result = std::regex_replace(result, std::regex(R"(\[)"), "");
            result = std::regex_replace(result, std::regex(R"(\])"), "");
            return trim(result);
        };

        return ForeignKey{
            clean(col_match[1].str()),
            clean(ref_match[1].str()),
            clean(ref_match[2].str())
        };
    }
    return std::nullopt;
}

std::optional<Column> parse_column_line(const String& line) {
    String normalized = std::regex_replace(line, std::regex(R"(\[(\w+)\])"), R"("$1")");

    normalized = std::regex_replace(normalized,
                                    std::regex(R"(CONSTRAINT\s+["`\[]?\w+["`\]]?\s+DEFAULT\s*(\([^)]*\)))",
                                               std::regex_constants::icase),
                                    R"(DEFAULT $1)");

    normalized = std::regex_replace(normalized,
                                    std::regex(R"(CONSTRAINT\s+\w+\s+(?:NOT\s+NULL|NULL|UNIQUE|PRIMARY\s+KEY|CHECK\s*\([^)]*\)))",
                                               std::regex_constants::icase),
                                    R"()");

    std::regex col_re(R"(["`]?([\w\s]+?)["`]?\s+["`]?(\w+)["`]?\s*(\([^)]*\))?)",
                      std::regex_constants::icase);
    std::smatch match;

    if (!std::regex_search(normalized, match, col_re)) {
        return std::nullopt;
    }

    Column col;
    col.name = trim(match[1].str());
    col.sql_type = match[2].str();
    if (match[3].matched) {
        col.sql_type += match[3].str();
    }

    String rest = match.suffix().str();

    std::optional<Int64> len;
    std::optional<Int64> scl;
    std::regex len_re(R"(\((\d+)(?:\s*,\s*(\d+))?\))");
    std::smatch len_match;
    String len_text = match[3].matched ? match[3].str() : "";
    if (std::regex_search(len_text, len_match, len_re)) {
        len = std::stoll(len_match[1].str());
        if (len_match[2].matched) {
            scl = std::stoll(len_match[2].str());
        }
    }
    col.length = len;
    col.scale = scl;

    col.auto_increment = to_lower(normalized).find("identity") != String::npos ||
                         to_lower(normalized).find("autoincrement") != String::npos;
    col.nullable = to_lower(rest).find("not null") == String::npos &&
                   to_lower(rest).find("null") == String::npos;
    col.primary_key = to_lower(rest).find("primary key") != String::npos;
    col.unique = to_lower(rest).find("unique") != String::npos;

    std::regex def_re(R"(DEFAULT\s*(?:ON\s+NULL\s*)?(\([^)]+\)|\S+))", std::regex_constants::icase);
    std::smatch def_match;
    if (std::regex_search(rest, def_match, def_re)) {
        String val = def_match[1].str();
        if (val.front() == '(' && val.back() == ')') {
            val = val.substr(1, val.size() - 2);
        }
        col.default_value = val;
    }

    return col;
}

int find_matching_paren(const String& s, size_t start) {
    int depth = 0;
    for (size_t i = start; i < s.size(); ++i) {
        if (s[i] == '(') depth++;
        else if (s[i] == ')') {
            depth--;
            if (depth == 0) return static_cast<int>(i);
        }
    }
    return -1;
}

Table* find_table(std::vector<Table>& tables, const String& name) {
    String key = to_lower(trim_chars(name, "\"'`[]"));
    for (auto& t : tables) {
        if (to_lower(trim_chars(t.name, "\"'`[]")) == key) {
            return &t;
        }
    }
    return nullptr;
}

void parse_alter_table(std::vector<Table>& tables, const String& sql) {
    std::regex tbl_re(R"(ALTER\s+TABLE\s+["`\[]?([\w\s]+?)["`\]]?\s+ADD\b)",
                      std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_search(sql, match, tbl_re)) {
        return;
    }

    Table* table = find_table(tables, match[1].str());
    if (!table) {
        return;
    }

    String upper = to_upper(sql);
    if (upper.find("PRIMARY KEY") != String::npos) {
        for (const auto& pk : extract_paren_names(sql)) {
            for (auto& col : table->columns) {
                if (to_lower(trim_chars(col.name, "\"'`[]")) == to_lower(trim_chars(pk, "\"'`[]"))) {
                    col.primary_key = true;
                    break;
                }
            }
        }
    } else if (upper.find("FOREIGN KEY") != String::npos) {
        if (auto fk = parse_fk(sql)) {
            table->foreign_keys.push_back(*fk);
        }
    }
}

std::optional<Table> parse_create_table(const String& sql) {
    std::regex table_re(R"(CREATE\s+(?:OR\s+REPLACE\s+)?TABLE\s+(?:\[?\w+\]?\.)?["`\[]?([\w\s]+?)["`\]]?\s*(?:UNDER\s+\w+)?\s*\()",
                        std::regex_constants::icase);
    std::smatch match;

    if (!std::regex_search(sql, match, table_re)) {
        return std::nullopt;
    }

    Table table;
    table.name = trim(match[1].str());

    size_t paren_start = sql.find('(');
    if (paren_start == String::npos) {
        return table;
    }

    int paren_end = find_matching_paren(sql, paren_start);
    if (paren_end == -1) {
        return table;
    }

    String body = sql.substr(paren_start + 1, paren_end - paren_start - 1);
    auto lines = split_column_lines(body);

    std::vector<String> pk_columns;

    for (size_t i = 0; i < lines.size(); ++i) {
        String line = trim(lines[i]);
        String upper = to_upper(line);

        if (line.empty()) continue;

        if (upper.rfind("CONSTRAINT", 0) == 0) {
            if (upper.find("PRIMARY KEY") != String::npos) {
                auto cols = extract_paren_names(line);
                pk_columns.insert(pk_columns.end(), cols.begin(), cols.end());
            } else if (upper.find("FOREIGN KEY") != String::npos) {
                if (auto fk = parse_fk(line)) {
                    table.foreign_keys.push_back(*fk);
                }
            } else if (upper.find("CHECK") != String::npos) {
            }
            continue;
        }

        if (upper.rfind("USING INDEX", 0) == 0 ||
            upper.rfind("ORGANIZATION INDEX", 0) == 0 ||
            upper.rfind("TABLESPACE", 0) == 0 ||
            upper.rfind("STORAGE", 0) == 0 ||
            upper.rfind("PCTFREE", 0) == 0 ||
            upper.rfind("INITRANS", 0) == 0 ||
            upper.rfind("MAXTRANS", 0) == 0 ||
            upper.rfind("LOGGING", 0) == 0 ||
            upper.rfind("NOLOGGING", 0) == 0 ||
            upper.rfind("COMPRESS", 0) == 0 ||
            upper.rfind("NOCOMPRESS", 0) == 0 ||
            upper.rfind("CACHE", 0) == 0 ||
            upper.rfind("NOCACHE", 0) == 0 ||
            upper.rfind("RESULT_CACHE", 0) == 0 ||
            upper.rfind("SCHEMA", 0) == 0 ||
            upper.rfind("LOCATION", 0) == 0 ||
            upper.rfind("ENGINE", 0) == 0 ||
            upper.rfind("CHARSET", 0) == 0 ||
            upper.rfind("COLLATE", 0) == 0 ||
            upper.rfind("INDEX", 0) == 0 ||
            upper.rfind("PRIMARY KEY", 0) == 0 ||
            upper.rfind("FOREIGN KEY", 0) == 0 ||
            upper.rfind("UNIQUE", 0) == 0 ||
            upper.rfind("CHECK", 0) == 0 ||
            upper.rfind("KEY", 0) == 0) {
            continue;
        }

        if (upper.rfind("PRIMARY KEY", 0) == 0) {
            auto cols = extract_paren_names(line);
            pk_columns.insert(pk_columns.end(), cols.begin(), cols.end());
            continue;
        }

        if (upper.rfind("FOREIGN KEY", 0) == 0) {
            if (auto fk = parse_fk(line)) {
                table.foreign_keys.push_back(*fk);
            }
            continue;
        }

        if (upper.rfind("UNIQUE", 0) == 0 ||
            upper.rfind("INDEX", 0) == 0 ||
            upper.rfind("KEY", 0) == 0 ||
            upper.rfind("CHECK", 0) == 0 ||
            upper.find("CREATE INDEX") != String::npos) {
            continue;
        }

        if (auto col = parse_column_line(line)) {
            table.columns.push_back(*col);
        }
    }

    for (const auto& pk : pk_columns) {
        for (auto& col : table.columns) {
            if (to_lower(trim_chars(col.name, "\"'`[]")) == to_lower(trim_chars(pk, "\"'`[]"))) {
                col.primary_key = true;
                break;
            }
        }
    }

    return table;
}

void resolve_foreign_key_types(std::vector<Table>& tables) {
    std::unordered_map<String, Table*> table_map;
    for (auto& t : tables) {
        table_map[to_lower(t.name)] = &t;
    }

    for (auto& t : tables) {
        for (auto& fk : t.foreign_keys) {
            auto it = table_map.find(to_lower(fk.ref_table));
            if (it != table_map.end() && it->second->pk_column()) {
                for (auto& col : t.columns) {
                    if (to_lower(col.name) == to_lower(fk.column)) {
                        col.sql_type = it->second->pk_column()->sql_type;
                        break;
                    }
                }
            }
        }
    }
}

std::vector<Table> parse_tables(const String& sql) {
    std::vector<Table> tables;
    auto stmts = split_statements(sql);

    for (const auto& stmt : stmts) {
        String upper = to_upper(trim(stmt));
        if (upper.rfind("CREATE TABLE", 0) == 0 ||
            upper.rfind("CREATE OR REPLACE TABLE", 0) == 0) {
            if (auto table = parse_create_table(stmt)) {
                tables.push_back(*table);
            }
        } else if (upper.rfind("ALTER TABLE", 0) == 0) {
            parse_alter_table(tables, stmt);
        }
    }

    resolve_foreign_key_types(tables);
    return tables;
}

Strings parse_sql(const String& sql) {
    return split_statements(sql);
}

} // namespace novagen
