cmake_minimum_required(VERSION 3.18.4)

project(
        QBasic
        VERSION 0.1
        DESCRIPTION "Mini-Basic interpreter written with QT GUI"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
qt_standard_project_setup()

include(FetchContent)

# Catch2
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.1
)
FetchContent_MakeAvailable(Catch2)
include(CTest)
include(Catch)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
)

# SpdLog
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif ()

include_directories(include)
include_directories(ui)

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(ui)
add_subdirectory(test)