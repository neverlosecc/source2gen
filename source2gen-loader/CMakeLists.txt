add_executable(source2gen-loader)

file(GLOB_RECURSE source2gen_loader_SOURCES "src/**.cpp")
file(GLOB_RECURSE source2gen_loader_HEADERS "include/**.h")

target_sources(source2gen-loader PRIVATE ${source2gen_loader_SOURCES} ${source2gen_loader_HEADERS})
target_include_directories(source2gen-loader PRIVATE
    "include"
)

target_link_libraries(source2gen-loader PRIVATE
    source2gen-shared
    ValveFileVDF
)

if(MSVC)
    target_link_options(source2gen-loader PRIVATE "/pdbaltpath:%_PDB%")
endif()

target_compile_definitions(source2gen-loader PRIVATE
    "${SOURCE2GEN_GAME}"
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
    "_WIN32_WINNT=0x601"
)
