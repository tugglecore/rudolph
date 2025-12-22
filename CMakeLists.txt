cmake_minimum_required(VERSION 3.31...4.0)
project(rudolph VERSION 0.1
                DESCRIPTION "Best CSV toolkit In The Land"
                LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(ExternalProject)
include(FetchContent)

FetchContent_Declare(
    criterion_ep
    GIT_REPOSITORY https://github.com/Snaipe/Criterion.git
    GIT_TAG f6d40d5b9f4b30d915aaefe649baa82df10715ab 
)

FetchContent_MakeAvailable(criterion_ep)

ExternalProject_Add(
    ext_cri
    SOURCE_DIR ${criterion_ep_SOURCE_DIR}
    CONFIGURE_COMMAND meson setup ${criterion_ep_BINARY_DIR} ${criterion_ep_SOURCE_DIR}
    BUILD_COMMAND meson compile -C ${criterion_ep_BINARY_DIR}
    INSTALL_COMMAND meson install -C ${criterion_ep_BINARY_DIR} --destdir ${criterion_ep_BINARY_DIR}/install
    BUILD_BYPRODUCTS ${criterion_ep_BINARY_DIR}/install/usr/local/include
)

# ExternalProject_Add(
#     criterion_ep
#     GIT_REPOSITORY https://github.com/Snaipe/Criterion.git
#     GIT_TAG f6d40d5b9f4b30d915aaefe649baa82df10715ab 
#     CONFIGURE_COMMAND meson setup <BINARY_DIR> <SOURCE_DIR>
#     BUILD_COMMAND meson compile -C <BINARY_DIR>
#     INSTALL_COMMAND ""
#     BUILD_BYPRODUCTS <SOURCE_DIR>/include
#     VERBATIM
# )
#
# ExternalProject_Get_Property(criterion_ep SOURCE_DIR BINARY_DIR)
#
# set(CRI_SOURCE_DIR "${SOURCE_DIR}")
# set(CRI_BINARY_DIR "${BINARY_DIR}")
#
# # Debug print if needed
# message(STATUS "Criterion source dir: ${CRI_SOURCE_DIR}")
# message(STATUS "Criterion build dir:  ${CRI_BINARY_DIR}")
#
#
# ExternalProject_Get_Property(criterion_ep SOURCE_DIR BINARY_DIR)
# set(LIB_LOCATION "${BINARY_DIR}/src/libcriterion.a")
# set(INC_LOCATION "${SOURCE_DIR}/include")
#
# file(MAKE_DIRECTORY ${INC_LOCATION})
# file(MAKE_DIRECTORY "${BINARY_DIR}/src")
# file(TOUCH "${BINARY_DIR}/src/libcriterion.a")



include(CTest)
add_subdirectory(src bin)
add_subdirectory(tests)
