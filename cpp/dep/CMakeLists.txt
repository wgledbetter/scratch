# Easy Stuff -------------------------------------------------------------------
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_subdirectory(CLI11 EXCLUDE_FROM_ALL)
add_subdirectory(magic_enum EXCLUDE_FROM_ALL)
add_subdirectory(eigen EXCLUDE_FROM_ALL)
add_subdirectory(argparse EXCLUDE_FROM_ALL)
add_subdirectory(Fast-DDS EXCLUDE_FROM_ALL)
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
add_subdirectory(pugixml EXCLUDE_FROM_ALL)
add_subdirectory(glfw EXCLUDE_FROM_ALL)

# ImGui Targets ----------------------------------------------------------------

add_library(imgui imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
target_include_directories(imgui PUBLIC imgui)

add_library(imgui_glfw imgui/backends/imgui_impl_glfw.cpp)
target_include_directories(imgui_glfw PUBLIC imgui imgui/backends)

add_library(imgui_opengl3 imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_opengl3 PUBLIC imgui imgui/backends)

# ImPlot Target ----------------------------------------------------------------

add_library(implot implot/implot.cpp implot/implot_items.cpp)
target_include_directories(implot PUBLIC implot)
target_link_libraries(implot PUBLIC imgui)

# GLEW -------------------------------------------------------------------------
if(WIN32)
  set(GLEW_URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip)
  set(GLEW_LIBS ${CMAKE_CURRENT_BINARY_DIR}/glew-2.2.0/lib/Release/x64/glew32.lib
                ${CMAKE_CURRENT_BINARY_DIR}/glew-2.2.0/bin/Release/x64/glew32.dll
  )
  set(GLEW_S_LIBS ${CMAKE_CURRENT_BINARY_DIR}/glew-2.2.0/lib/Release/x64/glew32s.lib)
else()
  set(GLEW_URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip)
endif()

file(DOWNLOAD ${GLEW_URL} ${CMAKE_CURRENT_BINARY_DIR}/glew.zip)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/glew.zip)

add_library(glew INTERFACE)
target_include_directories(glew INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/glew-2.2.0/include)
target_link_libraries(glew INTERFACE ${GLEW_LIBS})

add_library(glew_s INTERFACE)
target_include_directories(glew_s INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/glew-2.2.0/include)
target_link_libraries(glew_s INTERFACE ${GLEW_S_LIBS})
