#include "generator.hpp"
#include <sstream>
#include <algorithm>

namespace novagen {

static String python_default(const String& raw) {
    String d = raw;
    String lower = d;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "current_timestamp" || lower == "now()" || lower == "sysdate" || lower == "getdate()") {
        return "datetime.utcnow";
    }
    if (lower == "current_date" || lower == "current_date()") {
        return "datetime.date.today";
    }
    if (lower == "current_time" || lower == "current_time()") {
        return "datetime.datetime.utcnow";
    }
    if (lower == "true") return "True";
    if (lower == "false") return "False";
    if (lower == "null") return "None";
    return d;
}

// ============================================================================
// Flask Generator Methods
// ============================================================================

String Generator::generate_flask_requirements_txt() {
    switch (orm_) {
        case ORMType::SQLAlchemy:
            return R"(flask==3.0.0
flask-sqlalchemy==3.1.1
flask-marshmallow==0.15.0
marshmallow-sqlalchemy==0.30.0
httpx==0.26.0
python-dotenv==1.0.0
pytest==7.4.4
pytest-flask==1.3.0
)";
        case ORMType::Slack:
            return R"(flask==3.0.0
flask-slack==0.0.3
slack==0.0.1
flask-marshmallow==0.15.0
marshmallow-sqlalchemy==0.30.0
httpx==0.26.0
python-dotenv==1.0.0
pytest==7.4.4
pytest-flask==1.3.0
)";
        case ORMType::Peewee:
            return R"(flask==3.0.0
flask-peewee==0.7.1
peewee==3.17.5
playhouse-pool==0.1.1
flask-marshmallow==0.15.0
httpx==0.26.0
python-dotenv==1.0.0
pytest==7.4.4
pytest-flask==1.3.0
)";
        default:
            return R"(flask==3.0.0
flask-sqlalchemy==3.1.1
flask-marshmallow==0.15.0
marshmallow-sqlalchemy==0.30.0
httpx==0.26.0
python-dotenv==1.0.0
pytest==7.4.4
pytest-flask==1.3.0
)";
    }
}

String Generator::generate_flask_config_py() {
    std::stringstream ss;
    ss << "import os\n";
    ss << "basedir = os.path.abspath(os.path.dirname(__file__))\n\n";

    switch (orm_) {
        case ORMType::SQLAlchemy:
            ss << "class Config:\n";
            ss << "    SECRET_KEY = os.environ.get('SECRET_KEY', 'dev-secret-key')\n";
            ss << "    SQLALCHEMY_DATABASE_URI = os.environ.get(\n";
            ss << "        'DATABASE_URL',\n";
            ss << "        'sqlite:///' + os.path.join(basedir, 'data', '" << project_name_ << ".db')\n";
            ss << "    )\n";
            ss << "    SQLALCHEMY_TRACK_MODIFICATIONS = False\n";
            ss << "    JSON_SORT_KEYS = False\n\n";
            ss << "class DevelopmentConfig(Config):\n";
            ss << "    DEBUG = True\n\n";
            ss << "class ProductionConfig(Config):\n";
            ss << "    DEBUG = False\n\n";
            ss << "config = {\n";
            ss << "    'development': DevelopmentConfig,\n";
            ss << "    'production': ProductionConfig,\n";
            ss << "    'default': DevelopmentConfig\n";
            ss << "}\n";
            break;
        case ORMType::Peewee:
            ss << "class Config:\n";
            ss << "    SECRET_KEY = os.environ.get('SECRET_KEY', 'dev-secret-key')\n";
            ss << "    DATABASE_NAME = os.environ.get('DATABASE_NAME', '" << project_name_ << ".db')\n";
            ss << "    DATABASE_PATH = os.path.join(basedir, 'data', DATABASE_NAME)\n";
            ss << "    DATABASE_URI = os.environ.get('DATABASE_URL', 'sqlite:///' + DATABASE_PATH)\n";
            ss << "    PEEWEE_DATABASE = DATABASE_URI\n";
            ss << "    JSON_SORT_KEYS = False\n\n";
            ss << "class DevelopmentConfig(Config):\n";
            ss << "    DEBUG = True\n\n";
            ss << "class ProductionConfig(Config):\n";
            ss << "    DEBUG = False\n\n";
            ss << "config = {\n";
            ss << "    'development': DevelopmentConfig,\n";
            ss << "    'production': ProductionConfig,\n";
            ss << "    'default': DevelopmentConfig\n";
            ss << "}\n";
            break;
        case ORMType::Slack:
        default:
            ss << "class Config:\n";
            ss << "    SECRET_KEY = os.environ.get('SECRET_KEY', 'dev-secret-key')\n";
            ss << "    SQLALCHEMY_DATABASE_URI = os.environ.get(\n";
            ss << "        'DATABASE_URL',\n";
            ss << "        'sqlite:///' + os.path.join(basedir, 'data', '" << project_name_ << ".db')\n";
            ss << "    )\n";
            ss << "    SQLALCHEMY_TRACK_MODIFICATIONS = False\n";
            ss << "    JSON_SORT_KEYS = False\n\n";
            ss << "class DevelopmentConfig(Config):\n";
            ss << "    DEBUG = True\n\n";
            ss << "class ProductionConfig(Config):\n";
            ss << "    DEBUG = False\n\n";
            ss << "config = {\n";
            ss << "    'development': DevelopmentConfig,\n";
            ss << "    'production': ProductionConfig,\n";
            ss << "    'default': DevelopmentConfig\n";
            ss << "}\n";
            break;
    }
    return ss.str();
}

String Generator::generate_flask_models_py(const std::vector<Table>& tables) {
    std::stringstream ss;

    switch (orm_) {
        case ORMType::SQLAlchemy:
            ss << "from flask_sqlalchemy import SQLAlchemy\n";
            ss << "from datetime import datetime\n\n";
            ss << "db = SQLAlchemy()\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(db.Model):\n";
                ss << "    __tablename__ = \"" << table.name << "\"\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = db.Column(";
                    ss << flask_column_type(col);
                    { String fk_ref = table.fk_ref(col.name); if (!fk_ref.empty()) ss << ", db.ForeignKey('" << fk_ref << "')"; }
                    if (col.primary_key) {
                        ss << ", primary_key=True";
                        if (col.auto_increment) {
                            ss << ", autoincrement=True";
                        }
                    }
                    if (!col.nullable && !col.primary_key) {
                        ss << ", nullable=False";
                    }
                    if (col.unique) {
                        ss << ", unique=True";
                    }
                    if (col.default_value) {
                        ss << ", default=" << python_default(col.default_value.value());
                    }
                    ss << ")\n";
                }

                for (const auto& fk : table.foreign_keys) {
                    ss << "    " << fk.ref_instance() << " = db.relationship('" << fk.ref_class() << "'";
                    if (fk.ref_class() == table.class_name() && table.pk_column()) {
                        ss << ", remote_side=[" << table.pk_column()->name << "]";
                    }
                    ss << ", backref=db.backref('" << table.instance_name() << "_items'))\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "\n            '" << col.name << "': self." << col.name;
                    first = false;
                }
                ss << "\n        }\n\n";

                ss << "    def __repr__(self):\n";
                ss << "        return f'<" << table.class_name() << " " << (table.pk_column() ? "{self." + table.pk_column()->name + "}" : "") << ">'\n\n";
            }
            break;

        case ORMType::Peewee:
            ss << "from peewee import Model, CharField, IntegerField, BooleanField, FloatField, TextField, DateField, TimeField, DateTimeField, ForeignKeyField, Database\n";
            ss << "from datetime import datetime\n\n";
            ss << "database = Database('sqlite:///data/" << project_name_ << ".db')\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(Model):\n";
                ss << "    class Meta:\n";
                ss << "        database = database\n";
                ss << "        table_name = '" << table.name << "'\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = ";
                    ss << peewee_column_type(col);
                    ss << "\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {\n";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "\n            '" << col.name << "': self." << col.name;
                    if (col.cpp_type() == "bool") {
                        ss << " if self." << col.name << " is not None else None";
                    }
                    first = false;
                }
                ss << "\n        }\n\n";

                ss << "    def __repr__(self):\n";
                ss << "        return f'<" << table.class_name() << " " << (table.pk_column() ? "{self." + table.pk_column()->name + "}" : "") << ">'\n\n";
            }
            break;

        case ORMType::Slack:
        default:
            ss << "from flask_slack import Slack\n";
            ss << "from flask_marshmallow import Marshmallow\n";
            ss << "from datetime import datetime\n\n";
            ss << "db = SQLAlchemy()\n";
            ss << "ma = Marshmallow()\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(db.Model):\n";
                ss << "    __tablename__ = \"" << table.name << "\"\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = db.Column(";
                    ss << flask_column_type(col);
                    { String fk_ref = table.fk_ref(col.name); if (!fk_ref.empty()) ss << ", db.ForeignKey('" << fk_ref << "')"; }
                    if (col.primary_key) {
                        ss << ", primary_key=True";
                        if (col.auto_increment) {
                            ss << ", autoincrement=True";
                        }
                    }
                    if (!col.nullable && !col.primary_key) {
                        ss << ", nullable=False";
                    }
                    if (col.unique) {
                        ss << ", unique=True";
                    }
                    if (col.default_value) {
                        ss << ", default=" << python_default(col.default_value.value());
                    }
                    ss << ")\n";
                }

                for (const auto& fk : table.foreign_keys) {
                    ss << "    " << fk.ref_instance() << " = db.relationship('" << fk.ref_class() << "'";
                    if (fk.ref_class() == table.class_name() && table.pk_column()) {
                        ss << ", remote_side=[" << table.pk_column()->name << "]";
                    }
                    ss << ", backref=db.backref('" << table.instance_name() << "_items'))\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "\n            '" << col.name << "': self." << col.name;
                    first = false;
                }
                ss << "\n        }\n\n";

                ss << "    def __repr__(self):\n";
                ss << "        return f'<" << table.class_name() << " " << (table.pk_column() ? "{" + table.pk_column()->field_name() + "}" : "") << ">'\n\n";
            }
            break;
    }

    return ss.str();
}

String Generator::flask_column_type(const Column& col) {
    String py_type = col.cpp_type();
    if (py_type == "int32_t" || py_type == "int64_t" || py_type == "int16_t" || py_type == "int8_t") {
        return "db.Integer";
    }
    if (py_type == "bool") {
        return "db.Boolean";
    }
    if (py_type == "float" || py_type == "double") {
        if (col.precision.has_value()) {
            return "db.Numeric(" + std::to_string(col.precision.value()) + (col.scale.has_value() ? ", " + std::to_string(col.scale.value()) : ", 0") + ")";
        }
        return "db.Float";
    }
    if (py_type == "std::string") {
        if (col.length.has_value()) {
            return "db.String(" + std::to_string(col.length.value()) + ")";
        }
        return "db.Text";
    }
    if (py_type.find("vector") != String::npos) {
        return "db.LargeBinary";
    }
    if (py_type == "std::chrono::system_clock::time_point") {
        return "db.DateTime";
    }
    if (py_type == "std::chrono::year_month_day") {
        return "db.Date";
    }
    if (py_type == "std::chrono::seconds") {
        return "db.Time";
    }
    return "db.String(255)";
}

String Generator::peewee_column_type(const Column& col) {
    String py_type = col.cpp_type();
    if (py_type == "int32_t" || py_type == "int64_t" || py_type == "int16_t" || py_type == "int8_t") {
        if (col.primary_key && col.auto_increment) {
            return "IntegerField(primary_key=True)";
        }
        return "IntegerField()";
    }
    if (py_type == "bool") {
        return "BooleanField()";
    }
    if (py_type == "float" || py_type == "double") {
        if (col.precision.has_value()) {
            return "FloatField()";
        }
        return "FloatField()";
    }
    if (py_type == "std::string") {
        if (col.length.has_value()) {
            return "CharField(max_length=" + std::to_string(col.length.value()) + ")";
        }
        return "TextField()";
    }
    if (py_type.find("vector") != String::npos) {
        return "BlobField()";
    }
    return "CharField(max_length=255)";
}

String Generator::generate_flask_routes_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "from flask import Blueprint, request, jsonify\n";
    ss << "from app.models import db, ";

    for (size_t i = 0; i < tables.size(); ++i) {
        ss << tables[i].class_name();
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "\nfrom app.durable_functions import start_orchestration\n\n";

    ss << namespace_ << "_bp = Blueprint('" << namespace_ << "', __name__)\n\n";

    switch (orm_) {
        case ORMType::Peewee:
            for (const auto& table : tables) {
                const auto& pk = table.pk_column();
                String pk_name = pk ? pk->field_name() : "id";
                String plural = pluralize(table.snake_name());
                String plural_path = pluralize(table.kebab_name());
                String pk_conv = (pk && pk->cpp_type().find("int") != String::npos) ? "int" : "string";

                ss << "\n@" << namespace_ << "_bp.route('/" << plural_path << "', methods=['GET'])\n";
                ss << "def get_" << plural << "():\n";
                ss << "    items = " << table.class_name() << ".select()\n";
                ss << "    return jsonify([item.to_dict() for item in items])\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['GET'])\n";
                ss << "def get_" << table.instance_name() << "_by_id(" << pk_name << "):\n";
                ss << "    try:\n";
                ss << "        item = " << table.class_name() << ".get_by_id(" << pk_name << ")\n";
                ss << "        return jsonify(item.to_dict())\n";
                ss << "    except DoesNotExist:\n";
                ss << "        return jsonify({'error': 'Not found'}), 404\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "', methods=['POST'])\n";
                ss << "def create_" << table.instance_name() << "():\n";
                ss << "    data = request.get_json()\n";
                ss << "    item = " << table.class_name() << "(**data)\n";
                ss << "    item.save()\n";
                ss << "    return jsonify(item.to_dict()), 201\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['PUT'])\n";
                ss << "def update_" << table.instance_name() << "(" << pk_name << "):\n";
                ss << "    try:\n";
                ss << "        item = " << table.class_name() << ".get_by_id(" << pk_name << ")\n";
                ss << "        data = request.get_json()\n";
                ss << "        for key, value in data.items():\n";
                ss << "            setattr(item, key, value)\n";
                ss << "        item.save()\n";
                ss << "        return jsonify(item.to_dict())\n";
                ss << "    except DoesNotExist:\n";
                ss << "        return jsonify({'error': 'Not found'}), 404\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['DELETE'])\n";
                ss << "def delete_" << table.instance_name() << "(" << pk_name << "):\n";
                ss << "    try:\n";
                ss << "        item = " << table.class_name() << ".get_by_id(" << pk_name << ")\n";
                ss << "        item.delete_instance()\n";
                ss << "        return '', 204\n";
                ss << "    except DoesNotExist:\n";
                ss << "        return jsonify({'error': 'Not found'}), 404\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/count', methods=['GET'])\n";
                ss << "def count_" << plural << "():\n";
                ss << "    count = " << table.class_name() << ".select().count()\n";
                ss << "    return jsonify({'count': count})\n";
            }
            break;

        case ORMType::Slack:
        case ORMType::SQLAlchemy:
        default:
            for (const auto& table : tables) {
                const auto& pk = table.pk_column();
                String pk_name = pk ? pk->field_name() : "id";
                String plural = pluralize(table.snake_name());
                String plural_path = pluralize(table.kebab_name());
                String pk_conv = (pk && pk->cpp_type().find("int") != String::npos) ? "int" : "string";

                ss << "\n@" << namespace_ << "_bp.route('/" << plural_path << "', methods=['GET'])\n";
                ss << "def get_" << plural << "():\n";
                ss << "    items = " << table.class_name() << ".query.all()\n";
                ss << "    return jsonify([item.to_dict() for item in items])\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['GET'])\n";
                ss << "def get_" << table.instance_name() << "_by_id(" << pk_name << "):\n";
                ss << "    item = " << table.class_name() << ".query.get_or_404(" << pk_name << ")\n";
                ss << "    return jsonify(item.to_dict())\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "', methods=['POST'])\n";
                ss << "def create_" << table.instance_name() << "():\n";
                ss << "    data = request.get_json()\n";
                ss << "    item = " << table.class_name() << "(**data)\n";
                ss << "    db.session.add(item)\n";
                ss << "    db.session.commit()\n";
                ss << "    return jsonify(item.to_dict()), 201\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['PUT'])\n";
                ss << "def update_" << table.instance_name() << "(" << pk_name << "):\n";
                ss << "    item = " << table.class_name() << ".query.get_or_404(" << pk_name << ")\n";
                ss << "    data = request.get_json()\n";
                ss << "    for key, value in data.items():\n";
                ss << "        setattr(item, key, value)\n";
                ss << "    db.session.commit()\n";
                ss << "    return jsonify(item.to_dict())\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/<" << pk_conv << ":" << pk_name << ">', methods=['DELETE'])\n";
                ss << "def delete_" << table.instance_name() << "(" << pk_name << "):\n";
                ss << "    item = " << table.class_name() << ".query.get_or_404(" << pk_name << ")\n";
                ss << "    db.session.delete(item)\n";
                ss << "    db.session.commit()\n";
                ss << "    return '', 204\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/count', methods=['GET'])\n";
                ss << "def count_" << plural << "():\n";
                ss << "    count = " << table.class_name() << ".query.count()\n";
                ss << "    return jsonify({'count': count})\n\n";

                ss << "@" << namespace_ << "_bp.route('/" << plural_path << "/async', methods=['POST'])\n";
                ss << "def create_" << table.instance_name() << "_async():\n";
                ss << "    data = request.get_json()\n";
                ss << "    item = " << table.class_name() << "(**data)\n";
                ss << "    db.session.add(item)\n";
                ss << "    db.session.commit()\n";
                ss << "    instance_id = start_orchestration('" << table.instance_name() << "_orchestrator', item." << pk_name << ")\n";
                ss << "    if not instance_id:\n";
                ss << "        return jsonify({'error': 'Failed to start orchestration'}), 502\n";
                ss << "    return jsonify({'instance_id': instance_id, '" << pk_name << "': item." << pk_name << "}), 202\n";
            }
            break;
    }

    return ss.str();
}

String Generator::generate_flask_app_py(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "#!/usr/bin/env python3\n";
    ss << "import os\n\n";
    ss << "from app import create_app\n\n";
    ss << "app = create_app()\n\n";
    ss << "if __name__ == '__main__':\n";
    ss << "    os.makedirs(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'data'), exist_ok=True)\n";
    ss << "    with app.app_context():\n";
    ss << "        from app.models import db\n";
    ss << "        db.create_all()\n";
    ss << "    app.run(host='0.0.0.0', port=8080, debug=True)\n";
    return ss.str();
}

String Generator::generate_flask_durable_client_py() {
    std::stringstream ss;
    ss << "import os\n";
    ss << "import httpx\n";
    ss << "from typing import Any, Dict, Optional\n\n";
    ss << "BASE_URL = os.environ.get(\n";
    ss << "    'DURABLE_FUNCTIONS_URL',\n";
    ss << "    'http://localhost:7071/runtime/webhooks/durabletask'\n";
    ss << ")\n";
    ss << "API_KEY = os.environ.get('DURABLE_FUNCTIONS_KEY', '')\n\n";
    ss << "def _headers() -> Dict[str, str]:\n";
    ss << "    headers = {'Content-Type': 'application/json'}\n";
    ss << "    if API_KEY:\n";
    ss << "        headers['x-functions-key'] = API_KEY\n";
    ss << "    return headers\n\n";
    ss << "def start_orchestration(orchestrator_name: str, payload: Any = None) -> Optional[str]:\n";
    ss << "    with httpx.Client(timeout=30) as client:\n";
    ss << "        resp = client.post(\n";
    ss << "            f'{BASE_URL}/orchestrators/{orchestrator_name}',\n";
    ss << "            json=payload, headers=_headers()\n";
    ss << "        )\n";
    ss << "        if resp.status_code == 202:\n";
    ss << "            return resp.json().get('id')\n";
    ss << "        return None\n\n";
    ss << "def get_status(instance_id: str) -> Optional[Dict[str, Any]]:\n";
    ss << "    with httpx.Client(timeout=30) as client:\n";
    ss << "        resp = client.get(\n";
    ss << "            f'{BASE_URL}/instances/{instance_id}',\n";
    ss << "            headers=_headers()\n";
    ss << "        )\n";
    ss << "        if resp.status_code == 200:\n";
    ss << "            return resp.json()\n";
    ss << "        return None\n\n";
    ss << "def send_event(instance_id: str, event_name: str, payload: Any = None) -> bool:\n";
    ss << "    with httpx.Client(timeout=30) as client:\n";
    ss << "        resp = client.post(\n";
    ss << "            f'{BASE_URL}/instances/{instance_id}/raiseevent/{event_name}',\n";
    ss << "            json=payload, headers=_headers()\n";
    ss << "        )\n";
    ss << "        return resp.status_code in (202, 204)\n\n";
    ss << "def terminate(instance_id: str, reason: str = '') -> bool:\n";
    ss << "    params = {'reason': reason} if reason else {}\n";
    ss << "    with httpx.Client(timeout=30) as client:\n";
    ss << "        resp = client.post(\n";
    ss << "            f'{BASE_URL}/instances/{instance_id}/terminate',\n";
    ss << "            params=params, headers=_headers()\n";
    ss << "        )\n";
    ss << "        return resp.status_code in (202, 204)\n";
    return ss.str();
}

String Generator::generate_flask_run_sh() {
    std::stringstream ss;
    ss << "#!/bin/bash\n\n";
    ss << "export FLASK_APP=run.py\n";
    ss << "export FLASK_ENV=development\n";
    ss << "flask run --host=0.0.0.0 --port=8080\n";
    return ss.str();
}

String Generator::generate_flask_gitignore() {
    return R"(__pycache__/
*.py[cod]
*$py.class
*.so
.Python
env/
venv/
.venv/
ENV/
build/
dist/
*.egg-info/
.eggs/
.pytest_cache/
.coverage
htmlcov/
*.db
*.db-journal
data/
.env
.idea/
.vscode/
*.swp
*.swo
*~
)";
}

String Generator::generate_flask_dockerfile(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "FROM python:3.11-slim\n\n";
    ss << "WORKDIR /app\n\n";
    ss << "COPY requirements.txt .\n";
    ss << "RUN pip install --no-cache-dir -r requirements.txt\n\n";
    ss << "COPY . .\n\n";
    ss << "RUN mkdir -p data\n\n";
    ss << "ENV FLASK_APP=run.py\n";
    ss << "ENV FLASK_ENV=production\n\n";
    ss << "EXPOSE 8080\n\n";
    ss << "CMD [\"python\", \"run.py\"]\n";
    return ss.str();
}

String Generator::generate_flask_docker_compose_yml() {
    std::stringstream ss;
    ss << "version: '3.8'\n\n";
    ss << "services:\n";
    ss << "  api:\n";
    ss << "    build: .\n";
    ss << "    ports:\n";
    ss << "      - \"8080:8080\"\n";
    ss << "    volumes:\n";
    ss << "      - ./data:/app/data\n";
    ss << "    environment:\n";
    ss << "      - FLASK_ENV=development\n";
    ss << "      - DATABASE_URL=sqlite:///data/" << project_name_ << ".db\n";
    return ss.str();
}

String Generator::generate_flask_readme(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "# " << project_name_ << " (Flask API)\n\n";
    ss << "Auto-generated Flask REST API project.\n\n";
    ss << "## Requirements\n";
    ss << "- Python 3.9+\n";
    ss << "- SQLite3\n\n";
    ss << "## Installation\n\n";
    ss << "```bash\n";
    ss << "pip install -r requirements.txt\n";
    ss << "mkdir -p data\n";
    ss << "```\n\n";
    ss << "## Running\n\n";
    ss << "```bash\n";
    ss << "python run.py\n";
    ss << "# or\n";
    ss << "flask run --host=0.0.0.0 --port=8080\n";
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

    ss << "## Docker\n\n";
    ss << "```bash\n";
    ss << "docker-compose up --build\n";
    ss << "```\n\n";
    ss << "Generated by NovaGen v1.0.0\n";
    return ss.str();
}

String Generator::generate_flask_pytest_ini() {
    return R"([pytest]
testpaths = tests
python_files = test_*.py
python_classes = Test*
python_functions = test_*
addopts = -v --tb=short
)";
}

String Generator::generate_flask_test_models_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import pytest\n";
    ss << "from app import create_app\n";
    ss << "from app.models import db, ";

    for (size_t i = 0; i < tables.size(); ++i) {
        ss << tables[i].class_name();
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "\n\n";
    ss << "@pytest.fixture\n";
    ss << "def app():\n";
    ss << "    app = create_app()\n";
    ss << "    app.config['TESTING'] = True\n";
    ss << "    app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///:memory:'\n\n";
    ss << "    with app.app_context():\n";
    ss << "        db.create_all()\n";
    ss << "        yield app\n";
    ss << "        db.drop_all()\n\n";
    ss << "@pytest.fixture\n";
    ss << "def client(app):\n";
    ss << "    return app.test_client()\n\n";

    for (const auto& table : tables) {
        ss << "def test_create_" << table.instance_name() << "(client):\n";
        ss << "    response = client.post('/api/" << pluralize(table.kebab_name()) << R"(', json={
            )";
        bool first = true;
        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;
            if (!first) ss << ",\n            ";
            first = false;
            if (col.cpp_type() == "std::string") {
                ss << "'" << col.name << "': 'test_value'";
            } else if (col.cpp_type() == "bool") {
                ss << "'" << col.name << "': true";
            } else {
                ss << "'" << col.name << "': 1";
            }
        }
        ss << "\n        })\n";
        ss << "    assert response.status_code == 201\n";
        ss << "    data = response.get_json()\n";
        ss << "    assert '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "' in data\n\n";

        ss << "def test_get_" << pluralize(table.snake_name()) << "(client):\n";
        ss << "    # Create first\n";
        ss << "    client.post('/api/" << pluralize(table.kebab_name()) << R"(', json={
            )";
        first = true;
        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;
            if (!first) ss << ",\n            ";
            first = false;
            if (col.cpp_type() == "std::string") {
                ss << "'" << col.name << "': 'test_value'";
            } else {
                ss << "'" << col.name << "': 1";
            }
        }
        ss << "\n        })\n\n";
        ss << "    response = client.get('/api/" << pluralize(table.kebab_name()) << "')\n";
        ss << "    assert response.status_code == 200\n";
        ss << "    data = response.get_json()\n";
        ss << "    assert len(data) >= 1\n\n";
    }

    return ss.str();
}

String Generator::generate_flask_test_routes_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import pytest\n";
    ss << "from app import create_app\n";
    ss << "from app.models import db\n\n";
    ss << "@pytest.fixture\n";
    ss << "def app():\n";
    ss << "    app = create_app()\n";
    ss << "    app.config['TESTING'] = True\n";
    ss << "    app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///:memory:'\n\n";
    ss << "    with app.app_context():\n";
    ss << "        db.create_all()\n";
    ss << "        yield app\n";
    ss << "        db.drop_all()\n\n";
    ss << "@pytest.fixture\n";
    ss << "def client(app):\n";
    ss << "    return app.test_client()\n\n";
    ss << "def test_health_endpoint(client):\n";
    ss << "    response = client.get('/health')\n";
    ss << "    assert response.status_code == 200\n";
    ss << "    assert response.get_json() == {'status': 'healthy'}\n\n";
    ss << "def test_api_info(client):\n";
    ss << "    response = client.get('/api')\n";
    ss << "    assert response.status_code == 200\n";
    ss << "    data = response.get_json()\n";
    ss << "    assert 'name' in data\n";
    ss << "    assert 'version' in data\n";
    ss << "    assert 'endpoints' in data\n";
    return ss.str();
}

String Generator::generate_flask_api_endpoints_json(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < tables.size(); ++i) {
        ss << "\"" << pluralize(tables[i].kebab_name()) << "\"";
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

String Generator::generate_security_jwt_flask() {
    std::stringstream ss;
    ss << R"(
# security/jwt_auth.py
import os
from datetime import datetime, timedelta
from functools import wraps
import jwt
from flask import request, jsonify, g

SECRET_KEY = os.environ.get('SECRET_KEY', 'dev-secret-key')
ALGORITHM = 'HS256'
TOKEN_EXPIRATION_HOURS = 24

def generate_token(user_id: str, role: str = 'user') -> str:
    """Generate a JWT token for the given user."""
    payload = {
        'user_id': user_id,
        'role': role,
        'exp': datetime.utcnow() + timedelta(hours=TOKEN_EXPIRATION_HOURS),
        'iat': datetime.utcnow()
    }
    return jwt.encode(payload, SECRET_KEY, algorithm=ALGORITHM)

def verify_token(token: str) -> dict:
    """Verify and decode a JWT token."""
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        return payload
    except jwt.ExpiredSignatureError:
        raise ValueError("Token has expired")
    except jwt.InvalidTokenError:
        raise ValueError("Invalid token")

def jwt_required(f):
    """Decorator to require JWT authentication for a route."""
    @wraps(f)
    def decorated(*args, **kwargs):
        auth_header = request.headers.get('Authorization')
        if not auth_header:
            return jsonify({'error': 'Missing authorization header'}), 401

        try:
            scheme, token = auth_header.split()
            if scheme.lower() != 'bearer':
                return jsonify({'error': 'Invalid authorization scheme'}), 401
        except ValueError:
            return jsonify({'error': 'Invalid authorization header format'}), 401

        try:
            payload = verify_token(token)
            g.current_user = payload
        except ValueError as e:
            return jsonify({'error': str(e)}), 401

        return f(*args, **kwargs)
    return decorated

def get_current_user():
    """Get the current authenticated user from the request context."""
    return getattr(g, 'current_user', None)
)";
    return ss.str();
}

String Generator::generate_security_oauth2_flask() {
    std::stringstream ss;
    ss << R"(
# security/oauth2.py
import os
from datetime import datetime, timedelta
from functools import wraps
import requests
from flask import request, jsonify, g, session, redirect, url_for

OAUTH2_PROVIDER_URL = os.environ.get('OAUTH2_PROVIDER_URL', 'https://oauth.example.com')
CLIENT_ID = os.environ.get('OAUTH2_CLIENT_ID', 'your-client-id')
CLIENT_SECRET = os.environ.get('OAUTH2_CLIENT_SECRET', 'your-client-secret')
REDIRECT_URI = os.environ.get('OAUTH2_REDIRECT_URI', 'http://localhost:5000/auth/callback')

class OAuth2Provider:
    def __init__(self):
        self.provider_url = OAUTH2_PROVIDER_URL
        self.client_id = CLIENT_ID
        self.client_secret = CLIENT_SECRET
        self.redirect_uri = REDIRECT_URI

    def get_authorization_url(self, state=None):
        """Get the URL to redirect the user to for authorization."""
        if state is None:
            state = os.urandom(16).hex()

        params = {
            'client_id': self.client_id,
            'redirect_uri': self.redirect_uri,
            'response_type': 'code',
            'scope': 'openid profile email',
            'state': state
        }
        query = '&'.join(f'{k}={v}' for k, v in params.items())
        return f'{self.provider_url}/authorize?{query}'

    def exchange_code_for_token(self, code):
        """Exchange an authorization code for an access token."""
        response = requests.post(
            f'{self.provider_url}/token',
            data={
                'grant_type': 'authorization_code',
                'code': code,
                'redirect_uri': self.redirect_uri,
                'client_id': self.client_id,
                'client_secret': self.client_secret
            }
        )
        if response.status_code == 200:
            return response.json()
        return None

    def get_user_info(self, access_token):
        """Get user information using the access token."""
        response = requests.get(
            f'{self.provider_url}/userinfo',
            headers={'Authorization': f'Bearer {access_token}'}
        )
        if response.status_code == 200:
            return response.json()
        return None

    def get_token_info(self, access_token):
        """Get token information (introspection)."""
        response = requests.post(
            f'{self.provider_url}/introspect',
            data={'token': access_token}
        )
        if response.status_code == 200:
            return response.json()
        return None

oauth2_provider = OAuth2Provider()

def oauth2_required(f):
    """Decorator to require OAuth2 authentication for a route."""
    @wraps(f)
    def decorated(*args, **kwargs):
        auth_header = request.headers.get('Authorization')
        if not auth_header or not auth_header.startswith('Bearer '):
            return jsonify({'error': 'Missing or invalid bearer token'}), 401

        token = auth_header[7:]
        token_info = oauth2_provider.get_token_info(token)

        if not token_info or not token_info.get('active', False):
            return jsonify({'error': 'Invalid or expired token'}), 401

        g.oauth2_token = token_info
        return f(*args, **kwargs)
    return decorated
)";
    return ss.str();
}

String Generator::generate_security_saml_flask() {
    std::stringstream ss;
    ss << R"(
# security/saml.py
import os
from functools import wraps
from flask import request, jsonify, g, redirect, session
import xmlsec
from lxml import etree

SAML_IDP_METADATA_URL = os.environ.get('SAML_IDP_METADATA_URL', 'https://idp.example.com/metadata')
SAML_SP_ENTITY_ID = os.environ.get('SAML_SP_ENTITY_ID', 'novagen-sp')
SAML_SP_PRIVATE_KEY = os.environ.get('SAML_SP_PRIVATE_KEY', '/path/to/sp-key.pem')
SAML_SP_CERTIFICATE = os.environ.get('SAML_SP_CERTIFICATE', '/path/to/sp-cert.pem')

class SAMLAuth:
    def __init__(self):
        self.idp_metadata_url = SAML_IDP_METADATA_URL
        self.sp_entity_id = SAML_SP_ENTITY_ID

    def generate_authn_request(self, redirect_uri):
        """Generate a SAML AuthnRequest."""
        auth_id = f'_id-{os.urandom(16).hex()}'
        issue_instant = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')

        request_xml = f'''<?xml version="1.0" encoding="UTF-8"?>
<samlp:AuthnRequest
    xmlns:samlp="urn:oasis:names:tc:SAML:2.0:protocol"
    ID="{auth_id}"
    Version="2.0"
    IssueInstant="{issue_instant}"
    AssertionConsumerServiceURL="{redirect_uri}"
    ProtocolBinding="urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST">
    <saml:Issuer xmlns:saml="urn:oasis:names:tc:SAML:2.0:assertion">
        {self.sp_entity_id}
    </saml:Issuer>
    <samlp:NameIDPolicy
        xmlns:samlp="urn:oasis:names:tc:SAML:2.0:protocol"
        Format="urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress"
        AllowCreate="true"/>
</samlp:AuthnRequest>'''

        return request_xml

    def validate_response(self, saml_response):
        """Validate a SAML Response and extract user information."""
        try:
            root = etree.fromstring(saml_response)

            # In production, verify:
            # 1. XML signature against IdP public key
            # 2. Assertion encryption
            # 3. Response/Assertion conditions (timestamps)
            # 4. Audience restriction (SP entity ID)

            ns = {
                'samlp': 'urn:oasis:names:tc:SAML:2.0:protocol',
                'saml': 'urn:oasis:names:tc:SAML:2.0:assertion'
            }

            status = root.find('.//samlp:StatusCode', ns)
            if status is not None:
                status_value = status.get('Value', '')
                if 'Success' not in status_value:
                    return {'valid': False, 'error': 'SAML authentication failed'}

            name_id = root.find('.//saml:NameID', ns)
            user_info = {
                'valid': True,
                'name_id': name_id.text if name_id is not None else None,
                'session_index': root.find('.//samlp:SessionIndex', ns).text if root.find('.//samlp:SessionIndex', ns) is not None else None
            }

            return user_info
        except Exception as e:
            return {'valid': False, 'error': str(e)}

saml_auth = SAMLAuth()

def saml_required(f):
    """Decorator to require SAML authentication for a route."""
    @wraps(f)
    def decorated(*args, **kwargs):
        saml_response = request.form.get('SAMLResponse')
        if not saml_response:
            return jsonify({'error': 'SAML authentication required'}), 401

        user_info = saml_auth.validate_response(saml_response)
        if not user_info.get('valid'):
            return jsonify({'error': user_info.get('error', 'Invalid SAML response')}), 401

        g.saml_user = user_info
        return f(*args, **kwargs)
    return decorated
)";
    return ss.str();
}

String Generator::generate_security_owasp_flask() {
    std::stringstream ss;
    ss << R"(
# security/owasp.py
import re
from flask import request, jsonify, g
from functools import wraps

XSS_PATTERNS = [
    re.compile(r'<script[^>]*>.*?</script>', re.IGNORECASE),
    re.compile(r'javascript:', re.IGNORECASE),
    re.compile(r'on\w+\s*=', re.IGNORECASE),
    re.compile(r'<[^>]+ on\w+=', re.IGNORECASE)
]

SQL_INJECTION_PATTERNS = [
    re.compile(r'\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|CREATE|TRUNCATE|EXEC|EXECUTE)\b', re.IGNORECASE),
    re.compile(r'--'),
    re.compile(r'/\*.*\*/'),
    re.compile(r';.*--')
]

class OWASPSecurity:
    def __init__(self):
        self.xss_patterns = XSS_PATTERNS
        self.sql_patterns = SQL_INJECTION_PATTERNS

    def apply_security_headers(self, response):
        """Apply OWASP-recommended security headers."""
        response.headers['X-Content-Type-Options'] = 'nosniff'
        response.headers['X-Frame-Options'] = 'DENY'
        response.headers['X-XSS-Protection'] = '1; mode=block'
        response.headers['Strict-Transport-Security'] = 'max-age=31536000; includeSubDomains'
        response.headers['Content-Security-Policy'] = "default-src 'self'"
        response.headers['Referrer-Policy'] = 'strict-origin-when-cross-origin'
        response.headers['Permissions-Policy'] = 'geolocation=(), microphone=(), camera=()'
        return response

    def check_xss(self, input_str):
        """Check if input contains potential XSS patterns."""
        if not input_str:
            return False
        for pattern in self.xss_patterns:
            if pattern.search(input_str):
                return True
        return False

    def check_sql_injection(self, input_str):
        """Check if input contains potential SQL injection patterns."""
        if not input_str:
            return False
        for pattern in self.sql_patterns:
            if pattern.search(input_str):
                return True
        return False

    def sanitize_input(self, input_str):
        """Sanitize input by removing potentially dangerous characters."""
        dangerous_chars = ['<', '>', '"', "'", '&']
        for char in dangerous_chars:
            input_str = input_str.replace(char, '')
        return input_str

    def validate_request(self):
        """Validate the current request for security threats."""
        threats = []

        # Check query parameters
        for key, value in request.args.items():
            if self.check_xss(value):
                threats.append(f'XSS threat in query param: {key}')
            if self.check_sql_injection(value):
                threats.append(f'SQL injection threat in query param: {key}')

        # Check form data
        for key, value in request.form.items():
            if self.check_xss(value):
                threats.append(f'XSS threat in form field: {key}')
            if self.check_sql_injection(value):
                threats.append(f'SQL injection threat in form field: {key}')

        # Check JSON body
        if request.is_json:
            json_data = request.get_json(silent=True)
            if json_data:
                for key, value in json_data.items() if isinstance(json_data, dict) else enumerate(json_data):
                    if isinstance(value, str):
                        if self.check_xss(value):
                            threats.append(f'XSS threat in JSON field: {key}')
                        if self.check_sql_injection(value):
                            threats.append(f'SQL injection threat in JSON field: {key}')

        return threats

owasp_security = OWASPSecurity()

def owasp_protect(f):
    """Decorator to apply OWASP security protections to a route."""
    @wraps(f)
    def decorated(*args, **kwargs):
        # Validate request
        threats = owasp_security.validate_request()
        if threats:
            return jsonify({'error': 'Potentially malicious input detected', 'threats': threats}), 400

        # Apply security headers to response
        response = f(*args, **kwargs)

        # If response is a tuple (response, status_code), handle accordingly
        if isinstance(response, tuple):
            resp, status = response
            if hasattr(resp, 'headers'):
                owasp_security.apply_security_headers(resp)
            return response

        if hasattr(response, 'headers'):
            owasp_security.apply_security_headers(response)

        return response
    return decorated
)";
    return ss.str();
}

String Generator::generate_security_ldap_flask() {
    std::stringstream ss;
    ss << R"(
# security/ldap_auth.py
import os
from functools import wraps
import base64
from flask import request, jsonify, g
try:
    import ldap
    LDAP_AVAILABLE = True
except ImportError:
    LDAP_AVAILABLE = False

LDAP_URL = os.environ.get('LDAP_URL', 'ldap://localhost:389')
LDAP_BASE_DN = os.environ.get('LDAP_BASE_DN', 'dc=example,dc=com')
LDAP_USER_DN_PATTERN = os.environ.get('LDAP_USER_DN_PATTERN', 'uid={username},ou=users,dc=example,dc=com')
LDAP_GROUP_DN_PATTERN = os.environ.get('LDAP_GROUP_DN_PATTERN', 'ou=groups,dc=example,dc=com')
LDAP_REQUIRED_GROUP = os.environ.get('LDAP_REQUIRED_GROUP', None)

class LDAPAuthentication:
    def __init__(self):
        self.ldap_url = LDAP_URL
        self.base_dn = LDAP_BASE_DN
        self.user_dn_pattern = LDAP_USER_DN_PATTERN
        self.required_group = LDAP_REQUIRED_GROUP

    def connect(self):
        """Establish connection to LDAP server."""
        if not LDAP_AVAILABLE:
            raise ImportError("python-ldap library not installed")

        conn = ldap.initialize(self.ldap_url)
        conn.protocol_version = ldap.VERSION3
        conn.set_option(ldap.OPT_REFERRALS, 0)
        conn.set_option(ldap.OPT_NETWORK_TIMEOUT, 10.0)
        return conn

    def authenticate(self, username, password):
        """Authenticate user against LDAP directory."""
        if not LDAP_AVAILABLE:
            return {'success': False, 'error': 'LDAP library not available'}

        if not username or not password:
            return {'success': False, 'error': 'Username and password required'}

        try:
            user_dn = self.user_dn_pattern.format(username=username)

            conn = self.connect()

            # Attempt to bind with user credentials
            result = conn.simple_bind_s(user_dn, password)

            if result:
                user_info = {
                    'success': True,
                    'dn': user_dn,
                    'username': username,
                    'groups': self.get_user_groups(conn, user_dn)
                }
                conn.unbind_s()
                return user_info

            conn.unbind_s()
            return {'success': False, 'error': 'Authentication failed'}

        except ldap.INVALID_CREDENTIALS:
            return {'success': False, 'error': 'Invalid username or password'}
        except ldap.LDAPError as e:
            return {'success': False, 'error': f'LDAP error: {str(e)}'}

    def get_user_groups(self, conn, user_dn):
        """Get groups for a user."""
        groups = []
        try:
            search_filter = f'(member={user_dn})'
            results = conn.search_s(
                self.base_dn,
                ldap.SCOPE_SUBTREE,
                search_filter,
                ['cn']
            )

            for dn, attrs in results:
                if dn and 'cn' in attrs:
                    groups.append(attrs['cn'][0].decode('utf-8'))

        except ldap.LDAPError:
            pass

        return groups

    def check_group_membership(self, username, required_group):
        """Check if user is member of required group."""
        if not LDAP_AVAILABLE:
            return True  # Skip check if LDAP not available

        try:
            conn = self.connect()
            conn.simple_bind_s()

            user_dn = self.user_dn_pattern.format(username=username)
            group_dn = f'cn={required_group},{LDAP_GROUP_DN_PATTERN}'

            search_filter = f'(&(member={user_dn})(objectClass=groupOfNames))'
            results = conn.search_s(
                group_dn,
                ldap.SCOPE_BASE,
                search_filter,
                ['cn']
            )

            conn.unbind_s()
            return len(results) > 0

        except ldap.LDAPError:
            return False

ldap_auth = LDAPAuthentication()

def parse_basic_auth(auth_header):
    """Parse Basic authentication header."""
    if not auth_header.startswith('Basic '):
        return None, None

    try:
        encoded = auth_header[6:]
        decoded = base64.b64decode(encoded).decode('utf-8')
        username, password = decoded.split(':', 1)
        return username, password
    except Exception:
        return None, None

def ldap_required(f):
    """Decorator to require LDAP authentication for a route."""
    @wraps(f)
    def decorated(*args, **kwargs):
        auth_header = request.headers.get('Authorization')

        if not auth_header:
            response = jsonify({'error': 'Authentication required'})
            response.headers['WWW-Authenticate'] = 'Basic realm="API"'
            return response, 401

        username, password = parse_basic_auth(auth_header)

        if not username or not password:
            return jsonify({'error': 'Invalid authentication format'}), 401

        result = ldap_auth.authenticate(username, password)

        if not result['success']:
            return jsonify({'error': result['error']}), 401

        # Check group membership if required
        if ldap_auth.required_group:
            if result['required_group'] not in result['groups']:
                return jsonify({'error': 'Insufficient permissions'}), 403

        g.current_user = result
        return f(*args, **kwargs)

    return decorated
)";
    return ss.str();
}

// ============================================================================
// FastAPI Generator Methods
// ============================================================================

String Generator::generate_fastapi_requirements_txt() {
    switch (orm_) {
        case ORMType::SQLAlchemy:
            return R"(fastapi==0.110.0
uvicorn==0.27.0
sqlalchemy==2.0.25
pydantic==2.6.0
python-dotenv==1.0.0
httpx==0.26.0
alembic==1.13.1
pytest==8.0.0
pytest-asyncio==0.23.4
)";
        case ORMType::Peewee:
            return R"(fastapi==0.110.0
uvicorn==0.27.0
peewee==3.17.5
playhouse-pool==0.1.1
pydantic==2.6.0
python-dotenv==1.0.0
httpx==0.26.0
pytest==8.0.0
pytest-asyncio==0.23.4
)";
        case ORMType::Slack:
        default:
            return R"(fastapi==0.110.0
uvicorn==0.27.0
sqlalchemy==2.0.25
pydantic==2.6.0
python-dotenv==1.0.0
httpx==0.26.0
alembic==1.13.1
pytest==8.0.0
pytest-asyncio==0.23.4
)";
    }
}

String Generator::generate_fastapi_main_py(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "from fastapi import FastAPI\n";
    ss << "from app.database import engine, Base\n";
    ss << "from app.routers import " << namespace_ << "_router\n\n";
    ss << "Base.metadata.create_all(bind=engine)\n\n";
    ss << "app = FastAPI(\n";
    ss << "    title='" << project_name_ << "',\n";
    ss << "    version='1.0.0',\n";
    ss << "    description='Auto-generated REST API'\n";
    ss << ")\n\n";
    ss << "app.include_router(" << namespace_ << "_router, prefix='/api', tags=['" << namespace_ << "'])\n\n";
    ss << "@app.get('/health')\n";
    ss << "def health():\n";
    ss << "    return {'status': 'healthy'}\n\n";
    ss << "@app.get('/api')\n";
    ss << "def api_info():\n";
    ss << "    return {\n";
    ss << "        'name': '" << project_name_ << "',\n";
    ss << "        'version': '1.0.0',\n";
    ss << "        'endpoints': " << generate_fastapi_api_endpoints_json(tables) << "\n";
    ss << "    }\n";
    return ss.str();
}

String Generator::generate_fastapi_models_py(const std::vector<Table>& tables) {
    std::stringstream ss;

    switch (orm_) {
        case ORMType::SQLAlchemy:
            ss << "from sqlalchemy import Column, Integer, String, Float, Boolean, Text, BigInteger, DateTime, ForeignKey\n";
            ss << "from sqlalchemy.orm import relationship, declarative_base\n\n";
            ss << "Base = declarative_base()\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(Base):\n";
                ss << "    __tablename__ = \"" << table.name << "\"\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = db.Column(";
                    ss << fastapi_column_type(col);
                    if (col.primary_key) {
                        ss << ", primary_key=True";
                        if (col.auto_increment) {
                            ss << ", autoincrement=True";
                        }
                    }
                    if (!col.nullable && !col.primary_key) {
                        ss << ", nullable=False";
                    }
                    if (col.unique) {
                        ss << ", unique=True";
                    }
                    ss << ")\n";
                }

                for (const auto& fk : table.foreign_keys) {
                    ss << "    " << fk.ref_instance() << " = db.relationship('" << fk.ref_class()
                       << "', back_populates='" << table.instance_name() << "_ref')\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "\n            '" << col.name << "': self." << col.name;
                    first = false;
                }
                ss << "\n        }\n";
            }
            break;

        case ORMType::Peewee:
            ss << "from peewee import Model, CharField, IntegerField, BooleanField, FloatField, TextField, DateField, TimeField, DateTimeField, ForeignKeyField, Database\n\n";
            ss << "database = Database('sqlite:///data/" << project_name_ << ".db')\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(Model):\n";
                ss << "    class Meta:\n";
                ss << "        database = database\n";
                ss << "        table_name = '" << table.name << "'\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = ";
                    ss << peewee_column_type_fastapi(col);
                    ss << "\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {\n";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "            '" << col.name << "': self." << col.name;
                    if (col.cpp_type() == "bool") {
                        ss << " if self." << col.name << " is not None else None";
                    }
                    first = false;
                }
                ss << "\n        }\n";
            }
            break;

        case ORMType::Slack:
        default:
            ss << "from sqlalchemy import Column, Integer, String, Float, Boolean, Text, BigInteger, DateTime, ForeignKey\n";
            ss << "from sqlalchemy.orm import relationship, declarative_base\n\n";
            ss << "Base = declarative_base()\n\n";

            for (const auto& table : tables) {
                ss << "class " << table.class_name() << "(Base):\n";
                ss << "    __tablename__ = \"" << table.name << "\"\n\n";

                for (const auto& col : table.columns) {
                    ss << "    " << col.name << " = db.Column(";
                    ss << fastapi_column_type(col);
                    if (col.primary_key) {
                        ss << ", primary_key=True";
                        if (col.auto_increment) {
                            ss << ", autoincrement=True";
                        }
                    }
                    if (!col.nullable && !col.primary_key) {
                        ss << ", nullable=False";
                    }
                    if (col.unique) {
                        ss << ", unique=True";
                    }
                    ss << ")\n";
                }

                for (const auto& fk : table.foreign_keys) {
                    ss << "    " << fk.ref_instance() << " = db.relationship('" << fk.ref_class()
                       << "', back_populates='" << table.instance_name() << "_ref')\n";
                }

                ss << "\n    def to_dict(self):\n";
                ss << "        return {";
                bool first = true;
                for (const auto& col : table.columns) {
                    if (!first) ss << ", ";
                    ss << "\n            '" << col.name << "': self." << col.name;
                    first = false;
                }
                ss << "\n        }\n";
            }
            break;
    }

    return ss.str();
}

String Generator::fastapi_column_type(const Column& col) {
    String py_type = col.cpp_type();
    if (py_type == "int32_t" || py_type == "int64_t" || py_type == "int16_t" || py_type == "int8_t") {
        return "Integer";
    }
    if (py_type == "bool") {
        return "Boolean";
    }
    if (py_type == "float" || py_type == "double") {
        if (col.precision.has_value()) {
            return "Numeric(" + std::to_string(col.precision.value()) + (col.scale.has_value() ? ", " + std::to_string(col.scale.value()) : "") + ")";
        }
        return "Float";
    }
    if (py_type == "std::string") {
        if (col.length.has_value() && col.length.value() < 4000) {
            return "String(" + std::to_string(col.length.value()) + ")";
        }
        return "Text";
    }
    if (py_type.find("vector") != String::npos) {
        return "LargeBinary";
    }
    if (py_type == "std::chrono::system_clock::time_point") {
        return "DateTime";
    }
    if (py_type == "std::chrono::year_month_day") {
        return "Date";
    }
    if (py_type == "std::chrono::seconds") {
        return "Time";
    }
    return "String(255)";
}

String Generator::peewee_column_type_fastapi(const Column& col) {
    String py_type = col.cpp_type();
    if (py_type == "int32_t" || py_type == "int64_t" || py_type == "int16_t" || py_type == "int8_t") {
        if (col.primary_key && col.auto_increment) {
            return "IntegerField(primary_key=True)";
        }
        return "IntegerField()";
    }
    if (py_type == "bool") {
        return "BooleanField()";
    }
    if (py_type == "float" || py_type == "double") {
        return "FloatField()";
    }
    if (py_type == "std::string") {
        if (col.length.has_value() && col.length.value() < 4000) {
            return "CharField(max_length=" + std::to_string(col.length.value()) + ")";
        }
        return "TextField()";
    }
    if (py_type.find("vector") != String::npos) {
        return "BlobField()";
    }
    return "CharField(max_length=255)";
}

String Generator::generate_fastapi_schemas_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "from pydantic import BaseModel\n";
    ss << "from typing import Optional\n";
    ss << "from datetime import datetime\n\n";

    for (const auto& table : tables) {
        ss << "class " << table.class_name() << "Base(BaseModel):\n";
        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;
            String py_type = fastapi_python_type(col);
            if (col.nullable) {
                ss << "    " << col.field_name() << ": Optional[" << py_type << "] = None\n";
            } else {
                ss << "    " << col.field_name() << ": " << py_type << "\n";
            }
        }
        ss << "\nclass " << table.class_name() << "Create(" << table.class_name() << "Base):\n";
        ss << "    pass\n\n";
        ss << "class " << table.class_name() << "Response(" << table.class_name() << "Base):\n";
        if (table.pk_column()) {
            ss << "    " << table.pk_column()->field_name() << ": int\n";
        }
        ss << "    class Config:\n";
        ss << "        from_attributes = True\n\n";
    }

    return ss.str();
}

String Generator::fastapi_python_type(const Column& col) {
    String cpp_type = col.cpp_type();
    if (cpp_type == "int32_t" || cpp_type == "int64_t" || cpp_type == "int16_t" || cpp_type == "int8_t") {
        return "int";
    }
    if (cpp_type == "bool") {
        return "bool";
    }
    if (cpp_type == "float" || cpp_type == "double") {
        return "float";
    }
    return "str";
}

String Generator::generate_fastapi_database_py(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "from sqlalchemy import create_engine\n";
    ss << "from sqlalchemy.orm import sessionmaker, declarative_base\n\n";
    ss << "DATABASE_URL = \"sqlite:///./" << project_name_ << ".db\"\n";
    ss << "engine = create_engine(DATABASE_URL, connect_args={\"check_same_thread\": False})\n";
    ss << "SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)\n";
    ss << "Base = declarative_base()\n\n";
    ss << "def get_db():\n";
    ss << "    db = SessionLocal()\n";
    ss << "    try:\n";
    ss << "        yield db\n";
    ss << "    finally:\n";
    ss << "        db.close()\n";
    return ss.str();
}

String Generator::generate_fastapi_routers_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "from fastapi import APIRouter, Depends, HTTPException\n";
    ss << "from sqlalchemy.orm import Session\n";
    ss << "from typing import List\n";
    ss << "from app.database import get_db\n";
    ss << "from app.models import Base, ";

    for (size_t i = 0; i < tables.size(); ++i) {
        ss << tables[i].class_name();
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "\nfrom app.schemas import ";

    for (size_t i = 0; i < tables.size(); ++i) {
        ss << tables[i].class_name() << "Create, " << tables[i].class_name() << "Response";
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "\n\n";

    ss << namespace_ << "_router = APIRouter()\n\n";

    for (const auto& table : tables) {
        const auto& pk = table.pk_column();
        String pk_name = pk ? pk->field_name() : "id";
        String plural = pluralize(table.snake_name());
        String plural_path = pluralize(table.kebab_name());

        ss << "@" << namespace_ << "_router.get('/" << plural_path << "', response_model=List[" << table.class_name() << "Response])\n";
        ss << "def get_" << plural << "(db: Session = Depends(get_db)):\n";
        ss << "    items = db.query(" << table.class_name() << ").all()\n";
        ss << "    return items\n\n";

        ss << "@" << namespace_ << "_router.get('/" << plural_path << "/<int:" << pk_name << "', response_model=" << table.class_name() << "Response)\n";
        ss << "def get_" << table.instance_name() << "_by_id(" << pk_name << ": int, db: Session = Depends(get_db)):\n";
        ss << "    item = db.query(" << table.class_name() << ").filter(" << table.class_name() << "." << pk_name << " == " << pk_name << ").first()\n";
        ss << "    if not item:\n";
        ss << "        raise HTTPException(status_code=404, detail='" << table.class_name() << " not found')\n";
        ss << "    return item\n\n";

        ss << "@" << namespace_ << "_router.post('/" << plural_path << "', response_model=" << table.class_name() << "Response, status_code=201)\n";
        ss << "def create_" << table.instance_name() << "(data: " << table.class_name() << "Create, db: Session = Depends(get_db)):\n";
        ss << "    item = " << table.class_name() << "(**data.model_dump())\n";
        ss << "    db.add(item)\n";
        ss << "    db.commit()\n";
        ss << "    db.refresh(item)\n";
        ss << "    return item\n\n";

        ss << "@" << namespace_ << "_router.put('/" << plural_path << "/<int:" << pk_name << "', response_model=" << table.class_name() << "Response)\n";
        ss << "def update_" << table.instance_name() << "(" << pk_name << ": int, data: " << table.class_name() << "Create, db: Session = Depends(get_db)):\n";
        ss << "    item = db.query(" << table.class_name() << ").filter(" << table.class_name() << "." << pk_name << " == " << pk_name << ").first()\n";
        ss << "    if not item:\n";
        ss << "        raise HTTPException(status_code=404, detail='" << table.class_name() << " not found')\n";
        ss << "    for key, value in data.model_dump(exclude_unset=True).items():\n";
        ss << "        setattr(item, key, value)\n";
        ss << "    db.commit()\n";
        ss << "    db.refresh(item)\n";
        ss << "    return item\n\n";

        ss << "@" << namespace_ << "_router.delete('/" << plural_path << "/<int:" << pk_name << ">', status_code=204)\n";
        ss << "def delete_" << table.instance_name() << "(" << pk_name << ": int, db: Session = Depends(get_db)):\n";
        ss << "    item = db.query(" << table.class_name() << ").filter(" << table.class_name() << "." << pk_name << " == " << pk_name << ").first()\n";
        ss << "    if not item:\n";
        ss << "        raise HTTPException(status_code=404, detail='" << table.class_name() << " not found')\n";
        ss << "    db.delete(item)\n";
        ss << "    db.commit()\n\n";

        ss << "@" << namespace_ << "_router.get('/" << plural_path << "/count')\n";
        ss << "def count_" << plural << "(db: Session = Depends(get_db)):\n";
        ss << "    count = db.query(" << table.class_name() << ").count()\n";
        ss << "    return {'count': count}\n";
    }

    return ss.str();
}

String Generator::generate_fastapi_alembic_ini(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "[alembic]\n";
    ss << "script = mako\n\n";
    ss << "[loggers]\n";
    ss << "keys = root,sqlalchemy,alembic\n\n";
    ss << "[handlers]\n";
    ss << "keys = console\n\n";
    ss << "[formatters]\n";
    ss << "keys = generic\n\n";
    ss << "[logger_root]\n";
    ss << "level = WARN\n";
    ss << "handlers = console\n";
    ss << "qualname =\n\n";
    ss << "[logger_sqlalchemy]\n";
    ss << "level = WARN\n";
    ss << "handlers =\n";
    ss << "qualname = sqlalchemy.engine\n\n";
    ss << "[logger_alembic]\n";
    ss << "level = INFO\n";
    ss << "handlers =\n";
    ss << "qualname = alembic\n\n";
    ss << "[handler_console]\n";
    ss << "class = StreamHandler\n";
    ss << "args = (sys.stderr,)\n";
    ss << "level = NOTSET\n";
    ss << "formatter = generic\n\n";
    ss << "[formatter_generic]\n";
    ss << "format = %(levelname)-5.5s [%(name)s] %(message)s\n";
    ss << "datefmt = %H:%M:%S\n";
    return ss.str();
}

String Generator::generate_fastapi_alembic_env_py(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "from logging.config import fileConfig\n";
    ss << "from sqlalchemy import engine_from_config\n";
    ss << "from sqlalchemy import pool\n";
    ss << "from alembic import context\n";
    ss << "from app.models import Base\n\n";
    ss << "config = context.config\n\n";
    ss << "if config.config_file_name is not None:\n";
    ss << "    fileConfig(config.config_file_name)\n\n";
    ss << "target_metadata = Base.metadata\n\n";
    ss << "def run_migrations_offline() -> None:\n";
    ss << "    url = config.get_main_option('sqlalchemy.url')\n";
    ss << "    context.configure(\n";
    ss << "        url=url,\n";
    ss << "        target_metadata=target_metadata,\n";
    ss << "        literal_binds=True,\n";
    ss << "        dialect_opts={'paramstyle': 'named'},\n";
    ss << "    )\n\n";
    ss << "    with context.begin_transaction():\n";
    ss << "        context.run_migrations()\n\n";
    ss << "def run_migrations_online() -> None:\n";
    ss << "    connectable = engine_from_config(\n";
    ss << "        config.get_section(config.config_ini_section, {}),\n";
    ss << "        prefix='sqlalchemy.',\n";
    ss << "        poolclass=pool.NullPool,\n";
    ss << "    )\n\n";
    ss << "    with connectable.connect() as connection:\n";
    ss << "        context.configure(connection=connection, target_metadata=target_metadata)\n\n";
    ss << "        with context.begin_transaction():\n";
    ss << "            context.run_migrations()\n\n";
    ss << "if context.is_offline_mode():\n";
    ss << "    run_migrations_offline()\n";
    ss << "else:\n";
    ss << "    run_migrations_online()\n";
    return ss.str();
}

String Generator::generate_fastapi_durable_client_py() {
    return generate_flask_durable_client_py();
}

String Generator::generate_fastapi_dockerfile(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "FROM python:3.11-slim\n\n";
    ss << "WORKDIR /app\n\n";
    ss << "COPY requirements.txt .\n";
    ss << "RUN pip install --no-cache-dir -r requirements.txt\n\n";
    ss << "COPY . .\n\n";
    ss << "EXPOSE 8080\n\n";
    ss << "CMD [\"uvicorn\", \"app.main:app\", \"--host\", \"0.0.0.0\", \"--port\", \"8080\"]\n";
    return ss.str();
}

String Generator::generate_fastapi_docker_compose_yml() {
    std::stringstream ss;
    ss << "version: '3.8'\n\n";
    ss << "services:\n";
    ss << "  api:\n";
    ss << "    build: .\n";
    ss << "    ports:\n";
    ss << "      - \"8080:8080\"\n";
    ss << "    volumes:\n";
    ss << "      - ./data:/app\n";
    return ss.str();
}

String Generator::generate_fastapi_gitignore() {
    return R"(__pycache__/
*.py[cod]
*$py.class
*.so
.Python
env/
venv/
.venv/
ENV/
build/
dist/
*.egg-info/
.eggs/
.pytest_cache/
.coverage
htmlcov/
*.db
.idea/
.vscode/
*.swp
*.swo
*~
alembic/
)";
}

String Generator::generate_fastapi_readme(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "# " << project_name_ << " (FastAPI)\n\n";
    ss << "Auto-generated FastAPI REST API project.\n\n";
    ss << "## Requirements\n";
    ss << "- Python 3.9+\n\n";
    ss << "## Installation\n\n";
    ss << "```bash\n";
    ss << "pip install -r requirements.txt\n";
    ss << "```\n\n";
    ss << "## Running\n\n";
    ss << "```bash\n";
    ss << "uvicorn app.main:app --host 0.0.0.0 --port 8080 --reload\n";
    ss << "# or\n";
    ss << "python -m uvicorn app.main:app --host 0.0.0.0 --port 8080\n";
    ss << "```\n\n";
    ss << "## API Documentation\n\n";
    ss << "FastAPI provides automatic API documentation at:\n";
    ss << "- Swagger UI: http://localhost:8080/docs\n";
    ss << "- ReDoc: http://localhost:8080/redoc\n\n";
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

    ss << "## Docker\n\n";
    ss << "```bash\n";
    ss << "docker-compose up --build\n";
    ss << "```\n\n";
    ss << "Generated by NovaGen v1.0.0\n";
    return ss.str();
}

String Generator::generate_fastapi_pytest_ini() {
    return R"([pytest]
testpaths = tests
asyncio_mode = auto
python_files = test_*.py
python_classes = Test*
python_functions = test_*
addopts = -v --tb=short
)";
}

String Generator::generate_fastapi_test_models_py(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "import pytest\n";
    ss << "from fastapi.testclient import TestClient\n";
    ss << "from app.main import app\n\n";
    ss << "@pytest.fixture\n";
    ss << "def client():\n";
    ss << "    return TestClient(app)\n\n";

    for (const auto& table : tables) {
        ss << "def test_create_" << table.instance_name() << "(client):\n";
        ss << "    response = client.post('/api/" << pluralize(table.kebab_name()) << R"(', json={
            )";
        bool first = true;
        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;
            if (!first) ss << ",\n            ";
            first = false;
            if (col.cpp_type() == "std::string") {
                ss << "'" << col.name << "': 'test_value'";
            } else if (col.cpp_type() == "bool") {
                ss << "'" << col.name << "': true";
            } else {
                ss << "'" << col.name << "': 1";
            }
        }
        ss << "\n        })\n";
        ss << "    assert response.status_code == 201\n";
        ss << "    data = response.json()\n";
        ss << "    assert '" << (table.pk_column() ? table.pk_column()->field_name() : "id") << "' in data\n\n";

        ss << "def test_get_" << pluralize(table.snake_name()) << "(client):\n";
        ss << "    # Create first\n";
        ss << "    client.post('/api/" << pluralize(table.kebab_name()) << R"(', json={
            )";
        first = true;
        for (const auto& col : table.columns) {
            if (col.primary_key && col.auto_increment) continue;
            if (!first) ss << ",\n            ";
            first = false;
            if (col.cpp_type() == "std::string") {
                ss << "'" << col.name << "': 'test_value'";
            } else {
                ss << "'" << col.name << "': 1";
            }
        }
        ss << "\n        })\n\n";
        ss << "    response = client.get('/api/" << pluralize(table.kebab_name()) << "')\n";
        ss << "    assert response.status_code == 200\n";
        ss << "    data = response.json()\n";
        ss << "    assert len(data) >= 1\n\n";
    }

    return ss.str();
}

String Generator::generate_fastapi_test_routers_py(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "import pytest\n";
    ss << "from fastapi.testclient import TestClient\n";
    ss << "from app.main import app\n\n";
    ss << "@pytest.fixture\n";
    ss << "def client():\n";
    ss << "    return TestClient(app)\n\n";
    ss << "def test_health_endpoint(client):\n";
    ss << "    response = client.get('/health')\n";
    ss << "    assert response.status_code == 200\n";
    ss << "    assert response.json() == {'status': 'healthy'}\n\n";
    ss << "def test_api_info(client):\n";
    ss << "    response = client.get('/api')\n";
    ss << "    assert response.status_code == 200\n";
    ss << "    data = response.json()\n";
    ss << "    assert 'name' in data\n";
    ss << "    assert 'version' in data\n";
    ss << "    assert 'endpoints' in data\n";
    return ss.str();
}

String Generator::generate_fastapi_api_endpoints_json(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < tables.size(); ++i) {
        ss << "\"" << pluralize(tables[i].kebab_name()) << "\"";
        if (i < tables.size() - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

String Generator::generate_security_jwt_fastapi() {
    std::stringstream ss;
    ss << R"(
# security/jwt_auth.py
from datetime import datetime, timedelta
from typing import Optional
from jose import JWTError, jwt
from passlib.context import CryptContext
from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
import os

SECRET_KEY = os.environ.get('SECRET_KEY', 'dev-secret-key-change-in-production')
ALGORITHM = 'HS256'
ACCESS_TOKEN_EXPIRE_MINUTES = 30

pwd_context = CryptContext(schemes=['bcrypt'], deprecated='auto')
security = HTTPBearer()

class TokenData:
    def __init__(self, user_id: str, role: str = 'user'):
        self.user_id = user_id
        self.role = role

def verify_password(plain_password: str, hashed_password: str) -> bool:
    """Verify a password against its hash."""
    return pwd_context.verify(plain_password, hashed_password)

def get_password_hash(password: str) -> str:
    """Hash a password."""
    return pwd_context.hash(password)

def create_access_token(data: dict, expires_delta: Optional[timedelta] = None) -> str:
    """Create a JWT access token."""
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.utcnow() + expires_delta
    else:
        expire = datetime.utcnow() + timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)

    to_encode.update({'exp': expire, 'iat': datetime.utcnow()})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt

def decode_token(token: str) -> TokenData:
    """Decode and verify a JWT token."""
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        user_id: str = payload.get('user_id')
        role: str = payload.get('role', 'user')
        if user_id is None:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail='Invalid token'
            )
        return TokenData(user_id=user_id, role=role)
    except JWTError:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Could not validate credentials'
        )

async def get_current_user(credentials: HTTPAuthorizationCredentials = Depends(security)) -> TokenData:
    """Dependency to get the current authenticated user."""
    token = credentials.credentials
    return decode_token(token)

async def require_role(required_role: str):
    """Dependency factory to require a specific role."""
    async def role_checker(current_user: TokenData = Depends(get_current_user)) -> TokenData:
        if current_user.role != required_role and current_user.role != 'admin':
            raise HTTPException(
                status_code=status.HTTP_403_FORBIDDEN,
                detail='Insufficient permissions'
            )
        return current_user
    return role_checker
)";
    return ss.str();
}

String Generator::generate_security_oauth2_fastapi() {
    std::stringstream ss;
    ss << R"(
# security/oauth2.py
from typing import Optional
from fastapi import Depends, HTTPException, status, Request
from fastapi.security import OAuth2AuthorizationCodeBearer
import httpx
import os

OAUTH2_PROVIDER_URL = os.environ.get('OAUTH2_PROVIDER_URL', 'https://oauth.example.com')
OAUTH2_CLIENT_ID = os.environ.get('OAUTH2_CLIENT_ID', 'your-client-id')
OAUTH2_CLIENT_SECRET = os.environ.get('OAUTH2_CLIENT_SECRET', 'your-client-secret')
OAUTH2_REDIRECT_URI = os.environ.get('OAUTH2_REDIRECT_URI', 'http://localhost:8000/auth/callback')

oauth2_scheme = OAuth2AuthorizationCodeBearer(
    authorizationUrl=f'{OAUTH2_PROVIDER_URL}/authorize',
    tokenUrl=f'{OAUTH2_PROVIDER_URL}/token'
)

class OAuth2Provider:
    def __init__(self):
        self.provider_url = OAUTH2_PROVIDER_URL
        self.client_id = OAUTH2_CLIENT_ID
        self.client_secret = OAUTH2_CLIENT_SECRET
        self.redirect_uri = OAUTH2_REDIRECT_URI

    def get_authorization_url(self, state: Optional[str] = None) -> str:
        """Get the authorization URL to redirect users to."""
        if state is None:
            import secrets
            state = secrets.token_urlsafe(16)

        params = {
            'client_id': self.client_id,
            'redirect_uri': self.redirect_uri,
            'response_type': 'code',
            'scope': 'openid profile email',
            'state': state
        }
        query = '&'.join(f'{k}={v}' for k, v in params.items())
        return f'{self.provider_url}/authorize?{query}'

    async def exchange_code_for_token(self, code: str) -> Optional[dict]:
        """Exchange an authorization code for an access token."""
        async with httpx.AsyncClient() as client:
            response = await client.post(
                f'{self.provider_url}/token',
                data={
                    'grant_type': 'authorization_code',
                    'code': code,
                    'redirect_uri': self.redirect_uri,
                    'client_id': self.client_id,
                    'client_secret': self.client_secret
                }
            )
            if response.status_code == 200:
                return response.json()
            return None

    async def get_user_info(self, access_token: str) -> Optional[dict]:
        """Get user information using the access token."""
        async with httpx.AsyncClient() as client:
            response = await client.get(
                f'{self.provider_url}/userinfo',
                headers={'Authorization': f'Bearer {access_token}'}
            )
            if response.status_code == 200:
                return response.json()
            return None

    async def get_token_info(self, access_token: str) -> Optional[dict]:
        """Introspect the token to validate it."""
        async with httpx.AsyncClient() as client:
            response = await client.post(
                f'{self.provider_url}/introspect',
                data={'token': access_token}
            )
            if response.status_code == 200:
                return response.json()
            return None

oauth2_provider = OAuth2Provider()

async def get_current_user_oauth2(token: str = Depends(oauth2_scheme)) -> dict:
    """Dependency to get the current authenticated user via OAuth2."""
    token_info = await oauth2_provider.get_token_info(token)

    if not token_info or not token_info.get('active', False):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Invalid or expired token'
        )

    user_info = await oauth2_provider.get_user_info(token)
    if not user_info:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Could not fetch user info'
        )

    return user_info
)";
    return ss.str();
}

String Generator::generate_security_saml_fastapi() {
    std::stringstream ss;
    ss << R"(
# security/saml.py
from typing import Optional
from fastapi import Depends, HTTPException, status, Request, Form
from fastapi.responses import RedirectResponse
from lxml import etree
import os
import base64
import zlib

SAML_IDP_METADATA_URL = os.environ.get('SAML_IDP_METADATA_URL', 'https://idp.example.com/metadata')
SAML_SP_ENTITY_ID = os.environ.get('SAML_SP_ENTITY_ID', 'novagen-sp')
SAML_SP_ACS_URL = os.environ.get('SAML_SP_ACS_URL', 'http://localhost:8000/auth/saml/acs')
SAML_SP_PRIVATE_KEY = os.environ.get('SAML_SP_PRIVATE_KEY', '/path/to/sp-key.pem')
SAML_SP_CERTIFICATE = os.environ.get('SAML_SP_CERTIFICATE', '/path/to/sp-cert.pem')

class SAMLAuth:
    def __init__(self):
        self.idp_metadata_url = SAML_IDP_METADATA_URL
        self.sp_entity_id = SAML_SP_ENTITY_ID
        self.sp_acs_url = SAML_SP_ACS_URL

    def generate_authn_request(self) -> str:
        """Generate a SAML AuthnRequest."""
        import secrets
        auth_id = f'_id-{secrets.token_hex(16)}'
        issue_instant = '2024-01-01T00:00:00Z'

        request_xml = f'''<?xml version="1.0" encoding="UTF-8"?>
<samlp:AuthnRequest
    xmlns:samlp="urn:oasis:names:tc:SAML:2.0:protocol"
    ID="{auth_id}"
    Version="2.0"
    IssueInstant="{issue_instant}"
    AssertionConsumerServiceURL="{self.sp_acs_url}"
    ProtocolBinding="urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST">
    <saml:Issuer xmlns:saml="urn:oasis:names:tc:SAML:2.0:assertion">
        {self.sp_entity_id}
    </saml:Issuer>
    <samlp:NameIDPolicy
        xmlns:samlp="urn:oasis:names:tc:SAML:2.0:protocol"
        Format="urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress"
        AllowCreate="true"/>
</samlp:AuthnRequest>'''

        return base64.b64encode(request_xml.encode('utf-8')).decode('utf-8')

    def validate_response(self, saml_response: str) -> dict:
        """Validate SAML Response and extract user info."""
        try:
            decoded = base64.b64decode(saml_response)
            root = etree.fromstring(decoded)

            ns = {
                'samlp': 'urn:oasis:names:tc:SAML:2.0:protocol',
                'saml': 'urn:oasis:names:tc:SAML:2.0:assertion'
            }

            status_elem = root.find('.//samlp:StatusCode', ns)
            if status_elem is not None:
                status_value = status_elem.get('Value', '')
                if 'Success' not in status_value:
                    return {'valid': False, 'error': 'SAML authentication failed'}

            name_id = root.find('.//saml:NameID', ns)
            session_index = root.find('.//samlp:SessionIndex', ns)

            user_info = {
                'valid': True,
                'name_id': name_id.text if name_id is not None else None,
                'session_index': session_index.text if session_index is not None else None
            }

            return user_info

        except Exception as e:
            return {'valid': False, 'error': str(e)}

saml_auth = SAMLAuth()

async def get_current_user_saml(request: Request, saml_response: Optional[str] = Form(None)) -> dict:
    """Dependency to get the current authenticated user via SAML."""
    if not saml_response:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='SAML authentication required'
        )

    user_info = saml_auth.validate_response(saml_response)

    if not user_info.get('valid'):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail=user_info.get('error', 'Invalid SAML response')
        )

    return user_info
)";
    return ss.str();
}

String Generator::generate_security_owasp_fastapi() {
    std::stringstream ss;
    ss << R"(
# security/owasp.py
import re
from fastapi import Request, HTTPException
from fastapi.responses import JSONResponse
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.types import ASGIApp

XSS_PATTERNS = [
    re.compile(r'<script[^>]*>.*?</script>', re.IGNORECASE | re.DOTALL),
    re.compile(r'javascript:', re.IGNORECASE),
    re.compile(r'on\w+\s*=', re.IGNORECASE),
    re.compile(r'<[^>]+ on\w+=' , re.IGNORECASE)
]

SQL_INJECTION_PATTERNS = [
    re.compile(r'\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|CREATE|TRUNCATE|EXEC|EXECUTE)\b', re.IGNORECASE),
    re.compile(r'--'),
    re.compile(r'/\*.*\*/'),
    re.compile(r';.*--')
]

class OWASPSecurityMiddleware(BaseHTTPMiddleware):
    def __init__(self, app: ASGIApp):
        super().__init__(app)
        self.xss_patterns = XSS_PATTERNS
        self.sql_patterns = SQL_INJECTION_PATTERNS

    @staticmethod
    def apply_security_headers(response):
        """Apply OWASP-recommended security headers."""
        response.headers['X-Content-Type-Options'] = 'nosniff'
        response.headers['X-Frame-Options'] = 'DENY'
        response.headers['X-XSS-Protection'] = '1; mode=block'
        response.headers['Strict-Transport-Security'] = 'max-age=31536000; includeSubDomains'
        response.headers['Content-Security-Policy'] = "default-src 'self'"
        response.headers['Referrer-Policy'] = 'strict-origin-when-cross-origin'
        response.headers['Permissions-Policy'] = 'geolocation=(), microphone=(), camera=()'
        return response

    def check_xss(self, input_str: str) -> bool:
        """Check for XSS patterns in input."""
        if not input_str:
            return False
        for pattern in self.xss_patterns:
            if pattern.search(input_str):
                return True
        return False

    def check_sql_injection(self, input_str: str) -> bool:
        """Check for SQL injection patterns in input."""
        if not input_str:
            return False
        for pattern in self.sql_patterns:
            if pattern.search(input_str):
                return True
        return False

    def validate_request(self, request: Request) -> list:
        """Validate request for security threats."""
        threats = []

        # Check query parameters
        for key, value in request.query_params.items():
            if self.check_xss(value):
                threats.append(f'XSS threat in query param: {key}')
            if self.check_sql_injection(value):
                threats.append(f'SQL injection threat in query param: {key}')

        # Check form data
        form_data = await request.form()
        for key, value in form_data.items():
            value_str = str(value)
            if self.check_xss(value_str):
                threats.append(f'XSS threat in form field: {key}')
            if self.check_sql_injection(value_str):
                threats.append(f'SQL injection threat in form field: {key}')

        # Check JSON body
        if request.method in ['POST', 'PUT', 'PATCH']:
            try:
                json_data = await request.json()
                threats.extend(self._check_json(json_data))
            except Exception:
                pass

        return threats

    def _check_json(self, data, path=''):
        """Recursively check JSON data for threats."""
        threats = []
        if isinstance(data, dict):
            for key, value in data.items():
                new_path = f'{path}.{key}' if path else key
                threats.extend(self._check_json(value, new_path))
        elif isinstance(data, list):
            for i, item in enumerate(data):
                new_path = f'{path}[{i}]'
                threats.extend(self._check_json(item, new_path))
        elif isinstance(data, str):
            if self.check_xss(data):
                threats.append(f'XSS threat in JSON field: {path}')
            if self.check_sql_injection(data):
                threats.append(f'SQL injection threat in JSON field: {path}')
        return threats

    async def dispatch(self, request: Request, call_next):
        # Validate request
        threats = self.validate_request(request)
        if threats:
            return JSONResponse(
                status_code=400,
                content={'error': 'Potentially malicious input detected', 'threats': threats}
            )

        response = await call_next(request)
        return self.apply_security_headers(response)
)";
    return ss.str();
}

String Generator::generate_security_ldap_fastapi() {
    std::stringstream ss;
    ss << R"(
# security/ldap_auth.py
from typing import Optional
from fastapi import Depends, HTTPException, status, Header
import base64
import os

try:
    import ldap
    LDAP_AVAILABLE = True
except ImportError:
    LDAP_AVAILABLE = False

LDAP_URL = os.environ.get('LDAP_URL', 'ldap://localhost:389')
LDAP_BASE_DN = os.environ.get('LDAP_BASE_DN', 'dc=example,dc=com')
LDAP_USER_DN_PATTERN = os.environ.get('LDAP_USER_DN_PATTERN', 'uid={username},ou=users,dc=example,dc=com')
LDAP_GROUP_DN_PATTERN = os.environ.get('LDAP_GROUP_DN_PATTERN', 'ou=groups,dc=example,dc=com')
LDAP_REQUIRED_GROUP = os.environ.get('LDAP_REQUIRED_GROUP', None)

class LDAPAuthentication:
    def __init__(self):
        self.ldap_url = LDAP_URL
        self.base_dn = LDAP_BASE_DN
        self.user_dn_pattern = LDAP_USER_DN_PATTERN
        self.required_group = LDAP_REQUIRED_GROUP

    def connect(self):
        """Establish connection to LDAP server."""
        if not LDAP_AVAILABLE:
            raise ImportError("python-ldap library not installed. Install with: pip install python-ldap")

        conn = ldap.initialize(self.ldap_url)
        conn.protocol_version = ldap.VERSION3
        conn.set_option(ldap.OPT_REFERRALS, 0)
        conn.set_option(ldap.OPT_NETWORK_TIMEOUT, 10.0)
        return conn

    def authenticate(self, username: str, password: str) -> dict:
        """Authenticate user against LDAP directory."""
        if not LDAP_AVAILABLE:
            return {'success': False, 'error': 'LDAP library not available'}

        if not username or not password:
            return {'success': False, 'error': 'Username and password required'}

        try:
            user_dn = self.user_dn_pattern.format(username=username)
            conn = self.connect()
            conn.simple_bind_s(user_dn, password)

            user_info = {
                'success': True,
                'dn': user_dn,
                'username': username,
                'groups': self.get_user_groups(conn, user_dn)
            }
            conn.unbind_s()
            return user_info

        except ldap.INVALID_CREDENTIALS:
            return {'success': False, 'error': 'Invalid username or password'}
        except ldap.LDAPError as e:
            return {'success': False, 'error': f'LDAP error: {str(e)}'}

    def get_user_groups(self, conn, user_dn: str) -> list:
        """Get groups for a user."""
        groups = []
        try:
            search_filter = f'(member={user_dn})'
            results = conn.search_s(
                self.base_dn,
                ldap.SCOPE_SUBTREE,
                search_filter,
                ['cn']
            )

            for dn, attrs in results:
                if dn and b'cn' in attrs:
                    groups.append(attrs[b'cn'][0].decode('utf-8'))

        except ldap.LDAPError:
            pass

        return groups

ldap_auth = LDAPAuthentication()

def parse_basic_auth(auth_header: str) -> tuple:
    """Parse Basic authentication header."""
    if not auth_header.startswith('Basic '):
        return None, None

    try:
        encoded = auth_header[6:]
        decoded = base64.b64decode(encoded).decode('utf-8')
        username, password = decoded.split(':', 1)
        return username, password
    except Exception:
        return None, None

async def get_current_user_ldap(authorization: str = Header(None)) -> dict:
    """Dependency to get the current authenticated user via LDAP."""
    if not authorization:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Authentication required',
            headers={'WWW-Authenticate': 'Basic realm="API"'}
        )

    username, password = parse_basic_auth(authorization)

    if not username or not password:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail='Invalid authentication format'
        )

    result = ldap_auth.authenticate(username, password)

    if not result['success']:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail=result['error']
        )

    if LDAP_REQUIRED_GROUP and result['required_group'] not in result['groups']:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail='Insufficient permissions'
        )

    return result
)";
    return ss.str();
}

} // namespace novagen
