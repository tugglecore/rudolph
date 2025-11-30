include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 5a36b197fb34c0a77ac891c355596cb5c25aaf5b)
FetchContent_MakeAvailable(unity)

add_test(NAME smoke-test COMMAND rudolph --version)

add_executable(test-options options.test.c)
target_include_directories(test-options PUBLIC ../src)
target_link_libraries(test-options PRIVATE core unity)
target_compile_features(test-options PRIVATE c_std_17)
add_test(NAME test-options COMMAND test-options)

add_executable(test-reader reader.test.c)
target_include_directories(test-reader PUBLIC ../src)
target_link_libraries(test-reader PRIVATE core unity)
target_compile_features(test-reader PRIVATE c_std_17)
add_test(
    NAME test-reader
    COMMAND test-reader
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
)

add_executable(test-concat concat.test.c)
target_include_directories(test-concat PUBLIC ../src)
target_link_libraries(test-concat PRIVATE core unity)
target_compile_features(test-concat PRIVATE c_std_17)
add_test(NAME test-concat COMMAND test-concat)
