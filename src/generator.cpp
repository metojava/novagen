#include "generator.hpp"

#include <fstream>
#include <sstream>

namespace novagen {

String framework_to_string(Framework f) {
    switch (f) {
        case Framework::CrowCpp: return "crow";
        case Framework::Flask: return "flask";
        case Framework::FastAPI: return "fastapi";
        case Framework::SpringBoot: return "springboot";
        default: return "crow";
    }
}

Framework string_to_framework(const String& s) {
    if (s == "flask" || s == "Flask") return Framework::Flask;
    if (s == "fastapi" || s == "FastAPI") return Framework::FastAPI;
    if (s == "springboot" || s == "SpringBoot") return Framework::SpringBoot;
    return Framework::CrowCpp;
}

String frontend_type_to_string(FrontendType t) {
    switch (t) {
        case FrontendType::React: return "react";
        case FrontendType::Angular: return "angular";
        case FrontendType::Android: return "android";
        default: return "react";
    }
}

FrontendType string_to_frontend_type(const String& s) {
    if (s == "angular" || s == "Angular") return FrontendType::Angular;
    if (s == "android" || s == "Android") return FrontendType::Android;
    return FrontendType::React;
}

String react_bundler_to_string(ReactBundler b) {
    switch (b) {
        case ReactBundler::Vite: return "vite";
        case ReactBundler::CRA: return "cra";
        default: return "vite";
    }
}

ReactBundler string_to_react_bundler(const String& s) {
    if (s == "cra" || s == "CRA" || s == "create-react-app") return ReactBundler::CRA;
    if (s == "webpack" || s == "Webpack") return ReactBundler::Webpack;
    return ReactBundler::Vite;
}

String react_framework_to_string(ReactFramework f) {
    switch (f) {
        case ReactFramework::TypeScript: return "typescript";
        case ReactFramework::JavaScript: return "javascript";
        default: return "typescript";
    }
}

ReactFramework string_to_react_framework(const String& s) {
    if (s == "javascript" || s == "js" || s == "JavaScript") return ReactFramework::JavaScript;
    return ReactFramework::TypeScript;
}

String react_css_to_string(ReactCSS c) {
    switch (c) {
        case ReactCSS::Tailwind: return "tailwind";
        case ReactCSS::Bootstrap: return "bootstrap";
        case ReactCSS::Plain: return "plain";
        default: return "tailwind";
    }
}

ReactCSS string_to_react_css(const String& s) {
    if (s == "bootstrap" || s == "Bootstrap") return ReactCSS::Bootstrap;
    if (s == "plain" || s == "Plain") return ReactCSS::Plain;
    return ReactCSS::Tailwind;
}

String react_theme_to_string(ReactTheme t) {
    switch (t) {
        case ReactTheme::Light: return "light";
        case ReactTheme::Dark: return "dark";
        case ReactTheme::Auto: return "auto";
        default: return "light";
    }
}

ReactTheme string_to_react_theme(const String& s) {
    if (s == "dark" || s == "Dark") return ReactTheme::Dark;
    if (s == "auto" || s == "Auto") return ReactTheme::Auto;
    return ReactTheme::Light;
}

String react_state_to_string(ReactState s) {
    switch (s) {
        case ReactState::Context: return "context";
        case ReactState::Zustand: return "zustand";
        case ReactState::Redux: return "redux";
        default: return "context";
    }
}

ReactState string_to_react_state(const String& s) {
    if (s == "zustand" || s == "Zustand") return ReactState::Zustand;
    if (s == "redux" || s == "Redux") return ReactState::Redux;
    return ReactState::Context;
}

String angular_css_to_string(AngularCSS c) {
    switch (c) {
        case AngularCSS::Tailwind: return "tailwind";
        case AngularCSS::Bootstrap: return "bootstrap";
        case AngularCSS::Plain: return "plain";
        case AngularCSS::Material: return "material";
        default: return "tailwind";
    }
}

AngularCSS string_to_angular_css(const String& s) {
    if (s == "bootstrap" || s == "Bootstrap") return AngularCSS::Bootstrap;
    if (s == "plain" || s == "Plain") return AngularCSS::Plain;
    if (s == "material" || s == "Material") return AngularCSS::Material;
    return AngularCSS::Tailwind;
}

String angular_state_to_string(AngularState s) {
    switch (s) {
        case AngularState::Service: return "service";
        case AngularState::NgRx: return "ngrx";
        case AngularState::Redux: return "redux";
        case AngularState::Signals: return "signals";
        default: return "service";
    }
}

AngularState string_to_angular_state(const String& s) {
    if (s == "ngrx" || s == "NgRx") return AngularState::NgRx;
    if (s == "redux" || s == "Redux") return AngularState::Redux;
    if (s == "signals" || s == "Signals") return AngularState::Signals;
    return AngularState::Service;
}

String angular_http_to_string(AngularHTTP h) {
    switch (h) {
        case AngularHTTP::HttpClient: return "httpclient";
        case AngularHTTP::Fetch: return "fetch";
        case AngularHTTP::Axios: return "axios";
        default: return "httpclient";
    }
}

AngularHTTP string_to_angular_http(const String& s) {
    if (s == "fetch" || s == "Fetch") return AngularHTTP::Fetch;
    if (s == "axios" || s == "Axios") return AngularHTTP::Axios;
    return AngularHTTP::HttpClient;
}

String angular_bundler_to_string(AngularBundler b) {
    switch (b) {
        case AngularBundler::Esbuild: return "esbuild";
        case AngularBundler::Webpack: return "webpack";
        default: return "esbuild";
    }
}

AngularBundler string_to_angular_bundler(const String& s) {
    if (s == "webpack" || s == "Webpack") return AngularBundler::Webpack;
    return AngularBundler::Esbuild;
}

String security_to_string(SecurityType s) {
    switch (s) {
        case SecurityType::JWT: return "jwt";
        case SecurityType::OAuth2: return "oauth2";
        case SecurityType::SAML: return "saml";
        case SecurityType::OWASP: return "owasp";
        case SecurityType::LDAP: return "ldap";
        default: return "none";
    }
}

SecurityType string_to_security(const String& s) {
    if (s == "jwt" || s == "JWT" || s == "Jwt") return SecurityType::JWT;
    if (s == "oauth2" || s == "OAuth2" || s == "oauth") return SecurityType::OAuth2;
    if (s == "saml" || s == "SAML") return SecurityType::SAML;
    if (s == "owasp" || s == "OWASP") return SecurityType::OWASP;
    if (s == "ldap" || s == "LDAP") return SecurityType::LDAP;
    return SecurityType::None;
}

String android_language_to_string(AndroidLanguage l) {
    switch (l) {
        case AndroidLanguage::Kotlin: return "kotlin";
        case AndroidLanguage::Java: return "java";
        default: return "kotlin";
    }
}

AndroidLanguage string_to_android_language(const String& s) {
    if (s == "java" || s == "Java") return AndroidLanguage::Java;
    return AndroidLanguage::Kotlin;
}

String android_architecture_to_string(AndroidArchitecture a) {
    switch (a) {
        case AndroidArchitecture::MVVM: return "mvvm";
        case AndroidArchitecture::Clean: return "clean";
        default: return "mvvm";
    }
}

AndroidArchitecture string_to_android_architecture(const String& s) {
    if (s == "clean" || s == "Clean") return AndroidArchitecture::Clean;
    return AndroidArchitecture::MVVM;
}

String orm_to_string(ORMType o) {
    switch (o) {
        case ORMType::SQLAlchemy: return "sqlalchemy";
        case ORMType::Slack: return "slack";
        case ORMType::Peewee: return "peewee";
        default: return "sqlalchemy";
    }
}

ORMType string_to_orm(const String& s) {
    if (s == "slack" || s == "Slack") return ORMType::Slack;
    if (s == "peewee" || s == "Peewee") return ORMType::Peewee;
    return ORMType::SQLAlchemy;
}

Generator::Generator(const String& project_name,
                   const String& namespace_name,
                   const String& output_dir,
                   Framework framework)
    : project_name_(project_name),
      namespace_(namespace_name),
      output_dir_(output_dir),
      framework_(framework) {
}

void Generator::set_project_name(const String& name) {
    project_name_ = name;
}

void Generator::set_namespace(const String& ns) {
    namespace_ = ns;
}

void Generator::set_output_dir(const String& dir) {
    output_dir_ = dir;
}

void Generator::set_framework(Framework framework) {
    framework_ = framework;
}

void Generator::set_frontend_type(const String& type) {
    frontend_type_ = string_to_frontend_type(type);
}

void Generator::set_react_bundler(const String& bundler) {
    react_bundler_ = string_to_react_bundler(bundler);
}

void Generator::set_react_framework(const String& framework) {
    react_framework_ = string_to_react_framework(framework);
}

void Generator::set_react_css(const String& css) {
    react_css_ = string_to_react_css(css);
}

void Generator::set_react_theme(const String& theme) {
    react_theme_ = string_to_react_theme(theme);
}

void Generator::set_react_state(const String& state) {
    react_state_ = string_to_react_state(state);
}

void Generator::set_angular_css(const String& css) {
    angular_css_ = string_to_angular_css(css);
}

void Generator::set_angular_state(const String& state) {
    angular_state_ = string_to_angular_state(state);
}

void Generator::set_angular_http(const String& http) {
    angular_http_ = string_to_angular_http(http);
}

void Generator::set_angular_bundler(const String& bundler) {
    angular_bundler_ = string_to_angular_bundler(bundler);
}

void Generator::set_security(const String& security) {
    security_ = string_to_security(security);
}

void Generator::set_android_language(const String& language) {
    android_language_ = string_to_android_language(language);
}

void Generator::set_android_architecture(const String& arch) {
    android_architecture_ = string_to_android_architecture(arch);
}

void Generator::set_orm(const String& orm) {
    orm_ = string_to_orm(orm);
}

String Generator::last_error() const {
    return last_error_;
}

String Generator::replace_all(const String& s, const String& from, const String& to) {
    if (from.empty()) return s;
    String result;
    size_t pos = 0;
    size_t found;
    while ((found = s.find(from, pos)) != String::npos) {
        result += s.substr(pos, found - pos);
        result += to;
        pos = found + from.length();
    }
    result += s.substr(pos);
    return result;
}

bool Generator::create_directory(const fs::path& path) {
    try {
        fs::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        last_error_ = "Failed to create directory: " + path.string() + " - " + e.what();
        return false;
    }
}

bool Generator::write_file(const fs::path& path, const String& content) {
    try {
        fs::create_directories(path.parent_path());
        std::ofstream out(path);
        if (!out) {
            last_error_ = "Failed to open file for writing: " + path.string();
            return false;
        }
        out << content;
        out.close();
        return true;
    } catch (const std::exception& e) {
        last_error_ = "Failed to write file: " + path.string() + " - " + e.what();
        return false;
    }
}

bool Generator::generate(const std::vector<Table>& tables) {
    try {
        fs::path base = fs::path(output_dir_) / project_name_;

        switch (framework_) {
            case Framework::CrowCpp:
                if (!create_directory(base)) return false;
                if (!create_directory(base / "include" / namespace_)) return false;
                if (!create_directory(base / "src")) return false;
                if (!create_directory(base / "tests")) return false;
                if (!create_directory(base / "data")) return false;

                if (!write_file(base / "CMakeLists.txt", generate_cmake_crow(tables))) return false;
                if (!write_file(base / "conanfile.py", generate_conanfile_crow(tables))) return false;
                if (!write_file(base / ".gitignore", generate_gitignore_crow())) return false;
                if (!write_file(base / "README.md", generate_readme_crow(tables))) return false;

                if (!write_file(base / "include" / namespace_ / "models.h", generate_models_h_crow(tables))) return false;
                if (!write_file(base / "src" / "models.cpp", generate_models_cpp_crow(tables))) return false;
                if (!write_file(base / "include" / namespace_ / "database.h", generate_database_h_crow(tables))) return false;
                if (!write_file(base / "src" / "database.cpp", generate_database_cpp_crow(tables))) return false;
                if (!write_file(base / "include" / namespace_ / "handlers.h", generate_handlers_h_crow(tables))) return false;
                if (!write_file(base / "src" / "handlers.cpp", generate_handlers_cpp_crow(tables))) return false;
                if (!write_file(base / "src" / "routes.cpp", generate_routes_cpp_crow(tables))) return false;
                if (!write_file(base / "src" / "main.cpp", generate_main_cpp_crow(tables))) return false;
                if (!write_file(base / "include" / namespace_ / "dto.h", generate_dto_h_crow(tables))) return false;

                if (security_ != SecurityType::None) {
                    if (!create_directory(base / "include" / namespace_ / "security")) return false;
                    switch (security_) {
                        case SecurityType::JWT:
                            if (!write_file(base / "include" / namespace_ / "security" / "jwt.h", generate_security_jwt_crow())) return false;
                            break;
                        case SecurityType::OAuth2:
                            if (!write_file(base / "include" / namespace_ / "security" / "oauth2.h", generate_security_oauth2_crow())) return false;
                            break;
                        case SecurityType::SAML:
                            if (!write_file(base / "include" / namespace_ / "security" / "saml.h", generate_security_saml_crow())) return false;
                            break;
                        case SecurityType::OWASP:
                            if (!write_file(base / "include" / namespace_ / "security" / "owasp.h", generate_security_owasp_crow())) return false;
                            break;
                        case SecurityType::LDAP:
                            if (!write_file(base / "include" / namespace_ / "security" / "ldap.h", generate_security_ldap_crow())) return false;
                            break;
                        default:
                            break;
                    }
                }

                if (!write_file(base / "tests" / "CMakeLists.txt", generate_tests_cmake_crow(tables))) return false;
                if (!write_file(base / "tests" / "test_models.cpp", generate_test_models_cpp_crow(tables))) return false;
                if (!write_file(base / "tests" / "test_database.cpp", generate_test_database_cpp_crow(tables))) return false;
                break;

            case Framework::Flask:
                if (!create_directory(base)) return false;
                if (!create_directory(base / "app")) return false;
                if (!create_directory(base / "app" / "models")) return false;
                if (!create_directory(base / "app" / "routes")) return false;
                if (!create_directory(base / "tests")) return false;
                if (!create_directory(base / "migrations")) return false;

                if (!write_file(base / "requirements.txt", generate_flask_requirements_txt())) return false;
                if (!write_file(base / "config.py", generate_flask_config_py())) return false;
                if (!write_file(base / "run.py", generate_flask_app_py(tables))) return false;
                if (!write_file(base / "app" / "__init__.py", "from flask import Flask\nfrom config import Config\nfrom app.models import db\n\ndef create_app(config_class=Config):\n    app = Flask(__name__)\n    app.config.from_object(config_class)\n\n    db.init_app(app)\n\n    from app.routes import " + namespace_ + "_bp\n    app.register_blueprint(" + namespace_ + "_bp, url_prefix='/api')\n\n    @app.route('/health')\n    def health():\n        return {'status': 'healthy'}\n\n    @app.route('/api')\n    def api_info():\n        return {\n            'name': '" + project_name_ + "',\n            'version': '1.0.0',\n            'endpoints': " + generate_flask_api_endpoints_json(tables) + "\n        }\n\n    return app\n")) return false;
                if (!write_file(base / "app" / "models" / "__init__.py", generate_flask_models_py(tables))) return false;
                if (!write_file(base / "app" / "routes" / "__init__.py", generate_flask_routes_py(tables))) return false;
                if (!write_file(base / "app" / "durable_functions.py", generate_flask_durable_client_py())) return false;
                if (!write_file(base / "run.sh", generate_flask_run_sh())) return false;
                if (!write_file(base / ".gitignore", generate_flask_gitignore())) return false;
                if (!write_file(base / "Dockerfile", generate_flask_dockerfile(tables))) return false;
                if (!write_file(base / "docker-compose.yml", generate_flask_docker_compose_yml())) return false;
                if (!write_file(base / "README.md", generate_flask_readme(tables))) return false;
                if (!write_file(base / "pytest.ini", generate_flask_pytest_ini())) return false;
                if (!write_file(base / "tests" / "__init__.py", "")) return false;
                if (!write_file(base / "tests" / "test_models.py", generate_flask_test_models_py(tables))) return false;
                if (!write_file(base / "tests" / "test_routes.py", generate_flask_test_routes_py(tables))) return false;

                if (security_ != SecurityType::None) {
                    if (!create_directory(base / "app" / "security")) return false;
                    switch (security_) {
                        case SecurityType::JWT:
                            if (!write_file(base / "app" / "security" / "jwt_auth.py", generate_security_jwt_flask())) return false;
                            break;
                        case SecurityType::OAuth2:
                            if (!write_file(base / "app" / "security" / "oauth2.py", generate_security_oauth2_flask())) return false;
                            break;
                        case SecurityType::SAML:
                            if (!write_file(base / "app" / "security" / "saml.py", generate_security_saml_flask())) return false;
                            break;
                        case SecurityType::OWASP:
                            if (!write_file(base / "app" / "security" / "owasp.py", generate_security_owasp_flask())) return false;
                            break;
                        case SecurityType::LDAP:
                            if (!write_file(base / "app" / "security" / "ldap_auth.py", generate_security_ldap_flask())) return false;
                            break;
                        default:
                            break;
                    }
                }
                break;

            case Framework::FastAPI:
                if (!create_directory(base)) return false;
                if (!create_directory(base / "app")) return false;
                if (!create_directory(base / "app" / "routers")) return false;
                if (!create_directory(base / "tests")) return false;
                if (!create_directory(base / "alembic" / "versions")) return false;

                if (!write_file(base / "requirements.txt", generate_fastapi_requirements_txt())) return false;
                if (!write_file(base / "app" / "__init__.py", "from fastapi import FastAPI\nfrom app.database import engine, Base\nfrom app.routers import " + namespace_ + "_router\n\ndef create_app():\n    app = FastAPI(\n        title='" + project_name_ + "',\n        version='1.0.0',\n        description='Auto-generated REST API'\n    )\n\n    # Create tables\n    Base.metadata.create_all(bind=engine)\n\n    # Include routers\n    app.include_router(" + namespace_ + "_router, prefix='/api', tags=['" + namespace_ + "'])\n\n    @app.get('/health')\n    def health():\n        return {'status': 'healthy'}\n\n    @app.get('/api')\n    def api_info():\n        return {\n            'name': '" + project_name_ + "',\n            'version': '1.0.0',\n            'endpoints': " + generate_fastapi_api_endpoints_json(tables) + "\n        }\n\n    return app\n")) return false;
                if (!write_file(base / "app" / "main.py", generate_fastapi_main_py(tables))) return false;
                if (!write_file(base / "app" / "database.py", generate_fastapi_database_py(tables))) return false;
                if (!write_file(base / "app" / "models.py", generate_fastapi_models_py(tables))) return false;
                if (!write_file(base / "app" / "schemas.py", generate_fastapi_schemas_py(tables))) return false;
                if (!write_file(base / "app" / "routers" / "__init__.py", generate_fastapi_routers_py(tables))) return false;
                if (!write_file(base / "app" / "durable_functions.py", generate_fastapi_durable_client_py())) return false;
                if (!write_file(base / "alembic.ini", generate_fastapi_alembic_ini(tables))) return false;
                if (!write_file(base / "alembic" / "env.py", generate_fastapi_alembic_env_py(tables))) return false;
                if (!write_file(base / "Dockerfile", generate_fastapi_dockerfile(tables))) return false;
                if (!write_file(base / "docker-compose.yml", generate_fastapi_docker_compose_yml())) return false;
                if (!write_file(base / ".gitignore", generate_fastapi_gitignore())) return false;
                if (!write_file(base / "README.md", generate_fastapi_readme(tables))) return false;
                if (!write_file(base / "pytest.ini", generate_fastapi_pytest_ini())) return false;
                if (!write_file(base / "tests" / "__init__.py", "")) return false;
                if (!write_file(base / "tests" / "test_models.py", generate_fastapi_test_models_py(tables))) return false;
                if (!write_file(base / "tests" / "test_routers.py", generate_fastapi_test_routers_py(tables))) return false;

                if (security_ != SecurityType::None) {
                    if (!create_directory(base / "app" / "security")) return false;
                    switch (security_) {
                        case SecurityType::JWT:
                            if (!write_file(base / "app" / "security" / "jwt_auth.py", generate_security_jwt_fastapi())) return false;
                            break;
                        case SecurityType::OAuth2:
                            if (!write_file(base / "app" / "security" / "oauth2.py", generate_security_oauth2_fastapi())) return false;
                            break;
                        case SecurityType::SAML:
                            if (!write_file(base / "app" / "security" / "saml.py", generate_security_saml_fastapi())) return false;
                            break;
                        case SecurityType::OWASP:
                            if (!write_file(base / "app" / "security" / "owasp.py", generate_security_owasp_fastapi())) return false;
                            break;
                        case SecurityType::LDAP:
                            if (!write_file(base / "app" / "security" / "ldap_auth.py", generate_security_ldap_fastapi())) return false;
                            break;
                        default:
                            break;
                    }
                }
                break;

            case Framework::SpringBoot:
                if (!create_directory(base)) return false;
                if (!create_directory(base / "src" / "main" / "java" / namespace_ / "entity")) return false;
                if (!create_directory(base / "src" / "main" / "java" / namespace_ / "repository")) return false;
                if (!create_directory(base / "src" / "main" / "java" / namespace_ / "service")) return false;
                if (!create_directory(base / "src" / "main" / "java" / namespace_ / "controller")) return false;
                if (!create_directory(base / "src" / "main" / "resources" / "db" / "changelog")) return false;
                if (!create_directory(base / "src" / "test" / "java" / namespace_ / "service")) return false;
                if (!create_directory(base / "src" / "test" / "java" / namespace_ / "controller")) return false;
                if (!create_directory(base / "src" / "test" / "resources")) return false;

                if (!write_file(base / "pom.xml", generate_springboot_pom_xml(tables))) return false;
                if (!write_file(base / "src" / "main" / "resources" / "application.yml", generate_springboot_application_properties(tables))) return false;
                if (!write_file(base / "src" / "test" / "resources" / "application.yml", generate_springboot_application_test_properties(tables))) return false;
                if (!write_file(base / "src" / "main" / "java" / namespace_ / "Application.java", generate_springboot_app_java(tables))) return false;
                if (!write_file(base / "src" / "main" / "resources" / "db" / "changelog" / "db.changelog-master.yaml", generate_springboot_liquibase_changelog(tables))) return false;
                if (!write_file(base / "Dockerfile", generate_springboot_dockerfile(tables))) return false;
                if (!write_file(base / "docker-compose.yml", generate_springboot_docker_compose_yml(tables))) return false;
                if (!write_file(base / ".gitignore", generate_springboot_gitignore())) return false;
                if (!write_file(base / "README.md", generate_springboot_readme(tables))) return false;

                for (const auto& table : tables) {
                    if (!write_file(base / "src" / "main" / "java" / namespace_ / "entity" / (table.class_name() + ".java"),
                                  generate_springboot_entity_java(table))) return false;
                    if (!write_file(base / "src" / "main" / "java" / namespace_ / "repository" / (table.class_name() + "Repository.java"),
                                  generate_springboot_repository_java(table))) return false;
                    if (!write_file(base / "src" / "main" / "java" / namespace_ / "service" / (table.class_name() + "Service.java"),
                                  generate_springboot_service_java(table))) return false;
                    if (!write_file(base / "src" / "main" / "java" / namespace_ / "controller" / (table.class_name() + "Controller.java"),
                                  generate_springboot_controller_java(table))) return false;
                    if (!write_file(base / "src" / "test" / "java" / namespace_ / "service" / (table.class_name() + "ServiceTest.java"),
                                  generate_springboot_service_test_java(table))) return false;
                    if (!write_file(base / "src" / "test" / "java" / namespace_ / "controller" / (table.class_name() + "ControllerTest.java"),
                                  generate_springboot_controller_test_java(table))) return false;
                }

                if (security_ != SecurityType::None) {
                    if (!create_directory(base / "src" / "main" / "java" / namespace_ / "config")) return false;
                    switch (security_) {
                        case SecurityType::JWT:
                            if (!write_file(base / "src" / "main" / "java" / namespace_ / "config" / "SecurityConfig.java",
                                          generate_security_jwt_springboot())) return false;
                            break;
                        case SecurityType::OAuth2:
                            if (!write_file(base / "src" / "main" / "java" / namespace_ / "config" / "OAuth2SecurityConfig.java",
                                          generate_security_oauth2_springboot())) return false;
                            break;
                        case SecurityType::SAML:
                            if (!write_file(base / "src" / "main" / "java" / namespace_ / "config" / "Saml2SecurityConfig.java",
                                          generate_security_saml_springboot())) return false;
                            break;
                        case SecurityType::OWASP:
                            if (!write_file(base / "src" / "main" / "java" / namespace_ / "config" / "OWASPSecurityConfig.java",
                                          generate_security_owasp_springboot())) return false;
                            break;
                        case SecurityType::LDAP:
                            if (!write_file(base / "src" / "main" / "java" / namespace_ / "config" / "LdapSecurityConfig.java",
                                          generate_security_ldap_springboot())) return false;
                            break;
                        default:
                            break;
                    }
                }
                break;
        }

        if (generate_frontend_) {
            fs::path frontend_path;
            if (frontend_type_ == FrontendType::Android) {
                frontend_path = fs::path(output_dir_) / (project_name_ + "-android");
            } else {
                frontend_path = fs::path(output_dir_) / (project_name_ + "-frontend");
            }
            if (frontend_type_ == FrontendType::React) {
                if (!generate_react(frontend_path, tables)) return false;
            } else if (frontend_type_ == FrontendType::Angular) {
                if (!generate_angular_frontend(frontend_path, tables)) return false;
            } else if (frontend_type_ == FrontendType::Android) {
                if (!generate_android(frontend_path, tables)) return false;
            }
        }

        return true;
    } catch (const std::exception& e) {
        last_error_ = "Generation failed: " + String(e.what());
        return false;
    }
}

bool Generator::generate_from_sql(const String& sql) {
    auto tables = parse_tables(sql);
    if (tables.empty()) {
        last_error_ = "No tables found in SQL";
        return false;
    }
    return generate(tables);
}

// ============================================================================
// Crow C++ Generator Methods
// ============================================================================

String Generator::generate_cmake_crow(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(cmake_minimum_required(VERSION 3.20)
project()" << " " << project_name_ << R"(
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

include(FetchContent)
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.1
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(crow json)

add_executable()" << " " << project_name_ << R"(
    src/main.cpp
    src/models.cpp
    src/database.cpp
    src/handlers.cpp
    src/routes.cpp
)

target_include_directories()" << " " << project_name_ << R"( PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries()" << " " << project_name_ << R"( PRIVATE
    crow::crow
    nlohmann_json::nlohmann_json
    ${SQLITE3_LIBRARIES}
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS )" << project_name_ << R"( RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

include(CPack)
)";
    return ss.str();
}

String Generator::generate_models_h_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#pragma once\n\n";
    ss << "#include <string>\n#include <vector>\n#include <optional>\n#include <chrono>\n#include <cstdint>\n#include <nlohmann/json.hpp>\n\nusing json = nlohmann::json;\n\nnamespace " << namespace_ << " {\n\n";

    for (const auto& table : tables) {
        ss << "struct " << table.class_name() << " {\n";

        if (table.pk_column()) {
            ss << "    " << table.pk_column()->cpp_type() << " " << table.pk_column()->field_name() << ";\n";
        }

        for (const auto& col : table.columns) {
            if (!col.primary_key) {
                ss << "    " << col.cpp_type() << " " << col.field_name() << ";\n";
            }
        }

        ss << "\n    json to_json() const;\n";
        ss << "    static " << table.class_name() << " from_json(const json& j);\n";
        ss << "    bool validate() const;\n";
        ss << "    std::string to_string() const;\n";
        ss << "};\n\n";
    }

    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_models_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include \"" << namespace_ << "/models.h\"\n#include <sstream>\n#include <iomanip>\n\nnamespace " << namespace_ << " {\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();

        ss << "json " << table.class_name() << "::to_json() const {\n";
        ss << "    return json{\n";
        if (pk) {
            ss << "        {\"" << pk->field_name() << "\", " << pk->field_name() << "}";
        }
        for (const auto& col : table.columns) {
            if (!col.primary_key) {
                if (pk || &col != &table.columns.front()) ss << ",\n";
                ss << "        {\"" << col.field_name() << "\", " << col.field_name() << "}";
            }
        }
        ss << "\n    };\n";
        ss << "}\n\n";

        ss << table.class_name() << " " << table.class_name() << "::from_json(const json& j) {\n";
        ss << "    " << table.class_name() << " obj;\n";
        if (pk) {
            ss << "    if (j.contains(\"" << pk->field_name() << "\")) obj." << pk->field_name()
               << " = j[\"" << pk->field_name() << "\"].get<" << pk->cpp_type() << ">();\n";
        }
        for (const auto& col : table.columns) {
            if (!col.primary_key) {
                ss << "    if (j.contains(\"" << col.field_name() << "\")) obj." << col.field_name()
                   << " = j[\"" << col.field_name() << "\"].get<" << col.cpp_type() << ">();\n";
            }
        }
        ss << "    return obj;\n";
        ss << "}\n\n";

        ss << "bool " << table.class_name() << "::validate() const {\n";
        ss << "    return true;\n";
        ss << "}\n\n";

        ss << "std::string " << table.class_name() << "::to_string() const {\n";
        ss << "    std::ostringstream oss;\n";
        ss << "    oss << \"" << table.class_name() << " { \";\n";
        if (pk) {
            ss << "    oss << \"" << pk->field_name() << "=\" << " << pk->field_name() << ";\n";
        }
        for (const auto& col : table.columns) {
            if (!col.primary_key) {
                ss << "    oss << \", " << col.field_name() << "=\" << " << col.field_name() << ";\n";
            }
        }
        ss << "    oss << \" }\";\n";
        ss << "    return oss.str();\n";
        ss << "}\n\n";
    }

    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_database_h_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#pragma once\n\n";
    ss << "#include <string>\n#include <sqlite3.h>\n#include <memory>\n#include <vector>\n#include <optional>\n#include \"" << namespace_ << "/models.h\"\n\nnamespace " << namespace_ << " {\n\nclass Database {\npublic:\n    static Database& instance();\n    void open(const std::string& path);\n    void close();\n    void migrate();\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        String pk_type = pk ? pk->cpp_type() : "int64_t";
        String pk_name = pk ? pk->field_name() : "id";

        ss << "    int64_t insert_" << table.instance_name() << "(const " << table.class_name() << "& obj);\n";
        ss << "    bool update_" << table.instance_name() << "(" << pk_type << " " << pk_name
           << ", const " << table.class_name() << "& obj);\n";
        ss << "    bool delete_" << table.instance_name() << "(" << pk_type << " " << pk_name << ");\n";
        ss << "    std::optional<" << table.class_name() << "> get_" << table.instance_name()
           << "(" << pk_type << " " << pk_name << ");\n";
        ss << "    std::vector<" << table.class_name() << "> get_all_"
           << pluralize(table.instance_name()) << "();\n";
        ss << "    int64_t count_" << pluralize(table.instance_name()) << "();\n\n";
    }

    ss << "private:\n";
    ss << "    Database() = default;\n";
    ss << "    ~Database();\n";
    ss << "    Database(const Database&) = delete;\n";
    ss << "    Database& operator=(const Database&) = delete;\n\n";
    ss << "    sqlite3* db_ = nullptr;\n";
    ss << "    std::string db_path_;\n\n";
    ss << "    void execute_sql(const std::string& sql);\n";
    ss << "};\n\n} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_database_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include \"" << namespace_ << "/database.h\"\n#include <iostream>\n#include <sstream>\n\nnamespace " << namespace_ << " {\n\nusing std::string;\n\nDatabase& Database::instance() {\n    static Database instance;\n    return instance;\n}\n\nDatabase::~Database() {\n    close();\n}\n\nvoid Database::open(const string& path) {\n    db_path_ = path;\n    int rc = sqlite3_open(path.c_str(), &db_);\n    if (rc) {\n        throw std::runtime_error(\"Cannot open database: \" + std::string(sqlite3_errmsg(db_)));\n    }\n    sqlite3_exec(db_, \"PRAGMA foreign_keys = ON;\", nullptr, nullptr, nullptr);\n}\n\nvoid Database::close() {\n    if (db_) {\n        sqlite3_close(db_);\n        db_ = nullptr;\n    }\n}\n\nvoid Database::execute_sql(const string& sql) {\n    char* err_msg = nullptr;\n    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);\n    if (rc != SQLITE_OK) {\n        string error = err_msg ? err_msg : \"Unknown error\";\n        sqlite3_free(err_msg);\n        throw std::runtime_error(\"SQL error: \" + error);\n    }\n}\n\nvoid Database::migrate() {\n    string sql = R\"(\n";

    for (const auto& table : tables) {
        ss << "    CREATE TABLE IF NOT EXISTS " << table.name << " (\n";
        std::vector<String> col_defs;
        for (const auto& col : table.columns) {
            String def = "        " + col.name + " " + col.db_type();
            if (col.primary_key && col.auto_increment) {
                def += " PRIMARY KEY AUTOINCREMENT";
            } else if (col.primary_key) {
                def += " PRIMARY KEY";
            } else if (!col.nullable) {
                def += " NOT NULL";
            }
            if (col.unique) def += " UNIQUE";
            if (col.default_value) {
                def += " DEFAULT " + *col.default_value;
            }
            col_defs.push_back(def);
        }
        for (const auto& fk : table.foreign_keys) {
            col_defs.push_back("        FOREIGN KEY (" + fk.column + ") REFERENCES " +
                              fk.ref_table + "(" + fk.ref_column + ")");
        }
        for (size_t i = 0; i < col_defs.size(); ++i) {
            ss << col_defs[i];
            if (i < col_defs.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "    );\n";
    }

    ss << "    )\";\n";
    ss << "    execute_sql(sql);\n}\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        const auto& non_pk = table.non_pk_columns();
        String pk_type = pk ? pk->cpp_type() : "int64_t";
        String pk_name = pk ? pk->field_name() : "id";

        ss << "int64_t Database::insert_" << table.instance_name()
           << "(const " << table.class_name() << "& obj) {\n";
        ss << "    std::ostringstream sql;\n";
        ss << "    sql << \"INSERT INTO " << table.name << " (";
        if (!non_pk.empty()) {
            ss << non_pk[0].name;
            for (size_t i = 1; i < non_pk.size(); ++i) {
                ss << ", " << non_pk[i].name;
            }
        }
        ss << ") VALUES (";
        if (!non_pk.empty()) {
            ss << "?";
            for (size_t i = 1; i < non_pk.size(); ++i) {
                ss << ", ?";
            }
        }
        ss << ")\";\n\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        throw std::runtime_error(sqlite3_errmsg(db_));\n";
        ss << "    }\n\n";

        int idx = 1;
        for (const auto& col : non_pk) {
            if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "bool") {
                ss << "    sqlite3_bind_int64(stmt, " << idx << ", obj." << col.field_name() << ");\n";
            } else if (col.cpp_type() == "float" || col.cpp_type() == "double") {
                ss << "    sqlite3_bind_double(stmt, " << idx << ", obj." << col.field_name() << ");\n";
            } else {
                ss << "    sqlite3_bind_text(stmt, " << idx << ", obj." << col.field_name()
                   << ".c_str(), -1, SQLITE_TRANSIENT);\n";
            }
            ++idx;
        }

        ss << "\n    if (sqlite3_step(stmt) != SQLITE_DONE) {\n";
        ss << "        sqlite3_finalize(stmt);\n";
        ss << "        throw std::runtime_error(sqlite3_errmsg(db_));\n";
        ss << "    }\n";
        ss << "    int64_t id = sqlite3_last_insert_rowid(db_);\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return id;\n";
        ss << "}\n\n";

        ss << "bool Database::update_" << table.instance_name() << "(" << pk_type << " " << pk_name
           << ", const " << table.class_name() << "& obj) {\n";
        ss << "    std::ostringstream sql;\n";
        ss << "    sql << \"UPDATE " << table.name << " SET ";
        if (!non_pk.empty()) {
            ss << non_pk[0].name << " = ?";
            for (size_t i = 1; i < non_pk.size(); ++i) {
                ss << ", " << non_pk[i].name << " = ?";
            }
        }
        ss << " WHERE " << pk_name << " = ?\";\n\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        throw std::runtime_error(sqlite3_errmsg(db_));\n";
        ss << "    }\n\n";

        idx = 1;
        for (const auto& col : non_pk) {
            if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "bool") {
                ss << "    sqlite3_bind_int64(stmt, " << idx << ", obj." << col.field_name() << ");\n";
            } else if (col.cpp_type() == "float" || col.cpp_type() == "double") {
                ss << "    sqlite3_bind_double(stmt, " << idx << ", obj." << col.field_name() << ");\n";
            } else {
                ss << "    sqlite3_bind_text(stmt, " << idx << ", obj." << col.field_name()
                   << ".c_str(), -1, SQLITE_TRANSIENT);\n";
            }
            ++idx;
        }
        ss << "    sqlite3_bind_int64(stmt, " << idx << ", " << pk_name << ");\n\n";

        ss << "    bool success = sqlite3_step(stmt) == SQLITE_DONE;\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return success;\n";
        ss << "}\n\n";

        ss << "bool Database::delete_" << table.instance_name() << "(" << pk_type << " " << pk_name << ") {\n";
        ss << "    std::ostringstream sql;\n";
        ss << "    sql << \"DELETE FROM " << table.name << " WHERE " << pk_name << " = ?\";\n\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        throw std::runtime_error(sqlite3_errmsg(db_));\n";
        ss << "    }\n";
        ss << "    sqlite3_bind_int64(stmt, 1, " << pk_name << ");\n\n";
        ss << "    bool success = sqlite3_step(stmt) == SQLITE_DONE;\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return success;\n";
        ss << "}\n\n";

        ss << "std::optional<" << table.class_name() << "> Database::get_" << table.instance_name()
           << "(" << pk_type << " " << pk_name << ") {\n";
        ss << "    std::ostringstream sql;\n";
        ss << "    sql << \"SELECT * FROM " << table.name << " WHERE " << pk_name << " = ?\";\n\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        return std::nullopt;\n";
        ss << "    }\n";
        ss << "    sqlite3_bind_int64(stmt, 1, " << pk_name << ");\n\n";
        ss << "    std::optional<" << table.class_name() << "> result;\n";
        ss << "    if (sqlite3_step(stmt) == SQLITE_ROW) {\n";
        ss << "        " << table.class_name() << " obj;\n";

        int col_idx = 0;
        for (const auto& col : table.columns) {
            if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "bool") {
                ss << "        obj." << col.field_name() << " = sqlite3_column_int64(stmt, " << col_idx << ");\n";
            } else if (col.cpp_type() == "float" || col.cpp_type() == "double") {
                ss << "        obj." << col.field_name() << " = sqlite3_column_double(stmt, " << col_idx << ");\n";
            } else {
                ss << "        obj." << col.field_name()
                   << " = reinterpret_cast<const char*>(sqlite3_column_text(stmt, " << col_idx << "));\n";
            }
            ++col_idx;
        }
        ss << "        result = obj;\n";
        ss << "    }\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return result;\n";
        ss << "}\n\n";

        ss << "std::vector<" << table.class_name() << "> Database::get_all_"
           << pluralize(table.instance_name()) << "() {\n";
        ss << "    std::vector<" << table.class_name() << "> results;\n";
        ss << "    const char* sql = \"SELECT * FROM " << table.name << "\";\n\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        return results;\n";
        ss << "    }\n\n";
        ss << "    while (sqlite3_step(stmt) == SQLITE_ROW) {\n";
        ss << "        " << table.class_name() << " obj;\n";

        col_idx = 0;
        for (const auto& col : table.columns) {
            if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "bool") {
                ss << "        obj." << col.field_name() << " = sqlite3_column_int64(stmt, " << col_idx << ");\n";
            } else if (col.cpp_type() == "float" || col.cpp_type() == "double") {
                ss << "        obj." << col.field_name() << " = sqlite3_column_double(stmt, " << col_idx << ");\n";
            } else {
                ss << "        obj." << col.field_name()
                   << " = reinterpret_cast<const char*>(sqlite3_column_text(stmt, " << col_idx << "));\n";
            }
            ++col_idx;
        }
        ss << "        results.push_back(obj);\n";
        ss << "    }\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return results;\n";
        ss << "}\n\n";

        ss << "int64_t Database::count_" << pluralize(table.instance_name()) << "() {\n";
        ss << "    const char* sql = \"SELECT COUNT(*) FROM " << table.name << "\";\n";
        ss << "    sqlite3_stmt* stmt;\n";
        ss << "    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {\n";
        ss << "        return 0;\n";
        ss << "    }\n";
        ss << "    int64_t count = 0;\n";
        ss << "    if (sqlite3_step(stmt) == SQLITE_ROW) {\n";
        ss << "        count = sqlite3_column_int64(stmt, 0);\n";
        ss << "    }\n";
        ss << "    sqlite3_finalize(stmt);\n";
        ss << "    return count;\n";
        ss << "}\n\n";
    }

    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_handlers_h_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#pragma once\n\n";
    ss << "#include <crow.h>\n";
    ss << "\"" << namespace_ << "/models.h\"\n";
    ss << "\"" << namespace_ << "/database.h\"\n\nnamespace " << namespace_ << " {\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        String pk_type = pk ? pk->cpp_type() : "int64_t";
        String pk_name = pk ? pk->field_name() : "id";

        ss << "crow::response handle_get_" << table.instance_name() << "s();\n";
        ss << "crow::response handle_get_" << table.instance_name() << "(" << pk_type << " " << pk_name << ");\n";
        ss << "crow::response handle_create_" << table.instance_name() << "(const crow::request& req);\n";
        ss << "crow::response handle_update_" << table.instance_name() << "(" << pk_type << " " << pk_name
           << ", const crow::request& req);\n";
        ss << "crow::response handle_delete_" << table.instance_name() << "(" << pk_type << " " << pk_name << ");\n";
        ss << "crow::response handle_count_" << table.instance_name() << "s();\n\n";
    }

    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_handlers_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include \"" << namespace_ << "/handlers.h\"\n#include <sstream>\n\nnamespace " << namespace_ << " {\n\nusing json = nlohmann::json;\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        String pk_type = pk ? pk->cpp_type() : "int64_t";
        String pk_name = pk ? pk->field_name() : "id";

        ss << "crow::response handle_get_" << table.instance_name() << "s() {\n";
        ss << "    try {\n";
        ss << "        auto items = Database::instance().get_all_" << pluralize(table.instance_name()) << "();\n";
        ss << "        json arr = json::array();\n";
        ss << "        for (const auto& item : items) {\n";
        ss << "            arr.push_back(item.to_json());\n";
        ss << "        }\n";
        ss << "        return crow::response(arr.dump());\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";

        ss << "crow::response handle_get_" << table.instance_name() << "(" << pk_type << " " << pk_name << ") {\n";
        ss << "    try {\n";
        ss << "        auto item = Database::instance().get_" << table.instance_name() << "(" << pk_name << ");\n";
        ss << "        if (!item) {\n";
        ss << "            return crow::response(404, \"{\\\"error\\\": \\\"Not found\\\"}\");\n";
        ss << "        }\n";
        ss << "        return crow::response(item->to_json().dump());\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";

        ss << "crow::response handle_create_" << table.instance_name() << "(const crow::request& req) {\n";
        ss << "    try {\n";
        ss << "        auto body = json::parse(req.body);\n";
        ss << "        " << table.class_name() << " obj = " << table.class_name() << "::from_json(body);\n\n";
        ss << "        if (!obj.validate()) {\n";
        ss << "            return crow::response(400, \"{\\\"error\\\": \\\"Validation failed\\\"}\");\n";
        ss << "        }\n\n";
        ss << "        int64_t id = Database::instance().insert_" << table.instance_name() << "(obj);\n";
        ss << "        obj." << pk_name << " = id;\n\n";
        ss << "        return crow::response(201, obj.to_json().dump());\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";

        ss << "crow::response handle_update_" << table.instance_name() << "(" << pk_type << " " << pk_name
           << ", const crow::request& req) {\n";
        ss << "    try {\n";
        ss << "        auto existing = Database::instance().get_" << table.instance_name() << "(" << pk_name << ");\n";
        ss << "        if (!existing) {\n";
        ss << "            return crow::response(404, \"{\\\"error\\\": \\\"Not found\\\"}\");\n";
        ss << "        }\n\n";
        ss << "        auto body = json::parse(req.body);\n";
        ss << "        " << table.class_name() << " obj = " << table.class_name() << "::from_json(body);\n";
        ss << "        obj." << pk_name << " = " << pk_name << ";\n\n";
        ss << "        if (!obj.validate()) {\n";
        ss << "            return crow::response(400, \"{\\\"error\\\": \\\"Validation failed\\\"}\");\n";
        ss << "        }\n\n";
        ss << "        Database::instance().update_" << table.instance_name() << "(" << pk_name << ", obj);\n";
        ss << "        return crow::response(obj.to_json().dump());\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";

        ss << "crow::response handle_delete_" << table.instance_name() << "(" << pk_type << " " << pk_name << ") {\n";
        ss << "    try {\n";
        ss << "        bool deleted = Database::instance().delete_" << table.instance_name() << "(" << pk_name << ");\n";
        ss << "        if (!deleted) {\n";
        ss << "            return crow::response(404, \"{\\\"error\\\": \\\"Not found\\\"}\");\n";
        ss << "        }\n";
        ss << "        return crow::response(204);\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";

        ss << "crow::response handle_count_" << table.instance_name() << "s() {\n";
        ss << "    try {\n";
        ss << "        int64_t count = Database::instance().count_" << pluralize(table.instance_name()) << "();\n";
        ss << "        return crow::response(json{{{\"count\", count}}}.dump());\n";
        ss << "    } catch (const std::exception& e) {\n";
        ss << "        return crow::response(500, std::string(\"Error: \") + e.what());\n";
        ss << "    }\n";
        ss << "}\n\n";
    }

    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_routes_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include <crow.h>\n";
    ss << "\"" << namespace_ << "/handlers.h\"\n\nnamespace " << namespace_ << " {\n\nvoid setup_routes(crow::SimpleApp& app) {\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        String pk_type = pk ? pk->cpp_type() : "int64_t";
        String pk_name = pk ? pk->field_name() : "id";
        String plural = pluralize(table.kebab_name());

        ss << "\n    // " << table.class_name() << " routes\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "\").methods(crow::HTTPMethod::GET)\n";
        ss << "        (handle_get_" << table.instance_name() << "s);\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "/<" << pk_type << ">\")\n";
        ss << "        .methods(crow::HTTPMethod::GET)\n";
        ss << "        (handle_get_" << table.instance_name() << ");\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "\").methods(crow::HTTPMethod::POST)\n";
        ss << "        (handle_create_" << table.instance_name() << ");\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "/<" << pk_type << ">\")\n";
        ss << "        .methods(crow::HTTPMethod::PUT)\n";
        ss << "        (handle_update_" << table.instance_name() << ");\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "/<" << pk_type << ">\")\n";
        ss << "        .methods(crow::HTTPMethod::DELETE)\n";
        ss << "        (handle_delete_" << table.instance_name() << ");\n";
        ss << "    CROW_ROUTE(app, \"/api/" << plural << "/count\").methods(crow::HTTPMethod::GET)\n";
        ss << "        (handle_count_" << table.instance_name() << "s);\n";
    }

    ss << R"(
    CROW_ROUTE(app, "/health")([]() {
        return crow::response("OK");
    });
}

} // namespace )" << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_main_cpp_crow(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "#include <iostream>\n#include <crow.h>\n";
    ss << "\"" << namespace_ << "/database.h\"\n";
    ss << "\"" << namespace_ << "/routes.h\"\n\nint main(int argc, char* argv[]) {\n";
    ss << "    std::string db_path = \"data/" << project_name_ << ".db\";\n";
    ss << "    int port = 8080;\n\n";
    ss << "    for (int i = 1; i < argc - 1; ++i) {\n";
    ss << "        std::string arg = argv[i];\n";
    ss << "        if (arg == \"--db\" && i + 1 < argc) {\n";
    ss << "            db_path = argv[i + 1];\n";
    ss << "        }\n";
    ss << "        if (arg == \"--port\" && i + 1 < argc) {\n";
    ss << "            port = std::stoi(argv[i + 1]);\n";
    ss << "        }\n";
    ss << "    }\n\n";
    ss << "    try {\n";
    ss << "        Database::instance().open(db_path);\n";
    ss << "        Database::instance().migrate();\n\n";
    ss << "        crow::SimpleApp app;\n";
    ss << "        setup_routes(app);\n\n";
    ss << "        std::cout << \"Starting server on port \" << port << \"...\" << std::endl;\n";
    ss << "        app.port(port).multithreaded().run();\n\n";
    ss << "    } catch (const std::exception& e) {\n";
    ss << "        std::cerr << \"Error: \" << e.what() << std::endl;\n";
    ss << "        return 1;\n";
    ss << "    }\n\n";
    ss << "    return 0;\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_dto_h_crow(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "#pragma once\n\n";
    ss << "#include <crow.h>\n";
    ss << "\"" << namespace_ << "/models.h\"\n\nnamespace " << namespace_ << " {\n\nusing json = nlohmann::json;\n\n";
    ss << "struct ApiResponse {\n";
    ss << "    bool success;\n";
    ss << "    std::string message;\n";
    ss << "    json data;\n";
    ss << "};\n\n";
    ss << "} // namespace " << namespace_ << "\n";
    return ss.str();
}

String Generator::generate_conanfile_crow(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "from conans import ConanFile, CMake, tools\n";
    ss << "from conan.tools.cmake import CMakeToolchain, CMakeDeps\n";
    ss << "from conan.tools.layout import cmake_layout\n\n";
    ss << "class " << to_pascal(project_name_) << "Conan(ConanFile):\n";
    ss << "    name = \"" << project_name_ << "\"\n";
    ss << "    version = \"1.0.0\"\n";
    ss << "    license = \"MIT\"\n";
    ss << "    description = \"Auto-generated C++ REST API\"\n";
    ss << "    topics = (\"rest\", \"api\", \"crow\", \"cpp\")\n\n";
    ss << "    settings = \"os\", \"compiler\", \"build_type\", \"arch\"\n";
    ss << "    options = {\"shared\": [True, False], \"fPIC\": [True, False]}\n";
    ss << "    default_options = {\"shared\": False, \"fPIC\": True}\n\n";
    ss << "    def requirements(self):\n";
    ss << "        self.requires(\"sqlite3/3.45.0\")\n";
    ss << "        self.requires(\"nlohmann_json/3.11.3\")\n\n";
    ss << "    def build_requirements(self):\n";
    ss << "        self.tool_requires(\"cmake/3.25.0\")\n\n";
    ss << "    def layout(self):\n";
    ss << "        cmake_layout(self)\n\n";
    ss << "    def generate(self):\n";
    ss << "        tc = CMakeToolchain(self)\n";
    ss << "        tc.generate()\n";
    ss << "        deps = CMakeDeps(self)\n";
    ss << "        deps.generate()\n\n";
    ss << "    def build(self):\n";
    ss << "        cmake = CMake(self)\n";
    ss << "        cmake.configure()\n";
    ss << "        cmake.build()\n\n";
    ss << "    def package(self):\n";
    ss << "        cmake = CMake(self)\n";
    ss << "        cmake.install()\n";
    return ss.str();
}

String Generator::generate_gitignore_crow() {
    return R"(build/
*.exe
*.dll
*.so
*.dylib
*.a
*.lib
*.obj
*.o
*.pdb
*.ilk
*.exp
.vscode/
.idea/
*.swp
*.swo
*~
CMakeSettings.json
Testing/
*.trs
*.log
*.db
*.db-journal
data/
conanbuildinfo.txt
conaninfo.txt
cmake-build-/
)";
}

String Generator::generate_readme_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "# " << project_name_ << "\n\n";
    ss << "Auto-generated C++ REST API project using NovaGen + Crow framework.\n\n";
    ss << "## Requirements\n\n";
    ss << "- C++17 compiler (GCC 11+, Clang 14+, MSVC 2019+)\n";
    ss << "- CMake 3.20+\n";
    ss << "- SQLite3 development libraries\n\n";
    ss << "## Building\n\n";
    ss << "```bash\n";
    ss << "mkdir build && cd build\n";
    ss << "cmake .. -DCMAKE_BUILD_TYPE=Release\n";
    ss << "cmake --build . -j$(nproc)\n";
    ss << "```\n\n";
    ss << "## Running\n\n";
    ss << "```bash\n";
    ss << "./" << project_name_ << " --port 8080 --db data/" << project_name_ << ".db\n";
    ss << "```\n\n";
    ss << "## API Endpoints\n\n";
    ss << "| Method | Endpoint | Description |\n";
    ss << "|--------|----------|-------------|\n";
    ss << "| GET | /api | API info |\n";
    ss << "| GET | /health | Health check |\n";

    for (const auto& table : tables) {
        String plural = pluralize(table.kebab_name());
        ss << "| GET | /api/" << plural << " | List all " << table.name << " |\n";
        ss << "| GET | /api/" << plural << "/{id} | Get " << table.name << " by ID |\n";
        ss << "| POST | /api/" << plural << " | Create " << table.name << " |\n";
        ss << "| PUT | /api/" << plural << "/{id} | Update " << table.name << " |\n";
        ss << "| DELETE | /api/" << plural << "/{id} | Delete " << table.name << " |\n";
        ss << "| GET | /api/" << plural << "/count | Count " << table.name << " |\n\n";
    }

    ss << "Generated by NovaGen v1.0.0\n";
    return ss.str();
}

String Generator::generate_tests_cmake_crow(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(cmake_minimum_required(VERSION 3.20)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable()"
       << " " << project_name_ << R"(_tests
    test_models.cpp
    test_database.cpp
)

target_include_directories()"
       << " " << project_name_ << R"(_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries()"
       << " " << project_name_ << R"(_tests PRIVATE
    )" << project_name_ << R"(
    gtest_main
    ${SQLITE3_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests()"
       << " " << project_name_ << "_tests"
       << "\n";
    return ss.str();
}

String Generator::generate_test_models_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include <gtest/gtest.h>\n";
    ss << "\"" << namespace_ << "/models.h\"\n\nusing namespace " << namespace_ << ";\n\n";

    for (const auto& table : tables) {
        ss << "TEST(" << project_name_ << "ModelTest, Test" << table.class_name() << "Creation) {\n";
        ss << "    " << table.class_name() << " obj;\n";
        if (table.pk_column()) {
            ss << "    obj." << table.pk_column()->field_name() << " = 1;\n";
        }
        for (const auto& col : table.columns) {
            if (!col.primary_key) {
                if (col.cpp_type() == "std::string") {
                    ss << "    obj." << col.field_name() << " = \"test\";\n";
                } else {
                    ss << "    obj." << col.field_name() << " = 0;\n";
                }
            }
        }
        ss << "    json j = obj.to_json();\n";
        ss << "    EXPECT_TRUE(j.contains(\"" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "\"));\n";
        ss << "    " << table.class_name() << " parsed = " << table.class_name() << "::from_json(j);\n";
        ss << "    EXPECT_TRUE(parsed.validate());\n";
        ss << "}\n\n";
    }

    ss << "int main(int argc, char** argv) {\n";
    ss << "    ::testing::InitGoogleTest(&argc, argv);\n";
    ss << "    return RUN_ALL_TESTS();\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_test_database_cpp_crow(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "#include <gtest/gtest.h>\n";
    ss << "\"" << namespace_ << "/database.h\"\n";
    ss << "#include <filesystem>\n\n";
    ss << "using namespace " << namespace_ << ";\n\n";
    ss << "class " << project_name_ << "DatabaseTest : public ::testing::Test {\n";
    ss << "protected:\n";
    ss << "    void SetUp() override {\n";
    ss << "        test_db_path = \"data/test_" << project_name_ << ".db\";\n";
    ss << "        std::filesystem::create_directories(\"data\");\n";
    ss << "        std::filesystem::remove(test_db_path);\n";
    ss << "        Database::instance().open(test_db_path);\n";
    ss << "        Database::instance().migrate();\n";
    ss << "    }\n\n";
    ss << "    void TearDown() override {\n";
    ss << "        Database::instance().close();\n";
    ss << "        std::filesystem::remove(test_db_path);\n";
    ss << "    }\n\n";
    ss << "    std::string test_db_path;\n";
    ss << "};\n\n";
    ss << "TEST_F(" << project_name_ << "DatabaseTest, TestConnection) {\n";
    ss << "    EXPECT_NO_THROW(Database::instance().migrate());\n";
    ss << "}\n\n";

    for (const auto& table : tables) {
        ss << "TEST_F(" << project_name_ << "DatabaseTest, Test" << table.class_name() << "Crud) {\n";
        ss << "    " << table.class_name() << " obj;\n";
        if (table.pk_column()) {
            ss << "    obj." << table.pk_column()->field_name() << " = 0;\n";
        }
        for (const auto& col : table.columns) {
            if (!col.primary_key && col.cpp_type() == "std::string") {
                ss << "    obj." << col.field_name() << " = \"test_value\";\n";
            }
        }
        ss << "\n    int64_t id = Database::instance().insert_" << table.instance_name() << "(obj);\n";
        ss << "    EXPECT_GT(id, 0);\n\n";
        ss << "    auto retrieved = Database::instance().get_" << table.instance_name() << "(id);\n";
        ss << "    ASSERT_TRUE(retrieved.has_value());\n\n";
        ss << "    Database::instance().update_" << table.instance_name() << "(id, obj);\n\n";
        ss << "    bool deleted = Database::instance().delete_" << table.instance_name() << "(id);\n";
        ss << "    EXPECT_TRUE(deleted);\n";
        ss << "}\n\n";
    }

    return ss.str();
}

String Generator::generate_security_jwt_crow() {
    std::stringstream ss;
    ss << R"(
#pragma once

#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <string>
#include <chrono>

class JWTMiddleware {
public:
    struct Context {
        std::string user_id;
        std::string role;
        bool authenticated = false;
    };

    JWTMiddleware(const std::string& secret_key)
        : secret_key_(secret_key) {}

    Context validate_token(const std::string& token) {
        Context ctx;
        try {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                .with_issuer("novagen")
                .allow_algorithm(jwt::algorithm::hs256{secret_key_});

            verifier.verify(decoded);

            ctx.user_id = decoded.get_payload_claim("user_id").as_string();
            ctx.role = decoded.get_payload_claim("role").as_string();
            ctx.authenticated = true;
        } catch (const std::exception&) {
            ctx.authenticated = false;
        }
        return ctx;
    }

    std::string generate_token(const std::string& user_id, const std::string& role) {
        auto token = jwt::create()
            .set_issuer("novagen")
            .set_payload_claim("user_id", jwt::claim(user_id))
            .set_payload_claim("role", jwt::claim(role))
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
            .sign(jwt::algorithm::hs256{secret_key_});
        return token;
    }

private:
    std::string secret_key_;
};

#define JWT_AUTHMiddleware(secret) \
    ([](crow::request& req, crow::response& res) { \
        auto token = req.get_header_value("Authorization"); \
        if (token.substr(0, 7) != "Bearer ") { \
            res.code = 401; \
            res.body = "{\"error\": \"Missing or invalid authorization header\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
        std::string actual_token = token.substr(7); \
        JWTMiddleware jwt(secret); \
        auto ctx = jwt.validate_token(actual_token); \
        if (!ctx.authenticated) { \
            res.code = 401; \
            res.body = "{\"error\": \"Invalid or expired token\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
    })

#endif // JWT_MIDDLEWARE_H_
)";
    return ss.str();
}

String Generator::generate_security_oauth2_crow() {
    std::stringstream ss;
    ss << R"(
#pragma once

#include <crow.h>
#include <string>
#include <map>

class OAuth2Middleware {
public:
    OAuth2Middleware(const std::string& provider_url, const std::string& client_id)
        : provider_url_(provider_url), client_id_(client_id) {}

    struct TokenInfo {
        std::string access_token;
        std::string token_type;
        int expires_in;
        std::string scope;
        bool valid = false;
    };

    TokenInfo validate_token(const std::string& bearer_token) {
        TokenInfo info;
        info.access_token = bearer_token;

        // In production, validate with OAuth2 provider
        // For now, check Bearer token format
        if (bearer_token.length() > 20) {
            info.valid = true;
            info.token_type = "Bearer";
            info.expires_in = 3600;
        }
        return info;
    }

    std::string get_authorization_url(const std::string& redirect_uri, const std::string& state) {
        return provider_url_ + "/authorize?client_id=" + client_id_ +
               "&redirect_uri=" + redirect_uri + "&state=" + state + "&response_type=code";
    }

    std::string exchange_code_for_token(const std::string& code, const std::string& redirect_uri) {
        // In production, make POST request to token endpoint
        return "access_token_placeholder";
    }

private:
    std::string provider_url_;
    std::string client_id_;
};

#define OAuth2_AUTHMiddleware(provider_url, client_id) \
    ([](crow::request& req, crow::response& res) { \
        auto auth_header = req.get_header_value("Authorization"); \
        if (auth_header.empty() || auth_header.substr(0, 7) != "Bearer ") { \
            res.code = 401; \
            res.body = "{\"error\": \"Missing or invalid bearer token\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
        OAuth2Middleware oauth(provider_url, client_id); \
        auto info = oauth.validate_token(auth_header.substr(7)); \
        if (!info.valid) { \
            res.code = 401; \
            res.body = "{\"error\": \"Invalid or expired token\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
    })

#endif // OAUTH2_MIDDLEWARE_H_
)";
    return ss.str();
}

String Generator::generate_security_saml_crow() {
    std::stringstream ss;
    ss << R"(
#pragma once

#include <crow.h>
#include <string>
#include <xmlsec/xmlsec.h>
#include <xmlsec(keys.h>
#include <xmlsec(keys token.h>

class SAMLMiddleware {
public:
    SAMLMiddleware(const std::string& idp_metadata_path, const std::string& sp_entity_id)
        : idp_metadata_path_(idp_metadata_path), sp_entity_id_(sp_entity_id) {}

    struct SAMLAssertion {
        std::string name_id;
        std::string session_index;
        std::map<std::string, std::string> attributes;
        bool valid = false;
    };

    SAMLAssertion validate_response(const std::string& saml_response) {
        SAMLAssertion assertion;

        // In production, validate SAML Response XML signature using xmlsec
        // Decrypt Assertion, verify signature against IdP public key

        if (!saml_response.empty()) {
            assertion.valid = true;
            assertion.name_id = "user@example.com";
            assertion.session_index = "session123";
        }
        return assertion;
    }

    std::string generate_authn_request(const std::string& redirect_uri) {
        return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
               "<samlp:AuthnRequest xmlns:samlp=\"urn:oasis:names:tc:SAML:2.0:protocol\" "
               "ID=\"_123\" Version=\"2.0\" IssueInstant=\"2024-01-01T00:00:00Z\" "
               "AssertionConsumerServiceURL=\"" + redirect_uri + "\" "
               "ProtocolBinding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\">"
               "<saml:Issuer xmlns:saml=\"urn:oasis:names:tc:SAML:2.0:assertion\">" +
               sp_entity_id_ + "</saml:Issuer>"
               "</samlp:AuthnRequest>";
    }

private:
    std::string idp_metadata_path_;
    std::string sp_entity_id_;
};

#define SAML_AUTHMiddleware(idp_path, sp_entity) \
    ([](crow::request& req, crow::response& res) { \
        auto saml_response = req.get_header_value("X-SAML-Response"); \
        if (saml_response.empty()) { \
            res.code = 401; \
            res.body = "{\"error\": \"SAML authentication required\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
        SAMLMiddleware saml(idp_path, sp_entity); \
        auto assertion = saml.validate_response(saml_response); \
        if (!assertion.valid) { \
            res.code = 401; \
            res.body = "{\"error\": \"Invalid SAML assertion\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
    })

#endif // SAML_MIDDLEWARE_H_
)";
    return ss.str();
}

String Generator::generate_security_owasp_crow() {
    std::stringstream ss;
    ss << R"OWASP_CROW(
#pragma once

#include <crow.h>
#include <string>
#include <regex>

class OWASPMiddleware {
public:
    OWASPMiddleware() {
        xss_patterns_ = {
            std::regex("<script[^>]*>.*?</script>", std::regex::icase),
            std::regex("javascript:", std::regex::icase),
            std::regex("on\\w+\\s*=", std::regex::icase)
        };

        sql_patterns_ = {
            std::regex("(\\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|CREATE|TRUNCATE|EXEC|EXECUTE)\\b)", std::regex::icase),
            std::regex("--"),
            std::regex("/\\*.*\\*/")
        };
    }

    struct SecurityHeaders {
        std::string x_content_type_options = "nosniff";
        std::string x_frame_options = "DENY";
        std::string x_xss_protection = "1; mode=block";
        std::string strict_transport_security = "max-age=31536000; includeSubDomains";
        std::string content_security_policy = "default-src 'self'";
        std::string referrer_policy = "strict-origin-when-cross-origin";
    };

    void apply_security_headers(crow::response& res) {
        SecurityHeaders headers;
        res.set_header("X-Content-Type-Options", headers.x_content_type_options);
        res.set_header("X-Frame-Options", headers.x_frame_options);
        res.set_header("X-XSS-Protection", headers.x_xss_protection);
        res.set_header("Strict-Transport-Security", headers.strict_transport_security);
        res.set_header("Content-Security-Policy", headers.content_security_policy);
        res.set_header("Referrer-Policy", headers.referrer_policy);
    }

    bool check_xss(const std::string& input) {
        for (const auto& pattern : xss_patterns_) {
            if (std::regex_search(input, pattern)) {
                return true;
            }
        }
        return false;
    }

    bool check_sql_injection(const std::string& input) {
        for (const auto& pattern : sql_patterns_) {
            if (std::regex_search(input, pattern)) {
                return true;
            }
        }
        return false;
    }

    bool sanitize_input(std::string& input) {
        // Remove or encode potentially dangerous characters
        std::regex special_chars("[<>\"'&]");
        input = std::regex_replace(input, special_chars, "");
        return true;
    }

private:
    std::vector<std::regex> xss_patterns_;
    std::vector<std::regex> sql_patterns_;
};

#define OWASP_PROTECT \
    ([](crow::request& req, crow::response& res) { \
        OWASPMiddleware owasp; \
        owasp.apply_security_headers(res); \
        \
        auto body = req.body; \
        if (owasp.check_xss(body) || owasp.check_sql_injection(body)) { \
            res.code = 400; \
            res.body = "{\"error\": \"Potentially malicious input detected\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
    })

#endif // OWASP_MIDDLEWARE_H_
)OWASP_CROW";
    return ss.str();
}

String Generator::generate_security_ldap_crow() {
    std::stringstream ss;
    ss << R"LDAP_CROW(
#pragma once

#include <crow.h>
#include <string>
#include <ldap.h>

class LDAPMiddleware {
public:
    LDAPMiddleware(const std::string& ldap_url, const std::string& base_dn)
        : ldap_url_(ldap_url), base_dn_(base_dn) {}

    struct LDAPConfig {
        int version = LDAP_VERSION3;
        int timeout = 30;
        bool start_tls = true;
    };

    struct UserInfo {
        std::string dn;
        std::string cn;
        std::vector<std::string> groups;
        bool authenticated = false;
    };

    bool connect() {
        int result = ldap_initialize(&ldap_connection_, ldap_url_.c_str());
        if (result != LDAP_SUCCESS) {
            return false;
        }

        result = ldap_set_option(ldap_connection_, LDAP_OPT_PROTOCOL_VERSION, &config_.version);
        if (result != LDAP_SUCCESS) {
            return false;
        }

        if (config_.start_tls) {
            result = ldap_start_tls_s(ldap_connection_, NULL, NULL);
        }

        return result == LDAP_SUCCESS;
    }

    UserInfo authenticate(const std::string& username, const std::string& password) {
        UserInfo user;

        std::string user_dn = "uid=" + username + "," + base_dn_;

        int result = ldap_simple_bind_s(ldap_connection_, user_dn.c_str(), password.c_str());

        if (result == LDAP_SUCCESS) {
            user.authenticated = true;
            user.dn = user_dn;
            user.cn = username;

            // Search for user's groups
            std::string search_filter = "(uid=" + username + ")";
            LDAPMessage* results = NULL;
            result = ldap_search_ext_s(
                ldap_connection_,
                base_dn_.c_str(),
                LDAP_SCOPE_SUBTREE,
                search_filter.c_str(),
                NULL,
                0,
                NULL,
                NULL,
                NULL,
                0,
                &results
            );

            if (result == LDAP_SUCCESS && results) {
                user.groups.push_back("users");
            }
        }

        return user;
    }

    void disconnect() {
        if (ldap_connection_) {
            ldap_unbind_ext_s(ldap_connection_, NULL, NULL);
            ldap_connection_ = nullptr;
        }
    }

    ~LDAPMiddleware() {
        disconnect();
    }

private:
    std::string ldap_url_;
    std::string base_dn_;
    LDAP* ldap_connection_ = nullptr;
    LDAPConfig config_;
};

#define LDAP_AUTHMiddleware(ldap_url, base_dn) \
    ([](crow::request& req, crow::response& res) { \
        auto auth_header = req.get_header_value("Authorization"); \
        if (auth_header.empty()) { \
            res.code = 401; \
            res.body = "{\"error\": \"Basic authentication required\"}"; \
            res.set_header("Content-Type", "application/json"); \
            res.set_header("WWW-Authenticate", \"Basic realm=\\"API\\"\"); \
            return; \
        } \
        \
        if (auth_header.substr(0, 6) != "Basic ") { \
            res.code = 401; \
            res.body = "{\"error\": \"Invalid authentication scheme\"}"; \
            res.set_header("Content-Type", "application/json"); \
            return; \
        } \
        \
        std::string credentials = auth_header.substr(6); \
        // Decode base64 credentials (username:password) \
        // For production, use proper base64 decoding \
    })

#endif // LDAP_MIDDLEWARE_H_
)LDAP_CROW";
    return ss.str();
}

bool Generator::generate_react(const fs::path& base, const std::vector<Table>& tables) {
    try {
        bool is_ts = react_framework_ == ReactFramework::TypeScript;
        String ext = is_ts ? ".tsx" : ".jsx";
        String js_ext = is_ts ? ".ts" : ".js";
        String store_ext = is_ts ? "StoreContext.tsx" : "StoreContext.jsx";

        if (!create_directory(base)) return false;
        if (!create_directory(base / "src" / "components" / "services")) return false;
        if (!create_directory(base / "src" / "store")) return false;

        if (!write_file(base / "package.json", generate_react_package_json(tables))) return false;

        if (react_bundler_ == ReactBundler::Vite) {
            if (!write_file(base / "vite.config.js", generate_react_vite_config(tables))) return false;
            if (!write_file(base / "postcss.config.js", "export default {\n  plugins: {\n    tailwindcss: {},\n    autoprefixer: {},\n  },\n}")) return false;
        } else if (react_bundler_ == ReactBundler::Webpack) {
            if (!write_file(base / "webpack.config.js", generate_react_webpack_config(tables))) return false;
            if (is_ts) {
                if (!write_file(base / "tsconfig.json", generate_react_tsconfig(tables))) return false;
            }
        } else {
            if (!write_file(base / "cra.config.js", generate_react_cra_config(tables))) return false;
        }

        if (!write_file(base / "index.html", generate_react_index_html(tables))) return false;

        if (is_ts) {
            if (!write_file(base / "src" / ("main" + ext), generate_react_main_tsx(tables))) return false;
        } else {
            if (!write_file(base / "src" / ("main" + ext), generate_react_main_jsx(tables))) return false;
        }

        if (react_css_ == ReactCSS::Tailwind) {
            if (!write_file(base / "tailwind.config.js", generate_react_tailwind_config(tables))) return false;
            if (!write_file(base / "src" / "index.css", "@tailwind base;\n@tailwind components;\n@tailwind utilities;")) return false;
        } else if (react_css_ == ReactCSS::Bootstrap) {
            if (!write_file(base / "src" / "index.css", generate_react_bootstrap_css(tables))) return false;
        } else {
            if (!write_file(base / "src" / "index.css", generate_react_index_css(tables))) return false;
        }

        if (react_state_ == ReactState::Context) {
            if (is_ts) {
                if (!write_file(base / "src" / "store" / store_ext, generate_react_store_context_ts(tables))) return false;
            } else {
                if (!write_file(base / "src" / "store" / store_ext, generate_react_store_context(tables))) return false;
            }
        } else if (react_state_ == ReactState::Zustand) {
            if (is_ts) {
                if (!write_file(base / "src" / "store" / ("useStore" + js_ext), generate_react_store_zustand_ts(tables))) return false;
            } else {
                if (!write_file(base / "src" / "store" / ("useStore" + js_ext), generate_react_store_zustand(tables))) return false;
            }
        } else if (react_state_ == ReactState::Redux) {
            if (!create_directory(base / "src" / "store" / "slices")) return false;
            if (is_ts) {
                if (!write_file(base / "src" / "store" / ("index" + js_ext), generate_react_store_redux_ts(tables))) return false;
            } else {
                if (!write_file(base / "src" / "store" / ("index" + js_ext), generate_react_store_redux(tables))) return false;
            }
        }

        if (is_ts) {
            if (!write_file(base / "src" / ("App" + ext), generate_react_app_tsx(tables))) return false;
            if (!write_file(base / "src" / "services" / ("api" + js_ext), generate_react_api_service_ts(tables))) return false;
        } else {
            if (!write_file(base / "src" / ("App" + ext), generate_react_app_jsx(tables))) return false;
            if (!write_file(base / "src" / "services" / ("api" + js_ext), generate_react_api_service(tables))) return false;
        }

        if (!write_file(base / "nginx.conf", generate_react_nginx_conf(tables))) return false;
        if (!write_file(base / "Dockerfile", generate_react_dockerfile(tables))) return false;
        if (!write_file(base / "docker-compose.yml", generate_react_docker_compose_yml(tables))) return false;
        if (!write_file(base / ".gitignore", generate_react_gitignore())) return false;
        if (!write_file(base / "README.md", generate_react_readme(tables))) return false;

        for (const auto& table : tables) {
            if (is_ts) {
                if (!write_file(base / "src" / "components" / (table.class_name() + "List" + ext),
                              generate_react_component_list_ts(table))) return false;
                if (!write_file(base / "src" / "components" / (table.class_name() + "Form" + ext),
                              generate_react_component_form_ts(table))) return false;
            } else {
                if (!write_file(base / "src" / "components" / (table.class_name() + "List" + ext),
                              generate_react_component_list(table))) return false;
                if (!write_file(base / "src" / "components" / (table.class_name() + "Form" + ext),
                              generate_react_component_form(table))) return false;
            }
        }

        return true;
    } catch (const std::exception& e) {
        last_error_ = "React generation failed: " + String(e.what());
        return false;
    }
}

} // namespace novagen
