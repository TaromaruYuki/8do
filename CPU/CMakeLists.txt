add_library(CPU STATIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_instructions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/CPU/devices/ram.hpp"
)
target_include_directories(CPU PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

if(CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET CPU PROPERTY CXX_STANDARD 20)
endif()