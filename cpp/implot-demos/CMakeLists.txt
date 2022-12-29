find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(implot-demos implot-demos.cpp)
target_link_libraries(implot-demos PUBLIC
  imgui
  imgui_glfw
  imgui_opengl3
  implot
  glfw
  GLEW::GLEW
  fmt::fmt
  curl
  ${OPENGL_LIBRARIES}
)
target_include_directories(implot-demos PUBLIC ../dep)
