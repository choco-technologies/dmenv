# =====================================================================
#               DMOD ENV Tests
# =====================================================================

# Find or fetch Unity test framework
include(FetchContent)

if(NOT TARGET unity)
    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG        v2.5.2
    )
    FetchContent_MakeAvailable(unity)
endif()

# Common test utilities
add_library(test_common INTERFACE)
target_include_directories(test_common INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(test_common INTERFACE dmod_inc)

# Common linker options for all tests
set(TEST_LINK_OPTIONS -L ${DMOD_DIR}/scripts -T ${DMOD_DIR}/scripts/main.ld)

# Test: Simple test
add_executable(test_simple test_simple.c)
target_link_libraries(test_simple PRIVATE dmenv unity test_common dmod_system)
target_link_options(test_simple PRIVATE ${TEST_LINK_OPTIONS})
add_test(NAME test_simple COMMAND test_simple)

# Test: Unit tests
add_executable(test_dmenv_unit test_dmenv_unit.c)
target_link_libraries(test_dmenv_unit PRIVATE dmenv unity test_common dmod_system)
target_link_options(test_dmenv_unit PRIVATE ${TEST_LINK_OPTIONS})
add_test(NAME test_dmenv_unit COMMAND test_dmenv_unit)

# Test: Minimal test
add_executable(test_minimal test_minimal.c)
target_link_libraries(test_minimal PRIVATE dmenv unity test_common dmod_system)
target_link_options(test_minimal PRIVATE ${TEST_LINK_OPTIONS})
add_test(NAME test_minimal COMMAND test_minimal)

# Enable coverage for test executables if requested
if(ENABLE_COVERAGE)
    target_compile_options(test_simple PRIVATE --coverage)
    target_link_options(test_simple PRIVATE --coverage)
    target_compile_options(test_dmenv_unit PRIVATE --coverage)
    target_link_options(test_dmenv_unit PRIVATE --coverage)
    target_compile_options(test_minimal PRIVATE --coverage)
    target_link_options(test_minimal PRIVATE --coverage)
endif()
