# TOOD: file should not be here

cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

project(source2sdk
  LANGUAGES CXX
)

file(GLOB_RECURSE source2sdk_headers "./**.hpp")

foreach(el ${source2sdk_headers})
    string(APPEND generated_cpp_contents "#include \"${el}\"\n")
endforeach()

set(generated_cpp_file "${CMAKE_BINARY_DIR}/all_headers.cpp")

file(WRITE ${generated_cpp_file} ${generated_cpp_contents})

add_library(${PROJECT_NAME} ${generated_cpp_file})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wfatal-errors"
    "-pedantic-errors"
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${source2sdk_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
