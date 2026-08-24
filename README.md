# NovaGen

C++ REST API Generator from SQL DDL with optional React frontend.

> **Disclaimer:** This software is provided as a showcase. The author takes no responsibility in any case.

Generates complete, production-ready REST API projects from SQL table definitions.

## Features

- **SQL DDL Parsing**: Parse `CREATE TABLE` statements to extract schema
- **Multiple SQL Dialects**: MySQL, PostgreSQL, SQLite, T-SQL (SQL Server), PL/SQL (Oracle)
- **Multiple Backend Frameworks**:
  - Crow (C++ web framework) - default
  - Flask (Python)
  - FastAPI (Python)
- **React Frontend**: Optional React frontend with multiple configuration options
- **SQLite Backend**: Built-in SQLite support via libsqlite3
- **JSON API**: RESTful JSON API with full CRUD operations
- **CMake Build**: Modern CMake-based build system
- **Unit Tests**: GoogleTest-based test suite (C++), pytest (Python)
- **Docker Support**: Dockerfile and docker-compose for all projects

## Supported SQL Dialects

NovaGen supports SQL DDL from multiple database systems:

| Dialect | Examples | Notes |
|---------|----------|-------|
| **MySQL** | `VARCHAR(255)`, `INT`, `AUTO_INCREMENT` | Standard MySQL syntax |
| **PostgreSQL** | `SERIAL`, `UUID`, `JSONB` | Also parses `CREATE TABLE` |
| **SQLite** | `INTEGER PRIMARY KEY` | Auto-increment handled |
| **T-SQL** (SQL Server) | `IDENTITY(1,1)`, `[bracketed]`, `GO` batches | Bracket quoting, `DATETIMEOFFSET` |
| **PL/SQL** (Oracle) | `VARCHAR2`, `NUMBER(10,2)`, `CLOB`, `CREATE OR REPLACE TABLE` | Storage clauses ignored, `SYS_REFCURSOR` supported |

## Building NovaGen

```bash
cd novagen
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build . -j$(nproc)
```

## Using NovaGen

### From SQL File

```bash
./novagen --input schema.sql --output ./projects --project myapi
```

### From SQL String

```bash
./novagen --sql "CREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(100));" --project myapi
```

### Options

```
-i, --input FILE      Input SQL DDL file
-s, --sql STRING      SQL DDL as string
-o, --output DIR      Output directory (default: ./output)
-p, --project NAME    Project name (default: api_server)
-n, --namespace NS   Namespace for routes/models (default: app)
-f, --framework FR    Backend framework: crow, flask, fastapi, springboot (default: crow)
--security SEC       Security: jwt, oauth2, saml, owasp, ldap (default: none)
--orm ORM            ORM: sqlalchemy, peewee, slack (default: sqlalchemy)
-r, --frontend FR    Frontend: react, angular, android (default: react)
-b, --bundler BN     Bundler: vite, cra, webpack (default: vite)
-e, --react-framework FR  Language: typescript, javascript (default: typescript)
-c, --css CSS        CSS framework: tailwind, bootstrap, plain (default: tailwind)
-t, --theme THEME    Theme: light, dark, auto (default: light)
-m, --state STATE    State management: zustand, redux, context (default: context)
--android-lang LANG  Language: kotlin, java (default: kotlin)
--android-arch ARCH  Architecture: mvvm, clean (default: mvvm)
-l, --list-tables    List parsed tables and exit
-v, --version        Show version
-h, --help           Show help
```

### Frontend Options

When `--frontend` or `-r` is specified, additional options become available:

| Option | Values | Default | Description |
|--------|--------|---------|-------------|
| `--bundler` | vite, cra, webpack | vite | Build tool / bundler |
| `--react-framework` | typescript, javascript | typescript | Development language |
| `--css` | tailwind, bootstrap, plain | tailwind | CSS framework |
| `--theme` | light, dark, auto | light | Color theme |
| `--state` | context, zustand, redux | context | State management |

## Examples

### C++ Crow Backend Only

```bash
./novagen --input schema.sql --project myapi
```

### Python Flask Backend

```bash
./novagen --input schema.sql --framework flask --project myapi
```

### Python FastAPI Backend

```bash
./novagen --input schema.sql --framework fastapi --project myapi
```

### Java Spring Boot Backend

```bash
./novagen --input schema.sql --framework springboot --project myapi
```

### T-SQL (SQL Server) Schema

```bash
./novagen --input mssql_schema.sql --framework crow --project myapi
```

Example T-SQL schema:
```sql
CREATE TABLE users (
    id INT PRIMARY KEY IDENTITY(1,1),
    name NVARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at DATETIME2 DEFAULT GETDATE()
);
GO

CREATE TABLE orders (
    id INT PRIMARY KEY IDENTITY(1,1),
    user_id INT NOT NULL REFERENCES users(id),
    total DECIMAL(10,2)
);
```

### PL/SQL (Oracle) Schema

```bash
./novagen --input oracle_schema.sql --framework crow --project myapi
```

Example Oracle schema:
```sql
CREATE OR REPLACE TABLE employees (
    employee_id NUMBER(10) PRIMARY KEY,
    first_name VARCHAR2(50) NOT NULL,
    last_name VARCHAR2(50) NOT NULL,
    email VARCHAR2(100) UNIQUE,
    hire_date DATE DEFAULT SYSDATE,
    salary NUMBER(8,2),
    department_id NUMBER(5)
) TABLESPACE users PCTFREE 10 PCTUSED 40;
```

### Flask with Peewee ORM

```bash
./novagen --input schema.sql --framework flask --orm peewee --project myapi
```

### FastAPI with Peewee ORM

```bash
./novagen --input schema.sql --framework fastapi --orm peewee --project myapi
```

### Backend with JWT Security

```bash
./novagen --input schema.sql --framework flask --security jwt --project myapi
./novagen --input schema.sql --framework fastapi --security jwt --project myapi
./novagen --input schema.sql --framework springboot --security jwt --project myapi
```

### Backend with OAuth2 Security

```bash
./novagen --input schema.sql --framework flask --security oauth2 --project myapi
./novagen --input schema.sql --framework springboot --security oauth2 --project myapi
```

### Backend with OWASP Security (CSRF, XSS, SQL Injection protection)

```bash
./novagen --input schema.sql --framework crow --security owasp --project myapi
./novagen --input schema.sql --framework flask --security owasp --project myapi
./novagen --input schema.sql --framework fastapi --security owasp --project myapi
```

### Backend with LDAP Authentication

```bash
./novagen --input schema.sql --framework flask --security ldap --project myapi
./novagen --input schema.sql --framework springboot --security ldap --project myapi
```

### React Frontend with Defaults (Vite + Tailwind + Light + Context)

```bash
./novagen --input schema.sql --frontend --project myapi
```

### React Frontend with CRA + Bootstrap + Dark Theme + Redux

```bash
./novagen --input schema.sql --frontend --bundler cra --css bootstrap --theme dark --state redux --project myapi
```

### React Frontend with Vite + Tailwind + Dark Theme + Zustand

```bash
./novagen --input schema.sql --frontend --bundler vite --css tailwind --theme dark --state zustand --project myapi
```

### Angular Frontend with Defaults (Esbuild + Tailwind + HttpClient)

```bash
./novagen --input schema.sql --frontend angular --project myapi
```

### Angular Frontend with Webpack + Bootstrap + NgRx

```bash
./novagen --input schema.sql --frontend angular --angular-bundler webpack --angular-css bootstrap --angular-state ngrx --project myapi
```

### Angular Frontend with Material + Signals + Axios

```bash
./novagen --input schema.sql --frontend angular --angular-css material --angular-state signals --angular-http axios --guards --admin --project myapi
```

### Android App with Retrofit + OkHttp + RxJava (Kotlin)

```bash
./novagen --input schema.sql --frontend android --project myapi
```

### Android App with Java + MVVM

```bash
./novagen --input schema.sql --frontend android --android-lang java --android-arch mvvm --project myapi
```

## Frontend Stack Details

### React Bundlers

- **vite** (default): Fast development server, optimized production builds
- **cra**: Classic create-react-app, familiar structure
- **webpack**: Full webpack config for maximum control

### Angular Bundlers

- **esbuild** (default): Fast builds, modern approach
- **webpack**: Full webpack config for maximum control

### CSS Frameworks

- **tailwind**: Utility-first CSS (default for React & Angular), highly customizable
- **bootstrap**: Component-based CSS framework
- **plain**: Minimal custom CSS with CSS variables for theming
- **material**: Angular Material components (Angular only)

### Themes

- **light** (default): Light color scheme
- **dark**: Dark color scheme with media query support
- **auto**: System preference detection

### State Management

**React:**
- **context** (default): React built-in Context API + useReducer
- **zustand**: Minimal, hook-based state management
- **redux**: Full Redux Toolkit with slices for each entity

**Angular:**
- **service** (default): Angular service-based state with BehaviorSubject
- **ngrx**: NgRx store, effects, and devtools
- **redux**: NgRx with Redux-like pattern
- **signals**: Angular Signals for reactive state

### HTTP Clients

- **httpclient** (default for Angular): Angular HttpClient module
- **fetch**: Native Fetch API
- **axios**: Axios HTTP client with interceptors

### CSS Frameworks

- **tailwind**: Utility-first CSS (default), highly customizable
- **bootstrap**: Component-based with react-bootstrap
- **plain**: Minimal custom CSS with CSS variables for theming

### Themes

- **light** (default): Light color scheme
- **dark**: Dark color scheme with media query support
- **auto**: System preference detection

### State Management

- **context** (default): React built-in Context API + useReducer
- **zustand**: Minimal, hook-based state management
- **redux**: Full Redux Toolkit with slices for each entity

## Example

### Input SQL (`schema.sql`)

```sql
CREATE TABLE users (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE posts (
    id INT PRIMARY KEY,
    user_id INT NOT NULL,
    title VARCHAR(200) NOT NULL,
    content TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

### Generate C++ Project

```bash
./novagen --input schema.sql --output ./projects --project myapi
```

### Generate with React Frontend

```bash
./novagen --input schema.sql --frontend --bundler vite --css tailwind --state zustand --project myapi
```

### Build Generated C++ Project

```bash
cd projects/myapi
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Run

```bash
./myapi --port 8080
```

### Run React Frontend

```bash
cd projects/myapi-frontend
npm install
npm run dev
```

### API Endpoints (C++ Crow)

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | /api | API info |
| GET | /health | Health check |
| GET | /api/users | List all users |
| GET | /api/users/{id} | Get user by ID |
| POST | /api/users | Create user |
| PUT | /api/users/{id} | Update user |
| DELETE | /api/users/{id} | Delete user |
| GET | /api/users/count | Count users |
| GET | /api/posts | List all posts |
| ... | ... | ... |

## Project Structure

### C++ (Crow)

```
myapi/
├── CMakeLists.txt          # Build configuration
├── conanfile.py           # Conan package config
├── README.md              # Documentation
├── .gitignore
├── include/
│   └── app/
│       ├── models.h       # Data model headers
│       ├── database.h     # Database interface
│       ├── handlers.h     # HTTP handler declarations
│       └── dto.h           # Data transfer objects
├── src/
│   ├── main.cpp           # Entry point
│   ├── models.cpp         # Model implementations
│   ├── database.cpp      # SQLite operations
│   ├── handlers.cpp       # HTTP handlers
│   └── routes.cpp         # Route setup
├── tests/
│   ├── test_models.cpp    # Model unit tests
│   └── test_database.cpp  # Database tests
└── data/                  # SQLite database location
```

### React Frontend

```
myapi-frontend/
├── package.json           # Dependencies and scripts
├── vite.config.js        # Vite configuration (or cra.config.js)
├── tailwind.config.js    # Tailwind config (if using Tailwind)
├── postcss.config.js     # PostCSS for Tailwind
├── index.html            # Entry HTML
├── nginx.conf            # Nginx configuration
├── Dockerfile            # Docker build
├── docker-compose.yml    # Full stack deployment
├── src/
│   ├── main.jsx          # Entry point
│   ├── App.jsx          # Main app component
│   ├── index.css         # Global styles
│   ├── components/       # Entity CRUD components
│   │   ├── UserList.jsx
│   │   ├── UserForm.jsx
│   │   ├── PostList.jsx
│   │   └── PostForm.jsx
│   ├── services/
│   │   └── api.js        # API client
│   └── store/            # State management
│       ├── StoreContext.jsx    # Context API store (default)
│       ├── useStore.js         # Zustand store
│       └── index.js            # Redux store (Redux only)
└── README.md
```

## Supported SQL Types

| SQL Type | C++ Type |
|----------|----------|
| INT, INTEGER | int32_t |
| BIGINT | int64_t |
| SMALLINT | int16_t |
| TINYINT | int8_t |
| BIT, BOOLEAN | bool |
| FLOAT, REAL | float |
| DOUBLE, DECIMAL, NUMERIC | double |
| VARCHAR, TEXT, CHAR | std::string |
| BLOB, BINARY | std::vector<uint8_t> |
| DATETIME, DATE, TIME | std::chrono::* |

## Docker Deployment

### Full Stack (C++ Backend + React)

```bash
cd myapi-frontend
docker-compose up --build
```

Access:
- Frontend: http://localhost:3000
- Backend API: http://localhost:8080/api

### Backend Only

```bash
cd myapi
docker build -t myapi .
docker run -p 8080:8080 -v ./data:/app/data myapi
```

## Dependencies

### C++ (Crow)

- Crow (header-only web framework)
- nlohmann/json (header-only JSON)
- SQLite3

### Python (Flask)

- Flask
- Flask-SQLAlchemy
- marshmallow-sqlalchemy

### Python (FastAPI)

- FastAPI
- SQLAlchemy
- Pydantic
- Alembic (migrations)

### React Frontend

- React 18
- React Router 6
- Axios
- react-hook-form
- (Optional) Tailwind CSS / Bootstrap
- (Optional) Zustand / Redux Toolkit

## License

MIT License
