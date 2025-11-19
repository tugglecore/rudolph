cmake_minimum_required(VERSION 3.31...4.0)
project(rudolph VERSION 0.1
                DESCRIPTION "Best CSV toolkit In The Land"
                LANGUAGES C
)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE 
        src/main.c
        src/reader.c
        src/echo.c
        src/slice.c
        src/stats.c
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
