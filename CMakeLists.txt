﻿cmake_minimum_required (VERSION 3.8)

if(POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("8do")

add_executable(${PROJECT_NAME} "src/main.cpp")

add_subdirectory("CPU")
add_subdirectory("extern")

target_include_directories(${PROJECT_NAME} PUBLIC 
	"include/" "CPU/include" "common/include" "devices/include" "extern/raylib/src"
	"extern/raylib-cpp/include" "extern/rlImGui/include" "extern/imgui"
	${CURSES_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE EightDoCPU EightDoExternal ${CURSES_LIBRARIES})
add_dependencies(${PROJECT_NAME} EightDoCPU EightDoExternal)

if(CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
