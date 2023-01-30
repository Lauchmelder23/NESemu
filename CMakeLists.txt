# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("NES Emulator")

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3)
if(NOT glfw3_FOUND)
	add_subdirectory("vendor/glfw")
endif()
add_subdirectory("vendor/glad")
add_subdirectory("vendor/spdlog")

set(IMGUI_SOURCES 
	${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_INCLUDE
	${CMAKE_SOURCE_DIR}/vendor
	${CMAKE_SOURCE_DIR}/vendor/imgui
)

# Include sub-projects.
add_subdirectory ("src")
