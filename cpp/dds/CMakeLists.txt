# Publisher ====================================================================
add_executable(dds-pub pub.cpp)
target_link_libraries(dds-pub PUBLIC fastrtps fastcdr fmt::fmt CLI11 magic_enum::magic_enum)

# Subscriber ===================================================================
add_executable(dds-sub sub.cpp)
target_link_libraries(dds-sub PUBLIC fastrtps fastcdr fmt::fmt CLI11 magic_enum::magic_enum)

# Minimal ======================================================================
add_executable(dds-minimal trulyMinimal.cpp)
target_link_libraries(dds-minimal PUBLIC fastrtps fastcdr fmt::fmt)

# Plotting Subscriber ==========================================================
find_package(OpenGL REQUIRED)

add_executable(dds-plot-sub plot-sub.cpp)
target_include_directories(dds-plot-sub PUBLIC ${SCRATCH_CPP_DIR})
target_link_libraries(dds-plot-sub PUBLIC
  fastrtps
  fastcdr
  fmt::fmt
  CLI11
  magic_enum::magic_enum
  imgui
  imgui_glfw
  imgui_opengl3
  implot
  glfw
  glew_s
  ${OPENGL_LIBRARIES}
)

# Data Message Publisher =======================================================
add_executable(dds-data-pub data-pub.cpp)
target_link_libraries(dds-data-pub PUBLIC fastrtps fastcdr fmt::fmt CLI11 magic_enum::magic_enum)
target_include_directories(dds-data-pub PUBLIC ${SCRATCH_CPP_DIR})
