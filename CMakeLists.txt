cmake_minimum_required(VERSION 3.16)
project(MentalLoadWidgets LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定 Qt 5.15.2，确保与用户环境兼容
find_package(Qt5 5.15.2 REQUIRED COMPONENTS Widgets Designer)

add_library(LoadTimelineWidget STATIC
    src/widget/LoadTimelineWidget.cpp
    src/widget/LoadTimelineWidget.h
)
target_link_libraries(LoadTimelineWidget PUBLIC Qt5::Widgets)
target_include_directories(LoadTimelineWidget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(LoadTimelineWidgetPlugin SHARED
    src/plugin/LoadTimelineWidgetPlugin.cpp
    src/plugin/LoadTimelineWidgetPlugin.h
)
target_link_libraries(LoadTimelineWidgetPlugin PRIVATE Qt5::Designer LoadTimelineWidget)
set_target_properties(LoadTimelineWidgetPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/designer
)

add_executable(mental_load_demo
    demo/main.cpp
    demo/MainWindow.cpp
    demo/MainWindow.h
)
target_link_libraries(mental_load_demo PRIVATE LoadTimelineWidget Qt5::Widgets)

