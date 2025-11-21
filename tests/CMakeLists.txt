include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 5a36b197fb34c0a77ac891c355596cb5c25aaf5b)
FetchContent_MakeAvailable(unity)
add_executable(test-rudolph test.c)
target_link_libraries(test-rudolph PRIVATE core unity)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)

add_test(NAME exp COMMAND test-rudolph)
