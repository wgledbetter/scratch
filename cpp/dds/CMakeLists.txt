# Subscriber ===================================================================
add_executable(dds-sub sub.cpp)
target_link_libraries(dds-sub PUBLIC fastrtps fastcdr fmt::fmt)
