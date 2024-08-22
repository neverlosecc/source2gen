add_executable(source2gen)

file(GLOB_RECURSE source2gen_SOURCES "src/**.cpp")
file(GLOB_RECURSE source2gen_HEADERS "include/**.h")

target_sources(source2gen PRIVATE ${source2gen_SOURCES} ${source2gen_HEADERS})

target_include_directories(source2gen PUBLIC
    "include"
)

if(MSVC)
    target_compile_options(source2gen PRIVATE "/bigobj")
    target_link_options(source2gen PRIVATE "/pdbaltpath:%_PDB%")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(source2gen PRIVATE "source2gen_DEBUG" "_DEBUG")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(source2gen PRIVATE "source2gen_RELEASE" "NDEBUG")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(source2gen PRIVATE
        -pedantic-errors
        -Wno-address-of-packed-member
        $<$<NOT:$<CXX_COMPILER_ID:GNU>>:-Wno-implicit-exception-spec-mismatch>
        -Wno-invalid-offsetof
        $<IF:$<CXX_COMPILER_ID:GNU>,-Wno-attributes,-Wno-unknown-attributes>
    )

    target_compile_definitions(source2gen PRIVATE
        "FORCEINLINE="
        "__forceinline="
    )
endif()

target_compile_definitions(source2gen PRIVATE
    "${SOURCE2GEN_GAME}"
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
    "_WIN32_WINNT=0x601"
)
