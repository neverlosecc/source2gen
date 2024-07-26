cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project(
    source2gen
    VERSION 0.1.0
    DESCRIPTION "Source2 games SDK generator"
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/neverlosecc/source2gen"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "source2gen: 32-bit targets are not supported")
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23 CACHE STRING "Default C++ standard")
message(STATUS "source2gen: C++ standard set to ${CMAKE_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++ standard")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Allow C++ extensions")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "source2gen: Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "source2gen: Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

find_package(argparse REQUIRED)

set(SOURCE2GEN_SUPPORTED_GAMES
    CS2
    DOTA2
    SBOX
    ARTIFACT2
    ARTIFACT1
    UNDERLORDS
    DESKJOB
    HL_ALYX
    THE_LAB_ROBOT_REPAIR
)

option(SOURCE2GEN_GAME "Game to generate" "CS2")

if(NOT SOURCE2GEN_GAME)
    message(STATUS "source2gen: No game specified. Using default: CS2")
    set(SOURCE2GEN_GAME "CS2")
endif()

if(NOT SOURCE2GEN_GAME IN_LIST SOURCE2GEN_SUPPORTED_GAMES)
    message(FATAL_ERROR "source2gen: Invalid value for SOURCE2GEN_GAME: ${SOURCE2GEN_GAME}")
endif()

file(GLOB_RECURSE source2gen_SOURCES "src/**.cpp")
list(REMOVE_ITEM source2gen_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE source2gen_HEADERS "include/**.h" "include/**.hpp")

add_library(lib${PROJECT_NAME} ${source2gen_SOURCES})

target_link_libraries(lib${PROJECT_NAME}
  argparse::argparse
)

target_include_directories(lib${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(lib${PROJECT_NAME} PRIVATE /bigobj)
endif()
    target_compile_options(lib${PROJECT_NAME} PRIVATE -Wfatal-errors)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(lib${PROJECT_NAME} PRIVATE "source2gen_DEBUG" "_DEBUG")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(lib${PROJECT_NAME} PRIVATE "source2gen_RELEASE" "NDEBUG")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
   ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" OR
   ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(lib${PROJECT_NAME} PRIVATE
      -pedantic-errors
      -Wno-address-of-packed-member
      $<$<NOT:$<CXX_COMPILER_ID:GNU>>:-Wno-implicit-exception-spec-mismatch>
      -Wno-invalid-offsetof
      $<IF:$<CXX_COMPILER_ID:GNU>,-Wno-attributes,-Wno-unknown-attributes>
    )

    target_compile_definitions(lib${PROJECT_NAME} PUBLIC
      "FORCEINLINE="
      "__forceinline="
      "__thiscall="
      "__cdecl="
    )
endif()

target_compile_definitions(lib${PROJECT_NAME} PUBLIC
    "${SOURCE2GEN_GAME}"
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
    "_WIN32_WINNT=0x601"
    "__cpp_lib_expected" # Because clangd-18 + GLIBCXX_3.4.33 doesn't know std::expected. This is a hack to make the language server work.
)

add_executable(${PROJECT_NAME} "src/main.cpp")
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})

add_subdirectory("test")
