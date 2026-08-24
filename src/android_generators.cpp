#include "generator.hpp"
#include <sstream>

namespace novagen {

static String android_java_type(const Column& col) {
    String t = col.sql_type;
    for (auto& c : t) c = toupper(c);

    if (t.find("INT") != String::npos) return "Integer";
    if (t.find("BIGINT") != String::npos) return "Long";
    if (t.find("SMALLINT") != String::npos) return "Short";
    if (t.find("TINYINT") != String::npos) return "Byte";
    if (t.find("BIT") != String::npos || t == "BOOLEAN") return "Boolean";
    if (t.find("DECIMAL") != String::npos || t.find("NUMERIC") != String::npos ||
        t.find("MONEY") != String::npos || t.find("FLOAT") != String::npos ||
        t.find("DOUBLE") != String::npos || t.find("REAL") != String::npos) return "Double";
    if (t.find("VARCHAR") != String::npos || t.find("CHAR") != String::npos ||
        t.find("TEXT") != String::npos || t == "STRING") return "String";
    if (t.find("DATE") != String::npos) return "java.time.LocalDate";
    if (t.find("TIME") != String::npos) return "java.time.LocalTime";
    if (t.find("DATETIME") != String::npos || t.find("TIMESTAMP") != String::npos) return "java.time.LocalDateTime";
    if (t.find("BLOB") != String::npos || t.find("BINARY") != String::npos) return "byte[]";
    return "String";
}

static String android_kotlin_type(const Column& col) {
    String t = col.sql_type;
    for (auto& c : t) c = toupper(c);

    if (t.find("INT") != String::npos) return "Int";
    if (t.find("BIGINT") != String::npos) return "Long";
    if (t.find("SMALLINT") != String::npos) return "Short";
    if (t.find("TINYINT") != String::npos) return "Byte";
    if (t.find("BIT") != String::npos || t == "BOOLEAN") return "Boolean";
    if (t.find("DECIMAL") != String::npos || t.find("NUMERIC") != String::npos ||
        t.find("MONEY") != String::npos || t.find("FLOAT") != String::npos ||
        t.find("DOUBLE") != String::npos || t.find("REAL") != String::npos) return "Double";
    if (t.find("VARCHAR") != String::npos || t.find("CHAR") != String::npos ||
        t.find("TEXT") != String::npos || t == "STRING") return "String";
    if (t.find("DATE") != String::npos) return "java.time.LocalDate";
    if (t.find("TIME") != String::npos) return "java.time.LocalTime";
    if (t.find("DATETIME") != String::npos || t.find("TIMESTAMP") != String::npos) return "java.time.LocalDateTime";
    if (t.find("BLOB") != String::npos || t.find("BINARY") != String::npos) return "ByteArray";
    return "String";
}

static String android_pk_java_type(const Table& table) {
    const Column* pk = table.pk_column();
    if (pk == nullptr) return "Long";
    return android_java_type(*pk);
}

static String android_java_parse_expr(const Column& col, const String& var) {
    String t = android_java_type(col);
    if (t == "Integer") {
        return "(" + var + ".length() > 0 ? Integer.parseInt(" + var + ") : null)";
    } else if (t == "Long") {
        return "(" + var + ".length() > 0 ? Long.parseLong(" + var + ") : null)";
    } else if (t == "Short") {
        return "(" + var + ".length() > 0 ? Short.parseShort(" + var + ") : null)";
    } else if (t == "Byte") {
        return "(" + var + ".length() > 0 ? Byte.parseByte(" + var + ") : null)";
    } else if (t == "Double") {
        return "(" + var + ".length() > 0 ? Double.parseDouble(" + var + ") : null)";
    } else if (t == "Boolean") {
        return "Boolean.parseBoolean(" + var + ")";
    } else if (t == "byte[]") {
        return var + ".getBytes()";
    } else if (t == "java.time.LocalDate") {
        return "(" + var + ".length() > 0 ? java.time.LocalDate.parse(" + var + ") : null)";
    } else if (t == "java.time.LocalTime") {
        return "(" + var + ".length() > 0 ? java.time.LocalTime.parse(" + var + ") : null)";
    } else if (t == "java.time.LocalDateTime") {
        return "(" + var + ".length() > 0 ? java.time.LocalDateTime.parse(" + var + ") : null)";
    }
    return var;
}

static String android_java_to_text_expr(const String& getter) {
    return "(" + getter + " != null ? String.valueOf(" + getter + ") : \"\")";
}

String Generator::generate_android_build_gradle(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    if (android_language_ == AndroidLanguage::Java) {
        ss << R"(buildscript {
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:8.2.0'
    }
}

allprojects {
    repositories {
        google()
        mavenCentral()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
)";
        return ss.str();
    }
    ss << R"(buildscript {
    ext.kotlin_version = '1.9.22'
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:8.2.0'
        classpath "org.jetbrains.kotlin:kotlin-gradle-plugin:$kotlin_version"
    }
}

allprojects {
    repositories {
        google()
        mavenCentral()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
)";
    return ss.str();
}

String Generator::generate_android_app_build_gradle(const std::vector<Table>& tables) {
    (void)tables;
    if (android_language_ == AndroidLanguage::Java) {
        return generate_android_app_build_gradle_java(tables);
    }
    std::stringstream ss;
    ss << R"(plugins {
    id 'com.android.application'
    id 'org.jetbrains.kotlin.android'
}

android {
    namespace ')" << namespace_ << R"('
    compileSdk 34

    defaultConfig {
        applicationId ")" << namespace_ << R"(.restclient"
        minSdk 24
        targetSdk 34
        versionCode 1
        versionName "1.0"

        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
    compileOptions {
        sourceCompatibility JavaVersion.VERSION_17
        targetCompatibility JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = '17'
    }
    buildFeatures {
        viewBinding true
    }
}

dependencies {
    // AndroidX Core
    implementation 'androidx.core:core-ktx:1.12.0'
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'com.google.android.material:material:1.11.0'
    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'

    // Retrofit for REST API
    implementation 'com.squareup.retrofit2:retrofit:2.9.0'
    implementation 'com.squareup.retrofit2:converter-gson:2.9.0'
    implementation 'com.squareup.retrofit2:adapter-rxjava2:2.9.0'

    // OkHttp
    implementation 'com.squareup.okhttp3:okhttp:4.12.0'
    implementation 'com.squareup.okhttp3:logging-interceptor:4.12.0'

    // RxJava
    implementation 'io.reactivex.rxjava2:rxjava:2.2.21'
    implementation 'io.reactivex.rxjava2:rxandroid:2.1.1'

    // Gson
    implementation 'com.google.code.gson:gson:2.10.1'

    // Coroutines
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-android:1.7.3'
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-core:1.7.3'

    // Lifecycle & ViewModel
    implementation 'androidx.lifecycle:lifecycle-viewmodel-ktx:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-livedata-ktx:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-runtime-ktx:2.7.0'

    // Activity & Fragment KTX
    implementation 'androidx.activity:activity-ktx:1.8.2'
    implementation 'androidx.fragment:fragment-ktx:1.6.2'

    // RecyclerView
    implementation 'androidx.recyclerview:recyclerview:1.3.2'

    // SwipeRefreshLayout
    implementation 'androidx.swiperefreshlayout:swiperefreshlayout:1.1.0'

    // Testing
    testImplementation 'junit:junit:4.13.2'
    androidTestImplementation 'androidx.test.ext:junit:1.1.5'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.5.1'
}
)";
    return ss.str();
}

String Generator::generate_android_settings_gradle(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "rootProject.name = \"" << project_name_ << "_android\"\n";
    ss << "include ':app'\n";
    return ss.str();
}

String Generator::generate_android_gradle_properties(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8\n";
    ss << "android.useAndroidX=true\n";
    ss << "android.enableJetifier=true\n";
    if (android_language_ != AndroidLanguage::Java) {
        ss << "kotlin.code.style=official\n";
    }
    return ss.str();
}

String Generator::generate_android_manifest(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android">

    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />

    <application
        android:name=".RestClientApp"
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="@string/app_name"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:supportsRtl="true"
        android:theme="@style/Theme.AppTheme"
        android:usesCleartextTraffic="true">

        <activity
            android:name=".ui.MainActivity"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>

        <activity
            android:name=".ui.EntityListActivity"
            android:exported="false"
            android:parentActivityName=".ui.MainActivity" />

        <activity
            android:name=".ui.EntityDetailActivity"
            android:exported="false"
            android:parentActivityName=".ui.EntityListActivity" />

    </application>

</manifest>
)";
    return ss.str();
}

String Generator::generate_android_application_class(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(package )" << namespace_ << R"(

import android.app.Application

class RestClientApp : Application() {

    lateinit var apiClient: ApiClient
        private set

    override fun onCreate() {
        super.onCreate()
        instance = this
        apiClient = ApiClient()
    }

    companion object {
        lateinit var instance: RestClientApp
            private set
    }
}
)";
    return ss.str();
}

String Generator::generate_android_api_service(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << R"(package )" << namespace_ << R"(.data.api

import )" << namespace_ << R"(.data.model.*
import retrofit2.http.*
import io.reactivex.*

interface ApiService {
)";
    for (const auto& table : tables) {
        String entityName = table.class_name();
        String kebabName = table.kebab_name();
        String camelName = table.instance_name();

        ss << "\n    // " << entityName << " endpoints\n";
        ss << "    @GET(\"api/" << kebabName << "\")\n";
        ss << "    fun get" << entityName << "List(): Observable<List<" << entityName << ">>\n\n";
        ss << "    @GET(\"api/" << kebabName << "/{id}\")\n";
        ss << "    fun get" << entityName << "(@Path(\"id\") id: Long): Observable<" << entityName << ">\n\n";
        ss << "    @POST(\"api/" << kebabName << "\")\n";
        ss << "    fun create" << entityName << "(@Body " << camelName << ": " << entityName << "Request): Observable<" << entityName << ">\n\n";
        ss << "    @PUT(\"api/" << kebabName << "/{id}\")\n";
        ss << "    fun update" << entityName << "(@Path(\"id\") id: Long, @Body " << camelName << ": " << entityName << "Request): Observable<" << entityName << ">\n\n";
        ss << "    @DELETE(\"api/" << kebabName << "/{id}\")\n";
        ss << "    fun delete" << entityName << "(@Path(\"id\") id: Long): Observable<Unit>\n";
    }
    ss << "\n    @GET(\"api\")\n";
    ss << "    fun getApiInfo(): Observable<ApiInfo>\n\n";
    ss << "    @GET(\"health\")\n";
    ss << "    fun healthCheck(): Observable<Map<String, String>>\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_api_client(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(package )" << namespace_ << R"(.data.api

import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory
import retrofit2.converter.gson.GsonConverterFactory
import java.util.concurrent.TimeUnit

class ApiClient {
    private val baseUrl = "http://10.0.2.2:8080/"

    private val loggingInterceptor = HttpLoggingInterceptor().apply {
        level = HttpLoggingInterceptor.Level.BODY
    }

    private val okHttpClient = OkHttpClient.Builder()
        .addInterceptor(loggingInterceptor)
        .connectTimeout(30, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    private val retrofit = Retrofit.Builder()
        .baseUrl(baseUrl)
        .client(okHttpClient)
        .addConverterFactory(GsonConverterFactory.create())
        .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
        .build()

    val apiService: ApiService = retrofit.create(ApiService::class.java)
}
)";
    return ss.str();
}

String Generator::generate_android_model(const Table& table) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.model;\n\n";
    ss << "import com.google.gson.annotations.SerializedName;\n\n";

    String entityName = table.class_name();
    String kebabName = table.kebab_name();

    ss << "data class " << entityName << "(\n";
    for (size_t i = 0; i < table.columns.size(); ++i) {
        const auto& col = table.columns[i];
        ss << "    @SerializedName(\"" << col.field_name() << "\")\n";
        ss << "    var " << col.field_name() << ": " << android_kotlin_type(col);
        if (col.nullable && !col.primary_key) {
            ss << " = null";
        } else if (col.cpp_type() == "std::string") {
            ss << " = \"\"";
        } else if (col.cpp_type() == "int" || col.cpp_type() == "int64_t") {
            ss << " = 0";
        }
        if (i < table.columns.size() - 1) ss << ",\n";
    }
    ss << "\n)\n\n";

    ss << "data class " << entityName << "Request(\n";
    for (size_t i = 0; i < table.columns.size(); ++i) {
        const auto& col = table.columns[i];
        if (col.primary_key) continue;
        ss << "    @SerializedName(\"" << col.field_name() << "\")\n";
        ss << "    var " << col.field_name() << ": " << android_kotlin_type(col);
        if (col.nullable) {
            ss << " = null";
        } else if (col.cpp_type() == "std::string") {
            ss << " = \"\"";
        } else if (col.cpp_type() == "int" || col.cpp_type() == "int64_t") {
            ss << " = 0";
        }
        if (i < table.columns.size() - 1) ss << ",\n";
    }
    ss << "\n)\n";

    return ss.str();
}

String Generator::generate_android_repository(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.repository\n\n";
    ss << "import " << namespace_ << ".RestClientApp\n";
    ss << "import " << namespace_ << ".data.model.*\n";
    ss << "import io.reactivex.Observable\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers\n";
    ss << "import io.reactivex.schedulers.Schedulers\n\n";

    ss << "class EntityRepository {\n";
    ss << "    private val apiService = RestClientApp.instance.apiClient.apiService\n\n";

    for (const auto& table : tables) {
        String entityName = table.class_name();
        String camelName = table.instance_name();

        ss << "\n    // " << entityName << " repository\n";
        ss << "    fun get" << entityName << "List(): Observable<List<" << entityName << ">> {\n";
        ss << "        return apiService.get" << entityName << "List()\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
        ss << "    }\n\n";

        ss << "    fun get" << entityName << "(id: Long): Observable<" << entityName << "> {\n";
        ss << "        return apiService.get" << entityName << "(id)\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
        ss << "    }\n\n";

        ss << "    fun create" << entityName << "(" << camelName << ": " << entityName << "Request): Observable<" << entityName << "> {\n";
        ss << "        return apiService.create" << entityName << "(" << camelName << ")\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
        ss << "    }\n\n";

        ss << "    fun update" << entityName << "(id: Long, " << camelName << ": " << entityName << "Request): Observable<" << entityName << "> {\n";
        ss << "        return apiService.update" << entityName << "(id, " << camelName << ")\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
        ss << "    }\n\n";

        ss << "    fun delete" << entityName << "(id: Long): Observable<Unit> {\n";
        ss << "        return apiService.delete" << entityName << "(id)\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
        ss << "    }\n";
    }

    ss << "\n    fun healthCheck(): Observable<Map<String, String>> {\n";
    ss << "        return apiService.healthCheck()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n\n";

    ss << "    fun getApiInfo(): Observable<ApiInfo> {\n";
    ss << "        return apiService.getApiInfo()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n";

    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_activity(const Table& table) {
    (void)table;
    std::stringstream ss;
    ss << R"(package )" << namespace_ << R"(.ui

import android.content.Intent
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import )" << namespace_ << R"(.RestClientApp
import )" << namespace_ << R"(.data.model.*
import )" << namespace_ << R"(.data.repository.EntityRepository
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.schedulers.Schedulers

class MainActivity : AppCompatActivity() {

    private lateinit var repository: EntityRepository

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        repository = EntityRepository()
        checkApiHealth()
    }

    private fun checkApiHealth() {
        val disposable = repository.healthCheck()
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe(
                { result ->
                    Toast.makeText(this, "API Connected: ${result["status"]}", Toast.LENGTH_SHORT).show()
                },
                { error ->
                    Toast.makeText(this, "API Error: ${error.message}", Toast.LENGTH_LONG).show()
                }
            )
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main_menu, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.action_)" << table.snake_name() << R"( -> {
                startActivity(Intent(this, EntityListActivity::class.java))
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }
}
)";
    return ss.str();
}

String Generator::generate_android_layout(const Table& table) {
    (void)table;
    std::stringstream ss;
    ss << R"(<?xml version="1.0" encoding="utf-8"?>
<androidx.coordinatorlayout.widget.CoordinatorLayout
    xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:app="http://schemas.android.com/apk/res-auto"
    xmlns:tools="http://schemas.android.com/tools"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    tools:context=".ui.MainActivity">

    <com.google.android.material.appbar.AppBarLayout
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:theme="@style/ThemeOverlay.AppCompat.Dark.ActionBar">

        <androidx.appcompat.widget.Toolbar
            android:id="@+id/toolbar"
            android:layout_width="match_parent"
            android:layout_height="?attr/actionBarSize"
            android:background="?attr/colorPrimary"
            app:popupTheme="@style/ThemeOverlay.AppCompat.Light" />

    </com.google.android.material.appbar.AppBarLayout>

    <LinearLayout
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        android:orientation="vertical"
        android:padding="16dp"
        app:layout_behavior="@string/appbar_scrolling_view_behavior">

        <TextView
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="@string/app_name"
            android:textSize="24sp"
            android:textStyle="bold"
            android:layout_gravity="center_horizontal"
            android:layout_marginTop="32dp" />

        <TextView
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="@string/api_connected"
            android:textSize="16sp"
            android:layout_gravity="center_horizontal"
            android:layout_marginTop="16dp" />

        <TextView
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="@string/select_entity"
            android:textSize="14sp"
            android:layout_gravity="center_horizontal"
            android:layout_marginTop="48dp" />

        <Button
            android:id="@+id/btn_)" << table.snake_name() << R"("
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:text="@string/)" << table.snake_name() << R"("
            android:layout_marginTop="8dp" />

    </LinearLayout>

</androidx.coordinatorlayout.widget.CoordinatorLayout>
)";
    return ss.str();
}

String Generator::generate_android_viewmodel(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();

    ss << "package " << namespace_ << ".ui.viewmodel\n\n";
    ss << "import androidx.lifecycle.ViewModel\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "Request\n";
    ss << "import )" << namespace_ << ".data.repository.EntityRepository\n";
    ss << "import io.reactivex.Observable\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers\n";
    ss << "import io.reactivex.schedulers.Schedulers\n\n";

    ss << "class " << entityName << "ViewModel : ViewModel() {\n";
    ss << "    private val repository = EntityRepository()\n\n";

    ss << "    fun getList(): Observable<List<" << entityName << ">> {\n";
    ss << "        return repository.get" << entityName << "List()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n\n";

    ss << "    fun get(id: Long): Observable<" << entityName << "> {\n";
    ss << "        return repository.get" << entityName << "(id)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n\n";

    ss << "    fun create(request: " << entityName << "Request): Observable<" << entityName << "> {\n";
    ss << "        return repository.create" << entityName << "(request)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n\n";

    ss << "    fun update(id: Long, request: " << entityName << "Request): Observable<" << entityName << "> {\n";
    ss << "        return repository.update" << entityName << "(id, request)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n\n";

    ss << "    fun delete(id: Long): Observable<Unit> {\n";
    ss << "        return repository.delete" << entityName << "(id)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_adapter(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();

    ss << "package " << namespace_ << ".ui.adapter\n\n";
    ss << "import android.view.LayoutInflater\n";
    ss << "import android.view.View\n";
    ss << "import android.view.ViewGroup\n";
    ss << "import android.widget.TextView\n";
    ss << "import androidx.recyclerview.widget.RecyclerView\n";
    ss << "import )" << namespace_ << ".R\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "\n\n";

    ss << "class " << entityName << "Adapter(\n";
    ss << "    private var items: List<" << entityName << ">,\n";
    ss << "    private val onItemClick: ((" << entityName << ") -> Unit)? = null\n";
    ss << ") : RecyclerView.Adapter<" << entityName << "Adapter.ViewHolder>() {\n\n";

    ss << "    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {\n";
    ss << "        val view = LayoutInflater.from(parent.context)\n";
    ss << "            .inflate(R.layout.item_" << table.snake_name() << ", parent, false)\n";
    ss << "        return ViewHolder(view)\n";
    ss << "    }\n\n";

    ss << "    override fun onBindViewHolder(holder: ViewHolder, position: Int) {\n";
    ss << "        val item = items[position]\n";
    ss << "        holder.bind(item)\n";
    ss << "        holder.itemView.setOnClickListener { onItemClick?.invoke(item) }\n";
    ss << "    }\n\n";

    ss << "    override fun getItemCount(): Int = items.size\n\n";

    ss << "    fun updateItems(newItems: List<" << entityName << ">) {\n";
    ss << "        items = newItems\n";
    ss << "        notifyDataSetChanged()\n";
    ss << "    }\n\n";

    ss << "    class ViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "        private val text" << to_pascal(col.field_name()) << ": TextView = itemView.findViewById(R.id.text_" << col.field_name() << ")\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key && table.columns.size() > 1) {
            ss << "        private val text" << to_pascal(col.field_name()) << ": TextView = itemView.findViewById(R.id.text_" << col.field_name() << ")\n";
        }
    }
    ss << "\n";
    ss << "        fun bind(item: " << entityName << ") {\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "            text" << to_pascal(col.field_name()) << ".text = item." << col.field_name() << "?.toString() ?: \"\"\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key && table.columns.size() > 1) {
            ss << "            text" << to_pascal(col.field_name()) << ".text = item." << col.field_name() << "?.toString() ?: \"\"\n";
        }
    }
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_strings_xml(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<resources>\n";
    ss << "    <string name=\"app_name\">" << project_name_ << " REST Client</string>\n";
    ss << "    <string name=\"api_connected\">API Connected</string>\n";
    ss << "    <string name=\"select_entity\">Select an entity to manage</string>\n";
    ss << "    <string name=\"loading\">Loading...</string>\n";
    ss << "    <string name=\"error_loading\">Error loading data</string>\n";
    ss << "    <string name=\"create\">Create</string>\n";
    ss << "    <string name=\"update\">Update</string>\n";
    ss << "    <string name=\"delete\">Delete</string>\n";
    ss << "    <string name=\"save\">Save</string>\n";
    ss << "    <string name=\"cancel\">Cancel</string>\n";
    ss << "    <string name=\"confirm_delete\">Are you sure you want to delete this item?</string>\n";
    ss << "    <string name=\"yes\">Yes</string>\n";
    ss << "    <string name=\"no\">No</string>\n";
    ss << "    <string name=\"success\">Success</string>\n";
    ss << "    <string name=\"error\">Error</string>\n";
    ss << "    <string name=\"retry\">Retry</string>\n";
    ss << "    <string name=\"no_data\">No data available</string>\n";
    ss << "    <string name=\"refresh\">Refresh</string>\n";
    ss << "    <string name=\"search\">Search</string>\n";
    for (const auto& table : tables) {
        ss << "    <string name=\"" << table.snake_name() << "\">" << table.name << "</string>\n";
    }
    ss << "</resources>\n";
    return ss.str();
}

String Generator::generate_android_colors_xml() {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<resources>\n";
    ss << "    <color name=\"colorPrimary\">#2196F3</color>\n";
    ss << "    <color name=\"colorPrimaryDark\">#1976D2</color>\n";
    ss << "    <color name=\"colorAccent\">#FF4081</color>\n";
    ss << "    <color name=\"white\">#FFFFFF</color>\n";
    ss << "    <color name=\"black\">#000000</color>\n";
    ss << "    <color name=\"gray\">#757575</color>\n";
    ss << "    <color name=\"lightGray\">#E0E0E0</color>\n";
    ss << "    <color name=\"error\">#F44336</color>\n";
    ss << "    <color name=\"success\">#4CAF50</color>\n";
    ss << "</resources>\n";
    return ss.str();
}

String Generator::generate_android_theme_xml() {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<resources>\n";
    ss << "    <style name=\"Theme.AppTheme\" parent=\"Theme.MaterialComponents.DayNight.DarkActionBar\">\n";
    ss << "        <item name=\"colorPrimary\">@color/colorPrimary</item>\n";
    ss << "        <item name=\"colorPrimaryDark\">@color/colorPrimaryDark</item>\n";
    ss << "        <item name=\"colorAccent\">@color/colorAccent</item>\n";
    ss << "    </style>\n";
    ss << "</resources>\n";
    return ss.str();
}

bool Generator::generate_android(const fs::path& base, const std::vector<Table>& tables) {
    try {
        if (!create_directory(base)) return false;
        if (!create_directory(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "api")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "repository")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / "adapter")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / "viewmodel")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "layout")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "values")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "menu")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "drawable")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "mipmap-hdpi")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "mipmap-mdpi")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "mipmap-xhdpi")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "mipmap-xxhdpi")) return false;
        if (!create_directory(base / "app" / "src" / "main" / "res" / "mipmap-xxxhdpi")) return false;
        if (!create_directory(base / "gradle" / "wrapper")) return false;

        if (!write_file(base / "build.gradle", generate_android_build_gradle(tables))) return false;
        if (!write_file(base / "settings.gradle", generate_android_settings_gradle(tables))) return false;
        if (!write_file(base / "gradle.properties", generate_android_gradle_properties(tables))) return false;
        if (!write_file(base / "app" / "build.gradle", generate_android_app_build_gradle(tables))) return false;
        if (!write_file(base / "app" / "src" / "main" / "AndroidManifest.xml", generate_android_manifest(tables))) return false;

        bool java = (android_language_ == AndroidLanguage::Java);
        String ext = java ? "java" : "kt";

        if (java) {
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "RestClientApp.java",
                          generate_android_application_class_java(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "api" / "ApiService.java",
                          generate_android_api_service_java(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "api" / "ApiClient.java",
                          generate_android_api_client_java(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "repository" / "EntityRepository.java",
                          generate_android_repository_java(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model" / "ApiInfo.java",
                          generate_android_apiinfo_java())) return false;
        } else {
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "RestClientApp.kt",
                          generate_android_application_class(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "api" / "ApiService.kt",
                          generate_android_api_service(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "api" / "ApiClient.kt",
                          generate_android_api_client(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "repository" / "EntityRepository.kt",
                          generate_android_repository(tables))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model" / "ApiInfo.kt",
                          generate_android_apiinfo_kt())) return false;
        }

        for (const auto& table : tables) {
            if (java) {
                if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model" / (table.class_name() + ".java"),
                              generate_android_dto_java(table))) return false;
                if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model" / (table.class_name() + "Request.java"),
                              generate_android_request_java(table))) return false;
            } else {
                if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "data" / "model" / (table.class_name() + ".kt"),
                              generate_android_model(table))) return false;
            }
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / "viewmodel" / (table.class_name() + "ViewModel." + ext),
                          (java ? generate_android_viewmodel_java(table) : generate_android_viewmodel(table)))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / "adapter" / (table.class_name() + "Adapter." + ext),
                          (java ? generate_android_adapter_java(table) : generate_android_adapter(table)))) return false;
            if (!write_file(base / "app" / "src" / "main" / "res" / "layout" / ("item_" + table.snake_name() + ".xml"),
                          generate_android_item_layout(table))) return false;
        }

        if (!tables.empty()) {
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / ("MainActivity." + ext),
                          (java ? generate_android_activity_java(tables[0]) : generate_android_activity(tables[0])))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / ("EntityListActivity." + ext),
                          (java ? generate_android_list_activity_java(tables[0]) : generate_android_list_activity(tables[0])))) return false;
            if (!write_file(base / "app" / "src" / "main" / "java" / namespace_ / "ui" / ("EntityDetailActivity." + ext),
                          (java ? generate_android_detail_activity_java(tables[0]) : generate_android_detail_activity(tables[0])))) return false;
            if (!write_file(base / "app" / "src" / "main" / "res" / "layout" / "activity_main.xml",
                          generate_android_layout(tables[0]))) return false;
            if (!write_file(base / "app" / "src" / "main" / "res" / "layout" / "activity_entity_list.xml",
                          generate_android_list_layout(tables[0]))) return false;
            if (!write_file(base / "app" / "src" / "main" / "res" / "layout" / "activity_entity_detail.xml",
                          generate_android_detail_layout(tables[0]))) return false;
        }

        if (!write_file(base / "app" / "src" / "main" / "res" / "values" / "strings.xml", generate_android_strings_xml(tables))) return false;
        if (!write_file(base / "app" / "src" / "main" / "res" / "values" / "colors.xml", generate_android_colors_xml())) return false;
        if (!write_file(base / "app" / "src" / "main" / "res" / "values" / "themes.xml", generate_android_theme_xml())) return false;
        if (!tables.empty()) {
            if (!write_file(base / "app" / "src" / "main" / "res" / "menu" / "main_menu.xml", generate_android_menu_xml(tables[0]))) return false;
        }

        if (!write_file(base / "gradle" / "wrapper" / "gradle-wrapper.properties", generate_android_gradle_wrapper_properties())) return false;

        if (!write_file(base / "gradlew", generate_android_gradlew())) return false;
        if (!write_file(base / "gradlew.bat", generate_android_gradlew_bat())) return false;

        if (!write_file(base / "local.properties", "sdk.dir=/path/to/android/sdk\n")) return false;
        if (!write_file(base / "app" / "proguard-rules.pro", "# Proguard rules\n")) return false;

        return true;
    } catch (const std::exception& e) {
        last_error_ = "Android generation failed: " + String(e.what());
        return false;
    }
}

String Generator::generate_android_item_layout(const Table& table) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<androidx.cardview.widget.CardView\n";
    ss << "    xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    ss << "    xmlns:app=\"http://schemas.android.com/apk/res-auto\"\n";
    ss << "    android:layout_width=\"match_parent\"\n";
    ss << "    android:layout_height=\"wrap_content\"\n";
    ss << "    android:layout_margin=\"8dp\"\n";
    ss << "    app:cardElevation=\"4dp\"\n";
    ss << "    app:cardCornerRadius=\"8dp\">\n\n";
    ss << "    <LinearLayout\n";
    ss << "        android:layout_width=\"match_parent\"\n";
    ss << "        android:layout_height=\"wrap_content\"\n";
    ss << "        android:orientation=\"vertical\"\n";
    ss << "        android:padding=\"16dp\">\n\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "        <TextView\n";
            ss << "            android:id=\"@+id/text_" << col.field_name() << "\"\n";
            ss << "            android:layout_width=\"wrap_content\"\n";
            ss << "            android:layout_height=\"wrap_content\"\n";
            ss << "            android:text=\"" << col.name << "\"\n";
            ss << "            android:textStyle=\"bold\"\n";
            ss << "            android:textSize=\"12sp\" />\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        <TextView\n";
            ss << "            android:id=\"@+id/text_" << col.field_name() << "\"\n";
            ss << "            android:layout_width=\"wrap_content\"\n";
            ss << "            android:layout_height=\"wrap_content\"\n";
            ss << "            android:text=\"" << col.name << "\"\n";
            ss << "            android:textSize=\"14sp\"\n";
            ss << "            android:layout_marginTop=\"4dp\" />\n";
        }
    }
    ss << "    </LinearLayout>\n\n";
    ss << "</androidx.cardview.widget.CardView>\n";
    return ss.str();
}

String Generator::generate_android_list_activity(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();

    ss << "package " << namespace_ << ".ui\n\n";
    ss << "import android.content.Intent\n";
    ss << "import android.os.Bundle\n";
    ss << "import android.view.View\n";
    ss << "import android.widget.Toast\n";
    ss << "import androidx.appcompat.app.AppCompatActivity\n";
    ss << "import androidx.lifecycle.ViewModelProvider\n";
    ss << "import androidx.recyclerview.widget.LinearLayoutManager\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "Request\n";
    ss << "import )" << namespace_ << ".ui.adapter." << entityName << "Adapter\n";
    ss << "import )" << namespace_ << ".ui.viewmodel." << entityName << "ViewModel\n";
    ss << "import )" << namespace_ << ".data.repository.EntityRepository\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers\n";
    ss << "import io.reactivex.schedulers.Schedulers\n\n";

    ss << "class EntityListActivity : AppCompatActivity() {\n";
    ss << "    private lateinit var viewModel: " << entityName << "ViewModel\n";
    ss << "    private lateinit var adapter: " << entityName << "Adapter\n";
    ss << "    private val repository = EntityRepository()\n\n";

    ss << "    override fun onCreate(savedInstanceState: Bundle?) {\n";
    ss << "        super.onCreate(savedInstanceState)\n";
    ss << "        setContentView(R.layout.activity_entity_list)\n\n";
    ss << "        supportActionBar?.title = \"" << entityName << " List\"\n\n";
    ss << "        setupRecyclerView()\n";
    ss << "        loadData()\n\n";
    ss << "        findViewById<View>(R.id.fab_add).setOnClickListener {\n";
    ss << "            startActivity(Intent(this, EntityDetailActivity::class.java))\n";
    ss << "        }\n\n";
    ss << "        findViewById<View>(R.id.swipe_refresh).setOnRefreshListener {\n";
    ss << "            loadData()\n";
    ss << "        }\n";
    ss << "    }\n\n";

    ss << "    private fun setupRecyclerView() {\n";
    ss << "        adapter = " << entityName << "Adapter(emptyList()) { item ->\n";
    ss << "            val intent = Intent(this, EntityDetailActivity::class.java)\n";
    const Column* pk = table.pk_column();
    if (pk != nullptr) {
        ss << "            intent.putExtra(\"entity_id\", item." << pk->field_name() << ")\n";
    } else {
        ss << "            intent.putExtra(\"entity_id\", item)\n";
    }
    ss << "            startActivity(intent)\n";
    ss << "        }\n";
    ss << "        findViewById<RecyclerView>(R.id.recycler_view).adapter = adapter\n";
    ss << "        findViewById<RecyclerView>(R.id.recycler_view).layoutManager = LinearLayoutManager(this)\n";
    ss << "    }\n\n";

    ss << "    private fun loadData() {\n";
    ss << "        val disposable = repository.get" << entityName << "List()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(\n";
    ss << "                { items ->\n";
    ss << "                    adapter.updateItems(items)\n";
    ss << "                    findViewById<View>(R.id.swipe_refresh).isRefreshing = false\n";
    ss << "                    findViewById<View>(R.id.progress_bar).visibility = View.GONE\n";
    ss << "                    findViewById<View>(R.id.text_empty).visibility = if (items.isEmpty()) View.VISIBLE else View.GONE\n";
    ss << "                },\n";
    ss << "                { error ->\n";
    ss << "                    Toast.makeText(this, \"Error: ${error.message}\", Toast.LENGTH_LONG).show()\n";
    ss << "                    findViewById<View>(R.id.swipe_refresh).isRefreshing = false\n";
    ss << "                    findViewById<View>(R.id.progress_bar).visibility = View.GONE\n";
    ss << "                }\n";
    ss << "            )\n";
    ss << "    }\n\n";

    ss << "    override fun onResume() {\n";
    ss << "        super.onResume()\n";
    ss << "        loadData()\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_detail_activity(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();

    ss << "package " << namespace_ << ".ui\n\n";
    ss << "import android.os.Bundle\n";
    ss << "import android.view.View\n";
    ss << "import android.widget.Toast\n";
    ss << "import androidx.appcompat.app.AlertDialog\n";
    ss << "import androidx.appcompat.app.AppCompatActivity\n";
    ss << "import )" << namespace_ << ".data.model." << entityName << "Request\n";
    ss << "import )" << namespace_ << ".data.repository.EntityRepository\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers\n";
    ss << "import io.reactivex.schedulers.Schedulers\n\n";

    ss << "class EntityDetailActivity : AppCompatActivity() {\n";
    ss << "    private val repository = EntityRepository()\n";
    ss << "    private var entityId: Long? = null\n";
    ss << "    private var isEditMode = false\n\n";

    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "    private lateinit var edit" << to_pascal(col.field_name()) << ": android.widget.EditText\n";
        }
    }

    ss << "\n    override fun onCreate(savedInstanceState: Bundle?) {\n";
    ss << "        super.onCreate(savedInstanceState)\n";
    ss << "        setContentView(R.layout.activity_entity_detail)\n\n";
    ss << "        entityId = intent.getLongExtra(\"entity_id\", -1).takeIf { it != -1L }\n";
    ss << "        isEditMode = entityId != null\n\n";
    ss << "        supportActionBar?.title = if (isEditMode) \"Edit " << entityName << "\" else \"Create " << entityName << "\"\n\n";

    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        edit" << to_pascal(col.field_name()) << " = findViewById(R.id.edit_" << col.field_name() << ")\n";
        }
    }

    ss << "\n        if (isEditMode) {\n";
    ss << "            loadEntity()\n";
    ss << "        }\n\n";

    ss << "        findViewById<View>(R.id.btn_save).setOnClickListener {\n";
    ss << "            saveEntity()\n";
    ss << "        }\n\n";

    ss << "        findViewById<View>(R.id.btn_delete).setOnClickListener {\n";
    ss << "            confirmDelete()\n";
    ss << "        }\n\n";

    ss << "        findViewById<View>(R.id.btn_delete).visibility = if (isEditMode) View.VISIBLE else View.GONE\n";
    ss << "    }\n\n";

    ss << "    private fun loadEntity() {\n";
    ss << "        findViewById<View>(R.id.progress_bar).visibility = View.VISIBLE\n";
    ss << "        val disposable = repository.get" << entityName << "(entityId!!)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(\n";
    ss << "                { entity ->\n";
    ss << "                    findViewById<View>(R.id.progress_bar).visibility = View.GONE\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "                    edit" << to_pascal(col.field_name()) << ".setText(entity." << col.field_name() << "?.toString() ?: \"\")\n";
        }
    }
    ss << "                },\n";
    ss << "                { error ->\n";
    ss << "                    Toast.makeText(this, \"Error: ${error.message}\", Toast.LENGTH_LONG).show()\n";
    ss << "                    finish()\n";
    ss << "                }\n";
    ss << "            )\n";
    ss << "    }\n\n";

    ss << "    private fun saveEntity() {\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        val " << col.field_name() << " = edit" << to_pascal(col.field_name()) << ".text.toString()\n";
        }
    }
    ss << "\n        findViewById<View>(R.id.progress_bar).visibility = View.VISIBLE\n";
    ss << "        val disposable = if (isEditMode) {\n";
    ss << "            repository.update" << entityName << "(entityId!!, " << entityName << "Request(";
    bool first = true;
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            if (!first) ss << ", ";
            ss << col.field_name();
            first = false;
        }
    }
    ss << "))\n";
    ss << "        } else {\n";
    ss << "            repository.create" << entityName << "(" << entityName << "Request(";
    first = true;
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            if (!first) ss << ", ";
            ss << col.field_name();
            first = false;
        }
    }
    ss << "))\n";
    ss << "        }\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(\n";
    ss << "                { Toast.makeText(this, \"Success\", Toast.LENGTH_SHORT).show(); finish() },\n";
    ss << "                { error ->\n";
    ss << "                    findViewById<View>(R.id.progress_bar).visibility = View.GONE\n";
    ss << "                    Toast.makeText(this, \"Error: ${error.message}\", Toast.LENGTH_LONG).show()\n";
    ss << "                }\n";
    ss << "            )\n";
    ss << "    }\n\n";

    ss << "    private fun confirmDelete() {\n";
    ss << "        AlertDialog.Builder(this)\n";
    ss << "            .setTitle(\"Delete\")\n";
    ss << "            .setMessage(\"Are you sure you want to delete this item?\")\n";
    ss << "            .setPositiveButton(\"Yes\") { _, _ -> deleteEntity() }\n";
    ss << "            .setNegativeButton(\"No\", null)\n";
    ss << "            .show()\n";
    ss << "    }\n\n";

    ss << "    private fun deleteEntity() {\n";
    ss << "        findViewById<View>(R.id.progress_bar).visibility = View.VISIBLE\n";
    ss << "        val disposable = repository.delete" << entityName << "(entityId!!)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(\n";
    ss << "                { Toast.makeText(this, \"Deleted\", Toast.LENGTH_SHORT).show(); finish() },\n";
    ss << "                { error ->\n";
    ss << "                    findViewById<View>(R.id.progress_bar).visibility = View.GONE\n";
    ss << "                    Toast.makeText(this, \"Error: ${error.message}\", Toast.LENGTH_LONG).show()\n";
    ss << "                }\n";
    ss << "            )\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_list_layout(const Table& table) {
    (void)table;
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<androidx.coordinatorlayout.widget.CoordinatorLayout\n";
    ss << "    xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    ss << "    xmlns:app=\"http://schemas.android.com/apk/res-auto\"\n";
    ss << "    android:layout_width=\"match_parent\"\n";
    ss << "    android:layout_height=\"match_parent\">\n\n";
    ss << "    <androidx.swiperefreshlayout.widget.SwipeRefreshLayout\n";
    ss << "        android:id=\"@+id/swipe_refresh\"\n";
    ss << "        android:layout_width=\"match_parent\"\n";
    ss << "        android:layout_height=\"match_parent\">\n\n";
    ss << "        <FrameLayout\n";
    ss << "            android:layout_width=\"match_parent\"\n";
    ss << "            android:layout_height=\"match_parent\">\n\n";
    ss << "            <androidx.recyclerview.widget.RecyclerView\n";
    ss << "                android:id=\"@+id/recycler_view\"\n";
    ss << "                android:layout_width=\"match_parent\"\n";
    ss << "                android:layout_height=\"match_parent\"\n";
    ss << "                android:clipToPadding=\"false\"\n";
    ss << "                android:padding=\"8dp\" />\n\n";
    ss << "            <ProgressBar\n";
    ss << "                android:id=\"@+id/progress_bar\"\n";
    ss << "                android:layout_width=\"wrap_content\"\n";
    ss << "                android:layout_height=\"wrap_content\"\n";
    ss << "                android:layout_gravity=\"center\" />\n\n";
    ss << "            <TextView\n";
    ss << "                android:id=\"@+id/text_empty\"\n";
    ss << "                android:layout_width=\"wrap_content\"\n";
    ss << "                android:layout_height=\"wrap_content\"\n";
    ss << "                android:layout_gravity=\"center\"\n";
    ss << "                android:text=\"@string/no_data\"\n";
    ss << "                android:textSize=\"16sp\"\n";
    ss << "                android:visibility=\"gone\" />\n\n";
    ss << "        </FrameLayout>\n\n";
    ss << "    </androidx.swiperefreshlayout.widget.SwipeRefreshLayout>\n\n";
    ss << "    <com.google.android.material.floatingactionbutton.FloatingActionButton\n";
    ss << "        android:id=\"@+id/fab_add\"\n";
    ss << "        android:layout_width=\"wrap_content\"\n";
    ss << "        android:layout_height=\"wrap_content\"\n";
    ss << "        android:layout_gravity=\"bottom|end\"\n";
    ss << "        android:layout_margin=\"16dp\"\n";
    ss << "        android:src=\"@android:drawable/ic_input_add\"\n";
    ss << "        app:tint=\"@color/white\" />\n\n";
    ss << "</androidx.coordinatorlayout.widget.CoordinatorLayout>\n";
    return ss.str();
}

String Generator::generate_android_detail_layout(const Table& table) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<ScrollView\n";
    ss << "    xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    ss << "    android:layout_width=\"match_parent\"\n";
    ss << "    android:layout_height=\"match_parent\">\n\n";
    ss << "    <LinearLayout\n";
    ss << "        android:layout_width=\"match_parent\"\n";
    ss << "        android:layout_height=\"wrap_content\"\n";
    ss << "        android:orientation=\"vertical\"\n";
    ss << "        android:padding=\"16dp\">\n\n";

    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        <com.google.android.material.textfield.TextInputLayout\n";
            ss << "            android:layout_width=\"match_parent\"\n";
            ss << "            android:layout_height=\"wrap_content\"\n";
            ss << "            android:hint=\"" << col.name << "\">\n";
            ss << "            <com.google.android.material.textfield.TextInputEditText\n";
            ss << "                android:id=\"@+id/edit_" << col.field_name() << "\"\n";
            ss << "                android:layout_width=\"match_parent\"\n";
            ss << "                android:layout_height=\"wrap_content\"\n";
            ss << "                android:inputType=\"text\" />\n";
            ss << "        </com.google.android.material.textfield.TextInputLayout>\n\n";
        }
    }

    ss << "        <ProgressBar\n";
    ss << "            android:id=\"@+id/progress_bar\"\n";
    ss << "            android:layout_width=\"wrap_content\"\n";
    ss << "            android:layout_height=\"wrap_content\"\n";
    ss << "            android:layout_gravity=\"center\"\n";
    ss << "            android:visibility=\"gone\" />\n\n";

    ss << "        <Button\n";
    ss << "            android:id=\"@+id/btn_save\"\n";
    ss << "            android:layout_width=\"match_parent\"\n";
    ss << "            android:layout_height=\"wrap_content\"\n";
    ss << "            android:text=\"@string/save\"\n";
    ss << "            android:layout_marginTop=\"16dp\" />\n\n";

    ss << "        <Button\n";
    ss << "            android:id=\"@+id/btn_delete\"\n";
    ss << "            android:layout_width=\"match_parent\"\n";
    ss << "            android:layout_height=\"wrap_content\"\n";
    ss << "            android:text=\"@string/delete\"\n";
    ss << "            android:layout_marginTop=\"8dp\"\n";
    ss << "            android:backgroundTint=\"@color/error\" />\n\n";

    ss << "    </LinearLayout>\n\n";
    ss << "</ScrollView>\n";
    return ss.str();
}

String Generator::generate_android_menu_xml(const Table& table) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<menu xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    ss << "    xmlns:app=\"http://schemas.android.com/apk/res-auto\">\n";
    ss << "    <item\n";
    ss << "        android:id=\"@+id/action_" << table.snake_name() << "\"\n";
    ss << "        android:title=\"" << table.class_name() << "\"\n";
    ss << "        app:showAsAction=\"never\" />\n";
    ss << "</menu>\n";
    return ss.str();
}

String Generator::generate_android_gradle_wrapper_properties() {
    std::stringstream ss;
    ss << "distributionBase=GRADLE_USER_HOME\n";
    ss << "distributionPath=wrapper/dists\n";
    ss << "distributionUrl=https\\://services.gradle.org/distributions/gradle-8.2-bin.zip\n";
    ss << "zipStoreBase=GRADLE_USER_HOME\n";
    ss << "zipStorePath=wrapper/dists\n";
    return ss.str();
}

String Generator::generate_android_gradlew() {
    std::stringstream ss;
    ss << R"(#!/bin/sh

##############################################################################
#
#   Gradle start up script for POSIX generated by Gradle.
#
##############################################################################

APP_NAME="Gradle"
APP_BASE_NAME=`basename "$0"`

# Add default JVM options here.
DEFAULT_JVM_OPTS='"-Xmx64m" "-Xms64m"'

# Use the maximum available, or set MAX_FD != -1 to use that value.
MAX_FD="maximum"

warn () {
    echo "$*"
}

die () {
    echo
    echo "$*"
    echo
    exit 1
}

# OS specific support (must be 'true' or 'false').
cygwin=false
msys=false
darwin=false
nonstop=false
case "`uname`" in
  CYGWIN* )
    cygwin=true
    ;;
  Darwin* )
    darwin=true
    ;;
  MSYS* | MINGW* )
    msys=true
    ;;
  NONSTOP* )
    nonstop=true
    ;;
esac

CLASSPATH=$APP_HOME/gradle/wrapper/gradle-wrapper.jar

# Determine the Java command to use to start the JVM.
if [ -n "$JAVA_HOME" ] ; then
    if [ -x "$JAVA_HOME/jre/sh/java" ] ; then
        JAVACMD="$JAVA_HOME/jre/sh/java"
    else
        JAVACMD="$JAVA_HOME/bin/java"
    fi
    if [ ! -x "$JAVACMD" ] ; then
        die "ERROR: JAVA_HOME is set to an invalid directory: $JAVA_HOME"
    fi
else
    JAVACMD="java"
    which java >/dev/null 2>&1 || die "ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH."
fi

# Resolve links: $0 may be a link
app_path=$0

# Need this for daisy-chained symlinks.
while
    APP_HOME=${app_path%"${app_path##*/}"}  # leaves a trailing /; empty if no leading path
    [ -h "$app_path" ]
do
    ls=` ls -ld "$app_path" `
    link=${ls#*' -> '}
    case $link in             #
      /*)   app_path=$link ;; #
      *)    app_path=$APP_HOME$link ;;
    esac
done

APP_HOME=`cd "$APP_HOME" && pwd -P` || exit

# Collect all arguments for the java command
set -- \
        "-Dorg.gradle.appname=$APP_BASE_NAME" \
        -classpath "$CLASSPATH" \
        org.gradle.wrapper.GradleWrapperMain \
        "$@"

# Stop when "xargs" is not available.
if ! command -v xargs >/dev/null 2>&1
then
    die "xargs is not available"
fi

exec "$JAVACMD" "$@"
)";
    return ss.str();
}

String Generator::generate_android_gradlew_bat() {
    std::stringstream ss;
    ss << R"(@rem
@rem Copyright 2015 the original author or authors.
@rem
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem      https://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.
@rem

@if "%DEBUG%"=="" @echo off
@rem ##########################################################################
@rem
@rem  Gradle startup script for Windows
@rem
@rem ##########################################################################

@rem Set local scope for the variables with windows NT shell
if "%OS%"=="Windows_NT" setlocal

set DIRNAME=%~dp0
if "%DIRNAME%"=="" set DIRNAME=.
@rem This is normally unused
set APP_BASE_NAME=%~n0
set APP_HOME=%DIRNAME%

@rem Resolve any "." and ".." in APP_HOME to make it shorter.
for %%i in ("%APP_HOME%") do set APP_HOME=%%~fi

@rem Add default JVM options here.
set DEFAULT_JVM_OPTS="-Xmx64m" "-Xms64m"

@rem Find java.exe
if defined JAVA_HOME goto findJavaFromJavaHome

set JAVA_EXE=java.exe
%JAVA_EXE% -version >NUL 2>&1
if %ERRORLEVEL% equ 0 goto execute

echo.
echo ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:findJavaFromJavaHome
set JAVA_HOME=%JAVA_HOME:"=%
set JAVA_EXE=%JAVA_HOME%/bin/java.exe

if exist "%JAVA_EXE%" goto execute

echo.
echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:execute
@rem Setup the command line
set CLASSPATH=%APP_HOME%\gradle\wrapper\gradle-wrapper.jar

@rem Execute Gradle
"%JAVA_EXE%" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %GRADLE_OPTS% "-Dorg.gradle.appname=%APP_BASE_NAME%" -classpath "%CLASSPATH%" org.gradle.wrapper.GradleWrapperMain %*

:end
@rem End local scope for the variables with windows NT shell
if %ERRORLEVEL% equ 0 goto mainEnd

:fail
rem Set variable GRADLE_EXIT_CONSOLE if you need the _script_ return code instead of
rem the _cmd.exe /c_ return code!
set EXIT_CODE=%ERRORLEVEL%
if %EXIT_CODE% equ 0 set EXIT_CODE=1
if not ""=="%GRADLE_EXIT_CONSOLE%" exit %EXIT_CODE%
exit /b %EXIT_CODE%

:mainEnd
if "%OS%"=="Windows_NT" endlocal

:omega
)";
    return ss.str();
}

// ============================================================================
// Java Android Generators
// ============================================================================

String Generator::generate_android_app_build_gradle_java(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << R"(plugins {
    id 'com.android.application'
}

android {
    namespace ')" << namespace_ << R"('
    compileSdk 34

    defaultConfig {
        applicationId ")" << namespace_ << R"(.restclient"
        minSdk 24
        targetSdk 34
        versionCode 1
        versionName "1.0"

        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
    compileOptions {
        sourceCompatibility JavaVersion.VERSION_17
        targetCompatibility JavaVersion.VERSION_17
    }
    buildFeatures {
        viewBinding true
    }
}

dependencies {
    // AndroidX Core
    implementation 'androidx.core:core:1.12.0'
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'com.google.android.material:material:1.11.0'
    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'

    // Retrofit for REST API
    implementation 'com.squareup.retrofit2:retrofit:2.9.0'
    implementation 'com.squareup.retrofit2:converter-gson:2.9.0'
    implementation 'com.squareup.retrofit2:adapter-rxjava2:2.9.0'

    // OkHttp
    implementation 'com.squareup.okhttp3:okhttp:4.12.0'
    implementation 'com.squareup.okhttp3:logging-interceptor:4.12.0'

    // RxJava
    implementation 'io.reactivex.rxjava2:rxjava:2.2.21'
    implementation 'io.reactivex.rxjava2:rxandroid:2.1.1'

    // Gson
    implementation 'com.google.code.gson:gson:2.10.1'

    // Lifecycle & ViewModel
    implementation 'androidx.lifecycle:lifecycle-viewmodel:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-livedata:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-runtime:2.7.0'

    // Activity & Fragment
    implementation 'androidx.activity:activity:1.8.2'
    implementation 'androidx.fragment:fragment:1.6.2'

    // RecyclerView
    implementation 'androidx.recyclerview:recyclerview:1.3.2'

    // SwipeRefreshLayout
    implementation 'androidx.swiperefreshlayout:swiperefreshlayout:1.1.0'

    // Testing
    testImplementation 'junit:junit:4.13.2'
    androidTestImplementation 'androidx.test.ext:junit:1.1.5'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.5.1'
}
)";
    return ss.str();
}

String Generator::generate_android_application_class_java(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "package " << namespace_ << ";\n\n";
    ss << "import android.app.Application;\n";
    ss << "import " << namespace_ << ".data.api.ApiClient;\n\n";
    ss << "public class RestClientApp extends Application {\n\n";
    ss << "    private static RestClientApp instance;\n";
    ss << "    private ApiClient apiClient;\n\n";
    ss << "    @Override\n";
    ss << "    public void onCreate() {\n";
    ss << "        super.onCreate();\n";
    ss << "        instance = this;\n";
    ss << "        apiClient = ApiClient.getInstance();\n";
    ss << "    }\n\n";
    ss << "    public static RestClientApp getInstance() {\n";
    ss << "        return instance;\n";
    ss << "    }\n\n";
    ss << "    public ApiClient getApiClient() {\n";
    ss << "        return apiClient;\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_api_service_java(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.api;\n\n";
    ss << "import java.util.List;\n";
    ss << "import java.util.Map;\n";
    ss << "import " << namespace_ << ".data.model.ApiInfo;\n";
    for (const auto& table : tables) {
        ss << "import " << namespace_ << ".data.model." << table.class_name() << ";\n";
        ss << "import " << namespace_ << ".data.model." << table.class_name() << "Request;\n";
    }
    ss << "import io.reactivex.Completable;\n";
    ss << "import io.reactivex.Observable;\n";
    ss << "import retrofit2.http.*;\n\n";

    ss << "public interface ApiService {\n";
    for (const auto& table : tables) {
        String entityName = table.class_name();
        String kebabName = table.kebab_name();
        String camelName = table.instance_name();
        String idType = android_pk_java_type(table);

        ss << "\n    // " << entityName << " endpoints\n";
        ss << "    @GET(\"api/" << kebabName << "\")\n";
        ss << "    Observable<List<" << entityName << ">> get" << entityName << "List();\n\n";
        ss << "    @GET(\"api/" << kebabName << "/{id}\")\n";
        ss << "    Observable<" << entityName << "> get" << entityName << "(@Path(\"id\") " << idType << " id);\n\n";
        ss << "    @POST(\"api/" << kebabName << "\")\n";
        ss << "    Observable<" << entityName << "> create" << entityName << "(@Body " << entityName << "Request " << camelName << ");\n\n";
        ss << "    @PUT(\"api/" << kebabName << "/{id}\")\n";
        ss << "    Observable<" << entityName << "> update" << entityName << "(@Path(\"id\") " << idType << " id, @Body " << entityName << "Request " << camelName << ");\n\n";
        ss << "    @DELETE(\"api/" << kebabName << "/{id}\")\n";
        ss << "    Completable delete" << entityName << "(@Path(\"id\") " << idType << " id);\n";
    }
    ss << "\n    @GET(\"api\")\n";
    ss << "    Observable<ApiInfo> getApiInfo();\n\n";
    ss << "    @GET(\"health\")\n";
    ss << "    Observable<Map<String, String>> healthCheck();\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_api_client_java(const std::vector<Table>& tables) {
    (void)tables;
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.api;\n\n";
    ss << "import java.util.concurrent.TimeUnit;\n";
    ss << "import okhttp3.OkHttpClient;\n";
    ss << "import okhttp3.logging.HttpLoggingInterceptor;\n";
    ss << "import retrofit2.Retrofit;\n";
    ss << "import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;\n";
    ss << "import retrofit2.converter.gson.GsonConverterFactory;\n\n";
    ss << "public class ApiClient {\n\n";
    ss << "    private static final String BASE_URL = \"http://10.0.2.2:8080/\";\n";
    ss << "    private static ApiClient instance;\n";
    ss << "    private final ApiService apiService;\n\n";
    ss << "    private ApiClient() {\n";
    ss << "        HttpLoggingInterceptor loggingInterceptor = new HttpLoggingInterceptor();\n";
    ss << "        loggingInterceptor.setLevel(HttpLoggingInterceptor.Level.BODY);\n\n";
    ss << "        OkHttpClient okHttpClient = new OkHttpClient.Builder()\n";
    ss << "            .addInterceptor(loggingInterceptor)\n";
    ss << "            .connectTimeout(30, TimeUnit.SECONDS)\n";
    ss << "            .readTimeout(30, TimeUnit.SECONDS)\n";
    ss << "            .writeTimeout(30, TimeUnit.SECONDS)\n";
    ss << "            .build();\n\n";
    ss << "        Retrofit retrofit = new Retrofit.Builder()\n";
    ss << "            .baseUrl(BASE_URL)\n";
    ss << "            .client(okHttpClient)\n";
    ss << "            .addConverterFactory(GsonConverterFactory.create())\n";
    ss << "            .addCallAdapterFactory(RxJava2CallAdapterFactory.create())\n";
    ss << "            .build();\n\n";
    ss << "        apiService = retrofit.create(ApiService.class);\n";
    ss << "    }\n\n";
    ss << "    public static synchronized ApiClient getInstance() {\n";
    ss << "        if (instance == null) {\n";
    ss << "            instance = new ApiClient();\n";
    ss << "        }\n";
    ss << "        return instance;\n";
    ss << "    }\n\n";
    ss << "    public ApiService getApiService() {\n";
    ss << "        return apiService;\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_dto_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();
    ss << "package " << namespace_ << ".data.model;\n\n";
    ss << "import com.google.gson.annotations.SerializedName;\n\n";
    ss << "public class " << entityName << " {\n\n";
    for (const auto& col : table.columns) {
        ss << "    @SerializedName(\"" << col.field_name() << "\")\n";
        ss << "    private " << android_java_type(col) << " " << to_camel(col.field_name()) << ";\n\n";
    }
    for (const auto& col : table.columns) {
        String cap = to_pascal(col.field_name());
        String f = to_camel(col.field_name());
        ss << "    public " << android_java_type(col) << " get" << cap << "() {\n";
        ss << "        return " << f << ";\n";
        ss << "    }\n\n";
        ss << "    public void set" << cap << "(" << android_java_type(col) << " value) {\n";
        ss << "        this." << f << " = value;\n";
        ss << "    }\n\n";
    }
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_request_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();
    ss << "package " << namespace_ << ".data.model;\n\n";
    ss << "import com.google.gson.annotations.SerializedName;\n\n";
    ss << "public class " << entityName << "Request {\n\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) continue;
        ss << "    @SerializedName(\"" << col.field_name() << "\")\n";
        ss << "    private " << android_java_type(col) << " " << to_camel(col.field_name()) << ";\n\n";
    }
    for (const auto& col : table.columns) {
        if (col.primary_key) continue;
        String cap = to_pascal(col.field_name());
        String f = to_camel(col.field_name());
        ss << "    public " << android_java_type(col) << " get" << cap << "() {\n";
        ss << "        return " << f << ";\n";
        ss << "    }\n\n";
        ss << "    public void set" << cap << "(" << android_java_type(col) << " value) {\n";
        ss << "        this." << f << " = value;\n";
        ss << "    }\n\n";
    }
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_apiinfo_kt() {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.model;\n\n";
    ss << "import com.google.gson.annotations.SerializedName;\n\n";
    ss << "data class ApiInfo(\n";
    ss << "    @SerializedName(\"name\")\n";
    ss << "    var name: String = \"\",\n";
    ss << "    @SerializedName(\"version\")\n";
    ss << "    var version: String = \"\",\n";
    ss << "    @SerializedName(\"endpoints\")\n";
    ss << "    var endpoints: List<Any> = emptyList()\n";
    ss << ")\n";
    return ss.str();
}

String Generator::generate_android_apiinfo_java() {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.model;\n\n";
    ss << "import com.google.gson.annotations.SerializedName;\n";
    ss << "import java.util.List;\n\n";
    ss << "public class ApiInfo {\n\n";
    ss << "    @SerializedName(\"name\")\n";
    ss << "    private String name;\n\n";
    ss << "    @SerializedName(\"version\")\n";
    ss << "    private String version;\n\n";
    ss << "    @SerializedName(\"endpoints\")\n";
    ss << "    private List<Object> endpoints;\n\n";
    ss << "    public String getName() {\n";
    ss << "        return name;\n";
    ss << "    }\n\n";
    ss << "    public void setName(String value) {\n";
    ss << "        this.name = value;\n";
    ss << "    }\n\n";
    ss << "    public String getVersion() {\n";
    ss << "        return version;\n";
    ss << "    }\n\n";
    ss << "    public void setVersion(String value) {\n";
    ss << "        this.version = value;\n";
    ss << "    }\n\n";
    ss << "    public List<Object> getEndpoints() {\n";
    ss << "        return endpoints;\n";
    ss << "    }\n\n";
    ss << "    public void setEndpoints(List<Object> value) {\n";
    ss << "        this.endpoints = value;\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_repository_java(const std::vector<Table>& tables) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".data.repository;\n\n";
    ss << "import java.util.List;\n";
    ss << "import java.util.Map;\n";
    ss << "import " << namespace_ << ".RestClientApp;\n";
    ss << "import " << namespace_ << ".data.api.ApiService;\n";
    ss << "import " << namespace_ << ".data.model.ApiInfo;\n";
    for (const auto& table : tables) {
        ss << "import " << namespace_ << ".data.model." << table.class_name() << ";\n";
        ss << "import " << namespace_ << ".data.model." << table.class_name() << "Request;\n";
    }
    ss << "import io.reactivex.Completable;\n";
    ss << "import io.reactivex.Observable;\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers;\n";
    ss << "import io.reactivex.schedulers.Schedulers;\n\n";

    ss << "public class EntityRepository {\n\n";
    ss << "    private final ApiService apiService = RestClientApp.getInstance().getApiClient().getApiService();\n\n";

    for (const auto& table : tables) {
        String entityName = table.class_name();
        String camelName = table.instance_name();
        String idType = android_pk_java_type(table);

        ss << "    public Observable<List<" << entityName << ">> get" << entityName << "List() {\n";
        ss << "        return apiService.get" << entityName << "List()\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
        ss << "    }\n\n";

        ss << "    public Observable<" << entityName << "> get" << entityName << "(" << idType << " id) {\n";
        ss << "        return apiService.get" << entityName << "(id)\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
        ss << "    }\n\n";

        ss << "    public Observable<" << entityName << "> create" << entityName << "(" << entityName << "Request " << camelName << ") {\n";
        ss << "        return apiService.create" << entityName << "(" << camelName << ")\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
        ss << "    }\n\n";

        ss << "    public Observable<" << entityName << "> update" << entityName << "(" << idType << " id, " << entityName << "Request " << camelName << ") {\n";
        ss << "        return apiService.update" << entityName << "(id, " << camelName << ")\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
        ss << "    }\n\n";

        ss << "    public Completable delete" << entityName << "(" << idType << " id) {\n";
        ss << "        return apiService.delete" << entityName << "(id)\n";
        ss << "            .subscribeOn(Schedulers.io())\n";
        ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
        ss << "    }\n\n";
    }

    ss << "    public Observable<Map<String, String>> healthCheck() {\n";
    ss << "        return apiService.healthCheck()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n\n";

    ss << "    public Observable<ApiInfo> getApiInfo() {\n";
    ss << "        return apiService.getApiInfo()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n";

    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_viewmodel_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();
    String idType = android_pk_java_type(table);

    ss << "package " << namespace_ << ".ui.viewmodel;\n\n";
    ss << "import java.util.List;\n";
    ss << "import androidx.lifecycle.ViewModel;\n";
    ss << "import " << namespace_ << ".data.model." << entityName << ";\n";
    ss << "import " << namespace_ << ".data.model." << entityName << "Request;\n";
    ss << "import " << namespace_ << ".data.repository.EntityRepository;\n";
    ss << "import io.reactivex.Completable;\n";
    ss << "import io.reactivex.Observable;\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers;\n";
    ss << "import io.reactivex.schedulers.Schedulers;\n\n";

    ss << "public class " << entityName << "ViewModel extends ViewModel {\n\n";
    ss << "    private final EntityRepository repository = new EntityRepository();\n\n";

    ss << "    public Observable<List<" << entityName << ">> getList() {\n";
    ss << "        return repository.get" << entityName << "List()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n\n";

    ss << "    public Observable<" << entityName << "> get(" << idType << " id) {\n";
    ss << "        return repository.get" << entityName << "(id)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n\n";

    ss << "    public Observable<" << entityName << "> create(" << entityName << "Request request) {\n";
    ss << "        return repository.create" << entityName << "(request)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n\n";

    ss << "    public Observable<" << entityName << "> update(" << idType << " id, " << entityName << "Request request) {\n";
    ss << "        return repository.update" << entityName << "(id, request)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n\n";

    ss << "    public Completable delete(" << idType << " id) {\n";
    ss << "        return repository.delete" << entityName << "(id)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread());\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_adapter_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();

    ss << "package " << namespace_ << ".ui.adapter;\n\n";
    ss << "import android.view.LayoutInflater;\n";
    ss << "import android.view.View;\n";
    ss << "import android.view.ViewGroup;\n";
    ss << "import android.widget.TextView;\n";
    ss << "import androidx.annotation.NonNull;\n";
    ss << "import androidx.recyclerview.widget.RecyclerView;\n";
    ss << "import " << namespace_ << ".R;\n";
    ss << "import " << namespace_ << ".data.model." << entityName << ";\n";
    ss << "import java.util.ArrayList;\n";
    ss << "import java.util.List;\n\n";

    ss << "public class " << entityName << "Adapter extends RecyclerView.Adapter<" << entityName << "Adapter.ViewHolder> {\n\n";
    ss << "    public interface OnItemClickListener {\n";
    ss << "        void onItemClick(" << entityName << " item);\n";
    ss << "    }\n\n";
    ss << "    private final List<" << entityName << "> items = new ArrayList<>();\n";
    ss << "    private final OnItemClickListener listener;\n\n";
    ss << "    public " << entityName << "Adapter(OnItemClickListener listener) {\n";
    ss << "        this.listener = listener;\n";
    ss << "    }\n\n";

    ss << "    @NonNull\n";
    ss << "    @Override\n";
    ss << "    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {\n";
    ss << "        View view = LayoutInflater.from(parent.getContext())\n";
    ss << "            .inflate(R.layout.item_" << table.snake_name() << ", parent, false);\n";
    ss << "        return new ViewHolder(view);\n";
    ss << "    }\n\n";

    ss << "    @Override\n";
    ss << "    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {\n";
    ss << "        final " << entityName << " item = items.get(position);\n";
    ss << "        holder.bind(item);\n";
    ss << "        holder.itemView.setOnClickListener(v -> {\n";
    ss << "            if (listener != null) {\n";
    ss << "                listener.onItemClick(item);\n";
    ss << "            }\n";
    ss << "        });\n";
    ss << "    }\n\n";

    ss << "    @Override\n";
    ss << "    public int getItemCount() {\n";
    ss << "        return items.size();\n";
    ss << "    }\n\n";

    ss << "    public void updateItems(List<" << entityName << "> newItems) {\n";
    ss << "        items.clear();\n";
    ss << "        items.addAll(newItems);\n";
    ss << "        notifyDataSetChanged();\n";
    ss << "    }\n\n";

    ss << "    static class ViewHolder extends RecyclerView.ViewHolder {\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "        private final TextView text" << to_pascal(col.field_name()) << ";\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key && table.columns.size() > 1) {
            ss << "        private final TextView text" << to_pascal(col.field_name()) << ";\n";
        }
    }
    ss << "\n        ViewHolder(@NonNull View itemView) {\n";
    ss << "            super(itemView);\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "            text" << to_pascal(col.field_name()) << " = itemView.findViewById(R.id.text_" << col.field_name() << ");\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key && table.columns.size() > 1) {
            ss << "            text" << to_pascal(col.field_name()) << " = itemView.findViewById(R.id.text_" << col.field_name() << ");\n";
        }
    }
    ss << "        }\n\n";
    ss << "        void bind(" << entityName << " item) {\n";
    for (const auto& col : table.columns) {
        if (col.primary_key) {
            ss << "            text" << to_pascal(col.field_name()) << ".setText(" << android_java_to_text_expr("item.get" + to_pascal(col.field_name()) + "()") << ");\n";
        }
    }
    for (const auto& col : table.columns) {
        if (!col.primary_key && table.columns.size() > 1) {
            ss << "            text" << to_pascal(col.field_name()) << ".setText(" << android_java_to_text_expr("item.get" + to_pascal(col.field_name()) + "()") << ");\n";
        }
    }
    ss << "        }\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_activity_java(const Table& table) {
    std::stringstream ss;
    ss << "package " << namespace_ << ".ui;\n\n";
    ss << "import android.content.Intent;\n";
    ss << "import android.os.Bundle;\n";
    ss << "import android.view.Menu;\n";
    ss << "import android.view.MenuItem;\n";
    ss << "import android.widget.Toast;\n";
    ss << "import androidx.appcompat.app.AppCompatActivity;\n";
    ss << "import " << namespace_ << ".R;\n";
    ss << "import " << namespace_ << ".data.repository.EntityRepository;\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers;\n";
    ss << "import io.reactivex.schedulers.Schedulers;\n";
    ss << "import java.util.Map;\n\n";

    ss << "public class MainActivity extends AppCompatActivity {\n\n";
    ss << "    private EntityRepository repository;\n\n";
    ss << "    @Override\n";
    ss << "    protected void onCreate(Bundle savedInstanceState) {\n";
    ss << "        super.onCreate(savedInstanceState);\n";
    ss << "        setContentView(R.layout.activity_main);\n\n";
    ss << "        repository = new EntityRepository();\n";
    ss << "        checkApiHealth();\n";
    ss << "    }\n\n";

    ss << "    private void checkApiHealth() {\n";
    ss << "        repository.healthCheck()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(result -> {\n";
    ss << "                String status = result.get(\"status\");\n";
    ss << "                Toast.makeText(this, \"API Connected: \" + status, Toast.LENGTH_SHORT).show();\n";
    ss << "            }, error ->\n";
    ss << "                Toast.makeText(this, \"API Error: \" + error.getMessage(), Toast.LENGTH_LONG).show()\n";
    ss << "            );\n";
    ss << "    }\n\n";

    ss << "    @Override\n";
    ss << "    public boolean onCreateOptionsMenu(Menu menu) {\n";
    ss << "        getMenuInflater().inflate(R.menu.main_menu, menu);\n";
    ss << "        return true;\n";
    ss << "    }\n\n";

    ss << "    @Override\n";
    ss << "    public boolean onOptionsItemSelected(MenuItem item) {\n";
    ss << "        if (item.getItemId() == R.id.action_" << table.snake_name() << ") {\n";
    ss << "            startActivity(new Intent(this, EntityListActivity.class));\n";
    ss << "            return true;\n";
    ss << "        }\n";
    ss << "        return super.onOptionsItemSelected(item);\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_list_activity_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();
    String idType = android_pk_java_type(table);
    const Column* pk = table.pk_column();
    String pkGetter = pk ? ("get" + to_pascal(pk->field_name()) + "()") : "toString()";

    ss << "package " << namespace_ << ".ui;\n\n";
    ss << "import android.content.Intent;\n";
    ss << "import android.os.Bundle;\n";
    ss << "import android.view.View;\n";
    ss << "import android.widget.Toast;\n";
    ss << "import androidx.appcompat.app.AppCompatActivity;\n";
    ss << "import androidx.recyclerview.widget.LinearLayoutManager;\n";
    ss << "import androidx.recyclerview.widget.RecyclerView;\n";
    ss << "import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;\n";
    ss << "import " << namespace_ << ".R;\n";
    ss << "import " << namespace_ << ".data.model." << entityName << ";\n";
    ss << "import " << namespace_ << ".data.repository.EntityRepository;\n";
    ss << "import " << namespace_ << ".ui.adapter." << entityName << "Adapter;\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers;\n";
    ss << "import io.reactivex.schedulers.Schedulers;\n\n";

    ss << "public class EntityListActivity extends AppCompatActivity {\n\n";
    ss << "    private " << entityName << "Adapter adapter;\n";
    ss << "    private final EntityRepository repository = new EntityRepository();\n\n";

    ss << "    @Override\n";
    ss << "    protected void onCreate(Bundle savedInstanceState) {\n";
    ss << "        super.onCreate(savedInstanceState);\n";
    ss << "        setContentView(R.layout.activity_entity_list);\n\n";
    ss << "        if (getSupportActionBar() != null) {\n";
    ss << "            getSupportActionBar().setTitle(\"" << entityName << " List\");\n";
    ss << "        }\n\n";
    ss << "        setupRecyclerView();\n";
    ss << "        loadData();\n\n";
    ss << "        findViewById(R.id.fab_add).setOnClickListener(v ->\n";
    ss << "            startActivity(new Intent(this, EntityDetailActivity.class))\n";
    ss << "        );\n\n";
    ss << "        SwipeRefreshLayout swipeRefresh = findViewById(R.id.swipe_refresh);\n";
    ss << "        swipeRefresh.setOnRefreshListener(this::loadData);\n";
    ss << "    }\n\n";

    ss << "    private void setupRecyclerView() {\n";
    ss << "        adapter = new " << entityName << "Adapter(item -> {\n";
    ss << "            Intent intent = new Intent(this, EntityDetailActivity.class);\n";
    ss << "            intent.putExtra(\"entity_id\", item." << pkGetter << ");\n";
    ss << "            startActivity(intent);\n";
    ss << "        });\n";
    ss << "        RecyclerView recyclerView = findViewById(R.id.recycler_view);\n";
    ss << "        recyclerView.setAdapter(adapter);\n";
    ss << "        recyclerView.setLayoutManager(new LinearLayoutManager(this));\n";
    ss << "    }\n\n";

    ss << "    private void loadData() {\n";
    ss << "        repository.get" << entityName << "List()\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(items -> {\n";
    ss << "                adapter.updateItems(items);\n";
    ss << "                SwipeRefreshLayout swipeRefresh = findViewById(R.id.swipe_refresh);\n";
    ss << "                swipeRefresh.setRefreshing(false);\n";
    ss << "                findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    ss << "                findViewById(R.id.text_empty).setVisibility(items.isEmpty() ? View.VISIBLE : View.GONE);\n";
    ss << "            }, error -> {\n";
    ss << "                Toast.makeText(this, \"Error: \" + error.getMessage(), Toast.LENGTH_LONG).show();\n";
    ss << "                SwipeRefreshLayout swipeRefresh = findViewById(R.id.swipe_refresh);\n";
    ss << "                swipeRefresh.setRefreshing(false);\n";
    ss << "                findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    ss << "            });\n";
    ss << "    }\n\n";

    ss << "    @Override\n";
    ss << "    protected void onResume() {\n";
    ss << "        super.onResume();\n";
    ss << "        loadData();\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

String Generator::generate_android_detail_activity_java(const Table& table) {
    std::stringstream ss;
    String entityName = table.class_name();
    String idType = android_pk_java_type(table);
    bool stringPk = (idType == "String");

    ss << "package " << namespace_ << ".ui;\n\n";
    ss << "import android.os.Bundle;\n";
    ss << "import android.view.View;\n";
    ss << "import android.widget.EditText;\n";
    ss << "import android.widget.Toast;\n";
    ss << "import androidx.appcompat.app.AlertDialog;\n";
    ss << "import androidx.appcompat.app.AppCompatActivity;\n";
    ss << "import " << namespace_ << ".R;\n";
    ss << "import " << namespace_ << ".data.model." << entityName << "Request;\n";
    ss << "import " << namespace_ << ".data.repository.EntityRepository;\n";
    ss << "import io.reactivex.android.schedulers.AndroidSchedulers;\n";
    ss << "import io.reactivex.schedulers.Schedulers;\n\n";

    ss << "public class EntityDetailActivity extends AppCompatActivity {\n\n";
    ss << "    private final EntityRepository repository = new EntityRepository();\n";
    if (stringPk) {
        ss << "    private String entityId;\n";
    } else {
        ss << "    private " << idType << " entityId = ";
        ss << (idType == "Long" ? "-1L" : "-1") << ";\n";
    }
    ss << "    private boolean isEditMode = false;\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "    private EditText edit" << to_pascal(col.field_name()) << ";\n";
        }
    }
    ss << "\n    @Override\n";
    ss << "    protected void onCreate(Bundle savedInstanceState) {\n";
    ss << "        super.onCreate(savedInstanceState);\n";
    ss << "        setContentView(R.layout.activity_entity_detail);\n\n";
    if (stringPk) {
        ss << "        entityId = getIntent().getStringExtra(\"entity_id\");\n";
        ss << "        isEditMode = entityId != null;\n";
    } else {
        String cast = "";
        if (idType == "Integer") cast = "(int) ";
        else if (idType == "Short") cast = "(short) ";
        else if (idType == "Byte") cast = "(byte) ";
        ss << "        entityId = " << cast << "getIntent().getLongExtra(\"entity_id\", -1);\n";
        ss << "        isEditMode = entityId != ";
        ss << (idType == "Long" ? "-1L" : "-1") << ";\n";
    }
    ss << "\n        if (getSupportActionBar() != null) {\n";
    ss << "            getSupportActionBar().setTitle(isEditMode ? \"Edit " << entityName << "\" : \"Create " << entityName << "\");\n";
    ss << "        }\n\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        edit" << to_pascal(col.field_name()) << " = findViewById(R.id.edit_" << col.field_name() << ");\n";
        }
    }
    ss << "\n        if (isEditMode) {\n";
    ss << "            loadEntity();\n";
    ss << "        }\n\n";
    ss << "        findViewById(R.id.btn_save).setOnClickListener(v -> saveEntity());\n\n";
    ss << "        findViewById(R.id.btn_delete).setOnClickListener(v -> confirmDelete());\n\n";
    ss << "        findViewById(R.id.btn_delete).setVisibility(isEditMode ? View.VISIBLE : View.GONE);\n";
    ss << "    }\n\n";

    ss << "    private void loadEntity() {\n";
    ss << "        findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);\n";
    ss << "        repository.get" << entityName << "(entityId)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(entity -> {\n";
    ss << "                findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "                edit" << to_pascal(col.field_name()) << ".setText(" << android_java_to_text_expr("entity.get" + to_pascal(col.field_name()) + "()") << ");\n";
        }
    }
    ss << "            }, error -> {\n";
    ss << "                Toast.makeText(this, \"Error: \" + error.getMessage(), Toast.LENGTH_LONG).show();\n";
    ss << "                finish();\n";
    ss << "            });\n";
    ss << "    }\n\n";

    ss << "    private void saveEntity() {\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        String " << col.field_name() << " = edit" << to_pascal(col.field_name()) << ".getText().toString();\n";
        }
    }
    ss << "\n        " << entityName << "Request request = new " << entityName << "Request();\n";
    for (const auto& col : table.columns) {
        if (!col.primary_key) {
            ss << "        request.set" << to_pascal(col.field_name()) << "(" << android_java_parse_expr(col, col.field_name()) << ");\n";
        }
    }
    ss << "\n        findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);\n";
    ss << "        if (isEditMode) {\n";
    ss << "            repository.update" << entityName << "(entityId, request)\n";
    ss << "                .subscribeOn(Schedulers.io())\n";
    ss << "                .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "                .subscribe(\n";
    ss << "                    entity -> { Toast.makeText(this, \"Success\", Toast.LENGTH_SHORT).show(); finish(); },\n";
    ss << "                    error -> {\n";
    ss << "                        findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    ss << "                        Toast.makeText(this, \"Error: \" + error.getMessage(), Toast.LENGTH_LONG).show();\n";
    ss << "                    }\n";
    ss << "                );\n";
    ss << "        } else {\n";
    ss << "            repository.create" << entityName << "(request)\n";
    ss << "                .subscribeOn(Schedulers.io())\n";
    ss << "                .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "                .subscribe(\n";
    ss << "                    entity -> { Toast.makeText(this, \"Success\", Toast.LENGTH_SHORT).show(); finish(); },\n";
    ss << "                    error -> {\n";
    ss << "                        findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    ss << "                        Toast.makeText(this, \"Error: \" + error.getMessage(), Toast.LENGTH_LONG).show();\n";
    ss << "                    }\n";
    ss << "                );\n";
    ss << "        }\n";
    ss << "    }\n\n";

    ss << "    private void confirmDelete() {\n";
    ss << "        new AlertDialog.Builder(this)\n";
    ss << "            .setTitle(\"Delete\")\n";
    ss << "            .setMessage(\"Are you sure you want to delete this item?\")\n";
    ss << "            .setPositiveButton(\"Yes\", (dialog, which) -> deleteEntity())\n";
    ss << "            .setNegativeButton(\"No\", null)\n";
    ss << "            .show();\n";
    ss << "    }\n\n";

    ss << "    private void deleteEntity() {\n";
    ss << "        findViewById(R.id.progress_bar).setVisibility(View.VISIBLE);\n";
    ss << "        repository.delete" << entityName << "(entityId)\n";
    ss << "            .subscribeOn(Schedulers.io())\n";
    ss << "            .observeOn(AndroidSchedulers.mainThread())\n";
    ss << "            .subscribe(\n";
    ss << "                () -> { Toast.makeText(this, \"Deleted\", Toast.LENGTH_SHORT).show(); finish(); },\n";
    ss << "                error -> {\n";
    ss << "                    findViewById(R.id.progress_bar).setVisibility(View.GONE);\n";
    ss << "                    Toast.makeText(this, \"Error: \" + error.getMessage(), Toast.LENGTH_LONG).show();\n";
    ss << "                }\n";
    ss << "            );\n";
    ss << "    }\n";
    ss << "}\n";
    return ss.str();
}

} // namespace novagen
