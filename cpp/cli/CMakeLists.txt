add_executable(cli main.cpp)
target_link_libraries(cli PUBLIC CLI11)
target_link_libraries(cli PUBLIC fmt::fmt magic_enum::magic_enum eigen)
