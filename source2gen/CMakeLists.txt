file(GLOB_RECURSE source2gen_SOURCES "src/**.cpp")
list(REMOVE_ITEM source2gen_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB_RECURSE source2gen_HEADERS "include/**.h" "include/**.hpp")

# We build a library first, because we can link the library against tests.
# main.cpp is compiled separately and also linked against the library.
add_library(lib${PROJECT_NAME} ${source2gen_SOURCES})

target_include_directories(lib${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lib${PROJECT_NAME}
  argparse::argparse
)

if(MSVC)
    target_compile_options(lib${PROJECT_NAME} PRIVATE "/bigobj")
    target_link_options(lib${PROJECT_NAME} PRIVATE "/pdbaltpath:%_PDB%")
endif()

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
        # TOOD: uncomment
        # -Wall
        # -Wextra
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

target_compile_definitions(lib${PROJECT_NAME} PRIVATE
    "${SOURCE2GEN_GAME}"
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
    "_WIN32_WINNT=0x601"
)

add_executable(${PROJECT_NAME} "src/main.cpp")
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})

add_subdirectory("test")
