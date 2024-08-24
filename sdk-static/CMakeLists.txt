# TOOD: use meson - but meson can't glob files

cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

project(source2sdk
  LANGUAGES CXX
)

file(GLOB_RECURSE source2sdk_headers "./**.hpp")

add_library(${PROJECT_NAME} ${source2sdk_headers})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wfatal-errors"
    "-pedantic-errors"
)

# Add a target that includes all headers of the generated library to check for compile-time errors

foreach(el ${source2sdk_headers})
    string(APPEND generated_cpp_contents "#include \"${el}\"\n")
endforeach()

set(generated_cpp_file "${CMAKE_BINARY_DIR}/all_headers.cpp")

file(WRITE ${generated_cpp_file} ${generated_cpp_contents})

add_library(${PROJECT_NAME}-compile-test ${generated_cpp_file})
set_target_properties(${PROJECT_NAME}-compile-test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME}-compile-test ${PROJECT_NAME})

target_compile_options(${PROJECT_NAME}-compile-test PRIVATE
    "-Wfatal-errors"
    "-pedantic-errors"
)
