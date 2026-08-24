#pragma once

#include "model.hpp"
#include <filesystem>
#include <string>

namespace novagen {

namespace fs = std::filesystem;

enum class Framework {
    CrowCpp,
    Flask,
    FastAPI,
    SpringBoot
};

enum class FrontendType {
    React,
    Angular,
    Android
};

enum class ReactBundler {
    Vite,
    CRA,
    Webpack
};

enum class ReactFramework {
    JavaScript,
    TypeScript
};

enum class ReactCSS {
    Tailwind,
    Bootstrap,
    Plain
};

enum class ReactTheme {
    Light,
    Dark,
    Auto
};

enum class ReactState {
    Context,
    Zustand,
    Redux
};

enum class AngularState {
    Service,
    NgRx,
    Redux,
    Signals
};

enum class SecurityType {
    None,
    JWT,
    OAuth2,
    SAML,
    OWASP,
    LDAP
};

enum class ORMType {
    SQLAlchemy,
    Slack,
    Peewee
};

enum class AngularCSS {
    Tailwind,
    Bootstrap,
    Plain,
    Material
};

enum class AngularHTTP {
    HttpClient,
    Fetch,
    Axios
};

enum class AngularBundler {
    Esbuild,
    Webpack
};

enum class AndroidLanguage {
    Kotlin,
    Java
};

enum class AndroidArchitecture {
    MVVM,
    Clean
};

class Generator {
public:
    Generator() = default;
    explicit Generator(const String& project_name,
                      const String& namespace_name = "app",
                      const String& output_dir = "./output",
                      Framework framework = Framework::CrowCpp);

    void set_project_name(const String& name);
    void set_namespace(const String& ns);
    void set_output_dir(const String& dir);
    void set_framework(Framework framework);
    void set_generate_frontend(bool generate) { generate_frontend_ = generate; }
    void set_frontend_type(const String& type);
    void set_react_bundler(const String& bundler);
    void set_react_framework(const String& framework);
    void set_react_css(const String& css);
    void set_react_theme(const String& theme);
    void set_react_state(const String& state);
    void set_angular_standalone(bool standalone) { angular_standalone_ = standalone; }
    void set_angular_guards(bool guards) { angular_guards_ = guards; }
    void set_angular_admin(bool admin) { angular_admin_ = admin; }
    void set_angular_css(const String& css);
    void set_angular_state(const String& state);
    void set_angular_http(const String& http);
    void set_angular_bundler(const String& bundler);
    void set_security(const String& security);
    void set_android_language(const String& language);
    void set_android_architecture(const String& arch);
    void set_orm(const String& orm);

    bool generate(const std::vector<Table>& tables);
    bool generate_from_sql(const String& sql);

    String last_error() const;
    Framework framework() const { return framework_; }
    bool generate_frontend() const { return generate_frontend_; }
    FrontendType frontend_type() const { return frontend_type_; }
    ReactBundler react_bundler() const { return react_bundler_; }
    ReactFramework react_framework() const { return react_framework_; }
    ReactCSS react_css() const { return react_css_; }
    ReactTheme react_theme() const { return react_theme_; }
    ReactState react_state() const { return react_state_; }
    bool angular_standalone() const { return angular_standalone_; }
    bool angular_guards() const { return angular_guards_; }
    bool angular_admin() const { return angular_admin_; }
    AngularCSS angular_css() const { return angular_css_; }
    AngularState angular_state() const { return angular_state_; }
    AngularHTTP angular_http() const { return angular_http_; }
    AngularBundler angular_bundler() const { return angular_bundler_; }
    SecurityType security() const { return security_; }
    AndroidLanguage android_language() const { return android_language_; }
    AndroidArchitecture android_architecture() const { return android_architecture_; }
    ORMType orm() const { return orm_; }

private:
    String project_name_{"api_server"};
    String namespace_{"app"};
    String output_dir_{"./output"};
    Framework framework_{Framework::CrowCpp};
    bool generate_frontend_{false};
    FrontendType frontend_type_{FrontendType::React};
    ReactBundler react_bundler_{ReactBundler::Vite};
    ReactFramework react_framework_{ReactFramework::TypeScript};
    ReactCSS react_css_{ReactCSS::Tailwind};
    ReactTheme react_theme_{ReactTheme::Light};
    ReactState react_state_{ReactState::Context};
    bool angular_standalone_{true};
    bool angular_guards_{false};
    bool angular_admin_{false};
    AngularCSS angular_css_{AngularCSS::Tailwind};
    AngularState angular_state_{AngularState::Service};
    AngularHTTP angular_http_{AngularHTTP::HttpClient};
    AngularBundler angular_bundler_{AngularBundler::Esbuild};
    SecurityType security_{SecurityType::None};
    AndroidLanguage android_language_{AndroidLanguage::Kotlin};
    AndroidArchitecture android_architecture_{AndroidArchitecture::MVVM};
    ORMType orm_{ORMType::SQLAlchemy};
    String last_error_;

    bool create_directory(const std::filesystem::path& path);
    bool write_file(const std::filesystem::path& path, const String& content);
    String replace_all(const String& s, const String& from, const String& to);

    // Crow C++ generators
    String generate_cmake_crow(const std::vector<Table>& tables);
    String generate_models_h_crow(const std::vector<Table>& tables);
    String generate_models_cpp_crow(const std::vector<Table>& tables);
    String generate_database_h_crow(const std::vector<Table>& tables);
    String generate_database_cpp_crow(const std::vector<Table>& tables);
    String generate_handlers_h_crow(const std::vector<Table>& tables);
    String generate_handlers_cpp_crow(const std::vector<Table>& tables);
    String generate_routes_cpp_crow(const std::vector<Table>& tables);
    String generate_main_cpp_crow(const std::vector<Table>& tables);
    String generate_dto_h_crow(const std::vector<Table>& tables);
    String generate_conanfile_crow(const std::vector<Table>& tables);
    String generate_gitignore_crow();
    String generate_readme_crow(const std::vector<Table>& tables);
    String generate_tests_cmake_crow(const std::vector<Table>& tables);
    String generate_test_models_cpp_crow(const std::vector<Table>& tables);
    String generate_test_database_cpp_crow(const std::vector<Table>& tables);
    String generate_security_jwt_crow();
    String generate_security_oauth2_crow();
    String generate_security_saml_crow();
    String generate_security_owasp_crow();
    String generate_security_ldap_crow();

    // Flask generators
    String generate_flask_models_py(const std::vector<Table>& tables);
    String generate_flask_routes_py(const std::vector<Table>& tables);
    String generate_flask_app_py(const std::vector<Table>& tables);
    String generate_flask_config_py();
    String generate_flask_requirements_txt();
    String generate_flask_api_endpoints_json(const std::vector<Table>& tables);
    String generate_flask_durable_client_py();
    String generate_flask_run_sh();
    String generate_flask_gitignore();
    String generate_flask_readme(const std::vector<Table>& tables);
    String generate_flask_dockerfile(const std::vector<Table>& tables);
    String generate_flask_docker_compose_yml();
    String generate_flask_test_models_py(const std::vector<Table>& tables);
    String generate_flask_test_routes_py(const std::vector<Table>& tables);
    String generate_flask_pytest_ini();
    String generate_security_jwt_flask();
    String generate_security_oauth2_flask();
    String generate_security_saml_flask();
    String generate_security_owasp_flask();
    String generate_security_ldap_flask();

    // FastAPI generators
    String generate_fastapi_main_py(const std::vector<Table>& tables);
    String generate_fastapi_models_py(const std::vector<Table>& tables);
    String generate_fastapi_schemas_py(const std::vector<Table>& tables);
    String generate_fastapi_database_py(const std::vector<Table>& tables);
    String generate_fastapi_routers_py(const std::vector<Table>& tables);
    String generate_fastapi_requirements_txt();
    String generate_fastapi_api_endpoints_json(const std::vector<Table>& tables);
    String generate_fastapi_alembic_ini(const std::vector<Table>& tables);
    String generate_fastapi_alembic_env_py(const std::vector<Table>& tables);
    String generate_fastapi_durable_client_py();
    String generate_fastapi_dockerfile(const std::vector<Table>& tables);
    String generate_fastapi_docker_compose_yml();
    String generate_fastapi_gitignore();
    String generate_fastapi_readme(const std::vector<Table>& tables);
    String generate_fastapi_pytest_ini();
    String generate_fastapi_test_models_py(const std::vector<Table>& tables);
    String generate_fastapi_test_routers_py(const std::vector<Table>& tables);
    String generate_security_jwt_fastapi();
    String generate_security_oauth2_fastapi();
    String generate_security_saml_fastapi();
    String generate_security_owasp_fastapi();
    String generate_security_ldap_fastapi();

    // React frontend generators
    String generate_react_package_json(const std::vector<Table>& tables);
    String generate_react_vite_config(const std::vector<Table>& tables);
    String generate_react_cra_config(const std::vector<Table>& tables);
    String generate_react_webpack_config(const std::vector<Table>& tables);
    String generate_react_tsconfig(const std::vector<Table>& tables);
    String generate_react_index_html(const std::vector<Table>& tables);
    String generate_react_main_tsx(const std::vector<Table>& tables);
    String generate_react_main_jsx(const std::vector<Table>& tables);
    String generate_react_index_css(const std::vector<Table>& tables);
    String generate_react_tailwind_config(const std::vector<Table>& tables);
    String generate_react_bootstrap_css(const std::vector<Table>& tables);
    String generate_react_app_tsx(const std::vector<Table>& tables);
    String generate_react_app_jsx(const std::vector<Table>& tables);
    String generate_react_store_context_ts(const std::vector<Table>& tables);
    String generate_react_store_context(const std::vector<Table>& tables);
    String generate_react_store_zustand_ts(const std::vector<Table>& tables);
    String generate_react_store_zustand(const std::vector<Table>& tables);
    String generate_react_store_redux_ts(const std::vector<Table>& tables);
    String generate_react_store_redux(const std::vector<Table>& tables);
    String generate_react_api_service_ts(const std::vector<Table>& tables);
    String generate_react_api_service(const std::vector<Table>& tables);
    String generate_react_component_list_ts(const Table& table);
    String generate_react_component_list(const Table& table);
    String generate_react_component_form_ts(const Table& table);
    String generate_react_component_form(const Table& table);
    String generate_react_dockerfile(const std::vector<Table>& tables);
    String generate_react_nginx_conf(const std::vector<Table>& tables);
    String generate_react_docker_compose_yml(const std::vector<Table>& tables);
    String generate_react_gitignore();
    String generate_react_readme(const std::vector<Table>& tables);
    bool generate_react(const fs::path& base, const std::vector<Table>& tables);

    // Angular frontend generators
    String generate_angular_package_json(const std::vector<Table>& tables);
    String generate_angular_angular_json(const std::vector<Table>& tables);
    String generate_angular_tsconfig_base(const std::vector<Table>& tables);
    String generate_angular_tsconfig_app(const std::vector<Table>& tables);
    String generate_angular_index_html(const std::vector<Table>& tables);
    String generate_angular_main_ts(const std::vector<Table>& tables);
    String generate_angular_app_component_ts(const std::vector<Table>& tables);
    String generate_angular_app_component_html(const std::vector<Table>& tables);
    String generate_angular_app_component_css(const std::vector<Table>& tables);
    String generate_angular_app_routing_module(const std::vector<Table>& tables);
    String generate_angular_app_module(const std::vector<Table>& tables);
    String generate_angular_models(const std::vector<Table>& tables);
    String generate_angular_service(const std::vector<Table>& tables);
    String generate_angular_auth_guard(const std::vector<Table>& tables);
    String generate_angular_admin_guard(const std::vector<Table>& tables);
    String generate_angular_auth_service(const std::vector<Table>& tables);
    String generate_angular_auth_interceptor(const std::vector<Table>& tables);
    String generate_angular_login_component(const std::vector<Table>& tables);
    String generate_angular_dashboard_component(const std::vector<Table>& tables);
    String generate_angular_admin_component_list(const Table& table);
    String generate_angular_admin_component_form(const Table& table);
    String generate_angular_store_service(const std::vector<Table>& tables);
    String generate_angular_store_ngrx(const std::vector<Table>& tables);
    String generate_angular_store_signals(const std::vector<Table>& tables);
    String generate_angular_component_list(const Table& table);
    String generate_angular_component_form(const Table& table);
    String generate_angular_dockerfile(const std::vector<Table>& tables);
    String generate_angular_nginx_conf(const std::vector<Table>& tables);
    String generate_angular_docker_compose_yml(const std::vector<Table>& tables);
    String generate_angular_gitignore();
    String generate_angular_readme(const std::vector<Table>& tables);
    bool generate_angular_frontend(const fs::path& base, const std::vector<Table>& tables);

    // Spring Boot generators
    String generate_springboot_pom_xml(const std::vector<Table>& tables);
    String generate_springboot_application_properties(const std::vector<Table>& tables);
    String generate_springboot_application_test_properties(const std::vector<Table>& tables);
    String generate_springboot_app_java(const std::vector<Table>& tables);
    String generate_springboot_entity_java(const Table& table);
    String generate_springboot_repository_java(const Table& table);
    String generate_springboot_service_java(const Table& table);
    String generate_springboot_controller_java(const Table& table);
    String generate_springboot_service_test_java(const Table& table);
    String generate_springboot_controller_test_java(const Table& table);
    String generate_springboot_liquibase_changelog(const std::vector<Table>& tables);
    String generate_springboot_dockerfile(const std::vector<Table>& tables);
    String generate_springboot_docker_compose_yml(const std::vector<Table>& tables);
    String generate_springboot_gitignore();
    String generate_springboot_readme(const std::vector<Table>& tables);
    String generate_security_jwt_springboot();
    String generate_security_oauth2_springboot();
    String generate_security_saml_springboot();
    String generate_security_owasp_springboot();
    String generate_security_ldap_springboot();

    // Android generators
    String generate_android_build_gradle(const std::vector<Table>& tables);
    String generate_android_app_build_gradle(const std::vector<Table>& tables);
    String generate_android_settings_gradle(const std::vector<Table>& tables);
    String generate_android_gradle_properties(const std::vector<Table>& tables);
    String generate_android_manifest(const std::vector<Table>& tables);
    String generate_android_application_class(const std::vector<Table>& tables);
    String generate_android_api_service(const std::vector<Table>& tables);
    String generate_android_api_client(const std::vector<Table>& tables);
    String generate_android_model(const Table& table);
    String generate_android_repository(const std::vector<Table>& tables);
    String generate_android_activity(const Table& table);
    String generate_android_layout(const Table& table);
    String generate_android_viewmodel(const Table& table);
    String generate_android_adapter(const Table& table);
    String generate_android_strings_xml(const std::vector<Table>& tables);
    String generate_android_colors_xml();
    String generate_android_theme_xml();
    String generate_android_dto_java(const Table& table);
    String generate_android_request_java(const Table& table);
    String generate_android_apiinfo_kt();
    String generate_android_apiinfo_java();
    String generate_android_application_class_java(const std::vector<Table>& tables);
    String generate_android_api_service_java(const std::vector<Table>& tables);
    String generate_android_api_client_java(const std::vector<Table>& tables);
    String generate_android_app_build_gradle_java(const std::vector<Table>& tables);
    String generate_android_repository_java(const std::vector<Table>& tables);
    String generate_android_viewmodel_java(const Table& table);
    String generate_android_adapter_java(const Table& table);
    String generate_android_activity_java(const Table& table);
    String generate_android_list_activity_java(const Table& table);
    String generate_android_detail_activity_java(const Table& table);
    String generate_android_item_layout(const Table& table);
    String generate_android_list_activity(const Table& table);
    String generate_android_detail_activity(const Table& table);
    String generate_android_list_layout(const Table& table);
    String generate_android_detail_layout(const Table& table);
    String generate_android_menu_xml(const Table& table);
    String generate_android_gradle_wrapper_properties();
    String generate_android_gradlew();
    String generate_android_gradlew_bat();
    bool generate_android(const fs::path& base, const std::vector<Table>& tables);

    // Type mapping helpers
    String flask_column_type(const Column& col);
    String peewee_column_type(const Column& col);
    String fastapi_column_type(const Column& col);
    String peewee_column_type_fastapi(const Column& col);
    String fastapi_python_type(const Column& col);
    String angular_type(const Column& col);

    // Common generators
    String generate_gitignore_common();
};

String framework_to_string(Framework f);
Framework string_to_framework(const String& s);
String frontend_type_to_string(FrontendType t);
FrontendType string_to_frontend_type(const String& s);
String react_bundler_to_string(ReactBundler b);
String react_css_to_string(ReactCSS c);
String react_theme_to_string(ReactTheme t);
String react_state_to_string(ReactState s);
ReactBundler string_to_react_bundler(const String& s);
String react_framework_to_string(ReactFramework f);
ReactFramework string_to_react_framework(const String& s);
ReactCSS string_to_react_css(const String& s);
ReactTheme string_to_react_theme(const String& s);
ReactState string_to_react_state(const String& s);
String angular_css_to_string(AngularCSS c);
AngularCSS string_to_angular_css(const String& s);
String angular_state_to_string(AngularState s);
AngularState string_to_angular_state(const String& s);
String angular_http_to_string(AngularHTTP h);
AngularHTTP string_to_angular_http(const String& s);
String angular_bundler_to_string(AngularBundler b);
AngularBundler string_to_angular_bundler(const String& s);
String android_language_to_string(AndroidLanguage l);
AndroidLanguage string_to_android_language(const String& s);
String android_architecture_to_string(AndroidArchitecture a);
AndroidArchitecture string_to_android_architecture(const String& s);

} // namespace novagen
