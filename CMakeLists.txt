cmake_minimum_required(VERSION 3.16)

project(math_expr_lexer VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(math_expr STATIC
    src/lexer/lexer.c
)

target_include_directories(math_expr
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(math_expr
    PUBLIC c_std_11
)

add_executable(math_expr_lexer
    src/app/main.c
)

target_link_libraries(math_expr_lexer
    PRIVATE
        math_expr
)

set_target_properties(math_expr_lexer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)

