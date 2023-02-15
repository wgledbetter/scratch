# Easy Stuff -------------------------------------------------------------------
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_subdirectory(CLI11 EXCLUDE_FROM_ALL)
add_subdirectory(magic_enum EXCLUDE_FROM_ALL)
add_subdirectory(eigen EXCLUDE_FROM_ALL)
add_subdirectory(argparse EXCLUDE_FROM_ALL)
add_subdirectory(Fast-DDS EXCLUDE_FROM_ALL)
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
add_subdirectory(pugixml EXCLUDE_FROM_ALL)

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
