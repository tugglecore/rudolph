add_library(core STATIC)
target_sources(core PRIVATE rudolph.c reader.c echo.c slice.c stats.c)
target_compile_features(core PRIVATE c_std_17)
target_compile_options(core PUBLIC -Werror -Wall -Wextra)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE core)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
target_compile_options(${PROJECT_NAME} PUBLIC -Werror -Wall -Wextra)
