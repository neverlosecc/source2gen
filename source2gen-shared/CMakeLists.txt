add_library(source2gen-shared INTERFACE)
add_library(source2gen::shared ALIAS source2gen-shared)

target_include_directories(source2gen-shared INTERFACE
    "include"
)
