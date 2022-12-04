add_library(
        main_window
        STATIC
        main_window.cpp
        main_window.h
        main_window.ui
)

target_link_libraries(
        main_window
        Qt6::Widgets
        engine_mini_basic
)