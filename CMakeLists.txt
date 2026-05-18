cmake_minimum_required(VERSION 3.22)
project(practice_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(
    notepad
    main.cpp
    main_window.h
    main_window.cpp
    find_replace_dialog.h
    find_replace_dialog.cpp
    word_frequency_dialog.h
    word_frequency_dialog.cpp
    notepad_exception.h
    simple_game_dialog.h
    simple_game_dialog.cpp
    text_transform.h
    sort.h
    spell_checker.h
    spell_checker.cpp
    spell_checker_highlighter.h
    spell_checker_highlighter.cpp
    spell_check_text_edit.h
    spell_check_text_edit.cpp
    find_replace_dialog.ui
    word_frequency_dialog.ui
)
target_link_libraries(notepad PRIVATE Qt6::Widgets)
