cmake_minimum_required(VERSION 3.28)
project(cask_cpp)

set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SPEC_DIR ${CMAKE_SOURCE_DIR}/spec)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(SPEC_LIB_DIR ${SPEC_DIR}/lib)

include_directories(${SRC_DIR})
include_directories(${SPEC_LIB_DIR})
include_directories(SYSTEM ${LIB_DIR})

include(FetchContent)

function(fetch URL DESTINATION)
    if (NOT EXISTS ${DESTINATION})
        message(STATUS "Downloading ${DESTINATION}...")
        file(DOWNLOAD ${URL} ${DESTINATION})
    endif ()
endfunction()

file(MAKE_DIRECTORY ${SPEC_LIB_DIR})
file(MAKE_DIRECTORY ${LIB_DIR})

fetch(
        "https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp"
        "${SPEC_LIB_DIR}/catch.hpp"
)

fetch(
        "https://raw.githubusercontent.com/s-ajensen/c2_bdd/refs/heads/master/c2_bdd.hpp"
        "${SPEC_LIB_DIR}/bdd.hpp"
)

if(NOT EXISTS ${LIB_DIR}/immer)
    message(STATUS "Installing immer...")
    FetchContent_Declare(
            immer
            GIT_REPOSITORY https://github.com/arximboldi/immer.git
            GIT_TAG v0.8.1
            GIT_SHALLOW TRUE
    )
    FetchContent_Populate(immer)
    file(COPY ${immer_SOURCE_DIR}/immer DESTINATION ${LIB_DIR})
endif()

file(GLOB SOURCES ${SRC_DIR}/*.cpp)
file(GLOB SPEC_SOURCES ${SPEC_DIR}/*_spec.cpp)

list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(cask ${SRC_DIR}/main.cpp ${SOURCES}
        src/core.hpp)

add_executable(cask_tests
        ${SPEC_DIR}/test.cpp
        ${SPEC_SOURCES}
        ${SOURCES}
        src/core.hpp
)

add_custom_target(test
        COMMAND cask_tests -s --reporter compact
        DEPENDS cask_tests
        COMMENT "Running tests..."
)

add_custom_target(run
        COMMAND cask ${ARGS}
        DEPENDS cask
        COMMENT "Running main executable..."
)

add_custom_target(clean_deps
        COMMAND ${CMAKE_COMMAND} -E remove_rf ${LIB_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove_rf ${SPEC_LIB_DIR}
        COMMENT "Removing installed dependencies..."
)