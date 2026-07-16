cmake_minimum_required(VERSION 3.20)
project(fcitx5_vnkey VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTS "Build unit tests" ON)
option(BUILD_ADDON "Build fcitx5 addon (tat de build rieng core)" ON)

add_subdirectory(core)

if (BUILD_ADDON)
    find_package(Fcitx5Core REQUIRED)
    find_package(Fcitx5Utils REQUIRED)
    add_subdirectory(addon)
endif ()

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests/unit)
endif ()
