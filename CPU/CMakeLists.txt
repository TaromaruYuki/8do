project(EightDoCPU)

add_library(${PROJECT_NAME} STATIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_instructions.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")

if(CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
