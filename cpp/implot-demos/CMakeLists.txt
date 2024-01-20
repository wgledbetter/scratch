find_package(OpenGL REQUIRED)
find_package(CURL)

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
  ${OPENGL_LIBRARIES}
)
target_include_directories(implot-demos PUBLIC ../dep)

# Map Demo #####################################################################

if(CURL_FOUND)
  add_executable(map-demo map-demo.cpp)
  target_link_libraries(map-demo PUBLIC
    imgui
    imgui_glfw
    imgui_opengl3
    implot
    glfw
    glew_s
    fmt::fmt
    CURL::libcurl
    ${OPENGL_LIBRARIES}
  )
  target_include_directories(map-demo PUBLIC ../dep)
endif()
