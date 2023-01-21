add_executable(cliEleven cli11.cpp)
target_link_libraries(cliEleven PUBLIC CLI11)
target_link_libraries(cliEleven PUBLIC fmt::fmt magic_enum::magic_enum eigen)

add_executable(cliArgparse argparse.cpp)
target_link_libraries(cliArgparse PUBLIC argparse::argparse)
target_link_libraries(cliArgparse PUBLIC fmt::fmt)
