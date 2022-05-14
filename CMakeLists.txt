cmake_minimum_required(VERSION 3.16)
project(thingybrowser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(building/cmake/ucm.cmake)
include(conan.cmake)

ucm_set_runtime(STATIC)
if (WIN32 OR MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    add_compile_definitions(__WXMSW__ _UNICODE)
    set(BUILD_SHARED_LIBS false)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CMAKE_WIN32_EXECUTABLE 1)
endif ()

set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
set(wxUSE_ZLIB builtin)
set(wxUSE_EXPAT builtin)
set(wxUSE_LIBJPEG builtin)
set(wxUSE_LIBPNG builtin)
set(wxUSE_LIBTIFF builtin)
add_subdirectory(libs/wxWidgets)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_add_remote(NAME conan.imanuel.dev
        URL https://conan.imanuel.dev/artifactory/api/conan/conan-virtual)

conan_cmake_configure(
        REQUIRES
        libthingy/0.24@libthingy/libthingy
        cpp-httplib/0.10.6
        nlohmann_json/3.10.5
        boost/1.78.0
        zlib/1.2.12
        GENERATORS cmake_find_package
)

conan_cmake_autodetect(settings)

if (MINGW)
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings}
            PROFILE ${CMAKE_SOURCE_DIR}/building/conan/linux_to_win64_conan)
else ()
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings})
endif ()

find_package(libthingy)

add_executable(thingybrowser main.rc main.cpp MainWindow.cpp MainApp.cpp settings/thingyverseSettingsPage.cpp settings/thingyverseSettingsPagePanel.cpp pages/mainwindow/ThingsPage.cpp pages/mainwindow/ThingsPage.h helper.h pages/mainwindow/tbDataPage.h pages/mainwindow/tbButtonGridPage.cpp pages/mainwindow/tbButtonGridPage.h pages/mainwindow/CollectionsPage.cpp pages/mainwindow/CollectionsPage.h pages/mainwindow/DesignersPage.cpp pages/mainwindow/DesignersPage.h things/ThingsWindow.cpp things/ThingsWindow.h helpers/font.h helpers/tbNoLinkHtmlWindow.h)
include_directories(libs/libthingy)
if (MINGW)
    target_link_libraries(thingybrowser PRIVATE wx::base wx::core wx::html wx::net wx::richtext libthingy::libthingy httplib::httplib -static gcc stdc++ winpthread)
else ()
    target_link_libraries(thingybrowser PRIVATE wx::base wx::core wx::html wx::net wx::richtext libthingy::libthingy httplib::httplib)
endif ()
