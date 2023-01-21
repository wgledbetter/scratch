# Publisher ====================================================================
add_executable(dds-pub pub.cpp)
target_link_libraries(dds-pub PUBLIC fastrtps fastcdr fmt::fmt)

# Subscriber ===================================================================
add_executable(dds-sub sub.cpp)
target_link_libraries(dds-sub PUBLIC fastrtps fastcdr fmt::fmt)
