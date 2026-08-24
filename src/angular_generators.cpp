#include "generator.hpp"
#include <sstream>

namespace novagen {

String Generator::generate_angular_package_json(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << project_name_ << "-frontend\",\n";
    ss << "  \"version\": \"1.0.0\",\n";
    ss << "  \"scripts\": {\n";
    ss << "    \"ng\": \"ng\",\n";
    ss << "    \"start\": \"ng serve\",\n";
    ss << "    \"build\": \"ng build\",\n";
    ss << "    \"watch\": \"ng build --watch --configuration development\",\n";
    ss << "    \"test\": \"ng test\"\n";
    ss << "  },\n";
    ss << "  \"private\": true,\n";
    ss << "  \"dependencies\": {\n";
    ss << "    \"@angular/animations\": \"^17.3.0\",\n";
    ss << "    \"@angular/common\": \"^17.3.0\",\n";
    ss << "    \"@angular/compiler\": \"^17.3.0\",\n";
    ss << "    \"@angular/core\": \"^17.3.0\",\n";
    ss << "    \"@angular/forms\": \"^17.3.0\",\n";
    ss << "    \"@angular/platform-browser\": \"^17.3.0\",\n";
    ss << "    \"@angular/platform-browser-dynamic\": \"^17.3.0\",\n";
    ss << "    \"@angular/router\": \"^17.3.0\",\n";
    ss << "    \"rxjs\": \"~7.8.0\",\n";
    ss << "    \"tslib\": \"^2.6.0\",\n";
    ss << "    \"zone.js\": \"~0.14.0\"\n";

    if (angular_guards_) {
        ss << ",\n    \"@auth0/angular-jwt\": \"^6.1.2\"\n";
    }

    if (angular_css_ == AngularCSS::Bootstrap) {
        ss << ",\n    \"bootstrap\": \"^5.3.3\"\n";
    } else if (angular_css_ == AngularCSS::Material) {
        ss << ",\n    \"@angular/material\": \"^17.3.0\",\n";
        ss << "    \"@angular/cdk\": \"^17.3.0\"\n";
    }

    if (angular_http_ == AngularHTTP::Axios) {
        ss << ",\n    \"axios\": \"^1.6.0\"\n";
    }

    if (angular_state_ == AngularState::NgRx) {
        ss << ",\n    \"@ngrx/store\": \"^17.0.0\",\n";
        ss << "    \"@ngrx/effects\": \"^17.0.0\",\n";
        ss << "    \"@ngrx/store-devtools\": \"^17.0.0\"\n";
    } else if (angular_state_ == AngularState::Redux) {
        ss << ",\n    \"@ngrx/store\": \"^17.0.0\",\n";
        ss << "    \"@ngrx/effects\": \"^17.0.0\"\n";
    }

    ss << "\n  },\n";
    ss << "  \"devDependencies\": {\n";
    ss << "    \"@angular-devkit/build-angular\": \"^17.3.0\",\n";
    ss << "    \"@angular/cli\": \"^17.3.0\",\n";
    ss << "    \"@angular/compiler-cli\": \"^17.3.0\",\n";
    ss << "    \"@types/jasmine\": \"~5.1.0\"\n";

    if (angular_css_ == AngularCSS::Tailwind) {
        ss << ",\n    \"tailwindcss\": \"^3.4.1\",\n";
        ss << "    \"autoprefixer\": \"^10.4.17\",\n";
        ss << "    \"postcss\": \"^8.4.35\"\n";
    }

    if (angular_bundler_ == AngularBundler::Webpack) {
        ss << ",\n    \"webpack\": \"^5.90.0\",\n";
        ss << "    \"webpack-cli\": \"^5.1.0\",\n";
        ss << "    \"webpack-dev-server\": \"^5.0.0\"\n";
    }

    ss << "\n  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_angular_json(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"({
  "$schema": "./node_modules/@angular/cli/lib/config/schema.json",
  "version": 1,
  "newProjectRoot": "projects",
  "projects": {
    "app": {
      "projectType": "application",
      "root": "",
      "sourceRoot": "src",
      "prefix": "app",
      "architect": {
        "build": {
          "builder": )";

    if (angular_bundler_ == AngularBundler::Webpack) {
        ss << "\"@angular-devkit/build-angular:browser\",\n";
    } else {
        ss << "\"@angular-devkit/build-angular:application\",\n";
    }

    ss << R"(
          "options": {
            "outputPath": "dist/app",
            "index": "src/index.html",
            )";

    if (angular_bundler_ == AngularBundler::Webpack) {
        ss << "\"main\": \"src/main.ts\",\n";
    } else {
        ss << "\"browser\": \"src/main.ts\",\n";
    }

    ss << R"(
            "polyfills": ["zone.js"],
            "tsConfig": "tsconfig.app.json",
            "assets": ["src/favicon.ico", "src/assets"],
            "styles": [)";

    if (angular_css_ == AngularCSS::Tailwind) {
        ss << "\n              \"src/styles.css\"";
    } else if (angular_css_ == AngularCSS::Bootstrap) {
        ss << "\n              \"node_modules/bootstrap/dist/css/bootstrap.min.css\",\n              \"src/styles.css\"";
    } else if (angular_css_ == AngularCSS::Material) {
        ss << "\n              \"node_modules/@angular/material/prebuilt-themes/indigo-pink.css\",\n              \"src/styles.css\"";
    } else {
        ss << "\n              \"src/styles.css\"";
    }

    ss << R"(
            ],
            "scripts": []
          },
          "serve": {
            "builder": "@angular-devkit/build-angular:dev-server",
            "configurations": {
              "production": {
                "buildTarget": "app:build:production"
              },
              "development": {
                "buildTarget": "app:build:development"
              }
            },
            "defaultConfiguration": "development"
          }
        }
      }
    }
  }
}
)";
    return ss.str();
}

String Generator::generate_angular_tsconfig_base(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"({
  "compileOnSave": false,
  "compilerOptions": {
    "outDir": "./dist/out-tsc",
    "forceConsistentCasingInFileNames": true,
    "strict": true,
    "noImplicitOverride": true,
    "noPropertyAccessFromIndexSignature": true,
    "noImplicitReturns": true,
    "noFallthroughCasesInSwitch": true,
    "skipLibCheck": true,
    "esModuleInterop": true,
    "sourceMap": true,
    "declaration": false,
    "experimentalDecorators": true,
    "moduleResolution": "bundler",
    "importHelpers": true,
    "target": "ES2022",
    "module": "ES2022",
    "useDefineForClassFields": false,
    "lib": ["ES2022", "dom"]
  },
  "angularCompilerOptions": {
    "enableI18nLegacyMessageIdFormat": false,
    "strictInjectionParameters": true,
    "strictInputAccessModifiers": true,
    "strictTemplates": true
  }
}
)";
    return ss.str();
}

String Generator::generate_angular_tsconfig_app(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"({
  "extends": "./tsconfig.json",
  "compilerOptions": {
    "outDir": "./out-tsc/app",
    "types": []
  },
  "files": [
    "src/main.ts"
  ],
  "include": [
    "src/**/*.d.ts"
  ]
}
)";
    return ss.str();
}

String Generator::generate_angular_index_html(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html lang=\"en\">\n";
    ss << "<head>\n";
    ss << "  <meta charset=\"utf-8\">\n";
    ss << "  <title>" << project_name_ << "</title>\n";
    ss << "  <base href=\"/\">\n";
    ss << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    if (react_theme_ == ReactTheme::Dark) {
        ss << "  <meta name=\"color-scheme\" content=\"dark\">\n";
    } else if (react_theme_ == ReactTheme::Auto) {
        ss << "  <meta name=\"color-scheme\" content=\"light dark\">\n";
    }
    ss << "  <link rel=\"icon\" type=\"image/x-icon\" href=\"favicon.ico\">\n";
    ss << "</head>\n";
    ss << "<body>\n";
    ss << "  <app-root></app-root>\n";
    ss << "</body>\n";
    ss << "</html>\n";
    return ss.str();
}

String Generator::generate_angular_main_ts(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(import { bootstrapApplication } from '@angular/platform-browser';
import { AppComponent } from './app/app.component';
import { appConfig } from './app/app.config';

bootstrapApplication(AppComponent, appConfig)
  .catch((err) => console.error(err));
)";
    return ss.str();
}

String Generator::generate_angular_app_component_ts(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { Component } from '@angular/core';\n";
    ss << "import { RouterOutlet } from '@angular/router';\n";

    if (angular_guards_) {
        ss << "import { AuthService } from './services/auth.service';\n";
    }

    ss << "@Component({\n";
    ss << "  selector: 'app-root',\n";

    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [RouterOutlet],\n";
    }

    ss << "  templateUrl: './app.component.html',\n";
    ss << "  styleUrl: './app.component.css'\n";
    ss << "})\n";
    ss << "export class AppComponent {\n";
    ss << "  title = '" << project_name_ << "';\n";

    if (angular_guards_) {
        ss << "  isAuthenticated = false;\n";
        ss << "  isAdmin = false;\n\n";
        ss << "  constructor(private authService: AuthService) {\n";
        ss << "    this.authService.isAuthenticated$.subscribe(auth => {\n";
        ss << "      this.isAuthenticated = auth;\n";
        ss << "    });\n";
        ss << "    this.authService.isAdmin$.subscribe(admin => {\n";
        ss << "      this.isAdmin = admin;\n";
        ss << "    });\n";
        ss << "  }\n\n";
        ss << "  logout(): void {\n";
        ss << "    this.authService.logout();\n";
        ss << "  }\n";
    }

    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_app_component_html(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << R"(<div class="container">
  <nav class="navbar navbar-expand-lg navbar-dark bg-dark mb-4">
    <div class="container-fluid">
      <a class="navbar-brand" href="/">)" << project_name_ << R"(</a>
      <div class="navbar-nav">
        <a class="nav-link" routerLink="/">Home</a>
        <a class="nav-link" routerLink="/">)" << project_name_ << R"(</a>
)";

    for (const auto& table : tables) {
        ss << "        <a class=\"nav-link\" routerLink=\"/" << pluralize(table.kebab_name()) << "\">" << table.class_name() << R"(</a>
)";
    }

    if (angular_admin_) {
        ss << R"(
        <a class="nav-link" routerLink="/admin">Admin</a>
)";
    }

    if (angular_guards_) {
        ss << R"ANG_LOGIN(
        <a class="nav-link" routerLink="/login">Login</a>
        <a class="nav-link" (click)="logout()" style="cursor: pointer;">Logout</a>
)ANG_LOGIN";
    }

    ss << R"(
      </div>
    </div>
  </nav>

  <router-outlet></router-outlet>
</div>
)";
    return ss.str();
}

String Generator::generate_angular_app_component_css(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.navbar {
  border-radius: 8px;
  margin-bottom: 20px;
}

.nav-link {
  cursor: pointer;
}
)";
    return ss.str();
}

String Generator::generate_angular_app_routing_module(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { Routes } from '@angular/routes';\n";

    if (angular_guards_) {
        ss << "import { inject } from '@angular/core';\n";
        ss << "import { AuthService } from '../services/auth.service';\n";
        ss << "import { Router } from '@angular/router';\n\n";

        ss << "const authGuard = () => {\n";
        ss << "  const authService = inject(AuthService);\n";
        ss << "  const router = inject(Router);\n";
        ss << "  if (authService.isAuthenticated()) {\n";
        ss << "    return true;\n";
        ss << "  }\n";
        ss << "  router.navigate(['/login']);\n";
        ss << "  return false;\n";
        ss << "};\n\n";

        ss << "const adminGuard = () => {\n";
        ss << "  const authService = inject(AuthService);\n";
        ss << "  const router = inject(Router);\n";
        ss << "  if (authService.isAdmin()) {\n";
        ss << "    return true;\n";
        ss << "  }\n";
        ss << "  router.navigate(['/']);\n";
        ss << "  return false;\n";
        ss << "};\n\n";
    }

    ss << "export const routes: Routes = [\n";
    ss << "  { path: '', component: HomeComponent },\n";

    if (angular_guards_) {
        ss << "  { path: 'login', component: LoginComponent },\n";
    }

    for (const auto& table : tables) {
        if (angular_guards_) {
            ss << "  { path: '" << pluralize(table.kebab_name()) << "', component: " << table.class_name() << "ListComponent, canActivate: [authGuard] },\n";
            ss << "  { path: '" << pluralize(table.kebab_name()) << "/new', component: " << table.class_name() << "FormComponent, canActivate: [authGuard] },\n";
            ss << "  { path: '" << pluralize(table.kebab_name()) << "/edit/:id', component: " << table.class_name() << "FormComponent, canActivate: [authGuard] },\n";
        } else {
            ss << "  { path: '" << pluralize(table.kebab_name()) << "', component: " << table.class_name() << "ListComponent },\n";
            ss << "  { path: '" << pluralize(table.kebab_name()) << "/new', component: " << table.class_name() << "FormComponent },\n";
            ss << "  { path: '" << pluralize(table.kebab_name()) << "/edit/:id', component: " << table.class_name() << "FormComponent },\n";
        }
    }

    if (angular_admin_) {
        if (angular_guards_) {
            ss << "  { path: 'admin', component: DashboardComponent, canActivate: [adminGuard] },\n";
        } else {
            ss << "  { path: 'admin', component: DashboardComponent },\n";
        }
    }

    ss << "];\n";
    return ss.str();
}

String Generator::generate_angular_app_module(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { NgModule } from '@angular/core';\n";
    ss << "import { BrowserModule } from '@angular/platform-browser';\n";
    ss << "import { HttpClientModule, HTTP_INTERCEPTORS } from '@angular/common/http';\n";
    ss << "import { FormsModule, ReactiveFormsModule } from '@angular/forms';\n";
    ss << "import { RouterModule } from '@angular/router';\n";
    ss << "import { CommonModule } from '@angular/common';\n\n";
    ss << "import { AppComponent } from './app.component';\n";
    ss << "import { HomeComponent } from './components/home/home.component';\n";

    if (angular_guards_) {
        ss << "import { LoginComponent } from './components/login/login.component';\n";
        ss << "import { AuthInterceptor } from './interceptors/auth.interceptor';\n";
    }

    for (const auto& table : tables) {
        ss << "import { " << table.class_name() << "ListComponent } from './components/" << table.instance_name() << "-list/" << table.instance_name() << "-list.component';\n";
        ss << "import { " << table.class_name() << "FormComponent } from './components/" << table.instance_name() << "-form/" << table.instance_name() << "-form.component';\n";
    }

    if (angular_admin_) {
        ss << "import { DashboardComponent } from './components/admin/dashboard/dashboard.component';\n";
        for (const auto& table : tables) {
            ss << "import { " << table.class_name() << "AdminListComponent } from './components/admin/" << table.instance_name() << "-admin-list/" << table.instance_name() << "-admin-list.component';\n";
            ss << "import { " << table.class_name() << "AdminFormComponent } from './components/admin/" << table.instance_name() << "-admin-form/" << table.instance_name() << "-admin-form.component';\n";
        }
    }

    ss << "\n@NgModule({\n";
    ss << "  declarations: [\n";
    ss << "    AppComponent,\n";
    ss << "    HomeComponent,\n";

    if (angular_guards_) {
        ss << "    LoginComponent,\n";
    }

    for (const auto& table : tables) {
        ss << "    " << table.class_name() << "ListComponent,\n";
        ss << "    " << table.class_name() << "FormComponent,\n";
    }

    if (angular_admin_) {
        ss << "    DashboardComponent,\n";
        for (const auto& table : tables) {
            ss << "    " << table.class_name() << "AdminListComponent,\n";
            ss << "    " << table.class_name() << "AdminFormComponent,\n";
        }
    }

    ss << "  ],\n";
    ss << "  imports: [\n";
    ss << "    BrowserModule,\n";
    ss << "    HttpClientModule,\n";
    ss << "    FormsModule,\n";
    ss << "    ReactiveFormsModule,\n";
    ss << "    CommonModule,\n";
    ss << "    RouterModule.forRoot(routes)\n";
    ss << "  ],\n";
    ss << "  providers: [\n";

    if (angular_guards_) {
        ss << "    { provide: HTTP_INTERCEPTORS, useClass: AuthInterceptor, multi: true },\n";
    }

    ss << "  ],\n";
    ss << "  bootstrap: [AppComponent]\n";
    ss << "})\n";
    ss << "export class AppModule { }\n";
    return ss.str();
}

String Generator::generate_angular_models(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "// Auto-generated models\n\n";

    ss << "export interface User {\n";
    ss << "  id: number;\n";
    ss << "  username: string;\n";
    ss << "  email: string;\n";
    ss << "  role: 'user' | 'admin';\n";
    ss << "  token?: string;\n";
    ss << "}\n\n";

    ss << "export interface LoginRequest {\n";
    ss << "  username: string;\n";
    ss << "  password: string;\n";
    ss << "}\n\n";

    ss << "export interface LoginResponse {\n";
    ss << "  token: string;\n";
    ss << "  user: User;\n";
    ss << "}\n\n";

    for (const auto& table : tables) {
        ss << "export interface " << table.class_name() << " {\n";
        for (const auto& col : table.columns) {
            ss << "  " << col.field_name() << ": " << angular_type(col) << ";\n";
        }
        ss << "}\n\n";

        ss << "export type Create" << table.class_name() << "Dto = Omit<" << table.class_name() << ", '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "'>;\n";
        ss << "export type Update" << table.class_name() << "Dto = Partial<Create" << table.class_name() << "Dto>;\n\n";
    }

    return ss.str();
}

String Generator::angular_type(const Column& col) {
    String cpp_type = col.cpp_type();
    if (cpp_type == "int32_t" || cpp_type == "int64_t" || cpp_type == "int16_t" || cpp_type == "int8_t") {
        return "number";
    }
    if (cpp_type == "bool") {
        return "boolean";
    }
    if (cpp_type == "float" || cpp_type == "double") {
        return "number";
    }
    return "string";
}

String Generator::generate_angular_service(const std::vector<Table>& tables) {
    std::stringstream ss;

    if (angular_http_ == AngularHTTP::HttpClient) {
        ss << "import { Injectable } from '@angular/core';\n";
        ss << "import { HttpClient } from '@angular/common/http';\n";
        ss << "import { Observable } from 'rxjs';\n";
        ss << "import { " << tables[0].class_name();

        for (size_t i = 1; i < tables.size(); ++i) {
            ss << ", " << tables[i].class_name();
        }
        ss << " } from '../models';\n\n";
        ss << "@Injectable({\n";
        ss << "  providedIn: 'root'\n";
        ss << "})\n";
        ss << "export class ApiService {\n";
        ss << "  private apiUrl = '/api';\n\n";

        for (const auto& table : tables) {
            String plural = pluralize(table.kebab_name());

            ss << "  get" << plural << "(): Observable<" << table.class_name() << "[]> {\n";
            ss << "    return this.http.get<" << table.class_name() << "[]>(`${this.apiUrl}/" << plural << "`);\n";
            ss << "  }\n\n";

            ss << "  get" << table.class_name() << "(id: number): Observable<" << table.class_name() << "> {\n";
            ss << "    return this.http.get<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "/${id}`);\n";
            ss << "  }\n\n";

            ss << "  create" << table.class_name() << "(data: Create" << table.class_name() << "Dto): Observable<" << table.class_name() << "> {\n";
            ss << "    return this.http.post<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "`, data);\n";
            ss << "  }\n\n";

            ss << "  update" << table.class_name() << "(id: number, data: Update" << table.class_name() << "Dto): Observable<" << table.class_name() << "> {\n";
            ss << "    return this.http.put<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "/${id}`, data);\n";
            ss << "  }\n\n";

            ss << "  delete" << table.class_name() << "(id: number): Observable<void> {\n";
            ss << "    return this.http.delete<void>(`${this.apiUrl}/" << plural << "/${id}`);\n";
            ss << "  }\n\n";
        }

        ss << "  constructor(private http: HttpClient) {}\n";
        ss << "}\n";

    } else if (angular_http_ == AngularHTTP::Axios) {
        ss << "import { Injectable } from '@angular/core';\n";
        ss << "import axios, { AxiosInstance } from 'axios';\n";
        ss << "import { " << tables[0].class_name();

        for (size_t i = 1; i < tables.size(); ++i) {
            ss << ", " << tables[i].class_name();
        }
        ss << " } from '../models';\n\n";
        ss << "@Injectable({\n";
        ss << "  providedIn: 'root'\n";
        ss << "})\n";
        ss << "export class ApiService {\n";
        ss << "  private apiUrl = '/api';\n";
        ss << "  private axiosInstance: AxiosInstance;\n\n";

        for (const auto& table : tables) {
            String plural = pluralize(table.kebab_name());

            ss << "  async get" << plural << "(): Promise<" << table.class_name() << "[]> {\n";
            ss << "    const response = await this.axiosInstance.get<" << table.class_name() << "[]>(`${this.apiUrl}/" << plural << "`);\n";
            ss << "    return response.data;\n";
            ss << "  }\n\n";

            ss << "  async get" << table.class_name() << "(id: number): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await this.axiosInstance.get<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "/${id}`);\n";
            ss << "    return response.data;\n";
            ss << "  }\n\n";

            ss << "  async create" << table.class_name() << "(data: Create" << table.class_name() << "Dto): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await this.axiosInstance.post<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "`, data);\n";
            ss << "    return response.data;\n";
            ss << "  }\n\n";

            ss << "  async update" << table.class_name() << "(id: number, data: Update" << table.class_name() << "Dto): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await this.axiosInstance.put<" << table.class_name() << ">(`${this.apiUrl}/" << plural << "/${id}`, data);\n";
            ss << "    return response.data;\n";
            ss << "  }\n\n";

            ss << "  async delete" << table.class_name() << "(id: number): Promise<void> {\n";
            ss << "    await this.axiosInstance.delete(`${this.apiUrl}/" << plural << "/${id}`);\n";
            ss << "  }\n\n";
        }

        ss << "  constructor() {\n";
        ss << "    this.axiosInstance = axios.create({\n";
        ss << "      baseURL: this.apiUrl,\n";
        ss << "      headers: { 'Content-Type': 'application/json' }\n";
        ss << "    });\n";
        ss << "  }\n";
        ss << "}\n";

    } else {
        ss << "import { Injectable } from '@angular/core';\n";
        ss << "import { " << tables[0].class_name();

        for (size_t i = 1; i < tables.size(); ++i) {
            ss << ", " << tables[i].class_name();
        }
        ss << " } from '../models';\n\n";
        ss << "@Injectable({\n";
        ss << "  providedIn: 'root'\n";
        ss << "})\n";
        ss << "export class ApiService {\n";
        ss << "  private apiUrl = '/api';\n\n";

        for (const auto& table : tables) {
            String plural = pluralize(table.kebab_name());

            ss << "  async get" << plural << "(): Promise<" << table.class_name() << "[]> {\n";
            ss << "    const response = await fetch(`${this.apiUrl}/" << plural << "`, {\n";
            ss << "      headers: { 'Content-Type': 'application/json' }\n";
            ss << "    });\n";
            ss << "    return response.json();\n";
            ss << "  }\n\n";

            ss << "  async get" << table.class_name() << "(id: number): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await fetch(`${this.apiUrl}/" << plural << "/${id}`, {\n";
            ss << "      headers: { 'Content-Type': 'application/json' }\n";
            ss << "    });\n";
            ss << "    return response.json();\n";
            ss << "  }\n\n";

            ss << "  async create" << table.class_name() << "(data: Create" << table.class_name() << "Dto): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await fetch(`${this.apiUrl}/" << plural << "`, {\n";
            ss << "      method: 'POST',\n";
            ss << "      headers: { 'Content-Type': 'application/json' },\n";
            ss << "      body: JSON.stringify(data)\n";
            ss << "    });\n";
            ss << "    return response.json();\n";
            ss << "  }\n\n";

            ss << "  async update" << table.class_name() << "(id: number, data: Update" << table.class_name() << "Dto): Promise<" << table.class_name() << "> {\n";
            ss << "    const response = await fetch(`${this.apiUrl}/" << plural << "/${id}`, {\n";
            ss << "      method: 'PUT',\n";
            ss << "      headers: { 'Content-Type': 'application/json' },\n";
            ss << "      body: JSON.stringify(data)\n";
            ss << "    });\n";
            ss << "    return response.json();\n";
            ss << "  }\n\n";

            ss << "  async delete" << table.class_name() << "(id: number): Promise<void> {\n";
            ss << "    await fetch(`${this.apiUrl}/" << plural << "/${id}`, {\n";
            ss << "      method: 'DELETE'\n";
            ss << "    });\n";
            ss << "  }\n\n";
        }

        ss << "  constructor() {}\n";
        ss << "}\n";
    }

    return ss.str();
}

String Generator::generate_angular_auth_service(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import { Injectable } from '@angular/core';\n";
    ss << "import { HttpClient } from '@angular/common/http';\n";
    ss << "import { BehaviorSubject, Observable, tap } from 'rxjs';\n";
    ss << "import { User, LoginRequest, LoginResponse } from '../models';\n\n";
    ss << "@Injectable({\n";
    ss << "  providedIn: 'root'\n";
    ss << "})\n";
    ss << "export class AuthService {\n";
    ss << "  private apiUrl = '/api/auth';\n";
    ss << "  private isAuthenticatedSubject = new BehaviorSubject<boolean>(this.hasToken());\n";
    ss << "  private isAdminSubject = new BehaviorSubject<boolean>(this.hasAdminRole());\n\n";
    ss << "  isAuthenticated$ = this.isAuthenticatedSubject.asObservable();\n";
    ss << "  isAdmin$ = this.isAdminSubject.asObservable();\n\n";
    ss << "  constructor(private http: HttpClient) {}\n\n";
    ss << "  login(credentials: LoginRequest): Observable<LoginResponse> {\n";
    ss << "    return this.http.post<LoginResponse>(`${this.apiUrl}/login`, credentials).pipe(\n";
    ss << "      tap(response => {\n";
    ss << "        localStorage.setItem('token', response.token);\n";
    ss << "        localStorage.setItem('user', JSON.stringify(response.user));\n";
    ss << "        this.isAuthenticatedSubject.next(true);\n";
    ss << "        this.isAdminSubject.next(response.user.role === 'admin');\n";
    ss << "      })\n";
    ss << "    );\n";
    ss << "  }\n\n";
    ss << "  logout(): void {\n";
    ss << "    localStorage.removeItem('token');\n";
    ss << "    localStorage.removeItem('user');\n";
    ss << "    this.isAuthenticatedSubject.next(false);\n";
    ss << "    this.isAdminSubject.next(false);\n";
    ss << "  }\n\n";
    ss << "  getToken(): string | null {\n";
    ss << "    return localStorage.getItem('token');\n";
    ss << "  }\n\n";
    ss << "  isAuthenticated(): boolean {\n";
    ss << "    return this.hasToken();\n";
    ss << "  }\n\n";
    ss << "  isAdmin(): boolean {\n";
    ss << "    return this.hasAdminRole();\n";
    ss << "  }\n\n";
    ss << "  private hasToken(): boolean {\n";
    ss << "    return !!localStorage.getItem('token');\n";
    ss << "  }\n\n";
    ss << "  private hasAdminRole(): boolean {\n";
    ss << "    const userStr = localStorage.getItem('user');\n";
    ss << "    if (!userStr) return false;\n";
    ss << "    try {\n";
    ss << "      const user: User = JSON.parse(userStr);\n";
    ss << "      return user.role === 'admin';\n";
    ss << "    } catch {\n";
    ss << "      return false;\n";
    ss << "    }\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_auth_guard(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import { inject } from '@angular/core';\n";
    ss << "import { CanActivateFn, Router } from '@angular/router';\n";
    ss << "import { AuthService } from '../services/auth.service';\n\n";
    ss << "export const authGuard: CanActivateFn = (route, state) => {\n";
    ss << "  const authService = inject(AuthService);\n";
    ss << "  const router = inject(Router);\n\n";
    ss << "  if (authService.isAuthenticated()) {\n";
    ss << "    return true;\n";
    ss << "  }\n\n";
    ss << "  router.navigate(['/login']);\n";
    ss << "  return false;\n";
    ss << "};\n";
    return ss.str();
}

String Generator::generate_angular_admin_guard(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import { inject } from '@angular/core';\n";
    ss << "import { CanActivateFn, Router } from '@angular/router';\n";
    ss << "import { AuthService } from '../services/auth.service';\n\n";
    ss << "export const adminGuard: CanActivateFn = (route, state) => {\n";
    ss << "  const authService = inject(AuthService);\n";
    ss << "  const router = inject(Router);\n\n";
    ss << "  if (authService.isAdmin()) {\n";
    ss << "    return true;\n";
    ss << "  }\n\n";
    ss << "  router.navigate(['/']);\n";
    ss << "  return false;\n";
    ss << "};\n";
    return ss.str();
}

String Generator::generate_angular_auth_interceptor(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import { Injectable } from '@angular/core';\n";
    ss << "import { HttpInterceptor, HttpRequest, HttpHandler, HttpEvent } from '@angular/common/http';\n";
    ss << "import { Observable } from 'rxjs';\n";
    ss << "import { AuthService } from '../services/auth.service';\n\n";
    ss << "@Injectable()\n";
    ss << "export class AuthInterceptor implements HttpInterceptor {\n";
    ss << "  constructor(private authService: AuthService) {}\n\n";
    ss << "  intercept(req: HttpRequest<any>, next: HttpHandler): Observable<HttpEvent<any>> {\n";
    ss << "    const token = this.authService.getToken();\n\n";
    ss << "    if (token) {\n";
    ss << "      const authReq = req.clone({\n";
    ss << "        setHeaders: {\n";
    ss << "          Authorization: `Bearer ${token}`\n";
    ss << "        }\n";
    ss << "      });\n";
    ss << "      return next.handle(authReq);\n";
    ss << "    }\n\n";
    ss << "    return next.handle(req);\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_login_component(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { Component } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { FormBuilder, FormGroup, ReactiveFormsModule, Validators } from '@angular/forms';\n";
    ss << "import { Router } from '@angular/router';\n";
    ss << "import { AuthService } from '../../services/auth.service';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, ReactiveFormsModule],\n";
    }
    ss << "  templateUrl: './login.component.html',\n";
    ss << "  styleUrl: './login.component.css'\n";
    ss << "})\n";
    ss << "export class LoginComponent {\n";
    ss << "  loginForm: FormGroup;\n";
    ss << "  error: string | null = null;\n";
    ss << "  loading = false;\n\n";
    ss << "  constructor(\n";
    ss << "    private fb: FormBuilder,\n";
    ss << "    private authService: AuthService,\n";
    ss << "    private router: Router\n";
    ss << "  ) {\n";
    ss << "    this.loginForm = this.fb.group({\n";
    ss << "      username: ['', Validators.required],\n";
    ss << "      password: ['', Validators.required]\n";
    ss << "    });\n";
    ss << "  }\n\n";
    ss << "  onSubmit(): void {\n";
    ss << "    if (this.loginForm.invalid) return;\n\n";
    ss << "    this.loading = true;\n";
    ss << "    this.error = null;\n\n";
    ss << "    this.authService.login(this.loginForm.value).subscribe({\n";
    ss << "      next: () => {\n";
    ss << "        this.router.navigate(['/']);\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.error?.message || 'Login failed';\n";
    ss << "        this.loading = false;\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_dashboard_component(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { Component, OnInit } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { RouterLink } from '@angular/router';\n";
    ss << "import { ApiService } from '../../../services/api.service';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, RouterLink],\n";
    }
    ss << "  templateUrl: './dashboard.component.html',\n";
    ss << "  styleUrl: './dashboard.component.css'\n";
    ss << "})\n";
    ss << "export class DashboardComponent implements OnInit {\n";
    ss << "  stats: any = {};\n";
    ss << "  loading = true;\n\n";
    ss << "  constructor(private apiService: ApiService) {}\n\n";
    ss << "  ngOnInit(): void {\n";
    ss << "    this.loadStats();\n";
    ss << "  }\n\n";
    ss << "  loadStats(): void {\n";

    for (const auto& table : tables) {
        String plural = pluralize(table.kebab_name());
        ss << "    this.apiService.get" << plural << "().subscribe({\n";
        ss << "      next: (data) => {\n";
        ss << "        this.stats['" << table.instance_name() << "Count'] = data.length;\n";
        ss << "      }\n";
        ss << "    });\n";
    }

    ss << "    this.loading = false;\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_component_list(const Table& table) {
    std::stringstream ss;
    String plural = pluralize(table.kebab_name());
    String pk_name = table.pk_column() ? table.pk_column()->field_name() : "id";

    ss << "import { Component, OnInit } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { RouterLink } from '@angular/router';\n";
    ss << "import { ApiService } from '../../services/api.service';\n";
    ss << "import { " << table.class_name() << " } from '../../models';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, RouterLink],\n";
    }
    ss << "  templateUrl: './" << table.instance_name() << "-list.component.html',\n";
    ss << "  styleUrl: './" << table.instance_name() << "-list.component.css'\n";
    ss << "})\n";

    ss << "export class " << table.class_name() << "ListComponent implements OnInit {\n";
    ss << "  " << plural << ": " << table.class_name() << "[] = [];\n";
    ss << "  loading = false;\n";
    ss << "  error: string | null = null;\n\n";

    ss << "  constructor(private apiService: ApiService) {}\n\n";

    ss << "  ngOnInit(): void {\n";
    ss << "    this.loadData();\n";
    ss << "  }\n\n";

    ss << "  loadData(): void {\n";
    ss << "    this.loading = true;\n";
    ss << "    this.apiService.get" << plural << "().subscribe({\n";
    ss << "      next: (data) => {\n";
    ss << "        this." << plural << " = data;\n";
    ss << "        this.loading = false;\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to load data';\n";
    ss << "        this.loading = false;\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n\n";

    ss << "  delete(id: number): void {\n";
    ss << "    if (!confirm('Are you sure you want to delete this item?')) return;\n";
    ss << "    this.apiService.delete" << table.class_name() << "(id).subscribe({\n";
    ss << "      next: () => {\n";
    ss << "        this." << plural << " = this." << plural << ".filter(item => item." << pk_name << " !== id);\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to delete';\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_component_form(const Table& table) {
    std::stringstream ss;
    String pk_name = table.pk_column() ? table.pk_column()->field_name() : "id";

    ss << "import { Component, OnInit } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { ActivatedRoute, Router } from '@angular/router';\n";
    ss << "import { FormBuilder, FormGroup, ReactiveFormsModule, Validators } from '@angular/forms';\n";
    ss << "import { ApiService } from '../../services/api.service';\n";
    ss << "import { " << table.class_name() << " } from '../../models';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, ReactiveFormsModule],\n";
    }
    ss << "  templateUrl: './" << table.instance_name() << "-form.component.html',\n";
    ss << "  styleUrl: './" << table.instance_name() << "-form.component.css'\n";
    ss << "})\n";

    ss << "export class " << table.class_name() << "FormComponent implements OnInit {\n";
    ss << "  form: FormGroup;\n";
    ss << "  isEditMode = false;\n";
    ss << "  loading = false;\n";
    ss << "  error: string | null = null;\n";
    ss << "  private id: number | null = null;\n\n";

    ss << "  constructor(\n";
    ss << "    private fb: FormBuilder,\n";
    ss << "    private apiService: ApiService,\n";
    ss << "    private route: ActivatedRoute,\n";
    ss << "    private router: Router\n";
    ss << "  ) {\n";
    ss << "    this.form = this.fb.group({\n";
    bool first = true;
    for (const auto& col : table.columns) {
        if (col.primary_key && col.auto_increment) continue;
        if (!first) ss << ",\n      ";
        first = false;
        if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
            ss << col.field_name() << ": [null, Validators.pattern(/^-?\\d+$/)]";
        } else {
            ss << col.field_name() << ": [null" << (col.nullable ? "" : ", Validators.required") << "]";
        }
    }
    ss << "\n    });\n";
    ss << "  }\n\n";

    ss << "  ngOnInit(): void {\n";
    ss << "    this.id = Number(this.route.snapshot.paramMap.get('id'));\n";
    ss << "    if (this.id) {\n";
    ss << "      this.isEditMode = true;\n";
    ss << "      this.loadData();\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  loadData(): void {\n";
    ss << "    if (!this.id) return;\n";
    ss << "    this.loading = true;\n";
    ss << "    this.apiService.get" << table.class_name() << "(this.id).subscribe({\n";
    ss << "      next: (data) => {\n";
    ss << "        this.form.patchValue(data);\n";
    ss << "        this.loading = false;\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to load data';\n";
    ss << "        this.loading = false;\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n\n";

    ss << "  onSubmit(): void {\n";
    ss << "    if (this.form.invalid) return;\n";
    ss << "    this.loading = true;\n";
    ss << "    this.error = null;\n\n";

    ss << "    if (this.isEditMode && this.id) {\n";
    ss << "      this.apiService.update" << table.class_name() << "(this.id, this.form.value).subscribe({\n";
    ss << "        next: () => this.router.navigate(['/" << pluralize(table.kebab_name()) << "']),\n";
    ss << "        error: (err) => {\n";
    ss << "          this.error = err.message || 'Failed to update';\n";
    ss << "          this.loading = false;\n";
    ss << "        }\n";
    ss << "      });\n";
    ss << "    } else {\n";
    ss << "      this.apiService.create" << table.class_name() << "(this.form.value).subscribe({\n";
    ss << "        next: () => this.router.navigate(['/" << pluralize(table.kebab_name()) << "']),\n";
    ss << "        error: (err) => {\n";
    ss << "          this.error = err.message || 'Failed to create';\n";
    ss << "          this.loading = false;\n";
    ss << "        }\n";
    ss << "      });\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  onCancel(): void {\n";
    ss << "    this.router.navigate(['/" << pluralize(table.kebab_name()) << "']);\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_admin_component_list(const Table& table) {
    std::stringstream ss;
    String plural = pluralize(table.kebab_name());
    String pk_name = table.pk_column() ? table.pk_column()->field_name() : "id";

    ss << "import { Component, OnInit } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { RouterLink } from '@angular/router';\n";
    ss << "import { ApiService } from '../../../services/api.service';\n";
    ss << "import { " << table.class_name() << " } from '../../../models';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, RouterLink],\n";
    }
    ss << "  templateUrl: './" << table.instance_name() << "-admin-list.component.html',\n";
    ss << "  styleUrl: './" << table.instance_name() << "-admin-list.component.css'\n";
    ss << "})\n";

    ss << "export class " << table.class_name() << "AdminListComponent implements OnInit {\n";
    ss << "  " << plural << ": " << table.class_name() << "[] = [];\n";
    ss << "  loading = false;\n";
    ss << "  error: string | null = null;\n\n";

    ss << "  constructor(private apiService: ApiService) {}\n\n";

    ss << "  ngOnInit(): void {\n";
    ss << "    this.loadData();\n";
    ss << "  }\n\n";

    ss << "  loadData(): void {\n";
    ss << "    this.loading = true;\n";
    ss << "    this.apiService.get" << plural << "().subscribe({\n";
    ss << "      next: (data) => {\n";
    ss << "        this." << plural << " = data;\n";
    ss << "        this.loading = false;\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to load data';\n";
    ss << "        this.loading = false;\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n\n";

    ss << "  delete(id: number): void {\n";
    ss << "    if (!confirm('Are you sure you want to delete this item?')) return;\n";
    ss << "    this.apiService.delete" << table.class_name() << "(id).subscribe({\n";
    ss << "      next: () => {\n";
    ss << "        this." << plural << " = this." << plural << ".filter(item => item." << pk_name << " !== id);\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to delete';\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_admin_component_form(const Table& table) {
    std::stringstream ss;

    ss << "import { Component, OnInit } from '@angular/core';\n";
    ss << "import { CommonModule } from '@angular/common';\n";
    ss << "import { ActivatedRoute, Router } from '@angular/router';\n";
    ss << "import { FormBuilder, FormGroup, ReactiveFormsModule, Validators } from '@angular/forms';\n";
    ss << "import { ApiService } from '../../../services/api.service';\n";
    ss << "import { " << table.class_name() << " } from '../../../models';\n\n";

    ss << "@Component({\n";
    if (angular_standalone_) {
        ss << "  standalone: true,\n";
        ss << "  imports: [CommonModule, ReactiveFormsModule],\n";
    }
    ss << "  templateUrl: './" << table.instance_name() << "-admin-form.component.html',\n";
    ss << "  styleUrl: './" << table.instance_name() << "-admin-form.component.css'\n";
    ss << "})\n";

    ss << "export class " << table.class_name() << "AdminFormComponent implements OnInit {\n";
    ss << "  form: FormGroup;\n";
    ss << "  isEditMode = false;\n";
    ss << "  loading = false;\n";
    ss << "  error: string | null = null;\n";
    ss << "  private id: number | null = null;\n\n";

    ss << "  constructor(\n";
    ss << "    private fb: FormBuilder,\n";
    ss << "    private apiService: ApiService,\n";
    ss << "    private route: ActivatedRoute,\n";
    ss << "    private router: Router\n";
    ss << "  ) {\n";
    ss << "    this.form = this.fb.group({\n";
    bool first = true;
    for (const auto& col : table.columns) {
        if (col.primary_key && col.auto_increment) continue;
        if (!first) ss << ",\n      ";
        first = false;
        if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
            ss << col.field_name() << ": [null, Validators.pattern(/^-?\\d+$/)]";
        } else {
            ss << col.field_name() << ": [null" << (col.nullable ? "" : ", Validators.required") << "]";
        }
    }
    ss << "\n    });\n";
    ss << "  }\n\n";

    ss << "  ngOnInit(): void {\n";
    ss << "    this.id = Number(this.route.snapshot.paramMap.get('id'));\n";
    ss << "    if (this.id) {\n";
    ss << "      this.isEditMode = true;\n";
    ss << "      this.loadData();\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  loadData(): void {\n";
    ss << "    if (!this.id) return;\n";
    ss << "    this.loading = true;\n";
    ss << "    this.apiService.get" << table.class_name() << "(this.id).subscribe({\n";
    ss << "      next: (data) => {\n";
    ss << "        this.form.patchValue(data);\n";
    ss << "        this.loading = false;\n";
    ss << "      },\n";
    ss << "      error: (err) => {\n";
    ss << "        this.error = err.message || 'Failed to load data';\n";
    ss << "        this.loading = false;\n";
    ss << "      }\n";
    ss << "    });\n";
    ss << "  }\n\n";

    ss << "  onSubmit(): void {\n";
    ss << "    if (this.form.invalid) return;\n";
    ss << "    this.loading = true;\n";
    ss << "    this.error = null;\n\n";

    ss << "    if (this.isEditMode && this.id) {\n";
    ss << "      this.apiService.update" << table.class_name() << "(this.id, this.form.value).subscribe({\n";
    ss << "        next: () => this.router.navigate(['/admin']),\n";
    ss << "        error: (err) => {\n";
    ss << "          this.error = err.message || 'Failed to update';\n";
    ss << "          this.loading = false;\n";
    ss << "        }\n";
    ss << "      });\n";
    ss << "    } else {\n";
    ss << "      this.apiService.create" << table.class_name() << "(this.form.value).subscribe({\n";
    ss << "        next: () => this.router.navigate(['/admin']),\n";
    ss << "        error: (err) => {\n";
    ss << "          this.error = err.message || 'Failed to create';\n";
    ss << "          this.loading = false;\n";
    ss << "        }\n";
    ss << "      });\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  onCancel(): void {\n";
    ss << "    this.router.navigate(['/admin']);\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_angular_dockerfile(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "FROM node:20-alpine AS build\n\n";
    ss << "WORKDIR /app\n\n";
    ss << "COPY package*.json ./\n";
    ss << "RUN npm ci\n\n";
    ss << "COPY . .\n";
    ss << "RUN npm run build\n\n";
    ss << "FROM nginx:alpine\n";
    ss << "COPY --from=build /app/dist/app/browser /usr/share/nginx/html\n";
    ss << "COPY nginx.conf /etc/nginx/conf.d/default.conf\n\n";
    ss << "EXPOSE 80\n\n";
    ss << "CMD [\"nginx\", \"-g\", \"daemon off;\"]\n";
    return ss.str();
}

String Generator::generate_angular_nginx_conf(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(server {
    listen 80;
    server_name localhost;
    root /usr/share/nginx/html;
    index index.html;

    location / {
        try_files $uri $uri/ /index.html;
    }

    location /api {
        proxy_pass http://backend:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
}
)";
    return ss.str();
}

String Generator::generate_angular_docker_compose_yml(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "version: '3.8'\n\n";
    ss << "services:\n";
    ss << "  frontend:\n";
    ss << "    build:\n";
    ss << "      context: .\n";
    ss << "      dockerfile: Dockerfile\n";
    ss << "    ports:\n";
    ss << "      - \"4200:80\"\n";
    ss << "    depends_on:\n";
    ss << "      - backend\n";
    ss << "    networks:\n";
    ss << "      - app-network\n\n";
    ss << "  backend:\n";
    ss << "    build:\n";
    ss << "      context: ../" << project_name_ << "\n";
    ss << "      dockerfile: Dockerfile\n";
    ss << "    ports:\n";
    ss << "      - \"8080:8080\"\n";
    ss << "    volumes:\n";
    ss << "      - ../" << project_name_ << "/data:/app/data\n";
    ss << "    networks:\n";
    ss << "      - app-network\n\n";
    ss << "networks:\n";
    ss << "  app-network:\n";
    ss << "    driver: bridge\n";
    return ss.str();
}

String Generator::generate_angular_gitignore() {
    return R"(# Angular
## Angular ##
# compiled output
/dist
/tmp
/app/**/*.js
/app/**/*.js.map

# dependencies
/node_modules

# IDEs and editors
.idea/
.project
.classpath
.DS_Store

# misc
.sass-cache/
connect.lock/
/coverage/*
/libpeerconnection.log/
testem.log
/typings

# e2e
/e2e/*.js
/e2e/*.map

# System Files
.DS_Store
Thumbs.db
)";
}

String Generator::generate_angular_readme(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "# " << project_name_ << " Frontend (Angular)\n\n";
    ss << "Angular frontend for " << project_name_ << " REST API.\n\n";
    ss << "## Configuration\n\n";
    ss << "- **Standalone Components**: " << (angular_standalone_ ? "Yes" : "No") << "\n";
    ss << "- **JWT Auth Guards**: " << (angular_guards_ ? "Yes" : "No") << "\n";
    ss << "- **Admin Pages**: " << (angular_admin_ ? "Yes" : "No") << "\n\n";
    ss << "## Requirements\n\n";
    ss << "- Node.js 18+\n";
    ss << "- Angular CLI\n\n";
    ss << "## Installation\n\n";
    ss << "```bash\n";
    ss << "npm install\n";
    ss << "```\n\n";
    ss << "## Development\n\n";
    ss << "```bash\n";
    ss << "ng serve\n";
    ss << "```\n\n";
    ss << "Frontend runs on http://localhost:4200 with API proxy to backend at http://localhost:8080\n\n";
    ss << "## Building\n\n";
    ss << "```bash\n";
    ss << "ng build\n";
    ss << "```\n\n";
    ss << "## Docker\n\n";
    ss << "```bash\n";
    ss << "docker-compose up --build\n";
    ss << "```\n\n";
    ss << "Frontend available at http://localhost:4200\n\n";

    if (angular_guards_) {
        ss << "## Authentication\n\n";
        ss << "JWT authentication is enabled. Routes are protected by auth guards.\n";
        ss << "Login endpoint: POST /api/auth/login\n";
    }

    if (angular_admin_) {
        ss << "## Admin Dashboard\n\n";
        ss << "Admin pages are available at /admin with CRUD operations for all entities.\n";
    }

    ss << "Generated by NovaGen v1.0.0\n";
    return ss.str();
}

bool Generator::generate_angular_frontend(const fs::path& base, const std::vector<Table>& tables) {
    try {
        if (!create_directory(base)) return false;
        if (!create_directory(base / "src" / "app" / "components")) return false;
        if (!create_directory(base / "src" / "app" / "services")) return false;
        if (!create_directory(base / "src" / "assets")) return false;

        if (!write_file(base / "package.json", generate_angular_package_json(tables))) return false;
        if (!write_file(base / "angular.json", generate_angular_angular_json(tables))) return false;
        if (!write_file(base / "tsconfig.json", generate_angular_tsconfig_base(tables))) return false;
        if (!write_file(base / "tsconfig.app.json", generate_angular_tsconfig_app(tables))) return false;
        if (!write_file(base / "src" / "index.html", generate_angular_index_html(tables))) return false;
        if (!write_file(base / "src" / "main.ts", generate_angular_main_ts(tables))) return false;
        if (!write_file(base / "src" / "styles.css", generate_react_index_css(tables))) return false;
        if (!write_file(base / "src" / "app" / "app.component.ts", generate_angular_app_component_ts(tables))) return false;
        if (!write_file(base / "src" / "app" / "app.component.html", generate_angular_app_component_html(tables))) return false;
        if (!write_file(base / "src" / "app" / "app.component.css", generate_angular_app_component_css(tables))) return false;
        if (!write_file(base / "nginx.conf", generate_angular_nginx_conf(tables))) return false;
        if (!write_file(base / "Dockerfile", generate_angular_dockerfile(tables))) return false;
        if (!write_file(base / "docker-compose.yml", generate_angular_docker_compose_yml(tables))) return false;
        if (!write_file(base / ".gitignore", generate_angular_gitignore())) return false;
        if (!write_file(base / "README.md", generate_angular_readme(tables))) return false;

        if (angular_standalone_) {
            if (!write_file(base / "src" / "app" / "app.routes.ts", generate_angular_app_routing_module(tables))) return false;
            if (!write_file(base / "src" / "app" / "app.config.ts", generate_angular_app_module(tables))) return false;
        } else {
            if (!write_file(base / "src" / "app" / "app-routing.module.ts", generate_angular_app_routing_module(tables))) return false;
            if (!write_file(base / "src" / "app" / "app.module.ts", generate_angular_app_module(tables))) return false;
        }

        if (!write_file(base / "src" / "app" / "models.ts", generate_angular_models(tables))) return false;
        if (!write_file(base / "src" / "app" / "services" / "api.service.ts", generate_angular_service(tables))) return false;

        if (angular_guards_) {
            if (!create_directory(base / "src" / "app" / "guards")) return false;
            if (!write_file(base / "src" / "app" / "services" / "auth.service.ts", generate_angular_auth_service(tables))) return false;
            if (!write_file(base / "src" / "app" / "guards" / "auth.guard.ts", generate_angular_auth_guard(tables))) return false;
            if (!write_file(base / "src" / "app" / "guards" / "admin.guard.ts", generate_angular_admin_guard(tables))) return false;
            if (!write_file(base / "src" / "app" / "interceptors" / "auth.interceptor.ts", generate_angular_auth_interceptor(tables))) return false;
            if (!write_file(base / "src" / "app" / "components" / "login" / "login.component.ts", generate_angular_login_component(tables))) return false;
        }

        if (angular_admin_) {
            if (!create_directory(base / "src" / "app" / "pages" / "admin")) return false;
            if (!write_file(base / "src" / "app" / "pages" / "admin" / "dashboard" / "dashboard.component.ts", generate_angular_dashboard_component(tables))) return false;

            for (const auto& table : tables) {
                if (!write_file(base / "src" / "app" / "pages" / "admin" / (table.class_name() + "List") / (table.class_name() + "List.component.ts"),
                              generate_angular_admin_component_list(table))) return false;
                if (!write_file(base / "src" / "app" / "pages" / "admin" / (table.class_name() + "Form") / (table.class_name() + "Form.component.ts"),
                              generate_angular_admin_component_form(table))) return false;
            }
        }

        for (const auto& table : tables) {
            if (!write_file(base / "src" / "app" / "components" / (table.class_name() + "List") / (table.class_name() + "List.component.ts"),
                          generate_angular_component_list(table))) return false;
            if (!write_file(base / "src" / "app" / "components" / (table.class_name() + "Form") / (table.class_name() + "Form.component.ts"),
                          generate_angular_component_form(table))) return false;
        }

        return true;
    } catch (const std::exception& e) {
        last_error_ = "Angular generation failed: " + String(e.what());
        return false;
    }
}

} // namespace novagen
