include(FetchContent)

FetchContent_Declare(ValveFileVDF SYSTEM
        GIT_REPOSITORY
        "https://github.com/TinyTinni/ValveFileVDF"
        GIT_TAG
        "2d4c60eb160bc413ef4bafb0c1fd5ccf92ae4705"
)
FetchContent_MakeAvailable(ValveFileVDF)
