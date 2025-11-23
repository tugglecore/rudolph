include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 5a36b197fb34c0a77ac891c355596cb5c25aaf5b)
FetchContent_MakeAvailable(unity)

add_test(NAME smoke-test-rudolph COMMAND rudolph --version)

add_executable(test-rudolph-options test.c)
target_include_directories(test-rudolph-options PUBLIC ../src)
target_link_libraries(test-rudolph-options PRIVATE core unity)
target_compile_features(test-rudolph-options PRIVATE c_std_17)

add_test(NAME test-rudolph-options COMMAND test-rudolph-options)
