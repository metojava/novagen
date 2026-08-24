#include "generator.hpp"
#include <sstream>

namespace novagen {

String Generator::generate_react_package_json(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << project_name_ << "-frontend\",\n";
    ss << "  \"version\": \"1.0.0\",\n";
    ss << "  \"private\": true,\n";
    ss << "  \"type\": \"module\",\n";
    ss << "  \"scripts\": {\n";

    if (react_bundler_ == ReactBundler::Vite) {
        ss << "    \"dev\": \"vite\",\n";
        ss << "    \"build\": \"vite build\",\n";
        ss << "    \"preview\": \"vite preview\",\n";
    } else {
        ss << "    \"start\": \"react-scripts start\",\n";
        ss << "    \"build\": \"react-scripts build\",\n";
        ss << "    \"test\": \"react-scripts test\",\n";
        ss << "    \"eject\": \"react-scripts eject\",\n";
    }

    ss << "    \"lint\": \"eslint . --ext js,jsx --report-unused-disable-directives --max-warnings 0\"\n";
    ss << "  },\n";
    ss << "  \"dependencies\": {\n";
    ss << "    \"react\": \"^18.2.0\",\n";
    ss << "    \"react-dom\": \"^18.2.0\",\n";
    ss << "    \"react-router-dom\": \"^6.22.0\",\n";
    ss << "    \"axios\": \"^1.6.7\",\n";
    ss << "    \"react-hook-form\": \"^7.50.1\"\n";

    if (react_css_ == ReactCSS::Bootstrap) {
        ss << ",\n    \"bootstrap\": \"^5.3.3\",\n";
        ss << "    \"react-bootstrap\": \"^2.10.0\"\n";
    }

    if (react_state_ == ReactState::Zustand) {
        ss << ",\n    \"zustand\": \"^4.5.0\"\n";
    } else if (react_state_ == ReactState::Redux) {
        ss << ",\n    \"@reduxjs/toolkit\": \"^2.2.0\",\n";
        ss << "    \"react-redux\": \"^9.1.0\"\n";
    }

    ss << "\n  },\n";
    ss << "  \"devDependencies\": {\n";
    ss << "    \"@types/react\": \"^18.2.55\",\n";
    ss << "    \"@types/react-dom\": \"^18.2.19\",\n";

    if (react_bundler_ == ReactBundler::Vite) {
        ss << "    \"@vitejs/plugin-react\": \"^4.2.1\",\n";
        ss << "    \"vite\": \"^5.1.0\",\n";
        ss << "    \"autoprefixer\": \"^10.4.17\",\n";
        ss << "    \"postcss\": \"^8.4.35\",\n";

        if (react_css_ == ReactCSS::Tailwind) {
            ss << "    \"tailwindcss\": \"^3.4.1\",\n";
        }
    } else {
        ss << "    \"react-scripts\": \"5.0.1\",\n";
        if (react_css_ == ReactCSS::Tailwind) {
            ss << "    \"tailwindcss\": \"^3.4.1\",\n";
            ss << "    \"autoprefixer\": \"^10.4.17\",\n";
            ss << "    \"postcss\": \"^8.4.35\",\n";
        }
    }

    ss << "    \"eslint\": \"^8.56.0\",\n";
    ss << "    \"eslint-plugin-react\": \"^7.33.2\",\n";
    ss << "    \"eslint-plugin-react-hooks\": \"^4.6.0\",\n";
    ss << "    \"eslint-plugin-react-refresh\": \"^0.4.5\"\n";
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_react_vite_config(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import { defineConfig } from 'vite'\n";
    ss << "import react from '@vitejs/plugin-react'\n\n";
    ss << "export default defineConfig({\n";
    ss << "  plugins: [react()],\n";
    ss << "  server: {\n";
    ss << "    port: 3000,\n";
    ss << "    proxy: {\n";
    ss << "      '/api': {\n";
    ss << "        target: 'http://localhost:8080',\n";
    ss << "        changeOrigin: true,\n";
    ss << "      },\n";
    ss << "    },\n";
    ss << "  },\n";
    ss << "})\n";
    return ss.str();
}

String Generator::generate_react_cra_config(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"({
  "extends": "react-scripts/config-overrides"
}
)";
    return ss.str();
}

String Generator::generate_react_webpack_config(const std::vector<Table>& tables) {
    (void)tables;
    bool is_ts = react_framework_ == ReactFramework::TypeScript;
    std::stringstream ss;
    ss << "const path = require('path');\n";
    ss << "const HtmlWebpackPlugin = require('html-webpack-plugin');\n";
    ss << "const MiniCssExtractPlugin = require('mini-css-extract-plugin');\n";
    ss << "const webpack = require('webpack');\n\n";
    ss << "module.exports = (env, argv) => ({\n";
    ss << "  mode: argv.mode || 'development',\n";
    ss << "  entry: './src/index." << (is_ts ? "tsx" : "jsx") << "',\n";
    ss << "  output: {\n";
    ss << "    path: path.resolve(__dirname, 'dist'),\n";
    ss << "    filename: '[name].[contenthash].js',\n";
    ss << "    clean: true,\n";
    ss << "    publicPath: '/',\n";
    ss << "  },\n";
    ss << "  resolve: {\n";
    if (is_ts) {
        ss << "    extensions: ['.tsx', '.ts', '.jsx', '.js'],\n";
    } else {
        ss << "    extensions: ['.jsx', '.js'],\n";
    }
    ss << "  },\n";
    ss << "  module: {\n";
    ss << "    rules: [\n";
    ss << "      {\n";
    ss << "        test: /\\.(js|jsx)$/,\n";
    ss << "        exclude: /node_modules/,\n";
    ss << "        use: {\n";
    ss << "          loader: 'babel-loader',\n";
    ss << "          options: {\n";
    ss << "            presets: ['@babel/preset-env', '@babel/preset-react'],\n";
    if (is_ts) {
        ss << "            presets: ['@babel/preset-env', '@babel/preset-react', '@babel/preset-typescript'],\n";
    }
    ss << "          },\n";
    ss << "        },\n";
    ss << "      },\n";
    if (is_ts) {
        ss << "      {\n";
        ss << "        test: /\\.tsx?$/,\n";
        ss << "        use: 'ts-loader',\n";
        ss << "        exclude: /node_modules/,\n";
        ss << "      },\n";
    }
    ss << "      {\n";
    ss << "        test: /\\.css$/,\n";
    ss << "        use: [\n";
    ss << "          argv.mode === 'production' ? MiniCssExtractPlugin.loader : 'style-loader',\n";
    ss << "          'css-loader',\n";
    ss << "          'postcss-loader',\n";
    ss << "        ],\n";
    ss << "      },\n";
    ss << "      {\n";
    ss << "        test: /\\.(png|svg|jpg|jpeg|gif|ico)$/i,\n";
    ss << "        type: 'asset/resource',\n";
    ss << "      },\n";
    ss << "      {\n";
    ss << "        test: /\\.(woff|woff2|eot|ttf|otf)$/i,\n";
    ss << "        type: 'asset/resource',\n";
    ss << "      },\n";
    ss << "    ],\n";
    ss << "  },\n";
    ss << "  plugins: [\n";
    ss << "    new HtmlWebpackPlugin({\n";
    ss << "      template: './index.html',\n";
    ss << "      inject: 'body',\n";
    ss << "    }),\n";
    if (react_theme_ == ReactTheme::Dark) {
        ss << "    new webpack.DefinePlugin({ 'process.env.NODE_ENV': JSON.stringify('production') }),\n";
    }
    ss << "  ],\n";
    ss << "  devServer: {\n";
    ss << "    static: {\n";
    ss << "      directory: path.join(__dirname, 'dist'),\n";
    ss << "    },\n";
    ss << "    compress: true,\n";
    ss << "    port: 3000,\n";
    ss << "    hot: true,\n";
    ss << "    historyApiFallback: true,\n";
    ss << "    proxy: [\n";
    ss << "      {\n";
    ss << "        context: ['/api', '/auth'],\n";
    ss << "        target: 'http://localhost:8080',\n";
    ss << "        changeOrigin: true,\n";
    ss << "      },\n";
    ss << "    ],\n";
    ss << "  },\n";
    ss << "  optimization: {\n";
    ss << "    splitChunks: {\n";
    ss << "      chunks: 'all',\n";
    ss << "    },\n";
    ss << "  },\n";
    ss << "});\n";
    return ss.str();
}

String Generator::generate_react_tsconfig(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"({
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "jsx": "react-jsx",
    "module": "ESNext",
    "moduleResolution": "bundler",
    "resolveJsonModule": true,
    "allowJs": true,
    "checkJs": false,
    "outDir": "./dist",
    "removeComments": true,
    "noEmit": false,
    "isolatedModules": true,
    "allowSyntheticDefaultImports": true,
    "esModuleInterop": true,
    "forceConsistentCasingInFileNames": true,
    "strict": true,
    "noUnusedLocals": true,
    "noUnusedParameters": true,
    "noImplicitReturns": true,
    "noFallthroughCasesInSwitch": true,
    "skipLibCheck": true
  },
  "include": ["src"],
  "exclude": ["node_modules", "dist"]
}
)";
    return ss.str();
}

String Generator::generate_react_index_html(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html lang=\"en\">\n";
    ss << "  <head>\n";
    ss << "    <meta charset=\"UTF-8\" />\n";
    ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";

    if (react_theme_ == ReactTheme::Dark) {
        ss << "    <meta name=\"color-scheme\" content=\"dark\" />\n";
    } else if (react_theme_ == ReactTheme::Auto) {
        ss << "    <meta name=\"color-scheme\" content=\"light dark\" />\n";
    }

    ss << "    <title>" << project_name_ << " - REST API Frontend</title>\n";
    ss << "  </head>\n";
    ss << "  <body>\n";
    ss << "    <div id=\"root\"></div>\n";
    ss << "    <script type=\"module\" src=\"/src/main.jsx\"></script>\n";
    ss << "  </body>\n";
    ss << "</html>\n";
    return ss.str();
}

String Generator::generate_react_main_jsx(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;

    if (react_state_ == ReactState::Redux) {
        ss << "import React from 'react'\n";
        ss << "import ReactDOM from 'react-dom/client'\n";
        ss << "import { Provider } from 'react-redux'\n";
        ss << "import { store } from './store'\n";
        ss << "import App from './App.jsx'\n";
        ss << "import './index.css'\n\n";
        ss << "ReactDOM.createRoot(document.getElementById('root')).render(\n";
        ss << "  <React.StrictMode>\n";
        ss << "    <Provider store={store}>\n";
        ss << "      <App />\n";
        ss << "    </Provider>\n";
        ss << "  </React.StrictMode>,\n";
        ss << ")\n";
    } else {
        ss << "import React from 'react'\n";
        ss << "import ReactDOM from 'react-dom/client'\n";
        ss << "import App from './App.jsx'\n";
        ss << "import './index.css'\n\n";
        ss << "ReactDOM.createRoot(document.getElementById('root')).render(\n";
        ss << "  <React.StrictMode>\n";
        ss << "    <App />\n";
        ss << "  </React.StrictMode>,\n";
        ss << ")\n";
    }

    return ss.str();
}

String Generator::generate_react_main_tsx(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;

    if (react_state_ == ReactState::Redux) {
        ss << "import React from 'react'\n";
        ss << "import ReactDOM from 'react-dom/client'\n";
        ss << "import { Provider } from 'react-redux'\n";
        ss << "import { store } from './store'\n";
        ss << "import App from './App.tsx'\n";
        ss << "import './index.css'\n\n";
        ss << "ReactDOM.createRoot(document.getElementById('root')!).render(\n";
        ss << "  <React.StrictMode>\n";
        ss << "    <Provider store={store}>\n";
        ss << "      <App />\n";
        ss << "    </Provider>\n";
        ss << "  </React.StrictMode>,\n";
        ss << ")\n";
    } else {
        ss << "import React from 'react'\n";
        ss << "import ReactDOM from 'react-dom/client'\n";
        ss << "import App from './App.tsx'\n";
        ss << "import './index.css'\n\n";
        ss << "ReactDOM.createRoot(document.getElementById('root')!).render(\n";
        ss << "  <React.StrictMode>\n";
        ss << "    <App />\n";
        ss << "  </React.StrictMode>,\n";
        ss << ")\n";
    }

    return ss.str();
}

String Generator::generate_react_index_css(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
  background-color: #f5f5f5;
  color: #333;
}

.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

nav {
  background-color: #2c3e50;
  padding: 1rem 2rem;
  margin-bottom: 2rem;
}

nav ul {
  list-style: none;
  display: flex;
  gap: 2rem;
}

nav a {
  color: white;
  text-decoration: none;
  font-weight: 500;
}

nav a:hover {
  color: #3498db;
}

h1 {
  margin-bottom: 1.5rem;
  color: #2c3e50;
}

.card {
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  padding: 1.5rem;
  margin-bottom: 1rem;
}

.btn {
  padding: 0.5rem 1rem;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background-color 0.2s;
}

.btn-primary {
  background-color: #3498db;
  color: white;
}

.btn-primary:hover {
  background-color: #2980b9;
}

.btn-danger {
  background-color: #e74c3c;
  color: white;
}

.btn-danger:hover {
  background-color: #c0392b;
}

.btn-secondary {
  background-color: #95a5a6;
  color: white;
}

.btn-secondary:hover {
  background-color: #7f8c8d;
}

table {
  width: 100%;
  border-collapse: collapse;
}

th, td {
  padding: 0.75rem;
  text-align: left;
  border-bottom: 1px solid #ddd;
}

th {
  background-color: #f8f9fa;
  font-weight: 600;
}

.form-group {
  margin-bottom: 1rem;
}

.form-group label {
  display: block;
  margin-bottom: 0.5rem;
  font-weight: 500;
}

.form-group input {
  width: 100%;
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 1rem;
}

.loading {
  text-align: center;
  padding: 2rem;
  color: #666;
}

.error {
  background-color: #fee;
  border: 1px solid #fcc;
  color: #c33;
  padding: 1rem;
  border-radius: 4px;
  margin-bottom: 1rem;
}

.success {
  background-color: #efe;
  border: 1px solid #cfc;
  color: #3c3;
  padding: 1rem;
  border-radius: 4px;
  margin-bottom: 1rem;
}

@media (prefers-color-scheme: dark) {
  body {
    background-color: #1a1a1a;
    color: #e0e0e0;
  }
  nav {
    background-color: #1a1a1a;
    border-bottom: 1px solid #333;
  }
  .card {
    background-color: #2d2d2d;
    color: #e0e0e0;
  }
  th {
    background-color: #3d3d3d;
  }
  th, td {
    border-bottom-color: #444;
  }
  .form-group input {
    background-color: #333;
    border-color: #444;
    color: #e0e0e0;
  }
}
)";
    return ss.str();
}

String Generator::generate_react_tailwind_config(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "/** @type {import('tailwindcss').Config} */\n";
    ss << "export default {\n";
    ss << "  content: [\n";
    ss << "    \"./index.html\",\n";
    ss << "    \"./src/**/*.{js,ts,jsx,tsx}\",\n";
    ss << "  ],\n";
    ss << "  darkMode: 'media',\n";
    ss << "  theme: {\n";
    ss << "    extend: {},\n";
    ss << "  },\n";
    ss << "  plugins: [],\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_react_bootstrap_css(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "@import 'bootstrap/dist/css/bootstrap.min.css';\n\n";
    ss << R"(.container { max-width: 1200px; }
.nav-link { cursor: pointer; }
)";
    return ss.str();
}

String Generator::generate_react_app_jsx(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { BrowserRouter, Routes, Route, Link } from 'react-router-dom'\n";

    if (react_state_ == ReactState::Redux) {
        ss << "import { useSelector } from 'react-redux'\n";
    } else if (react_state_ == ReactState::Zustand) {
        ss << "import { useStore } from './store/useStore'\n";
    }

    ss << "import './App.css'\n\n";

    for (const auto& table : tables) {
        ss << "import " << table.class_name() << "List from './components/" << table.class_name() << "List'\n";
        ss << "import " << table.class_name() << "Form from './components/" << table.class_name() << "Form'\n";
    }

    ss << R"(
function Home() {
  return (
    <div className="card">
      <h2>Welcome to )" << project_name_ << R"(</h2>
      <p>Select an entity from the navigation menu to manage your data.</p>
    </div>
  )
}
)";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << R"(
function NavBar() {
  const theme = localStorage.getItem('theme') || 'light';
  
  const toggleTheme = () => {
    const newTheme = theme === 'light' ? 'dark' : 'light';
    localStorage.setItem('theme', newTheme);
    document.documentElement.classList.toggle('dark', newTheme === 'dark');
  };

  return (
    <nav className="bg-gray-800 text-white p-4 mb-6">
      <div className="container mx-auto flex justify-between items-center">
        <ul className="flex gap-6">
          <li><Link to="/" className="hover:text-blue-400">Home</Link></li>
)";
        for (const auto& table : tables) {
            ss << "          <li><Link to=\"/" << pluralize(table.kebab_name()) << "\" className=\"hover:text-blue-400\">" << table.class_name() << "</Link></li>\n";
        }
        ss << R"(        </ul>
        <button onClick={toggleTheme} className="px-3 py-1 bg-gray-700 rounded hover:bg-gray-600">
          {theme === 'light' ? '🌙' : '☀️'}
        </button>
      </div>
    </nav>
  );
}
)";

        ss << R"(
function App() {
  const savedTheme = localStorage.getItem('theme');
  if (savedTheme === 'dark') {
    document.documentElement.classList.add('dark');
  }

  return (
    <BrowserRouter>
      <NavBar />
      <div className="container mx-auto px-4">
        <Routes>
          <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(        </Routes>
      </div>
    </BrowserRouter>
  )
}
)";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << R"(
function NavBar() {
  return (
    <nav className="navbar navbar-expand-lg navbar-dark bg-dark mb-4">
      <div className="container">
        <div className="navbar-nav">
          <Link to="/" className="nav-link">Home</Link>
)";
        for (const auto& table : tables) {
            ss << "          <Link to=\"/" << pluralize(table.kebab_name()) << "\" className=\"nav-link\">" << table.class_name() << "</Link>\n";
        }
        ss << R"(        </div>
      </div>
    </nav>
  );
}
)";

        ss << R"(
function App() {
  return (
    <BrowserRouter>
      <NavBar />
      <div className="container">
        <Routes>
          <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(        </Routes>
      </div>
    </BrowserRouter>
  )
}
)";

    } else {
        ss << R"(
function App() {
  return (
    <BrowserRouter>
      <div>
        <nav>
          <div className="container">
            <ul>
              <li><Link to="/">Home</Link></li>
)";
        for (const auto& table : tables) {
            ss << "              <li><Link to=\"/" << pluralize(table.kebab_name()) << "\">" << table.class_name() << "</Link></li>\n";
        }
        ss << R"(            </ul>
          </div>
        </nav>
        <div className="container">
          <Routes>
            <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(          </Routes>
        </div>
      </div>
    </BrowserRouter>
  )
}
)";
    }

    ss << "\nexport default App\n";
    return ss.str();
}

String Generator::generate_react_app_tsx(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { BrowserRouter, Routes, Route, Link } from 'react-router-dom'\n";

    if (react_state_ == ReactState::Redux) {
        ss << "import { useSelector } from 'react-redux'\n";
    } else if (react_state_ == ReactState::Zustand) {
        ss << "import { useStore } from './store/useStore'\n";
    }

    ss << "import './App.css'\n\n";

    for (const auto& table : tables) {
        ss << "import " << table.class_name() << "List from './components/" << table.class_name() << "List'\n";
        ss << "import " << table.class_name() << "Form from './components/" << table.class_name() << "Form'\n";
    }

    ss << R"(
interface HomeProps {}

const Home: React.FC<HomeProps> = () => {
  return (
    <div className="card">
      <h2>Welcome to )" << project_name_ << R"(</h2>
      <p>Select an entity from the navigation menu to manage your data.</p>
    </div>
  );
};
)";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << R"(
const NavBar: React.FC = () => {
  const [theme, setTheme] = React.useState(localStorage.getItem('theme') || 'light');

  const toggleTheme = () => {
    const newTheme = theme === 'light' ? 'dark' : 'light';
    setTheme(newTheme);
    localStorage.setItem('theme', newTheme);
    document.documentElement.classList.toggle('dark', newTheme === 'dark');
  };

  return (
    <nav className="bg-gray-800 text-white p-4 mb-6">
      <div className="container mx-auto flex justify-between items-center">
        <ul className="flex gap-6">
          <li><Link to="/" className="hover:text-blue-400">Home</Link></li>
)";
        for (const auto& table : tables) {
            ss << "          <li><Link to=\"/" << pluralize(table.kebab_name()) << "\" className=\"hover:text-blue-400\">" << table.class_name() << "</Link></li>\n";
        }
        ss << R"(        </ul>
        <button onClick={toggleTheme} className="px-3 py-1 bg-gray-700 rounded hover:bg-gray-600">
          {theme === 'light' ? '🌙' : '☀️'}
        </button>
      </div>
    </nav>
  );
};
)";

        ss << R"(
const App: React.FC = () => {
  React.useEffect(() => {
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme === 'dark') {
      document.documentElement.classList.add('dark');
    }
  }, []);

  return (
    <BrowserRouter>
      <NavBar />
      <div className="container mx-auto px-4">
        <Routes>
          <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(        </Routes>
      </div>
    </BrowserRouter>
  );
};
)";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << R"(
const NavBar: React.FC = () => {
  return (
    <nav className="navbar navbar-expand-lg navbar-dark bg-dark mb-4">
      <div className="container">
        <div className="navbar-nav">
          <Link to="/" className="nav-link">Home</Link>
)";
        for (const auto& table : tables) {
            ss << "          <Link to=\"/" << pluralize(table.kebab_name()) << "\" className=\"nav-link\">" << table.class_name() << "</Link>\n";
        }
        ss << R"(        </div>
      </div>
    </nav>
  );
};
)";

        ss << R"(
const App: React.FC = () => {
  return (
    <BrowserRouter>
      <NavBar />
      <div className="container">
        <Routes>
          <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "          <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(        </Routes>
      </div>
    </BrowserRouter>
  );
};
)";

    } else {
        ss << R"(
const App: React.FC = () => {
  return (
    <BrowserRouter>
      <div>
        <nav>
          <div className="container">
            <ul>
              <li><Link to="/">Home</Link></li>
)";
        for (const auto& table : tables) {
            ss << "              <li><Link to=\"/" << pluralize(table.kebab_name()) << "\">" << table.class_name() << "</Link></li>\n";
        }
        ss << R"(            </ul>
          </div>
        </nav>
        <div className="container">
          <Routes>
            <Route path="/" element={<Home />} />
)";
        for (const auto& table : tables) {
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "\" element={<" << table.class_name() << "List />} />\n";
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "/new\" element={<" << table.class_name() << "Form />} />\n";
            ss << "            <Route path=\"/" << pluralize(table.kebab_name()) << "/edit/:id\" element={<" << table.class_name() << "Form />} />\n";
        }
        ss << R"(          </Routes>
        </div>
      </div>
    </BrowserRouter>
  );
};
)";
    }

    ss << "\nexport default App\n";
    return ss.str();
}

String Generator::generate_react_store_context(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { createContext, useContext, useReducer, useCallback } from 'react'\n\n";

    for (const auto& table : tables) {
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n";
    }

    ss << R"(
const StoreContext = createContext(null);

const initialState = {
  loading: false,
  error: null,
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": [],\n";
    }

    ss << R"(
};

function reducer(state, action) {
  switch (action.type) {
    case 'SET_LOADING':
      return { ...state, loading: action.payload };
    case 'SET_ERROR':
      return { ...state, error: action.payload, loading: false };
    case 'CLEAR_ERROR':
      return { ...state, error: null };
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "    case 'SET_" << plural << "':\n";
        ss << "      return { ...state, " << plural << ": action.payload, loading: false };\n";
        ss << "    case 'ADD_" << table.instance_name() << "':\n";
        ss << "      return { ...state, " << plural << ": [...state." << plural << ", action.payload], loading: false };\n";
        ss << "    case 'UPDATE_" << table.instance_name() << "':\n";
        ss << "      return {\n";
        ss << "        ...state,\n";
        ss << "        " << plural << ": state." << plural << ".map(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " === action.payload." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "\n";
        ss << "            ? action.payload : item\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      };\n";
        ss << "    case 'DELETE_" << table.instance_name() << "':\n";
        ss << "      return {\n";
        ss << "        ...state,\n";
        ss << "        " << plural << ": state." << plural << ".filter(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " !== action.payload\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      };\n";
    }

    ss << R"(
    default:
      return state;
  }
}

export function StoreProvider({ children }) {
  const [state, dispatch] = useReducer(reducer, initialState);

  const actions = {
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "    fetch" << plural << ": useCallback(async () => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.getAll();\n";
        ss << "        dispatch({ type: 'SET_" << plural << "', payload: response.data });\n";
        ss << "      } catch (error) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    create" << table.class_name() << ": useCallback(async (data) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.create(data);\n";
        ss << "        dispatch({ type: 'ADD_" << table.instance_name() << "', payload: response.data });\n";
        ss << "        return response.data;\n";
        ss << "      } catch (error) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    update" << table.class_name() << ": useCallback(async (id, data) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.update(id, data);\n";
        ss << "        dispatch({ type: 'UPDATE_" << table.instance_name() << "', payload: response.data });\n";
        ss << "        return response.data;\n";
        ss << "      } catch (error) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    delete" << table.class_name() << ": useCallback(async (id) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        await " << table.instance_name() << "Api.delete(id);\n";
        ss << "        dispatch({ type: 'DELETE_" << table.instance_name() << "', payload: id });\n";
        ss << "      } catch (error) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";
    }

    ss << R"(
  };

  return (
    <StoreContext.Provider value={{ state, actions }}>
      {children}
    </StoreContext.Provider>
  );
}

export function useStore() {
  const context = useContext(StoreContext);
  if (!context) {
    throw new Error('useStore must be used within a StoreProvider');
  }
  return context;
}
)";
    return ss.str();
}

String Generator::generate_react_store_context_ts(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import React, { createContext, useContext, useReducer, useCallback, ReactNode } from 'react'\n\n";

    for (const auto& table : tables) {
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n";
        ss << "import { " << table.class_name() << " } from '../services/api'\n";
    }

    ss << R"(
interface State {
  loading: boolean;
  error: string | null;
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": " << table.class_name() << "[];\n";
    }

    ss << R"(
}

interface Action {
  type: string;
  payload?: any;
}

const initialState: State = {
  loading: false,
  error: null,
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": [],\n";
    }

    ss << R"(
};

function reducer(state: State, action: Action): State {
  switch (action.type) {
    case 'SET_LOADING':
      return { ...state, loading: action.payload };
    case 'SET_ERROR':
      return { ...state, error: action.payload, loading: false };
    case 'CLEAR_ERROR':
      return { ...state, error: null };
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "    case 'SET_" << plural << "':\n";
        ss << "      return { ...state, " << plural << ": action.payload as " << table.class_name() << "[], loading: false };\n";
        ss << "    case 'ADD_" << table.instance_name() << "':\n";
        ss << "      return { ...state, " << plural << ": [...state." << plural << ", action.payload as " << table.class_name() << "], loading: false };\n";
        ss << "    case 'UPDATE_" << table.instance_name() << "':\n";
        ss << "      return {\n";
        ss << "        ...state,\n";
        ss << "        " << plural << ": state." << plural << ".map(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " === (action.payload as " << table.class_name() << ")." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "\n";
        ss << "            ? action.payload : item\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      };\n";
        ss << "    case 'DELETE_" << table.instance_name() << "':\n";
        ss << "      return {\n";
        ss << "        ...state,\n";
        ss << "        " << plural << ": state." << plural << ".filter(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " !== action.payload\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      };\n";
    }

    ss << R"(
    default:
      return state;
  }
}

interface StoreContextType {
  state: State;
  actions: {
    loading: boolean;
    error: string | null;
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "    fetch" << plural << ": () => Promise<void>;\n";
        ss << "    create" << table.class_name() << ": (data: Omit<" << table.class_name() << ", '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "'>) => Promise<" << table.class_name() << ">;\n";
        ss << "    update" << table.class_name() << ": (id: number | string, data: " << table.class_name() << ") => Promise<" << table.class_name() << ">;\n";
        ss << "    delete" << table.class_name() << ": (id: number | string) => Promise<void>;\n";
    }

    ss << R"(
  };
}

const StoreContext = createContext<StoreContextType | null>(null);

export function StoreProvider({ children }: { children: ReactNode }) {
  const [state, dispatch] = useReducer(reducer, initialState);

  const actions = {
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "    fetch" << plural << ": useCallback(async () => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.getAll();\n";
        ss << "        dispatch({ type: 'SET_" << plural << "', payload: response.data });\n";
        ss << "      } catch (error: any) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    create" << table.class_name() << ": useCallback(async (data) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.create(data);\n";
        ss << "        dispatch({ type: 'ADD_" << table.instance_name() << "', payload: response.data });\n";
        ss << "        return response.data;\n";
        ss << "      } catch (error: any) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    update" << table.class_name() << ": useCallback(async (id, data) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        const response = await " << table.instance_name() << "Api.update(id, data);\n";
        ss << "        dispatch({ type: 'UPDATE_" << table.instance_name() << "', payload: response.data });\n";
        ss << "        return response.data;\n";
        ss << "      } catch (error: any) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";

        ss << "    delete" << table.class_name() << ": useCallback(async (id) => {\n";
        ss << "      dispatch({ type: 'SET_LOADING', payload: true });\n";
        ss << "      try {\n";
        ss << "        await " << table.instance_name() << "Api.delete(id);\n";
        ss << "        dispatch({ type: 'DELETE_" << table.instance_name() << "', payload: id });\n";
        ss << "      } catch (error: any) {\n";
        ss << "        dispatch({ type: 'SET_ERROR', payload: error.message });\n";
        ss << "        throw error;\n";
        ss << "      }\n";
        ss << "    }, []),\n";
    }

    ss << R"(
  };

  return (
    <StoreContext.Provider value={{ state, actions }}>
      {children}
    </StoreContext.Provider>
  );
}

export function useStore(): StoreContextType {
  const context = useContext(StoreContext);
  if (!context) {
    throw new Error('useStore must be used within a StoreProvider');
  }
  return context;
}
)";
    return ss.str();
}

String Generator::generate_react_store_zustand(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { create } from 'zustand'\n\n";

    for (const auto& table : tables) {
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n";
    }

    ss << R"(
const useStore = create((set, get) => ({
  loading: false,
  error: null,
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": [],\n";
    }

    ss << R"(
  setLoading: (loading) => set({ loading }),
  setError: (error) => set({ error, loading: false }),
  clearError: () => set({ error: null }),
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "  fetch" << plural << ": async () => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.getAll();\n";
        ss << "      set({ " << plural << ": response.data, loading: false });\n";
        ss << "    } catch (error) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  create" << table.class_name() << ": async (data) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.create(data);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": [...state." << plural << ", response.data],\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "      return response.data;\n";
        ss << "    } catch (error) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  update" << table.class_name() << ": async (id, data) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.update(id, data);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": state." << plural << ".map(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " === id ? response.data : item\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "      return response.data;\n";
        ss << "    } catch (error) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  delete" << table.class_name() << ": async (id) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      await " << table.instance_name() << "Api.delete(id);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": state." << plural << ".filter(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " !== id\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "    } catch (error) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";
    }

    ss << R"(
}));

export default useStore;
)";
    return ss.str();
}

String Generator::generate_react_store_zustand_ts(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { create } from 'zustand'\n\n";

    for (const auto& table : tables) {
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n";
        ss << "import { " << table.class_name() << " } from '../services/api'\n";
    }

    ss << R"(
interface StoreState {
  loading: boolean;
  error: string | null;
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": " << table.class_name() << "[];\n";
    }

    ss << R"(
}

interface StoreActions {
  setLoading: (loading: boolean) => void;
  setError: (error: string) => void;
  clearError: () => void;
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "  fetch" << plural << ": () => Promise<void>;\n";
        ss << "  create" << table.class_name() << ": (data: Omit<" << table.class_name() << ", '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "'>) => Promise<" << table.class_name() << ">;\n";
        ss << "  update" << table.class_name() << ": (id: number | string, data: " << table.class_name() << ") => Promise<" << table.class_name() << ">;\n";
        ss << "  delete" << table.class_name() << ": (id: number | string) => Promise<void>;\n";
    }

    ss << R"(
}

type Store = StoreState & StoreActions;

const useStore = create<Store>((set) => ({
  loading: false,
  error: null,
)";

    for (const auto& table : tables) {
        ss << "  " << pluralize(table.instance_name()) << ": [],\n";
    }

    ss << R"(
  setLoading: (loading) => set({ loading }),
  setError: (error) => set({ error, loading: false }),
  clearError: () => set({ error: null }),
)";

    for (const auto& table : tables) {
        String plural = pluralize(table.instance_name());
        ss << "  fetch" << plural << ": async () => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.getAll();\n";
        ss << "      set({ " << plural << ": response.data, loading: false });\n";
        ss << "    } catch (error: any) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  create" << table.class_name() << ": async (data) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.create(data);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": [...state." << plural << ", response.data],\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "      return response.data;\n";
        ss << "    } catch (error: any) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  update" << table.class_name() << ": async (id, data) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.update(id, data);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": state." << plural << ".map(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " === id ? response.data : item\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "      return response.data;\n";
        ss << "    } catch (error: any) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";

        ss << "  delete" << table.class_name() << ": async (id) => {\n";
        ss << "    set({ loading: true });\n";
        ss << "    try {\n";
        ss << "      await " << table.instance_name() << "Api.delete(id);\n";
        ss << "      set(state => ({\n";
        ss << "        " << plural << ": state." << plural << ".filter(item =>\n";
        ss << "          item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " !== id\n";
        ss << "        ),\n";
        ss << "        loading: false\n";
        ss << "      }));\n";
        ss << "    } catch (error: any) {\n";
        ss << "      set({ error: error.message, loading: false });\n";
        ss << "      throw error;\n";
        ss << "    }\n";
        ss << "  },\n";
    }

    ss << R"(
}));

export default useStore;
)";
    return ss.str();
}

String Generator::generate_react_store_redux(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { configureStore } from '@reduxjs/toolkit'\n";

    for (const auto& table : tables) {
        ss << "import " << table.instance_name() << "Reducer from './slices/" << table.instance_name() << "Slice'\n";
    }

    ss << R"(
export const store = configureStore({
  reducer: {
)";
    for (const auto& table : tables) {
        ss << "    " << table.instance_name() << ": " << table.instance_name() << "Reducer,\n";
    }
    ss << R"(
  },
});

export default store;
)";
    return ss.str();
}

String Generator::generate_react_store_redux_ts(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import { configureStore } from '@reduxjs/toolkit'\n";
    ss << "import type { TypedUseSelectorHook } from 'react-redux'\n";
    ss << "import type { RootState, AppDispatch } from './store'\n\n";

    for (const auto& table : tables) {
        ss << "import " << table.instance_name() << "Reducer from './slices/" << table.instance_name() << "Slice'\n";
    }

    ss << R"(
export const store = configureStore({
  reducer: {
)";
    for (const auto& table : tables) {
        ss << "    " << table.instance_name() << ": " << table.instance_name() << "Reducer,\n";
    }
    ss << R"(
  },
});

export type RootState = ReturnType<typeof store.getState>;
export type AppDispatch = typeof store.dispatch;

export const useAppSelector: TypedUseSelectorHook<RootState> = useSelector;
export const useAppDispatch: () => AppDispatch = useDispatch;

export default store;
)";
    return ss.str();
}

String Generator::generate_react_api_service(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import axios from 'axios'\n\n";
    ss << "const API_BASE = '/api'\n\n";
    ss << "const api = axios.create({\n";
    ss << "  baseURL: API_BASE,\n";
    ss << "  headers: {\n";
    ss << "    'Content-Type': 'application/json',\n";
    ss << "  },\n";
    ss << "})\n\n";

    for (const auto& table : tables) {
        String pk_name = table.pk_column() ? table.pk_column()->field_name() : "id";
        String plural = pluralize(table.kebab_name());

        ss << "export const " << table.instance_name() << "Api = {\n";
        ss << "  getAll: () => api.get('/" << plural << "'),\n";
        ss << "  getById: (id) => api.get('/" << plural << "/' + id),\n";
        ss << "  create: (data) => api.post('/" << plural << "', data),\n";
        ss << "  update: (id, data) => api.put('/" << plural << "/' + id, data),\n";
        ss << "  delete: (id) => api.delete('/" << plural << "/' + id),\n";
        ss << "  count: () => api.get('/" << plural << "/count'),\n";
        ss << "}\n\n";
    }

    ss << "export default api\n";
    return ss.str();
}

String Generator::generate_react_api_service_ts(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import axios, { AxiosInstance, AxiosResponse } from 'axios'\n\n";

    ss << "const API_BASE = '/api'\n\n";
    ss << "const api: AxiosInstance = axios.create({\n";
    ss << "  baseURL: API_BASE,\n";
    ss << "  headers: {\n";
    ss << "    'Content-Type': 'application/json',\n";
    ss << "  },\n";
    ss << "})\n\n";

    for (const auto& table : tables) {
        String pk_name = table.pk_column() ? table.pk_column()->field_name() : "id";
        String plural = pluralize(table.kebab_name());
        ss << "export interface " << table.class_name() << " {\n";
        ss << "  " << pk_name << ": number | string;\n";
        for (const auto& col : table.columns) {
            if (col.field_name() != pk_name) {
                ss << "  " << col.field_name() << ": " << col.cpp_type() << ";\n";
            }
        }
        ss << "}\n\n";

        ss << "type " << table.class_name() << "Create = Omit<" << table.class_name() << ", '" << pk_name << "'>;\n";
        ss << "type " << table.class_name() << "Update = " << table.class_name() << ";\n\n";

        ss << "export const " << table.instance_name() << "Api = {\n";
        ss << "  getAll: (): Promise<AxiosResponse<" << table.class_name() << "[]>> =>\n";
        ss << "    api.get('/" << plural << "'),\n";
        ss << "  getById: (id: number | string): Promise<AxiosResponse<" << table.class_name() << ">> =>\n";
        ss << "    api.get('/" << plural << "/' + id),\n";
        ss << "  create: (data: " << table.class_name() << "Create): Promise<AxiosResponse<" << table.class_name() << ">> =>\n";
        ss << "    api.post('/" << plural << "', data),\n";
        ss << "  update: (id: number | string, data: " << table.class_name() << "Update): Promise<AxiosResponse<" << table.class_name() << ">> =>\n";
        ss << "    api.put('/" << plural << "/' + id, data),\n";
        ss << "  delete: (id: number | string): Promise<AxiosResponse<void>> =>\n";
        ss << "    api.delete('/" << plural << "/' + id),\n";
        ss << "  count: (): Promise<AxiosResponse<number>> =>\n";
        ss << "    api.get('/" << plural << "/count'),\n";
        ss << "}\n\n";
    }

    ss << "export default api\n";
    return ss.str();
}

String Generator::generate_react_component_list(const Table& table) {
    std::stringstream ss;

    if (react_state_ == ReactState::Context || react_state_ == ReactState::Zustand) {
        String storeImport = react_state_ == ReactState::Context ? "useStore" : "useStore";
        ss << "import { useEffect } from 'react'\n";
        ss << "import { Link } from 'react-router-dom'\n";
        ss << "import { " << storeImport << " } from '../store/" << (react_state_ == ReactState::Context ? "StoreContext" : "useStore") << "'\n\n";

        ss << "function " << table.class_name() << "List() {\n";
        ss << "  const { state, actions } = " << storeImport << "()\n";
        ss << "  const " << pluralize(table.instance_name()) << " = state." << pluralize(table.instance_name()) << "\n";
        ss << "  const loading = state.loading\n";
        ss << "  const error = state.error\n\n";

        ss << "  useEffect(() => {\n";
        ss << "    actions.fetch" << pluralize(table.instance_name()) << "()\n";
        ss << "  }, [])\n\n";

        ss << "  const handleDelete = async (id) => {\n";
        ss << "    if (!window.confirm('Are you sure you want to delete this item?')) return\n";
        ss << "    try {\n";
        ss << "      await actions.delete" << table.class_name() << "(id)\n";
        ss << "    } catch (err) {\n";
        ss << "      // Error handled by store\n";
        ss << "    }\n";
        ss << "  }\n\n";

    } else if (react_state_ == ReactState::Redux) {
        ss << "import { useEffect } from 'react'\n";
        ss << "import { Link } from 'react-router-dom'\n";
        ss << "import { useDispatch, useSelector } from 'react-redux'\n";
        ss << "import { fetch" << pluralize(table.instance_name()) << ", delete" << table.class_name() << " } from '../store/slices/" << table.instance_name() << "Slice'\n\n";

        ss << "function " << table.class_name() << "List() {\n";
        ss << "  const dispatch = useDispatch()\n";
        ss << "  const " << pluralize(table.instance_name()) << " = useSelector(state => state." << table.instance_name() << ".items)\n";
        ss << "  const loading = useSelector(state => state." << table.instance_name() << ".loading)\n";
        ss << "  const error = useSelector(state => state." << table.instance_name() << ".error)\n\n";

        ss << "  useEffect(() => {\n";
        ss << "    dispatch(fetch" << pluralize(table.instance_name()) << "())\n";
        ss << "  }, [dispatch])\n\n";

        ss << "  const handleDelete = async (id) => {\n";
        ss << "    if (!window.confirm('Are you sure you want to delete this item?')) return\n";
        ss << "    dispatch(delete" << table.class_name() << "(id))\n";
        ss << "  }\n\n";

    } else {
        ss << "import { useState, useEffect } from 'react'\n";
        ss << "import { Link } from 'react-router-dom'\n";
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n\n";

        ss << "function " << table.class_name() << "List() {\n";
        ss << "  const [data, setData] = useState([])\n";
        ss << "  const [loading, setLoading] = useState(true)\n";
        ss << "  const [error, setError] = useState(null)\n\n";

        ss << "  useEffect(() => {\n";
        ss << "    fetchData()\n";
        ss << "  }, [])\n\n";

        ss << "  const fetchData = async () => {\n";
        ss << "    try {\n";
        ss << "      setLoading(true)\n";
        ss << "      const response = await " << table.instance_name() << "Api.getAll()\n";
        ss << "      setData(response.data)\n";
        ss << "      setError(null)\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.response?.data?.message || 'Failed to fetch data')\n";
        ss << "    } finally {\n";
        ss << "      setLoading(false)\n";
        ss << "    }\n";
        ss << "  }\n\n";

        ss << "  const handleDelete = async (id) => {\n";
        ss << "    if (!window.confirm('Are you sure you want to delete this item?')) return\n";
        ss << "    try {\n";
        ss << "      await " << table.instance_name() << "Api.delete(id)\n";
        ss << "      fetchData()\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.response?.data?.message || 'Failed to delete')\n";
        ss << "    }\n";
        ss << "  }\n\n";
    }

    ss << "  if (loading) return <div className=\"loading\">Loading...</div>\n\n";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div className=\"flex justify-between items-center mb-4\">\n";
        ss << "        <h1 className=\"text-2xl font-bold\">" << table.class_name() << " List</h1>\n";
        ss << "        <Link to={\"/" << pluralize(table.kebab_name()) << "/new\"} className=\"bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"bg-white p-6 rounded shadow\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <div className=\"bg-white rounded shadow overflow-hidden\">\n";
        ss << "          <table className=\"min-w-full\">\n";
        ss << "            <thead className=\"bg-gray-50\">\n";
        ss << "              <tr>\n";
        for (const auto& col : table.columns) {
            ss << "                <th className=\"px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider\">" << col.field_name() << "</th>\n";
        }
        ss << "                <th className=\"px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider\">Actions</th>\n";
        ss << "              </tr>\n";
        ss << "            </thead>\n";
        ss << "            <tbody className=\"bg-white divide-y divide-gray-200\">\n";
        ss << "              {data.map((item) => (\n";
        ss << "                <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                  <td className=\"px-6 py-4 whitespace-nowrap text-sm text-gray-900\">{item." << col.field_name() << "}</td>\n";
        }
        ss << "                  <td className=\"px-6 py-4 whitespace-nowrap text-sm\">\n";
        ss << "                    <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"text-indigo-600 hover:text-indigo-900 mr-4\">Edit</Link>\n";
        ss << "                    <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"text-red-600 hover:text-red-900\">Delete</button>\n";
        ss << "                  </td>\n";
        ss << "                </tr>\n";
        ss << "              ))}\n";
        ss << "            </tbody>\n";
        ss << "          </table>\n";
        ss << "        </div>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div className=\"d-flex justify-content-between align-items-center mb-4\">\n";
        ss << "        <h1>" << table.class_name() << " List</h1>\n";
        ss << "        <Link to={\"/" << pluralize(table.kebab_name()) << "/new\"} className=\"btn btn-primary\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"alert alert-danger\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"card\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <table className=\"table table-striped table-hover\">\n";
        ss << "          <thead>\n";
        ss << "            <tr>\n";
        for (const auto& col : table.columns) {
            ss << "              <th>" << col.field_name() << "</th>\n";
        }
        ss << "              <th>Actions</th>\n";
        ss << "            </tr>\n";
        ss << "          </thead>\n";
        ss << "          <tbody>\n";
        ss << "            {data.map((item) => (\n";
        ss << "              <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                <td>{item." << col.field_name() << "}</td>\n";
        }
        ss << "                <td>\n";
        ss << "                  <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"btn btn-sm btn-secondary me-2\">Edit</Link>\n";
        ss << "                  <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"btn btn-sm btn-danger\">Delete</button>\n";
        ss << "                </td>\n";
        ss << "              </tr>\n";
        ss << "            ))}\n";
        ss << "          </tbody>\n";
        ss << "        </table>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div style={{display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '1rem'}}>\n";
        ss << "        <h1>" << table.class_name() << " List</h1>\n";
        ss << "        <Link to=\"/" << pluralize(table.kebab_name()) << "/new\" className=\"btn btn-primary\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"error\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"card\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <div className=\"card\">\n";
        ss << "          <table>\n";
        ss << "            <thead>\n";
        ss << "              <tr>\n";
        for (const auto& col : table.columns) {
            ss << "                <th>" << col.field_name() << "</th>\n";
        }
        ss << "                <th>Actions</th>\n";
        ss << "              </tr>\n";
        ss << "            </thead>\n";
        ss << "            <tbody>\n";
        ss << "              {data.map((item) => (\n";
        ss << "                <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                  <td>{item." << col.field_name() << "}</td>\n";
        }
        ss << "                  <td>\n";
        ss << "                    <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"btn btn-secondary\" style={{marginRight: '0.5rem'}}>Edit</Link>\n";
        ss << "                    <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"btn btn-danger\">Delete</button>\n";
        ss << "                  </td>\n";
        ss << "                </tr>\n";
        ss << "              ))}\n";
        ss << "            </tbody>\n";
        ss << "          </table>\n";
        ss << "        </div>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";
    }

    ss << "}\n\n";
    ss << "export default " << table.class_name() << "List\n";

    return ss.str();
}

String Generator::generate_react_component_list_ts(const Table& table) {
    std::stringstream ss;
    ss << "import React, { useEffect } from 'react'\n";
    ss << "import { Link, useParams } from 'react-router-dom'\n";
    ss << "import { " << table.class_name() << " } from '../services/api'\n\n";

    ss << "const " << table.class_name() << "List: React.FC = () => {\n";
    ss << "  const [data, setData] = React.useState<" << table.class_name() << "[]>([])\n";
    ss << "  const [loading, setLoading] = React.useState<boolean>(true)\n";
    ss << "  const [error, setError] = React.useState<string | null>(null)\n\n";

    ss << "  useEffect(() => {\n";
    ss << "    fetchData()\n";
    ss << "  }, [])\n\n";

    ss << "  const fetchData = async () => {\n";
    ss << "    try {\n";
    ss << "      setLoading(true)\n";
    ss << "      const response = await " << table.instance_name() << "Api.getAll()\n";
    ss << "      setData(response.data)\n";
    ss << "      setError(null)\n";
    ss << "    } catch (err: any) {\n";
    ss << "      setError(err.response?.data?.message || 'Failed to fetch data')\n";
    ss << "    } finally {\n";
    ss << "      setLoading(false)\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  const handleDelete = async (id: number | string) => {\n";
    ss << "    if (!window.confirm('Are you sure you want to delete this item?')) return\n";
    ss << "    try {\n";
    ss << "      await " << table.instance_name() << "Api.delete(id)\n";
    ss << "      fetchData()\n";
    ss << "    } catch (err: any) {\n";
    ss << "      setError(err.response?.data?.message || 'Failed to delete')\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  if (loading) return <div className=\"loading\">Loading...</div>\n\n";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div className=\"flex justify-between items-center mb-4\">\n";
        ss << "        <h1 className=\"text-2xl font-bold\">" << table.class_name() << " List</h1>\n";
        ss << "        <Link to={\"/" << pluralize(table.kebab_name()) << "/new\"} className=\"bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"bg-white p-6 rounded shadow\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <div className=\"bg-white rounded shadow overflow-hidden\">\n";
        ss << "          <table className=\"min-w-full\">\n";
        ss << "            <thead className=\"bg-gray-50\">\n";
        ss << "              <tr>\n";
        for (const auto& col : table.columns) {
            ss << "                <th className=\"px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider\">" << col.field_name() << "</th>\n";
        }
        ss << "                <th className=\"px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider\">Actions</th>\n";
        ss << "              </tr>\n";
        ss << "            </thead>\n";
        ss << "            <tbody className=\"bg-white divide-y divide-gray-200\">\n";
        ss << "              {data.map((item) => (\n";
        ss << "                <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                  <td className=\"px-6 py-4 whitespace-nowrap text-sm text-gray-900\">{item." << col.field_name() << "}</td>\n";
        }
        ss << "                  <td className=\"px-6 py-4 whitespace-nowrap text-sm\">\n";
        ss << "                    <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"text-indigo-600 hover:text-indigo-900 mr-4\">Edit</Link>\n";
        ss << "                    <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"text-red-600 hover:text-red-900\">Delete</button>\n";
        ss << "                  </td>\n";
        ss << "                </tr>\n";
        ss << "              ))}\n";
        ss << "            </tbody>\n";
        ss << "          </table>\n";
        ss << "        </div>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div className=\"d-flex justify-content-between align-items-center mb-4\">\n";
        ss << "        <h1>" << table.class_name() << " List</h1>\n";
        ss << "        <Link to={\"/" << pluralize(table.kebab_name()) << "/new\"} className=\"btn btn-primary\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"alert alert-danger\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"card\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <table className=\"table table-striped table-hover\">\n";
        ss << "          <thead>\n";
        ss << "            <tr>\n";
        for (const auto& col : table.columns) {
            ss << "              <th>" << col.field_name() << "</th>\n";
        }
        ss << "              <th>Actions</th>\n";
        ss << "            </tr>\n";
        ss << "          </thead>\n";
        ss << "          <tbody>\n";
        ss << "            {data.map((item) => (\n";
        ss << "              <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                <td>{item." << col.field_name() << "}</td>\n";
        }
        ss << "                <td>\n";
        ss << "                  <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"btn btn-sm btn-secondary me-2\">Edit</Link>\n";
        ss << "                  <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"btn btn-sm btn-danger\">Delete</button>\n";
        ss << "                </td>\n";
        ss << "              </tr>\n";
        ss << "            ))}\n";
        ss << "          </tbody>\n";
        ss << "        </table>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <div style={{display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '1rem'}}>\n";
        ss << "        <h1>" << table.class_name() << " List</h1>\n";
        ss << "        <Link to=\"/" << pluralize(table.kebab_name()) << "/new\" className=\"btn btn-primary\">Add New</Link>\n";
        ss << "      </div>\n\n";
        ss << "      {error && <div className=\"error\">{error}</div>}\n\n";
        ss << "      {data.length === 0 ? (\n";
        ss << "        <div className=\"card\">No data available</div>\n";
        ss << "      ) : (\n";
        ss << "        <div className=\"card\">\n";
        ss << "          <table>\n";
        ss << "            <thead>\n";
        ss << "              <tr>\n";
        for (const auto& col : table.columns) {
            ss << "                <th>" << col.field_name() << "</th>\n";
        }
        ss << "                <th>Actions</th>\n";
        ss << "              </tr>\n";
        ss << "            </thead>\n";
        ss << "            <tbody>\n";
        ss << "              {data.map((item) => (\n";
        ss << "                <tr key={item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "}>\n";
        for (const auto& col : table.columns) {
            ss << "                  <td>{item." << col.field_name() << "}</td>\n";
        }
        ss << "                  <td>\n";
        ss << "                    <Link to={\"/" << pluralize(table.kebab_name()) << "/edit/\" + item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << "} className=\"btn btn-secondary\" style={{marginRight: '0.5rem'}}>Edit</Link>\n";
        ss << "                    <button onClick={() => handleDelete(item." << (table.pk_column() ? table.pk_column()->field_name() : "id") << ")} className=\"btn btn-danger\">Delete</button>\n";
        ss << "                  </td>\n";
        ss << "                </tr>\n";
        ss << "              ))}\n";
        ss << "            </tbody>\n";
        ss << "          </table>\n";
        ss << "        </div>\n";
        ss << "      )}\n";
        ss << "    </div>\n";
        ss << "  )\n";
    }

    ss << "}\n\n";
    ss << "export default " << table.class_name() << "List\n";

    return ss.str();
}

String Generator::generate_react_component_form(const Table& table) {
    std::stringstream ss;

    if (react_state_ == ReactState::Context || react_state_ == ReactState::Zustand) {
        String storeImport = react_state_ == ReactState::Context ? "useStore" : "useStore";
        ss << "import { useState, useEffect } from 'react'\n";
        ss << "import { useNavigate, useParams } from 'react-router-dom'\n";
        ss << "import { " << storeImport << " } from '../store/" << (react_state_ == ReactState::Context ? "StoreContext" : "useStore") << "'\n\n";

        ss << "function " << table.class_name() << "Form() {\n";
        ss << "  const navigate = useNavigate()\n";
        ss << "  const { id } = useParams()\n";
        ss << "  const isEditMode = Boolean(id)\n";
        ss << "  const { state, actions } = " << storeImport << "()\n\n";

    } else if (react_state_ == ReactState::Redux) {
        ss << "import { useState, useEffect } from 'react'\n";
        ss << "import { useNavigate, useParams } from 'react-router-dom'\n";
        ss << "import { useDispatch, useSelector } from 'react-redux'\n";
        ss << "import { fetch" << table.class_name() << "ById, create" << table.class_name() << ", update" << table.class_name() << " } from '../store/slices/" << table.instance_name() << "Slice'\n\n";

        ss << "function " << table.class_name() << "Form() {\n";
        ss << "  const navigate = useNavigate()\n";
        ss << "  const { id } = useParams()\n";
        ss << "  const isEditMode = Boolean(id)\n";
        ss << "  const dispatch = useDispatch()\n";
        ss << "  const item = useSelector(state => id ? state." << table.instance_name() << ".currentItem : null)\n";
        ss << "  const loading = useSelector(state => state." << table.instance_name() << ".loading)\n\n";

    } else {
        ss << "import { useState, useEffect } from 'react'\n";
        ss << "import { useNavigate, useParams } from 'react-router-dom'\n";
        ss << "import { " << table.instance_name() << "Api } from '../services/api'\n\n";

        ss << "function " << table.class_name() << "Form() {\n";
        ss << "  const navigate = useNavigate()\n";
        ss << "  const { id } = useParams()\n";
        ss << "  const isEditMode = Boolean(id)\n\n";
    }

    ss << "  const [formData, setFormData] = useState({\n";
    bool first = true;
    for (const auto& col : table.columns) {
        if (col.primary_key && col.auto_increment) continue;
        if (!first) ss << ",\n    ";
        first = false;
        if (col.cpp_type() == "std::string") {
            ss << col.field_name() << ": ''";
        } else if (col.cpp_type() == "bool") {
            ss << col.field_name() << ": false";
        } else {
            ss << col.field_name() << ": ''";
        }
    }
    ss << "\n  })\n";
    ss << "  const [error, setError] = useState(null)\n\n";

    if (react_state_ == ReactState::Context || react_state_ == ReactState::Zustand) {
        ss << "  useEffect(() => {\n";
        ss << "    if (isEditMode && id) {\n";
        ss << "      const item = state." << pluralize(table.instance_name()) << ".find(i => i." << (table.pk_column() ? table.pk_column()->field_name() : "id") << " === parseInt(id))\n";
        ss << "      if (item) setFormData(item)\n";
        ss << "    }\n";
        ss << "  }, [id, state." << pluralize(table.instance_name()) << "])\n\n";
    } else if (react_state_ == ReactState::Redux) {
        ss << "  useEffect(() => {\n";
        ss << "    if (isEditMode && id) {\n";
        ss << "      dispatch(fetch" << table.class_name() << "ById(id))\n";
        ss << "    }\n";
        ss << "  }, [id, dispatch])\n\n";
        ss << "  useEffect(() => {\n";
        ss << "    if (item) setFormData(item)\n";
        ss << "  }, [item])\n\n";
    } else {
        ss << "  useEffect(() => {\n";
        ss << "    if (isEditMode) {\n";
        ss << "      fetchData()\n";
        ss << "    }\n";
        ss << "  }, [id])\n\n";

        ss << "  const fetchData = async () => {\n";
        ss << "    try {\n";
        ss << "      const response = await " << table.instance_name() << "Api.getById(id)\n";
        ss << "      setFormData(response.data)\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.response?.data?.message || 'Failed to fetch data')\n";
        ss << "    }\n";
        ss << "  }\n\n";
    }

    ss << "  const handleChange = (e) => {\n";
    ss << "    const { name, value, type, checked } = e.target\n";
    ss << "    setFormData(prev => ({\n";
    ss << "      ...prev,\n";
    ss << "      [name]: type === 'checkbox' ? checked : value\n";
    ss << "    }))\n";
    ss << "  }\n\n";

    ss << "  const handleSubmit = async (e) => {\n";
    ss << "    e.preventDefault()\n";
    ss << "    setError(null)\n\n";

    if (react_state_ == ReactState::Context || react_state_ == ReactState::Zustand) {
        ss << "    try {\n";
        ss << "      if (isEditMode) {\n";
        ss << "        await actions.update" << table.class_name() << "(id, formData)\n";
        ss << "      } else {\n";
        ss << "        await actions.create" << table.class_name() << "(formData)\n";
        ss << "      }\n";
        ss << "      navigate('/" << pluralize(table.kebab_name()) << "')\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.response?.data?.message || 'Failed to save')\n";
        ss << "    }\n\n";
    } else if (react_state_ == ReactState::Redux) {
        ss << "    try {\n";
        ss << "      if (isEditMode) {\n";
        ss << "        await dispatch(update" << table.class_name() << "({ id, data: formData })).unwrap()\n";
        ss << "      } else {\n";
        ss << "        await dispatch(create" << table.class_name() << "(formData)).unwrap()\n";
        ss << "      }\n";
        ss << "      navigate('/" << pluralize(table.kebab_name()) << "')\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.message || 'Failed to save')\n";
        ss << "    }\n\n";
    } else {
        ss << "    try {\n";
        ss << "      if (isEditMode) {\n";
        ss << "        await " << table.instance_name() << "Api.update(id, formData)\n";
        ss << "      } else {\n";
        ss << "        await " << table.instance_name() << "Api.create(formData)\n";
        ss << "      }\n";
        ss << "      navigate('/" << pluralize(table.kebab_name()) << "')\n";
        ss << "    } catch (err) {\n";
        ss << "      setError(err.response?.data?.message || 'Failed to save')\n";
        ss << "    }\n\n";
    }

    ss << "  }\n\n";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1 className=\"text-2xl font-bold mb-4\">{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4\">{error}</div>}\n\n";
        ss << "      <div className=\"bg-white p-6 rounded shadow\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"mb-4\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\" className=\"block text-sm font-medium text-gray-700 mb-1\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              className=\"rounded border-gray-300\"\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "              className=\"w-full px-3 py-2 border border-gray-300 rounded focus:outline-none focus:ring-2 focus:ring-blue-500\"\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div className=\"flex gap-4 mt-6\">\n";
        ss << "            <button type=\"submit\" className=\"bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600 disabled:opacity-50\">\n";
        ss << "              Save\n";
        ss << "            </button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"bg-gray-500 text-white px-4 py-2 rounded hover:bg-gray-600\">\n";
        ss << "              Cancel\n";
        ss << "            </button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1>{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"alert alert-danger\">{error}</div>}\n\n";
        ss << "      <div className=\"card\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"mb-3\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\" className=\"form-label\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              className=\"form-check-input\"\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "              className=\"form-control\"\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div className=\"d-flex gap-2 mt-4\">\n";
        ss << "            <button type=\"submit\" className=\"btn btn-primary\">Save</button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"btn btn-secondary\">Cancel</button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1>{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"error\">{error}</div>}\n\n";
        ss << "      <div className=\"card\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"form-group\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << "}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div style={{display: 'flex', gap: '1rem', marginTop: '1rem'}}>\n";
        ss << "            <button type=\"submit\" className=\"btn btn-primary\">Save</button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"btn btn-secondary\">Cancel</button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";
    }

    ss << "}\n\n";
    ss << "export default " << table.class_name() << "Form\n";

    return ss.str();
}

String Generator::generate_react_component_form_ts(const Table& table) {
    std::stringstream ss;
    ss << "import React, { useState, useEffect } from 'react'\n";
    ss << "import { useNavigate, useParams } from 'react-router-dom'\n";
    ss << "import { " << table.class_name() << ", " << table.instance_name() << "Api } from '../services/api'\n\n";

    ss << "const " << table.class_name() << "Form: React.FC = () => {\n";
    ss << "  const navigate = useNavigate()\n";
    ss << "  const { id } = useParams<{ id?: string }>()\n";
    ss << "  const isEditMode = Boolean(id)\n\n";

    ss << "  const [formData, setFormData] = useState<Partial<" << table.class_name() << ">>({\n";
    bool first = true;
    for (const auto& col : table.columns) {
        if (col.primary_key && col.auto_increment) continue;
        if (!first) ss << ",\n    ";
        first = false;
        if (col.cpp_type() == "std::string") {
            ss << col.field_name() << ": ''";
        } else if (col.cpp_type() == "bool") {
            ss << col.field_name() << ": false";
        } else {
            ss << col.field_name() << ": ''";
        }
    }
    ss << "\n  })\n";
    ss << "  const [error, setError] = useState<string | null>(null)\n\n";

    ss << "  useEffect(() => {\n";
    ss << "    if (isEditMode && id) {\n";
    ss << "      fetchData(id)\n";
    ss << "    }\n";
    ss << "  }, [id])\n\n";

    ss << "  const fetchData = async (itemId: string) => {\n";
    ss << "    try {\n";
    ss << "      const response = await " << table.instance_name() << "Api.getById(itemId)\n";
    ss << "      setFormData(response.data)\n";
    ss << "    } catch (err: any) {\n";
    ss << "      setError(err.response?.data?.message || 'Failed to fetch data')\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {\n";
    ss << "    const { name, value, type, checked } = e.target\n";
    ss << "    setFormData(prev => ({\n";
    ss << "      ...prev,\n";
    ss << "      [name]: type === 'checkbox' ? checked : value\n";
    ss << "    }))\n";
    ss << "  }\n\n";

    ss << "  const handleSubmit = async (e: React.FormEvent) => {\n";
    ss << "    e.preventDefault()\n";
    ss << "    setError(null)\n\n";
    ss << "    try {\n";
    ss << "      if (isEditMode && id) {\n";
    ss << "        await " << table.instance_name() << "Api.update(id, formData as " << table.class_name() << ")\n";
    ss << "      } else {\n";
    ss << "        await " << table.instance_name() << "Api.create(formData as Omit<" << table.class_name() << ", '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "'>)\n";
    ss << "      }\n";
    ss << "      navigate('/" << pluralize(table.kebab_name()) << "')\n";
    ss << "    } catch (err: any) {\n";
    ss << "      setError(err.response?.data?.message || 'Failed to save')\n";
    ss << "    }\n\n";
    ss << "  }\n\n";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1 className=\"text-2xl font-bold mb-4\">{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4\">{error}</div>}\n\n";
        ss << "      <div className=\"bg-white p-6 rounded shadow\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"mb-4\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\" className=\"block text-sm font-medium text-gray-700 mb-1\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << " as boolean}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              className=\"rounded border-gray-300\"\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << " as string || ''}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "              className=\"w-full px-3 py-2 border border-gray-300 rounded focus:outline-none focus:ring-2 focus:ring-blue-500\"\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div className=\"flex gap-4 mt-6\">\n";
        ss << "            <button type=\"submit\" className=\"bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600 disabled:opacity-50\">\n";
        ss << "              Save\n";
        ss << "            </button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"bg-gray-500 text-white px-4 py-2 rounded hover:bg-gray-600\">\n";
        ss << "              Cancel\n";
        ss << "            </button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1>{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"alert alert-danger\">{error}</div>}\n\n";
        ss << "      <div className=\"card\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"mb-3\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\" className=\"form-label\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << " as boolean}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              className=\"form-check-input\"\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << " as string || ''}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "              className=\"form-control\"\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div className=\"d-flex gap-2 mt-4\">\n";
        ss << "            <button type=\"submit\" className=\"btn btn-primary\">Save</button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"btn btn-secondary\">Cancel</button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";

    } else {
        ss << "  return (\n";
        ss << "    <div>\n";
        ss << "      <h1>{isEditMode ? 'Edit' : 'Create'} " << table.class_name() << "</h1>\n\n";
        ss << "      {error && <div className=\"error\">{error}</div>}\n\n";
        ss << "      <div className=\"card\">\n";
        ss << "        <form onSubmit={handleSubmit}>\n";

        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;

            String input_type = "text";
            if (col.cpp_type() == "bool") {
                input_type = "checkbox";
            } else if (col.cpp_type() == "int32_t" || col.cpp_type() == "int64_t" || col.cpp_type() == "int16_t" || col.cpp_type() == "int8_t") {
                input_type = "number";
            }

            ss << "          <div className=\"form-group\">\n";
            ss << "            <label htmlFor=\"" << col.field_name() << "\">" << col.field_name() << "</label>\n";
            if (col.cpp_type() == "bool") {
                ss << "            <input\n";
                ss << "              type=\"checkbox\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              checked={formData." << col.field_name() << " as boolean}\n";
                ss << "              onChange={handleChange}\n";
                ss << "            />\n";
            } else {
                ss << "            <input\n";
                ss << "              type=\"" << input_type << "\"\n";
                ss << "              name=\"" << col.field_name() << "\"\n";
                ss << "              value={formData." << col.field_name() << " as string || ''}\n";
                ss << "              onChange={handleChange}\n";
                ss << "              " << (col.nullable ? "" : "required") << "\n";
                ss << "            />\n";
            }
            ss << "          </div>\n";
        }

        ss << "          <div style={{display: 'flex', gap: '1rem', marginTop: '1rem'}}>\n";
        ss << "            <button type=\"submit\" className=\"btn btn-primary\">Save</button>\n";
        ss << "            <button type=\"button\" onClick={() => navigate('/" << pluralize(table.kebab_name()) << "')} className=\"btn btn-secondary\">Cancel</button>\n";
        ss << "          </div>\n";
        ss << "        </form>\n";
        ss << "      </div>\n";
        ss << "    </div>\n";
        ss << "  )\n";
    }

    ss << "}\n\n";
    ss << "export default " << table.class_name() << "Form\n";

    return ss.str();
}

String Generator::generate_react_dockerfile(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;

    if (react_bundler_ == ReactBundler::Vite) {
        ss << "FROM node:20-alpine AS build\n\n";
        ss << "WORKDIR /app\n\n";
        ss << "COPY package*.json ./\n";
        ss << "RUN npm ci\n\n";
        ss << "COPY . .\n";
        ss << "RUN npm run build\n\n";
        ss << "FROM nginx:alpine\n";
        ss << "COPY --from=build /app/dist /usr/share/nginx/html\n";
        ss << "COPY nginx.conf /etc/nginx/conf.d/default.conf\n\n";
        ss << "EXPOSE 80\n\n";
        ss << "CMD [\"nginx\", \"-g\", \"daemon off;\"]\n";
    } else {
        ss << "FROM node:20-alpine AS build\n\n";
        ss << "WORKDIR /app\n\n";
        ss << "COPY package*.json ./\n";
        ss << "RUN npm ci\n\n";
        ss << "COPY . .\n";
        ss << "RUN npm run build\n\n";
        ss << "FROM nginx:alpine\n";
        ss << "COPY --from=build /app/build /usr/share/nginx/html\n";
        ss << "COPY nginx.conf /etc/nginx/conf.d/default.conf\n\n";
        ss << "EXPOSE 80\n\n";
        ss << "CMD [\"nginx\", \"-g\", \"daemon off;\"]\n";
    }

    return ss.str();
}

String Generator::generate_react_nginx_conf(const std::vector<Table>& tables) {
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

String Generator::generate_react_docker_compose_yml(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "version: '3.8'\n\n";
    ss << "services:\n";
    ss << "  frontend:\n";
    ss << "    build:\n";
    ss << "      context: .\n";
    ss << "      dockerfile: Dockerfile\n";
    ss << "    ports:\n";
    ss << "      - \"3000:80\"\n";
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

String Generator::generate_react_gitignore() {
    return R"(# Logs
logs
*.log
npm-debug.log*
yarn-debug.log*
yarn-error.log*
pnpm-debug.log*
lerna-debug.log*

node_modules
dist
dist-ssr
*.local

# Editor directories and files
.vscode/*
!.vscode/extensions.json
.idea
.DS_Store
*.suo
*.ntvs*
*.njsproj
*.sln
*.sw?
)";
}

String Generator::generate_react_readme(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "# " << project_name_ << " Frontend\n\n";
    ss << "React frontend for " << project_name_ << " REST API.\n\n";
    ss << "## Configuration\n\n";
    ss << "- **Bundler**: " << react_bundler_to_string(react_bundler_) << "\n";
    ss << "- **CSS Framework**: " << react_css_to_string(react_css_) << "\n";
    ss << "- **Theme**: " << react_theme_to_string(react_theme_) << "\n";
    ss << "- **State Management**: " << react_state_to_string(react_state_) << "\n\n";
    ss << "## Requirements\n\n";
    ss << "- Node.js 18+\n";
    ss << "- npm or yarn\n\n";
    ss << "## Installation\n\n";
    ss << "```bash\n";
    ss << "npm install\n";
    ss << "```\n\n";

    if (react_bundler_ == ReactBundler::Vite) {
        ss << "## Development\n\n";
        ss << "```bash\n";
        ss << "npm run dev\n";
        ss << "```\n\n";
        ss << "Frontend runs on http://localhost:3000 with API proxy to backend at http://localhost:8080\n\n";
        ss << "## Building\n\n";
        ss << "```bash\n";
        ss << "npm run build\n";
        ss << "```\n";
    } else {
        ss << "## Development\n\n";
        ss << "```bash\n";
        ss << "npm start\n";
        ss << "```\n\n";
        ss << "Frontend runs on http://localhost:3000\n\n";
        ss << "## Building\n\n";
        ss << "```bash\n";
        ss << "npm run build\n";
        ss << "```\n";
    }

    ss << "\n## Docker\n\n";
    ss << "```bash\n";
    ss << "docker-compose up --build\n";
    ss << "```\n\n";
    ss << "Frontend available at http://localhost:3000\n\n";

    if (react_css_ == ReactCSS::Tailwind) {
        ss << "## Tailwind CSS\n\n";
        ss << "This project uses Tailwind CSS. To customize the theme, edit `tailwind.config.js`.\n\n";
    } else if (react_css_ == ReactCSS::Bootstrap) {
        ss << "## Bootstrap\n\n";
        ss << "This project uses React Bootstrap for UI components.\n\n";
    }

    if (react_state_ == ReactState::Redux) {
        ss << "## Redux State Management\n\n";
        ss << "This project uses Redux Toolkit for state management. Store slices are in `src/store/slices/`.\n\n";
    } else if (react_state_ == ReactState::Zustand) {
        ss << "## Zustand State Management\n\n";
        ss << "This project uses Zustand for state management. The store is defined in `src/store/useStore.js`.\n\n";
    } else {
        ss << "## Context API State Management\n\n";
        ss << "This project uses React Context API for state management. The store is in `src/store/StoreContext.jsx`.\n\n";
    }

    ss << "Generated by NovaGen v1.0.0\n";
    return ss.str();
}

} // namespace novagen
