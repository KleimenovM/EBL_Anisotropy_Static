cmake_minimum_required(VERSION 3.20)

project(EBL_Anisotropy_Static
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CATALOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/catalogs")
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output")


add_library(
    ebl_core

    src/galaxy/galaxyReader.cpp
    src/geometry/spherical.cpp
    src/geometry/vector3d.cpp
)


target_include_directories(
    ebl_core
    PUBLIC
    include
)


# Reader test
add_executable(
    test_biteau_reader
    tests/unit/test_biteau_reader.cpp
)

target_link_libraries(
    test_biteau_reader
    PRIVATE
    ebl_core
)

target_compile_definitions(
    test_biteau_reader
    PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/test_data"
)


# Full catalog histogram test
add_executable(
    test_biteau_histogram
    tests/integration/test_biteau_histogram.cpp
)

target_link_libraries(
    test_biteau_histogram
    PRIVATE
    ebl_core
)

target_compile_definitions(
    test_biteau_histogram
    PRIVATE
    CATALOG_DIR="${CATALOG_DIR}"
    OUTPUT_DIR="${OUTPUT_DIR}"
)