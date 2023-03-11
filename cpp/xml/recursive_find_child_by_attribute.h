#pragma once

#include "pugixml.hpp"

pugi::xml_node recursive_find_child_by_attribute(const pugi::xml_node xn,
                                                 const pugi::char_t*  attr_name,
                                                 const pugi::char_t*  attr_value) {
  pugi::xml_node out = xn.find_child_by_attribute(attr_name, attr_value);

  if (out.empty()) {
    for (auto& ch: xn.children()) {
      pugi::xml_node chOut = recursive_find_child_by_attribute(ch, attr_name, attr_value);
      if (!chOut.empty()) {
        return chOut;
      }
    }
  }

  return out;
}
