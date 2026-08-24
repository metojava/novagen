#include "generator.hpp"
#include <sstream>

namespace novagen {

static String java_type_from_sql(const String& sql_type) {
    String upper = sql_type;
    for (auto& c : upper) c = toupper(c);

    if (upper.find("INT") != String::npos) return "Integer";
    if (upper.find("BIGINT") != String::npos) return "Long";
    if (upper.find("SMALLINT") != String::npos) return "Short";
    if (upper.find("TINYINT") != String::npos) return "Byte";
    if (upper.find("BIT") != String::npos || upper == "BOOLEAN") return "Boolean";
    if (upper.find("DECIMAL") != String::npos || upper.find("NUMERIC") != String::npos ||
        upper.find("MONEY") != String::npos || upper.find("FLOAT") != String::npos ||
        upper.find("DOUBLE") != String::npos || upper.find("REAL") != String::npos) return "java.math.BigDecimal";
    if (upper.find("VARCHAR") != String::npos || upper.find("CHAR") != String::npos ||
        upper.find("TEXT") != String::npos || upper == "STRING") return "String";
    if (upper.find("DATE") != String::npos) return "java.time.LocalDate";
    if (upper.find("TIME") != String::npos) return "java.time.LocalTime";
    if (upper.find("DATETIME") != String::npos || upper.find("TIMESTAMP") != String::npos) return "java.time.LocalDateTime";
    if (upper.find("BLOB") != String::npos || upper.find("BINARY") != String::npos) return "byte[]";
    return "String";
}

static String liquibase_type(const Column& col) {
    String t = col.sql_type;
    for (auto& c : t) c = toupper(c);

    if (t.find("INT") != String::npos) return "INT";
    if (t.find("BIGINT") != String::npos) return "BIGINT";
    if (t.find("SMALLINT") != String::npos) return "SMALLINT";
    if (t.find("TINYINT") != String::npos) return "TINYINT";
    if (t.find("BIT") != String::npos || t == "BOOLEAN") return "BOOLEAN";
    if (t.find("DECIMAL") != String::npos || t.find("NUMERIC") != String::npos) {
        if (col.precision.has_value()) {
            if (col.scale.has_value()) {
                return "DECIMAL(" + std::to_string(*col.precision) + "," + std::to_string(*col.scale) + ")";
            }
            return "DECIMAL(" + std::to_string(*col.precision) + ")";
        }
        return "DECIMAL";
    }
    if (t.find("MONEY") != String::npos) return "DECIMAL(19,4)";
    if (t.find("FLOAT") != String::npos || t.find("DOUBLE") != String::npos || t.find("REAL") != String::npos) return "DOUBLE";
    if (t.find("VARCHAR") != String::npos || t.find("CHAR") != String::npos) {
        if (col.length.has_value()) return "VARCHAR(" + std::to_string(*col.length) + ")";
        return "VARCHAR(255)";
    }
    if (t.find("TEXT") != String::npos) return "CLOB";
    if (t.find("DATE") != String::npos) return "DATE";
    if (t.find("TIME") != String::npos) return "TIME";
    if (t.find("DATETIME") != String::npos || t.find("TIMESTAMP") != String::npos) return "TIMESTAMP";
    if (t.find("BLOB") != String::npos || t.find("BINARY") != String::npos) return "BLOB";
    return "VARCHAR(255)";
}

String Generator::generate_springboot_pom_xml(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>
    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>3.2.0</version>
    </parent>
    <groupId>)" << namespace_ << R"(</groupId>
    <artifactId>)" << project_name_ << R"(</artifactId>
    <version>1.0.0</version>
    <name>)" << project_name_ << R"(</name>
    <properties>
        <java.version>17</java.version>
    </properties>
    <dependencies>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-data-jpa</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-web</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-validation</artifactId>
        </dependency>
        <dependency>
            <groupId>org.projectlombok</groupId>
            <artifactId>lombok</artifactId>
            <optional>true</optional>
        </dependency>
        <dependency>
            <groupId>com.h2database</groupId>
            <artifactId>h2</artifactId>
            <scope>runtime</scope>
        </dependency>
        <dependency>
            <groupId>org.liquibase</groupId>
            <artifactId>liquibase-core</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-test</artifactId>
            <scope>test</scope>
        </dependency>
    </dependencies>
    <build>
        <plugins>
            <plugin>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-maven-plugin</artifactId>
                <configuration>
                    <excludes>
                        <exclude>
                            <groupId>org.projectlombok</groupId>
                            <artifactId>lombok</artifactId>
                        </exclude>
                    </excludes>
                </configuration>
            </plugin>
        </plugins>
    </build>
</project>)";
    return ss.str();
}

String Generator::generate_springboot_application_properties(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(spring:
  datasource:
    url: jdbc:h2:mem:)" << project_name_ << R"(;DB_CLOSE_DELAY=-1;DB_CLOSE_ON_EXIT=FALSE
    driver-class-name: org.h2.Driver
    username: sa
    password: 
  jpa:
    hibernate:
      ddl-auto: none
    show-sql: true
    properties:
      hibernate:
        format_sql: true
        dialect: org.hibernate.dialect.H2Dialect
  liquibase:
    change-log: classpath:db/changelog/db.changelog-master.yaml
    enabled: true
  h2:
    console:
      enabled: true
      path: /h2-console

server:
  port: 8080

logging:
  level:
    root: INFO
    )" << namespace_ << R"(: DEBUG
    org.hibernate.SQL: DEBUG)";
    return ss.str();
}

String Generator::generate_springboot_application_test_properties(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(spring:
  datasource:
    url: jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1;DB_CLOSE_ON_EXIT=FALSE
    driver-class-name: org.h2.Driver
    username: sa
    password: 
  jpa:
    hibernate:
      ddl-auto: create-drop
    show-sql: false
  liquibase:
    enabled: false

server:
  port: 0

logging:
  level:
    root: WARN)";
    return ss.str();
}

String Generator::generate_springboot_app_java(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "package " << namespace_ << ";\n\n";
    ss << "import org.springframework.boot.SpringApplication;\n";
    ss << "import org.springframework.boot.autoconfigure.SpringBootApplication;\n";
    ss << "import org.springframework.boot.context.properties.ConfigurationPropertiesScan;\n\n";
    ss << "@SpringBootApplication\n";
    ss << "@ConfigurationPropertiesScan\n";
    ss << "public class Application {\n";
    ss << "    public static void main(String[] args) {\n";
    ss << "        SpringApplication.run(Application.class, args);\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_entity_java(const Table& table) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".entity;\n\n";

    ss << "import jakarta.persistence.*;\n";
    ss << "import lombok.*;\n\n";

    bool hasNotNull = false;
    for (const auto& col : table.columns) {
        if (!col.nullable && !col.primary_key) {
            hasNotNull = true;
            break;
        }
    }
    if (hasNotNull) {
        ss << "import jakarta.validation.constraints.NotNull;\n";
    }

    ss << "\n@Data\n";
    ss << "@NoArgsConstructor\n";
    ss << "@AllArgsConstructor\n";
    ss << "@Builder\n";
    if (table.name != to_pascal(table.name)) {
        ss << "@Table(name = \"" << table.name << "\")\n";
    }
    ss << "@Entity\n";
    ss << "public class " << table.class_name() << " {\n\n";

    for (const auto& col : table.columns) {
        bool isFK = false;
        for (const auto& fk : table.foreign_keys) {
            if (fk.column == col.name) {
                isFK = true;
                break;
            }
        }

        if (col.primary_key) {
            ss << "    @Id\n";
            if (col.auto_increment) {
                ss << "    @GeneratedValue(strategy = GenerationType.IDENTITY)\n";
            }
        }

        if (isFK) {
            ss << "    @ManyToOne(fetch = FetchType.LAZY)\n";
            ss << "    @JoinColumn(name = \"" << col.name << "\"";
            if (!col.nullable) ss << ", nullable = false";
            ss << ")\n";
        } else {
            if (!col.nullable && !col.primary_key) {
                ss << "    @NotNull\n";
            }

            String colAnnot = "    @Column";
            bool hasAnnot = false;
            if (col.name != to_camel(col.name)) {
                colAnnot += "(name = \"" + col.name + "\"";
                hasAnnot = true;
            }
            if (!col.nullable && !col.primary_key) {
                if (!hasAnnot) colAnnot += "(nullable = false";
                else colAnnot += ", nullable = false";
                hasAnnot = true;
            }
            if (col.unique) {
                if (!hasAnnot) colAnnot += "(unique = true";
                else colAnnot += ", unique = true";
                hasAnnot = true;
            }
            if (hasAnnot) colAnnot += ")";
            if (hasAnnot) ss << colAnnot << "\n";
        }

        ss << "    private " << java_type_from_sql(col.sql_type) << " " << col.field_name() << ";\n\n";
    }

    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_repository_java(const Table& table) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".repository;\n\n";
    ss << "import " << namespace_ << ".entity." << table.class_name() << ";\n";
    ss << "import org.springframework.data.jpa.repository.JpaRepository;\n";
    ss << "import org.springframework.stereotype.Repository;\n\n";
    ss << "@Repository\n";
    ss << "public interface " << table.class_name() << "Repository extends JpaRepository<" << table.class_name() << ", "
       << java_type_from_sql(table.pk_column() ? table.pk_column()->sql_type : "INT") << "> {\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_service_java(const Table& table) {
    std::stringstream ss;
    String cn = table.class_name();
    String n = table.instance_name();
    String pkType = java_type_from_sql(table.pk_column() ? table.pk_column()->sql_type : "Integer");

    ss << "package " << namespace_ << ".service;\n\n";
    ss << "import " << namespace_ << ".entity." << cn << ";\n";
    ss << "import " << namespace_ << ".repository." << cn << "Repository;\n";
    ss << "import org.springframework.stereotype.Service;\n";
    ss << "import org.springframework.transaction.annotation.Transactional;\n\n";
    ss << "import java.util.List;\n\n";
    ss << "@Service\n";
    ss << "@Transactional\n";
    ss << "public class " << cn << "Service {\n\n";
    ss << "    private final " << cn << "Repository " << n << "Repository;\n\n";
    ss << "    public " << cn << "Service(" << cn << "Repository " << n << "Repository) {\n";
    ss << "        this." << n << "Repository = " << n << "Repository;\n";
    ss << "    }\n\n";
    ss << "    public List<" << cn << "> findAll() {\n";
    ss << "        return " << n << "Repository.findAll();\n";
    ss << "    }\n\n";
    ss << "    public " << cn << " findById(" << pkType << " id) {\n";
    ss << "        return " << n << "Repository.findById(id)\n";
    ss << "                .orElseThrow(() -> new RuntimeException(\"" << cn << " not found with id: \" + id));\n";
    ss << "    }\n\n";
    ss << "    public " << cn << " save(" << cn << " " << n << ") {\n";
    ss << "        return " << n << "Repository.save(" << n << ");\n";
    ss << "    }\n\n";
    ss << "    public " << cn << " update(" << pkType << " id, " << cn << " " << n << ") {\n";
    ss << "        " << cn << " existing = findById(id);\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) continue;
        bool isFK = false;
        for (const auto& fk : table.foreign_keys) {
            if (fk.column == col.name) { isFK = true; break; }
        }
        if (!isFK) {
            ss << "        if (" << n << ".get" << to_pascal(col.field_name()) << "() != null) {\n";
            ss << "            existing.set" << to_pascal(col.field_name()) << "(" << n << ".get" << to_pascal(col.field_name()) << "());\n";
            ss << "        }\n";
        }
    }
    ss << "        return " << n << "Repository.save(existing);\n";
    ss << "    }\n\n";
    ss << "    public void delete(" << pkType << " id) {\n";
    ss << "        " << n << "Repository.deleteById(id);\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_controller_java(const Table& table) {
    std::stringstream ss;
    String cn = table.class_name();
    String n = table.instance_name();
    String pkType = java_type_from_sql(table.pk_column() ? table.pk_column()->sql_type : "Integer");
    String path = table.kebab_name();

    ss << "package " << namespace_ << ".controller;\n\n";
    ss << "import " << namespace_ << ".entity." << cn << ";\n";
    ss << "import " << namespace_ << ".service." << cn << "Service;\n";
    ss << "import org.springframework.http.HttpStatus;\n";
    ss << "import org.springframework.http.ResponseEntity;\n";
    ss << "import org.springframework.web.bind.annotation.*;\n\n";
    ss << "import java.util.List;\n\n";
    ss << "@RestController\n";
    ss << "@RequestMapping(\"/api/" << path << "\")\n";
    ss << "public class " << cn << "Controller {\n\n";
    ss << "    private final " << cn << "Service " << n << "Service;\n\n";
    ss << "    public " << cn << "Controller(" << cn << "Service " << n << "Service) {\n";
    ss << "        this." << n << "Service = " << n << "Service;\n";
    ss << "    }\n\n";
    ss << "    @GetMapping\n";
    ss << "    public ResponseEntity<List<" << cn << ">> getAll() {\n";
    ss << "        return ResponseEntity.ok(" << n << "Service.findAll());\n";
    ss << "    }\n\n";
    ss << "    @GetMapping(\"/{id}\")\n";
    ss << "    public ResponseEntity<" << cn << "> getById(@PathVariable " << pkType << " id) {\n";
    ss << "        return ResponseEntity.ok(" << n << "Service.findById(id));\n";
    ss << "    }\n\n";
    ss << "    @PostMapping\n";
    ss << "    public ResponseEntity<" << cn << "> create(@RequestBody " << cn << " " << n << ") {\n";
    ss << "        " << cn << " saved = " << n << "Service.save(" << n << ");\n";
    ss << "        return ResponseEntity.status(HttpStatus.CREATED).body(saved);\n";
    ss << "    }\n\n";
    ss << "    @PutMapping(\"/{id}\")\n";
    ss << "    public ResponseEntity<" << cn << "> update(@PathVariable " << pkType << " id, @RequestBody " << cn << " " << n << ") {\n";
    ss << "        return ResponseEntity.ok(" << n << "Service.update(id, " << n << "));\n";
    ss << "    }\n\n";
    ss << "    @DeleteMapping(\"/{id}\")\n";
    ss << "    public ResponseEntity<Void> delete(@PathVariable " << pkType << " id) {\n";
    ss << "        " << n << "Service.delete(id);\n";
    ss << "        return ResponseEntity.noContent().build();\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_service_test_java(const Table& table) {
    std::stringstream ss;
    String cn = table.class_name();
    String n = table.instance_name();
    String pkType = java_type_from_sql(table.pk_column() ? table.pk_column()->sql_type : "Integer");

    ss << "package " << namespace_ << ".service;\n\n";
    ss << "import " << namespace_ << ".entity." << cn << ";\n";
    ss << "import " << namespace_ << ".repository." << cn << "Repository;\n";
    ss << "import org.junit.jupiter.api.Test;\n";
    ss << "import org.junit.jupiter.api.extension.ExtendWith;\n";
    ss << "import org.mockito.InjectMocks;\n";
    ss << "import org.mockito.Mock;\n";
    ss << "import org.mockito.junit.jupiter.MockitoExtension;\n\n";
    ss << "import java.util.Optional;\n";
    ss << "import java.util.List;\n\n";
    ss << "import static org.junit.jupiter.api.Assertions.*;\n";
    ss << "import static org.mockito.Mockito.*;\n\n";
    ss << "@ExtendWith(MockitoExtension.class)\n";
    ss << "public class " << cn << "ServiceTest {\n\n";
    ss << "    @Mock\n";
    ss << "    private " << cn << "Repository " << n << "Repository;\n\n";
    ss << "    @InjectMocks\n";
    ss << "    private " << cn << "Service " << n << "Service;\n\n";
    ss << "    @Test\n";
    ss << "    void findAll_ShouldReturnList() {\n";
    ss << "        when(" << n << "Repository.findAll()).thenReturn(List.of(new " << cn << "()));\n";
    ss << "        assertFalse(" << n << "Service.findAll().isEmpty());\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void findById_WhenExists_ShouldReturnEntity() {\n";
    ss << "        when(" << n << "Repository.findById(any())).thenReturn(Optional.of(new " << cn << "()));\n";
    ss << "        assertNotNull(" << n << "Service.findById(1));\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void findById_WhenNotExists_ShouldThrow() {\n";
    ss << "        when(" << n << "Repository.findById(1)).thenReturn(Optional.empty());\n";
    ss << "        assertThrows(RuntimeException.class, () -> " << n << "Service.findById(1));\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void save_ShouldPersist() {\n";
    ss << "        " << cn << " entity = new " << cn << "();\n";
    ss << "        when(" << n << "Repository.save(entity)).thenReturn(entity);\n";
    ss << "        assertNotNull(" << n << "Service.save(entity));\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void delete_ShouldRemove() {\n";
    ss << "        doNothing().when(" << n << "Repository).deleteById(1);\n";
    ss << "        " << n << "Service.delete(1);\n";
    ss << "        verify(" << n << "Repository, times(1)).deleteById(1);\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_controller_test_java(const Table& table) {
    std::stringstream ss;
    String cn = table.class_name();
    String n = table.instance_name();
    String pkType = java_type_from_sql(table.pk_column() ? table.pk_column()->sql_type : "Integer");
    String path = table.kebab_name();

    ss << "package " << namespace_ << ".controller;\n\n";
    ss << "import " << namespace_ << ".entity." << cn << ";\n";
    ss << "import " << namespace_ << ".service." << cn << "Service;\n";
    ss << "import com.fasterxml.jackson.databind.ObjectMapper;\n";
    ss << "import org.junit.jupiter.api.Test;\n";
    ss << "import org.springframework.beans.factory.annotation.Autowired;\n";
    ss << "import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;\n";
    ss << "import org.springframework.boot.test.mock.bean.MockBean;\n";
    ss << "import org.springframework.http.MediaType;\n";
    ss << "import org.springframework.test.web.servlet.MockMvc;\n\n";
    ss << "import java.util.List;\n\n";
    ss << "import static org.mockito.Mockito.*;\n";
    ss << "import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.*;\n";
    ss << "import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;\n\n";
    ss << "@WebMvcTest(" << cn << "Controller.class)\n";
    ss << "public class " << cn << "ControllerTest {\n\n";
    ss << "    @Autowired\n";
    ss << "    private MockMvc mockMvc;\n\n";
    ss << "    @MockBean\n";
    ss << "    private " << cn << "Service " << n << "Service;\n\n";
    ss << "    @Autowired\n";
    ss << "    private ObjectMapper objectMapper;\n\n";
    ss << "    @Test\n";
    ss << "    void getAll_ShouldReturn200() throws Exception {\n";
    ss << "        when(" << n << "Service.findAll()).thenReturn(List.of());\n";
    ss << "        mockMvc.perform(get(\"/api/" << path << "\"))\n";
    ss << "                .andExpect(status().isOk());\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void getById_WhenExists_ShouldReturn200() throws Exception {\n";
    ss << "        " << cn << " entity = new " << cn << "();\n";
    ss << "        when(" << n << "Service.findById(1)).thenReturn(entity);\n";
    ss << "        mockMvc.perform(get(\"/api/" << path << "/1\"))\n";
    ss << "                .andExpect(status().isOk());\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void create_ShouldReturn201() throws Exception {\n";
    ss << "        " << cn << " entity = new " << cn << "();\n";
    ss << "        when(" << n << "Service.save(any())).thenReturn(entity);\n";
    ss << "        mockMvc.perform(post(\"/api/" << path << "\")\n";
    ss << "                .contentType(MediaType.APPLICATION_JSON)\n";
    ss << "                .content(objectMapper.writeValueAsString(new " << cn << "())))\n";
    ss << "                .andExpect(status().isCreated());\n";
    ss << "    }\n\n";
    ss << "    @Test\n";
    ss << "    void delete_ShouldReturn204() throws Exception {\n";
    ss << "        doNothing().when(" << n << "Service).delete(1);\n";
    ss << "        mockMvc.perform(delete(\"/api/" << path << "/1\"))\n";
    ss << "                .andExpect(status().isNoContent());\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_springboot_liquibase_changelog(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "databaseChangeLog:\n";
    ss << "  - include:\n";
    ss << "      file: db/changelog/001-initial-schema.yaml\n";
    return ss.str();
}

String Generator::generate_springboot_dockerfile(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(FROM eclipse-temurin:17-jdk-alpine AS build
WORKDIR /app
COPY pom.xml .
COPY src ./src
RUN apk add --no-cache maven && \
    mvn clean package -DskipTests

FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
COPY --from=build /app/target/*.jar app.jar
EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"])";
    return ss.str();
}

String Generator::generate_springboot_docker_compose_yml(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(version: '3.8'
services:
  app:
    build: .
    ports:
      - "8080:8080"
    environment:
      - SPRING_PROFILES_ACTIVE=prod
    networks:
      - app-network

networks:
  app-network:
    driver: bridge)";
    return ss.str();
}

String Generator::generate_springboot_gitignore() {
    return R"(# Maven
target/
pom.xml.tag
pom.xml.releaseBackup
pom.xml.versionsBackup
pom.xml.next
release.properties

# IDE
.idea/
*.iml
.vscode/
.settings/
.project
.classpath

# Compiled
*.class

# Log
*.log

# OS
.DS_Store
Thumbs.db)";
}

String Generator::generate_springboot_readme(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "# " << project_name_ << " - Spring Boot REST API\n\n";
    ss << "Generated by NovaGen v1.0.0\n\n";
    ss << "## Requirements\n\n";
    ss << "- Java 17+\n";
    ss << "- Maven 3.8+\n\n";
    ss << "## Build\n\n";
    ss << "```bash\n";
    ss << "mvn clean package\n";
    ss << "```\n\n";
    ss << "## Run\n\n";
    ss << "```bash\n";
    ss << "mvn spring-boot:run\n";
    ss << "# or\n";
    ss << "java -jar target/" << project_name_ << "-1.0.0.jar\n";
    ss << "```\n\n";
    ss << "## API Endpoints\n\n";
    for (const auto& table : tables) {
        ss << "- **" << table.class_name() << "**: `GET/POST /api/" << table.kebab_name() << "`\n";
        ss << "  - `GET /api/" << table.kebab_name() << "/{id}`\n";
        ss << "  - `PUT /api/" << table.kebab_name() << "/{id}`\n";
        ss << "  - `DELETE /api/" << table.kebab_name() << "/{id}`\n\n";
    }
    ss << "## H2 Console\n\n";
    ss << "Available at http://localhost:8080/h2-console\n";
    ss << "- JDBC URL: `jdbc:h2:mem:" << project_name_ << "`\n";
    ss << "- Username: `sa`\n";
    ss << "- Password: (empty)\n\n";
    ss << "## Docker\n\n";
    ss << "```bash\n";
    ss << "docker-compose up --build\n";
    ss << "```\n";
    return ss.str();
}

String Generator::generate_security_jwt_springboot() {
    std::stringstream ss;
    ss << R"(
package com.example.app.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;
import io.jsonwebtoken.*;
import io.jsonwebtoken.security.Keys;
import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.Key;
import java.util.Date;
import java.util.List;

@Configuration
@EnableWebSecurity
public class SecurityConfig {

    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http, JwtAuthenticationFilter jwtFilter) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**", "/health", "/api").permitAll()
                .anyRequest().authenticated()
            )
            .addFilterBefore(jwtFilter, UsernamePasswordAuthenticationFilter.class);

        return http.build();
    }
}

@Component
public class JwtAuthenticationFilter extends OncePerRequestFilter {

    private static final String SECRET_KEY = System.getenv("JWT_SECRET") != null ?
        System.getenv("JWT_SECRET") : "dev-secret-key-change-in-production-min-32-chars";
    private static final long EXPIRATION_TIME = 86400000; // 24 hours

    private Key getSigningKey() {
        return Keys.hmacShaKeyFor(SECRET_KEY.getBytes(StandardCharsets.UTF_8));
    }

    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response,
                                    FilterChain filterChain) throws ServletException, IOException {
        String authHeader = request.getHeader("Authorization");

        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }

        String token = authHeader.substring(7);

        try {
            Claims claims = Jwts.parserBuilder()
                .setSigningKey(getSigningKey())
                .build()
                .parseClaimsJws(token)
                .getBody();

            String userId = claims.getSubject();
            String role = claims.get("role", String.class);

            List<org.springframework.security.core.authority.SimpleGrantedAuthority> authorities =
                List.of(new org.springframework.security.core.authority.SimpleGrantedAuthority("ROLE_" + role));

            org.springframework.security.core.context.SecurityContextHolder.getContext().setAuthentication(
                new org.springframework.security.authentication.UsernamePasswordAuthenticationToken(userId, null, authorities)
            );

        } catch (JwtException e) {
            response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
            response.getWriter().write("{\"error\": \"Invalid or expired token\"}");
            return;
        }

        filterChain.doFilter(request, response);
    }
}

class JwtTokenProvider {

    private static final String SECRET_KEY = System.getenv("JWT_SECRET") != null ?
        System.getenv("JWT_SECRET") : "dev-secret-key-change-in-production-min-32-chars";
    private static final long EXPIRATION_TIME = 86400000;

    private Key getSigningKey() {
        return Keys.hmacShaKeyFor(SECRET_KEY.getBytes(StandardCharsets.UTF_8));
    }

    public String generateToken(String userId, String role) {
        Date now = new Date();
        Date expiryDate = new Date(now.getTime() + EXPIRATION_TIME);

        return Jwts.builder()
            .setSubject(userId)
            .claim("role", role)
            .setIssuedAt(now)
            .setExpiration(expiryDate)
            .signWith(getSigningKey(), SignatureAlgorithm.HS256)
            .compact();
    }

    public boolean validateToken(String token) {
        try {
            Jwts.parserBuilder().setSigningKey(getSigningKey()).build().parseClaimsJws(token);
            return true;
        } catch (JwtException | IllegalArgumentException e) {
            return false;
        }
    }

    public String getUserIdFromToken(String token) {
        Claims claims = Jwts.parserBuilder()
            .setSigningKey(getSigningKey())
            .build()
            .parseClaimsJws(token)
            .getBody();
        return claims.getSubject();
    }
}
)";
    return ss.str();
}

String Generator::generate_security_oauth2_springboot() {
    std::stringstream ss;
    ss << R"(
package com.example.app.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.oauth2.client.registration.ClientRegistration;
import org.springframework.security.oauth2.client.registration.ClientRegistrationRepository;
import org.springframework.security.oauth2.client.registration.InMemoryClientRegistrationRepository;
import org.springframework.security.oauth2.core.AuthorizationGrantType;
import org.springframework.security.oauth2.core.ClientAuthenticationMethod;

@Configuration
@EnableWebSecurity
public class OAuth2SecurityConfig {

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/oauth2/**", "/auth/**", "/health", "/api").permitAll()
                .anyRequest().authenticated()
            )
            .oauth2Login(oauth2 -> oauth2
                .authorizationEndpoint(authorization -> authorization
                    .baseUri("/oauth2/authorization")
                )
                .redirectionEndpoint(redirection -> redirection
                    .baseUri("/login/oauth2/code/*")
                )
            );

        return http.build();
    }

    @Bean
    public ClientRegistrationRepository clientRegistrationRepository() {
        return new InMemoryClientRegistrationRepository(
            githubClientRegistration(),
            googleClientRegistration()
        );
    }

    private ClientRegistration githubClientRegistration() {
        return ClientRegistration.withRegistrationId("github")
            .clientId(System.getenv("GITHUB_CLIENT_ID") != null ?
                System.getenv("GITHUB_CLIENT_ID") : "your-github-client-id")
            .clientSecret(System.getenv("GITHUB_CLIENT_SECRET") != null ?
                System.getenv("GITHUB_CLIENT_SECRET") : "your-github-client-secret")
            .clientAuthenticationMethod(ClientAuthenticationMethod.CLIENT_SECRET_BASIC)
            .authorizationGrantType(AuthorizationGrantType.AUTHORIZATION_CODE)
            .redirectUri("{baseUrl}/login/oauth2/code/github")
            .scope("read:user", "user:email")
            .authorizationUri("https://github.com/login/oauth2/authorize")
            .tokenUri("https://github.com/login/oauth2/access_token")
            .userInfoUri("https://api.github.com/user")
            .userNameAttributeName("id")
            .build();
    }

    private ClientRegistration googleClientRegistration() {
        return ClientRegistration.withRegistrationId("google")
            .clientId(System.getenv("GOOGLE_CLIENT_ID") != null ?
                System.getenv("GOOGLE_CLIENT_ID") : "your-google-client-id")
            .clientSecret(System.getenv("GOOGLE_CLIENT_SECRET") != null ?
                System.getenv("GOOGLE_CLIENT_SECRET") : "your-google-client-secret")
            .clientAuthenticationMethod(ClientAuthenticationMethod.CLIENT_SECRET_BASIC)
            .authorizationGrantType(AuthorizationGrantType.AUTHORIZATION_CODE)
            .redirectUri("{baseUrl}/login/oauth2/code/google")
            .scope("openid", "profile", "email")
            .authorizationUri("https://accounts.google.com/o/oauth2/v2/auth")
            .tokenUri("https://oauth2.googleapis.com/token")
            .userInfoUri("https://www.googleapis.com/oauth2/v3/userinfo")
            .userNameAttributeName("sub")
            .build();
    }
}
)";
    return ss.str();
}

String Generator::generate_security_saml_springboot() {
    std::stringstream ss;
    ss << R"(
package com.example.app.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.saml2.provider.service.registration.InMemoryRelyingPartyRegistrationRepository;
import org.springframework.security.saml2.provider.service.registration.RelyingPartyRegistration;
import org.springframework.security.saml2.provider.service.registration.RelyingPartyRegistrationRepository;
import org.springframework.security.saml2.provider.service.registration.RelyingPartyAssertingPartyDetails;
import org.springframework.security.saml2.provider.service.registration.Saml2MessageBinding;
import org.springframework.security.web.SecurityFilterChain;

@Configuration
@EnableWebSecurity
public class Saml2SecurityConfig {

    private static final String IDP_ENTITY_ID = System.getenv("SAML_IDP_ENTITY_ID") != null ?
        System.getenv("SAML_IDP_ENTITY_ID") : "https://idp.example.com/metadata";
    private static final String IDP_SSO_URL = System.getenv("SAML_IDP_SSO_URL") != null ?
        System.getenv("SAML_IDP_SSO_URL") : "https://idp.example.com/sso";
    private static final String SP_ENTITY_ID = "novagen-sp";

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/saml2/**", "/saml/**", "/health", "/api").permitAll()
                .anyRequest().authenticated()
            )
            .saml2Login(saml2 -> saml2
                .relyingPartyRegistrationRepository(relyingPartyRegistrationRepository())
            );

        return http.build();
    }

    @Bean
    public RelyingPartyRegistrationRepository relyingPartyRegistrationRepository() {
        RelyingPartyRegistration registration = RelyingPartyRegistrations
            .fromMetadataLocation(IDP_ENTITY_ID)
            .registrationId("saml")
            .entityId(SP_ENTITY_ID)
            .assertionConsumerServiceLocation("{baseUrl}/login/saml2/sso/{registrationId}")
            .build();

        return new InMemoryRelyingPartyRegistrationRepository(registration);
    }
}
)";
    return ss.str();
}

String Generator::generate_security_owasp_springboot() {
    std::stringstream ss;
    ss << R"OWASP(
package com.example.app.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.csrf.CookieCsrfTokenRepository;
import org.springframework.security.web.csrf.CsrfTokenRequestAttributeHandler;
import org.springframework.web.filter.OncePerRequestFilter;
import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;
import org.springframework.security.web.header.writers.ReferrerPolicyHeaderWriter;
import org.springframework.security.web.header.writers.XXssProtectionHeaderWriter;
import org.springframework.web.cors.CorsConfiguration;
import org.springframework.web.cors.CorsConfigurationSource;
import org.springframework.web.cors.UrlBasedCorsConfigurationSource;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Pattern;

@Configuration
@EnableWebSecurity
public class OWASPSecurityConfig {

    private static final List<Pattern> XSS_PATTERNS = Arrays.asList(
        Pattern.compile("<script[^>]*>.*?</script>", Pattern.CASE_INSENSITIVE | Pattern.DOTALL),
        Pattern.compile("javascript:", Pattern.CASE_INSENSITIVE),
        Pattern.compile("on\\w+\\s*=", Pattern.CASE_INSENSITIVE)
    );

    private static final List<Pattern> SQL_INJECTION_PATTERNS = Arrays.asList(
        Pattern.compile("\\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|CREATE|TRUNCATE|EXEC|EXECUTE)\\b", Pattern.CASE_INSENSITIVE),
        Pattern.compile("--"),
        Pattern.compile("/\\*.*\\*/")
    );

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http, InputValidationFilter inputFilter) throws Exception {
        CsrfTokenRequestAttributeHandler requestHandler = new CsrfTokenRequestAttributeHandler();
        requestHandler.setCsrfRequestAttributeName("_csrf");

        http
            .cors(cors -> cors.configurationSource(corsConfigurationSource()))
            .sessionManagement(session -> session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
            .csrf(csrf -> csrf
                .csrfTokenRepository(CookieCsrfTokenRepository.withHttpOnlyFalse())
                .csrfRequestHandler(requestHandler)
                .ignoringRequestMatchers("/api/**")
            )
            .addFilterBefore(inputFilter, UsernamePasswordAuthenticationFilter.class)
            .headers(headers -> headers
                .contentTypeOptions(content -> {})
                .xssProtection(xss -> xss.enable())
                .cacheControl(cache -> {})
                .frameOptions(frame -> frame.deny())
                .httpStrictTransportSecurity(hsts -> hsts
                    .includeSubDomains(true)
                    .maxAgeInSeconds(31536000)
                )
                .contentSecurityPolicy(csp -> csp
                    .policyDirectives("default-src 'self'")
                )
                .referrerPolicy(referrer -> referrer
                    .policy(ReferrerPolicyHeaderWriter.ReferrerPolicy.STRICT_ORIGIN_WHEN_CROSS_ORIGIN)
                )
                .permissionsPolicy(permissions -> permissions
                    .policy("geolocation=(), microphone=(), camera=()")
                )
            )
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/health", "/api").permitAll()
                .anyRequest().authenticated()
            );

        return http.build();
    }

    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        configuration.setAllowedOrigins(List.of("http://localhost:3000", "http://localhost:4200"));
        configuration.setAllowedMethods(Arrays.asList("GET", "POST", "PUT", "DELETE", "OPTIONS"));
        configuration.setAllowedHeaders(List.of("*"));
        configuration.setAllowCredentials(true);
        configuration.setMaxAge(3600L);

        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", configuration);
        return source;
    }
}

@Component
public class InputValidationFilter extends OncePerRequestFilter {

    private static final List<Pattern> XSS_PATTERNS = Arrays.asList(
        Pattern.compile("<script[^>]*>.*?</script>", Pattern.CASE_INSENSITIVE | Pattern.DOTALL),
        Pattern.compile("javascript:", Pattern.CASE_INSENSITIVE),
        Pattern.compile("on\\w+\\s*=", Pattern.CASE_INSENSITIVE)
    );

    private static final List<Pattern> SQL_INJECTION_PATTERNS = Arrays.asList(
        Pattern.compile("\\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|CREATE|TRUNCATE|EXEC|EXECUTE)\\b", Pattern.CASE_INSENSITIVE),
        Pattern.compile("--"),
        Pattern.compile("/\\*.*\\*/")
    );

    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response,
                                    FilterChain filterChain) throws ServletException, IOException {
        // Check XSS and SQL injection in parameters
        for (String key : request.getParameterMap().keySet()) {
            String[] values = request.getParameterMap().get(key);
            for (String value : values) {
                if (containsXSS(value) || containsSQLInjection(value)) {
                    response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
                    response.getWriter().write("{\"error\": \"Potentially malicious input detected\"}");
                    return;
                }
            }
        }

        filterChain.doFilter(request, response);
    }

    private boolean containsXSS(String input) {
        if (input == null) return false;
        for (Pattern pattern : XSS_PATTERNS) {
            if (pattern.matcher(input).find()) {
                return true;
            }
        }
        return false;
    }

    private boolean containsSQLInjection(String input) {
        if (input == null) return false;
        for (Pattern pattern : SQL_INJECTION_PATTERNS) {
            if (pattern.matcher(input).find()) {
                return true;
            }
        }
        return false;
    }
}
)OWASP";
    return ss.str();
}

String Generator::generate_security_ldap_springboot() {
    std::stringstream ss;
    ss << R"(
package com.example.app.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.ldap.authentication.ad.ActiveDirectoryLdapAuthenticationProvider;
import org.springframework.security.web.SecurityFilterChain;

@Configuration
@EnableWebSecurity
public class LdapSecurityConfig {

    private static final String LDAP_URL = System.getenv("LDAP_URL") != null ?
        System.getenv("LDAP_URL") : "ldap://localhost:389";
    private static final String LDAP_BASE_DN = System.getenv("LDAP_BASE_DN") != null ?
        System.getenv("LDAP_BASE_DN") : "dc=example,dc=com";
    private static final String LDAP_USER_DN_PATTERN = System.getenv("LDAP_USER_DN_PATTERN") != null ?
        System.getenv("LDAP_USER_DN_PATTERN") : "uid={0},ou=users";
    private static final String LDAP_GROUP_SEARCH_BASE = System.getenv("LDAP_GROUP_SEARCH_BASE") != null ?
        System.getenv("LDAP_GROUP_SEARCH_BASE") : "ou=groups";
    private static final String LDAP_REQUIRED_GROUP = System.getenv("LDAP_REQUIRED_GROUP");

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**", "/health", "/api").permitAll()
                .anyRequest().authenticated()
            )
            .httpBasic(basic -> basic.realmName("API"))
            .authenticationProvider(ldapAuthenticationProvider());

        return http.build();
    }

    @Bean
    public ActiveDirectoryLdapAuthenticationProvider ldapAuthenticationProvider() {
        ActiveDirectoryLdapAuthenticationProvider provider =
            new ActiveDirectoryLdapAuthenticationProvider(LDAP_BASE_DN, LDAP_URL);

        provider.setConvertSubErrorCodesToExceptions(true);
        provider.setUseAuthenticationRequestCredentials(true);

        if (LDAP_REQUIRED_GROUP != null && !LDAP_REQUIRED_GROUP.isEmpty()) {
            provider.setAuthoritiesMapper(authorities -> {
                // Custom authority mapping logic here
                return authorities;
            });
        }

        return provider;
    }
}
)";
    return ss.str();
}

} // namespace novagen
