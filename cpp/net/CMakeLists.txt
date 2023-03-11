add_executable(send-udp send-udp.cpp)
target_link_libraries(send-udp PUBLIC CLI11 fmt::fmt)
target_include_directories(send-udp PUBLIC ${SCRATCH_CPP_DIR})

add_executable(showip showip.cpp)
target_link_libraries(showip PUBLIC CLI11 fmt::fmt)
