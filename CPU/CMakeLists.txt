add_library(CPU "src/cpu.cpp" "src/cpu_instructions.cpp" "include/CPU/devices/ram.hpp" "include/CPU/lib.hpp")
target_include_directories(CPU PUBLIC "include/")

if(CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET CPU PROPERTY CXX_STANDARD 20)
endif()