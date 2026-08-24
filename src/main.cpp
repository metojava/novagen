#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>

#include "novagen/model.hpp"
#include "novagen/generator.hpp"

namespace fs = std::filesystem;

void print_version() {
    std::cout << "NovaGen v1.0.0 - REST API Generator\n";
}

void print_help(const char* program) {
    std::cout << "Usage: " << program << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input FILE      Input SQL DDL file\n";
    std::cout << "  -s, --sql STRING     SQL DDL as string (mutually exclusive with --input)\n";
    std::cout << "  -o, --output DIR     Output directory (default: ./output)\n";
    std::cout << "  -p, --project NAME   Project name (default: api_server)\n";
    std::cout << "  -n, --namespace NS   Namespace for routes/models (default: app)\n";
    std::cout << "  -f, --framework FR   Backend framework: crow, flask, fastapi, springboot (default: crow)\n";
    std::cout << "  --security SEC      Security: jwt, oauth2, saml, owasp, ldap, none (default: none)\n";
    std::cout << "  -r, --frontend FR   Frontend framework: react, angular, android (default: react)\n";
    std::cout << "\nReact Options:\n";
    std::cout << "  -b, --bundler BN     Bundler: vite, cra, webpack (React only, default: vite)\n";
    std::cout << "  -e, --react-framework FR  Language: typescript, javascript (default: typescript)\n";
    std::cout << "  -c, --css CSS        CSS: tailwind, bootstrap, plain (default: tailwind)\n";
    std::cout << "  -t, --theme THEME     Theme: light, dark, auto (default: light)\n";
    std::cout << "  -m, --state STATE    State: context, zustand, redux (default: context)\n";
    std::cout << "\nAngular Options:\n";
    std::cout << "  --standalone         Generate standalone components (default: true)\n";
    std::cout << "  --guards              Generate JWT auth guards (default: false)\n";
    std::cout << "  --admin               Generate admin pages/components (default: false)\n";
    std::cout << "  --angular-css CSS     CSS: tailwind, bootstrap, plain, material (default: tailwind)\n";
    std::cout << "  --angular-state STATE State: service, ngrx, redux, signals (default: service)\n";
    std::cout << "  --angular-http HTTP   HTTP: httpclient, fetch, axios (default: httpclient)\n";
    std::cout << "  --angular-bundler BN  Bundler: esbuild, webpack (default: esbuild)\n";
    std::cout << "\nAndroid Options:\n";
    std::cout << "  --android-lang LANG  Language: kotlin, java (default: kotlin)\n";
    std::cout << "  --android-arch ARCH  Architecture: mvvm, clean (default: mvvm)\n";
    std::cout << "\nOther:\n";
    std::cout << "  -l, --list-tables    List parsed tables and exit\n";
    std::cout << "  -v, --version        Show version\n";
    std::cout << "  -h, --help           Show this help\n";
    std::cout << "\nBackend Frameworks:\n";
    std::cout << "  crow       - C++ REST API using Crow framework (default)\n";
    std::cout << "  flask      - Python REST API using Flask\n";
    std::cout << "  fastapi    - Python REST API using FastAPI\n";
    std::cout << "  springboot - Java REST API using Spring Boot\n";
    std::cout << "\nORM Options (Flask/FastAPI):\n";
    std::cout << "  sqlalchemy - SQLAlchemy ORM (default)\n";
    std::cout << "  peewee     - Peewee ORM\n";
    std::cout << "  slack      - Slack ORM\n";
    std::cout << "\nFrontend Frameworks:\n";
    std::cout << "  react    - React 18 with Vite/CRA (default)\n";
    std::cout << "  angular  - Angular 17+ with standalone components\n";
    std::cout << "  android  - Android app with Retrofit/OkHttp/RxJava\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program << " --input schema.sql --project myapi\n";
    std::cout << "  " << program << " --input schema.sql --frontend react --project myapi\n";
    std::cout << "  " << program << " --input schema.sql --frontend angular --project myapi\n";
    std::cout << "  " << program << " --input schema.sql --frontend angular --guards --admin\n";
    std::cout << "  " << program << " --input schema.sql --frontend android --project myapi\n";
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string sql_string;
    std::string output_dir = "./output";
    std::string project_name = "api_server";
    std::string namespace_name = "app";
    std::string framework_str = "crow";
    std::string frontend_str = "react";
    std::string bundler = "vite";
    std::string react_framework = "typescript";
    std::string css = "tailwind";
    std::string theme = "light";
    std::string state = "context";
    bool angular_standalone = true;
    bool angular_guards = false;
    bool angular_admin = false;
    std::string angular_css = "tailwind";
    std::string angular_state = "service";
    std::string angular_http = "httpclient";
    std::string angular_bundler = "esbuild";
    std::string android_language = "kotlin";
    std::string android_architecture = "mvvm";
    std::string security = "none";
    std::string orm = "sqlalchemy";
    bool list_tables = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) {
                input_file = argv[++i];
            } else {
                std::cerr << "Error: --input requires a file path\n";
                return 1;
            }
        } else if (arg == "-s" || arg == "--sql") {
            if (i + 1 < argc) {
                sql_string = argv[++i];
            } else {
                std::cerr << "Error: --sql requires a string\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_dir = argv[++i];
            } else {
                std::cerr << "Error: --output requires a directory path\n";
                return 1;
            }
        } else if (arg == "-p" || arg == "--project") {
            if (i + 1 < argc) {
                project_name = argv[++i];
            } else {
                std::cerr << "Error: --project requires a name\n";
                return 1;
            }
        } else if (arg == "-n" || arg == "--namespace") {
            if (i + 1 < argc) {
                namespace_name = argv[++i];
            } else {
                std::cerr << "Error: --namespace requires a name\n";
                return 1;
            }
        } else if (arg == "-f" || arg == "--framework") {
            if (i + 1 < argc) {
                framework_str = argv[++i];
            } else {
                std::cerr << "Error: --framework requires a name\n";
                return 1;
            }
        } else if (arg == "-r" || arg == "--frontend") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                frontend_str = argv[++i];
            } else {
                frontend_str = "react";
            }
        } else if (arg == "-b" || arg == "--bundler") {
            if (i + 1 < argc) {
                bundler = argv[++i];
            } else {
                std::cerr << "Error: --bundler requires a value (vite/cra/webpack)\n";
                return 1;
            }
        } else if (arg == "-e" || arg == "--react-framework") {
            if (i + 1 < argc) {
                react_framework = argv[++i];
            } else {
                std::cerr << "Error: --react-framework requires a value (typescript/javascript)\n";
                return 1;
            }
        } else if (arg == "-c" || arg == "--css") {
            if (i + 1 < argc) {
                css = argv[++i];
            } else {
                std::cerr << "Error: --css requires a value (tailwind/bootstrap/plain)\n";
                return 1;
            }
        } else if (arg == "-t" || arg == "--theme") {
            if (i + 1 < argc) {
                theme = argv[++i];
            } else {
                std::cerr << "Error: --theme requires a value (light/dark/auto)\n";
                return 1;
            }
        } else if (arg == "-m" || arg == "--state") {
            if (i + 1 < argc) {
                state = argv[++i];
            } else {
                std::cerr << "Error: --state requires a value\n";
                return 1;
            }
        } else if (arg == "--standalone") {
            angular_standalone = true;
        } else if (arg == "--guards") {
            angular_guards = true;
        } else if (arg == "--admin") {
            angular_admin = true;
        } else if (arg == "--angular-css") {
            if (i + 1 < argc) {
                angular_css = argv[++i];
            } else {
                std::cerr << "Error: --angular-css requires a value (tailwind/bootstrap/plain/material)\n";
                return 1;
            }
        } else if (arg == "--angular-state") {
            if (i + 1 < argc) {
                angular_state = argv[++i];
            } else {
                std::cerr << "Error: --angular-state requires a value (service/ngrx/redux/signals)\n";
                return 1;
            }
        } else if (arg == "--angular-http") {
            if (i + 1 < argc) {
                angular_http = argv[++i];
            } else {
                std::cerr << "Error: --angular-http requires a value (httpclient/fetch/axios)\n";
                return 1;
            }
        } else if (arg == "--angular-bundler") {
            if (i + 1 < argc) {
                angular_bundler = argv[++i];
            } else {
                std::cerr << "Error: --angular-bundler requires a value (esbuild/webpack)\n";
                return 1;
            }
        } else if (arg == "--android-lang") {
            if (i + 1 < argc) {
                android_language = argv[++i];
            } else {
                std::cerr << "Error: --android-lang requires a value (kotlin/java)\n";
                return 1;
            }
        } else if (arg == "--android-arch") {
            if (i + 1 < argc) {
                android_architecture = argv[++i];
            } else {
                std::cerr << "Error: --android-arch requires a value (mvvm/clean)\n";
                return 1;
            }
        } else if (arg == "--security") {
            if (i + 1 < argc) {
                security = argv[++i];
            } else {
                std::cerr << "Error: --security requires a value (jwt/oauth2/saml/owasp/ldap/none)\n";
                return 1;
            }
        } else if (arg == "--orm") {
            if (i + 1 < argc) {
                orm = argv[++i];
            } else {
                std::cerr << "Error: --orm requires a value (sqlalchemy/peewee/slack)\n";
                return 1;
            }
        } else if (arg == "-l" || arg == "--list-tables") {
            list_tables = true;
        } else if (arg == "-v" || arg == "--version") {
            print_version();
            return 0;
        } else if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_help(argv[0]);
            return 1;
        }
    }

    novagen::Framework framework = novagen::string_to_framework(framework_str);
    std::string framework_name = novagen::framework_to_string(framework);
    std::cout << "NovaGen v1.0.0 - " << framework_name << " REST API Generator\n";

    if (input_file.empty() && sql_string.empty()) {
        std::cerr << "Error: Either --input or --sql must be provided\n";
        print_help(argv[0]);
        return 1;
    }

    if (!input_file.empty() && !sql_string.empty()) {
        std::cerr << "Error: --input and --sql are mutually exclusive\n";
        return 1;
    }

    std::string sql_content;
    if (!input_file.empty()) {
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file: " << input_file << "\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        sql_content = buffer.str();
        file.close();
    } else {
        sql_content = sql_string;
    }

    std::cout << "Parsing SQL DDL...\n";

    try {
        auto tables = novagen::parse_tables(sql_content);

        if (tables.empty()) {
            std::cerr << "Warning: No tables found in SQL DDL\n";
            return 1;
        }

        std::cout << "Found " << tables.size() << " table(s):\n";
        for (const auto& table : tables) {
            std::cout << "  - " << table.name << " ("
                     << table.columns.size() << " columns, "
                     << table.foreign_keys.size() << " FKs)\n";
        }

        if (list_tables) {
            return 0;
        }

        fs::path project_path = fs::path(output_dir) / project_name;

        if (fs::exists(project_path)) {
            std::cout << "Project directory '" << project_path << "' exists. Overwrite? [y/N] ";
            std::string response;
            std::getline(std::cin, response);
            if (response != "y" && response != "Y") {
                std::cout << "Aborted.\n";
                return 0;
            }
        }

        std::cout << "\nGenerating project: " << project_name << "\n";
        std::cout << "Output: " << project_path << "\n";
        if (frontend_str != "none") {
            std::cout << "Frontend: " << frontend_str;
            if (frontend_str == "angular") {
                std::cout << " (standalone=" << (angular_standalone ? "true" : "false");
                std::cout << ", guards=" << (angular_guards ? "true" : "false");
                std::cout << ", admin=" << (angular_admin ? "true" : "false") << ")";
            }
            std::cout << "\n";
        }

        novagen::Generator generator(project_name, namespace_name, output_dir, framework);
        generator.set_generate_frontend(true);
        generator.set_frontend_type(frontend_str);
        generator.set_react_bundler(bundler);
        generator.set_react_framework(react_framework);
        generator.set_react_css(css);
        generator.set_react_theme(theme);
        generator.set_react_state(state);
        generator.set_angular_standalone(angular_standalone);
        generator.set_angular_guards(angular_guards);
        generator.set_angular_admin(angular_admin);
        generator.set_angular_css(angular_css);
        generator.set_angular_state(angular_state);
        generator.set_angular_http(angular_http);
        generator.set_angular_bundler(angular_bundler);
        generator.set_security(security);
        generator.set_orm(orm);
        generator.set_android_language(android_language);
        generator.set_android_architecture(android_architecture);
        if (!generator.generate(tables)) {
            std::cerr << "Error: " << generator.last_error() << "\n";
            return 1;
        }

        std::cout << "\nProject generated successfully!\n";
        if (frontend_str != "none") {
            fs::path frontend_path = fs::path(output_dir) / (project_name + "-frontend");
            if (frontend_str == "android") {
                frontend_path = fs::path(output_dir) / (project_name + "-android");
                std::cout << "\nAndroid app: " << frontend_path << "\n";
                std::cout << "To build Android app:\n";
                std::cout << "  cd " << frontend_path << "\n";
                std::cout << "  ./gradlew assembleDebug\n";
                std::cout << "  # or open in Android Studio\n";
            } else {
                std::cout << "\n" << frontend_str << " frontend: " << frontend_path << "\n";
                if (frontend_str == "react") {
                    std::cout << "To run frontend:\n";
                    std::cout << "  cd " << frontend_path << "\n";
                    std::cout << "  npm install\n";
                    std::cout << "  npm run dev\n";
                } else if (frontend_str == "angular") {
                    std::cout << "To run frontend:\n";
                    std::cout << "  cd " << frontend_path << "\n";
                    std::cout << "  npm install\n";
                    std::cout << "  ng serve\n";
                }
            }
        }

        if (framework == novagen::Framework::CrowCpp) {
            std::cout << "\nTo build (C++ Crow):\n";
            std::cout << "  cd " << project_path << "\n";
            std::cout << "  mkdir build && cd build\n";
            std::cout << "  cmake .. -DCMAKE_BUILD_TYPE=Release\n";
            std::cout << "  cmake --build . -j$(nproc)\n";
            std::cout << "\nTo run:\n";
            std::cout << "  ./" << project_name << " --port 8080\n";
            std::cout << "\nAPI available at http://localhost:8080/api\n";
        } else if (framework == novagen::Framework::Flask) {
            std::cout << "\nTo run (Flask):\n";
            std::cout << "  cd " << project_path << "\n";
            std::cout << "  pip install -r requirements.txt\n";
            std::cout << "  python run.py\n";
            std::cout << "\nAPI available at http://localhost:8080/api\n";
        } else if (framework == novagen::Framework::FastAPI) {
            std::cout << "\nTo run (FastAPI):\n";
            std::cout << "  cd " << project_path << "\n";
            std::cout << "  pip install -r requirements.txt\n";
            std::cout << "  uvicorn app.main:app --host 0.0.0.0 --port 8080 --reload\n";
            std::cout << "\nAPI docs at http://localhost:8080/docs\n";
            std::cout << "API available at http://localhost:8080/api\n";
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
