add_library(
        engine_mini_basic
        STATIC
        lib.cpp
)

target_link_libraries(
        engine_mini_basic
        tokenizer
)