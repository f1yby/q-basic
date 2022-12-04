add_executable(
        mini_basic_gui
        main.cpp
)

target_link_libraries(
        mini_basic_gui
        main_window
        Qt6::Widgets
)