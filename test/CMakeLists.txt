project(${CMAKE_PROJECT_NAME}-test)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
  "src/codegen/test.cpp.cpp"
)

target_link_libraries(${PROJECT_NAME}
  GTest::gtest_main
  lib${CMAKE_PROJECT_NAME}
)