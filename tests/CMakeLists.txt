include(FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 5a36b197fb34c0a77ac891c355596cb5c25aaf5b
)

FetchContent_MakeAvailable(unity)

# add_library(criterion STATIC IMPORTED)
#
# set_target_properties(criterion PROPERTIES
#     IMPORTED_LOCATION ${criterion_ep_BINARY_DIR}/install/usr/local/lib64/libcriterion.a
#     INTERFACE_INCLUDE_DIRECTORIES ${criterion_ep_BINARY_DIR}/install/usr/local/include
# )
#
# add_dependencies(criterion ext_cri)

add_library(criterion INTERFACE)
add_dependencies(criterion ext_cri)

set(CRITERION_LIB_PATH ${criterion_ep_BINARY_DIR}/install/usr/local/lib64/libcriterion.a)
set(CRITERION_INCLUDE_DIR ${criterion_ep_BINARY_DIR}/install/usr/local/include)

target_link_options(criterion INTERFACE "${CRITERION_LIB_PATH}")
target_include_directories(criterion INTERFACE "${CRITERION_INCLUDE_DIR}")

add_library(test-core STATIC helpers.c)
target_link_libraries(test-core PUBLIC core unity)
# target_include_directories(test-core SYSTEM PRIVATE ${criterion_ep_SOURCE_DIR}/include)

add_test(NAME smoke-test COMMAND rudolph --version)

# add_executable(test-options options.test.c)
# add_dependencies(test-options criterion)
# target_link_libraries(test-options PRIVATE test-core criterion)
# target_include_directories(test-options PRIVATE ${criterion_ep_SOURCE_DIR}/include)
# add_test(NAME test-options COMMAND test-options)

# add_executable(test-reader reader.test.c)
# target_link_libraries(test-reader PRIVATE test-core)
# add_test(
#     NAME test-reader
#     COMMAND test-reader
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
# )
#
add_executable(test-concat concat.test.c)
target_link_libraries(test-concat PRIVATE test-core criterion)
# target_include_directories(test-concat SYSTEM PRIVATE ${criterion_ep_SOURCE_DIR}/include)
# add_test(NAME test-concat COMMAND test-concat)
