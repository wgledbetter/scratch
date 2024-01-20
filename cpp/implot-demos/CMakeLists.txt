find_package(OpenGL REQUIRED)

# Basic and Function Demo ######################################################

add_executable(implot-demos implot-demos.cpp)
target_link_libraries(implot-demos PUBLIC
  imgui
  imgui_glfw
  imgui_opengl3
  implot
  glfw
  glew_s
  fmt::fmt
  curl
  ${OPENGL_LIBRARIES}
)
target_include_directories(implot-demos PUBLIC ../dep)

# Map Demo #####################################################################

add_executable(map-demo map-demo.cpp)
target_link_libraries(map-demo PUBLIC
  imgui
  imgui_glfw
  imgui_opengl3
  implot
  glfw
  glew_s
  fmt::fmt
  curl
  ${OPENGL_LIBRARIES}
)
target_include_directories(map-demo PUBLIC ../dep)
