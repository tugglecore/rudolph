cmake_minimum_required(VERSION 3.31...4.0)
project(rudolph VERSION 0.1
                DESCRIPTION "Best CSV toolkit In The Land"
                LANGUAGES C
)
include(CTest)
add_subdirectory(src bin)
add_subdirectory(tests)
