# =====================================================================
#               DMOD ENV CMake Configuration
# =====================================================================
cmake_minimum_required(VERSION 3.10)

# ======================================================================
#               DMOD ENV
# ======================================================================
project(dmenv
    VERSION 1.0
    DESCRIPTION "DMOD Environment Variables Manager"
    LANGUAGES C CXX)

set(DMENV_DONT_IMPLEMENT_DMOD_API OFF CACHE BOOL "Do not implement DMOD API in dmenv library")


# ======================================================================
#               Fetch DMOD repository
# ======================================================================
include(FetchContent)

# Only fetch and build dmod if it's not already available as a target
if(NOT TARGET dmod_inc)
    FetchContent_Declare(
        dmod
        GIT_REPOSITORY https://github.com/choco-technologies/dmod.git
        GIT_TAG        develop
    )

    # ======================================================================
    #               DMOD Configuration
    # ======================================================================
    set(DMOD_MODE "DMOD_SYSTEM" CACHE STRING "DMOD build mode")
    set(DMOD_BUILD_TESTS OFF CACHE BOOL "Build tests")
    set(DMOD_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
    set(DMOD_BUILD_TOOLS OFF CACHE BOOL "Build tools")
    set(DMOD_BUILD_TEMPLATES OFF CACHE BOOL "Build templates")
    
    # Pass coverage flags to DMOD if enabled
    if(ENABLE_COVERAGE)
        set(DMOD_ENABLE_COVERAGE ON CACHE BOOL "Enable DMOD coverage")
    endif()

    FetchContent_MakeAvailable(dmod)
    set(DMOD_DIR ${dmod_SOURCE_DIR} CACHE PATH "DMOD source directory" FORCE)
else()
    message(STATUS "dmod target already exists, skipping FetchContent")
endif()

# ======================================================================
#               DMOD ENV Library
# ======================================================================
set(MODULE_NAME dmenv)
add_library(${MODULE_NAME} STATIC
    src/dmenv.c
    src/dmenv_registrations.c
)

target_compile_definitions(${MODULE_NAME} 
    PRIVATE 
        $<$<BOOL:${DMENV_DONT_IMPLEMENT_DMOD_API}>:DMENV_DONT_IMPLEMENT_DMOD_API>
        DMENV_VERSION="${PROJECT_VERSION}"
)

target_include_directories(${MODULE_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)   

target_link_libraries(${MODULE_NAME} 
    PRIVATE    
        dmod_inc
    )

# Enable coverage for dmenv library if requested
if(ENABLE_COVERAGE)
    target_compile_options(${MODULE_NAME} PRIVATE --coverage)
    target_link_options(${MODULE_NAME} PRIVATE --coverage)
endif()

# Only run create_library_makefile if not in CI environment
# The memory_analysis.cmake script path has issues in FetchContent context
if(NOT DEFINED ENV{CI} AND COMMAND create_library_makefile)
    create_library_makefile(${MODULE_NAME})
endif()

# ======================================================================
#               Tests
# ======================================================================
option(DMENV_BUILD_TESTS "Build tests" OFF)

if(DMENV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
