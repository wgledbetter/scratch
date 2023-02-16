add_executable(xml xml.cpp)
target_link_libraries(xml PUBLIC pugixml::static CLI11 fmt::fmt)
