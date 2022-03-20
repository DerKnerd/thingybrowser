cmake_minimum_required(VERSION 3.16)
project(thingybrowser)

set(CMAKE_CXX_STANDARD 20)

include(./ucm.cmake)
include(./conan.cmake)

ucm_set_runtime(STATIC)
if (WIN32 OR MINGW)
    add_compile_definitions(__WXMSW__ _UNICODE)
    set(BUILD_SHARED_LIBS false)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CMAKE_WIN32_EXECUTABLE 1)
endif ()

set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
set(USE_AUI 0)
set(USE_RIBBON 0)
set(USE_RICHTEXT 0)
set(USE_XRC 0)
add_subdirectory(libs/wxWidgets)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_add_remote(NAME conan.imanuel.dev
        URL https://conan.imanuel.dev/artifactory/api/conan/conan-virtual)

conan_cmake_configure(
        REQUIRES
        nlohmann_json/3.10.5
        cpp-httplib/0.10.3
        openssl/3.0.1
        libthingy/0.11@libthingy/libthingy
        GENERATORS cmake_find_package
)

conan_cmake_autodetect(settings)

if (MINGW)
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings}
            PROFILE ${CMAKE_SOURCE_DIR}/linux_to_win64_conan)
else ()
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings}
            SETTINGS ${CONAN_SETTINGS})
endif ()

find_package(libthingy)

add_executable(thingybrowser main.cpp MainWindow.cpp MainApp.cpp)
target_link_libraries(thingybrowser PRIVATE wx::base wx::core libthingy::libthingy)
