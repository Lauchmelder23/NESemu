add_executable(nesemu
	"main.cpp"
	"Application.cpp" 
	"Bus.cpp"
	"CPU.cpp"
	"Cartridge.cpp"
	"mappers/Mapper000.cpp" 
	"Log.cpp" 
	"PPU.cpp" 
	"gfx/Window.cpp"
	"debugger/CPUWatcher.cpp"
	"debugger/Debugger.cpp" 
	"debugger/PPUWatcher.cpp" 
	"debugger/Disassembler.cpp" 
	"debugger/MemoryViewer.cpp" 
	"debugger/NametableViewer.cpp"  "ControllerPort.cpp" "controllers/StandardController.cpp" "gfx/Input.cpp" "debugger/ControllerPortViewer.cpp" "gfx/Screen.cpp" "debugger/Palettes.cpp" "APU.cpp" "debugger/PatternTableViewer.cpp" "mappers/Mapper003.cpp" "mappers/Mapper001.cpp" "debugger/OAMViewer.cpp")

target_include_directories(nesemu PRIVATE
	mappers
	gfx
	debugger
	${IMGUI_INCLUDE}
)

target_sources(nesemu PRIVATE
	${IMGUI_SOURCES}
)

target_link_libraries(nesemu
	spdlog
	glfw
	glad
	${CMAKE_DL_LIBS}
)

if (WIN32) 
	target_compile_options(nesemu PRIVATE "/W4" "/WX" "/wd4996")
else()
	target_compile_options(nesemu PRIVATE "-Wall" "-Werror")
endif()

add_custom_command(TARGET nesemu POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/roms $<TARGET_FILE_DIR:nesemu>/roms
)