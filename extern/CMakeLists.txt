cmake_minimum_required (VERSION 3.8)

project("EightDoExternal")

add_subdirectory("raylib")
add_subdirectory("raylib-cpp")
add_subdirectory("rlImGui")

add_library(${PROJECT_NAME} STATIC
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_tables.cpp" 
    "imgui/imgui_widgets.cpp" 
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib raylib_cpp rlImGui)
target_include_directories(${PROJECT_NAME} PUBLIC "imgui")

add_dependencies(${PROJECT_NAME} raylib raylib_cpp rlImGui)