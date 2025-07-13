cmake_minimum_required(VERSION 3.10)
project(LEAF)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify the Clang compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Add compiler warnings and optimizations
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# Specify the build type if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add executable
add_executable(LEAF main.cpp
        Framework/ModelLoader/ModelLoader.cpp
        Framework/headers/ModelLoader.h
        Framework/ModelSlicer/ModelSlicer.cpp
        Framework/headers/ModelSlicer.h
        Framework/headers/Logger.h
        Framework/Utils/Logger.cpp
        Framework/headers/VectorHash.h
        Framework/ModelSlicer/SliceDirectory.cpp
        Framework/headers/SliceDirectory.h
        Framework/headers/Constants.h
        Framework/headers/Slice.h
        Framework/headers/SliceHash.h
        Framework/DiskManager/DiskManager.cpp
        Framework/headers/DiskManager.h
        Framework/headers/SliceCache.h
        Framework/ModelLoader/SliceCache.cpp
        Framework/library/Math.h
        Framework/library/Math.cpp)

# Test executable
add_executable(LEAF_tests
        Tests/TestModelSlicer.cpp
        Framework/headers/Constants.h
        Framework/headers/Logger.h
        Framework/headers/ModelLoader.h
        Framework/headers/ModelSlicer.h
        Framework/headers/Slice.h
        Framework/headers/SliceDirectory.h
        Framework/headers/VectorHash.h
        Framework/ModelSlicer/ModelSlicer.cpp  # Include sources needed for tests
        Framework/Utils/Logger.cpp
)

target_link_libraries(LEAF PRIVATE spdlog::spdlog )
target_link_libraries(LEAF_tests PRIVATE spdlog::spdlog fmt::fmt GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME LEAF_tests COMMAND LEAF_tests)
