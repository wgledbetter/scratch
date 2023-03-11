#include <fmt/format.h>
#include <fmt/ostream.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <string>

#include "pugixml.hpp"
#include "recursive_find_child_by_attribute.h"

int main() {
  // Get file from command line ==============================================================================
  CLI::App app("XML");

  std::string fName = "xml.xml";
  app.add_option("-f,--file", fName, "The xml file to fuck with.");

  std::string attrib = "value";
  app.add_option("-a,--attrib", attrib, "The attribute you want to find in the document.");

  std::string value = "x";
  app.add_option("-v,--value", value, "The value of the attribute you want to find.");

  CLI11_PARSE(app);

  // Do pugi stuff ===========================================================================================
  pugi::xml_document     doc;
  pugi::xml_parse_result result = doc.load_file(fName.c_str());
  if (!result) {
    fmt::print("Failed to parse file {}.\n", fName);
  } else {
    fmt::print("Result: {}.\n", result.description());
  }

  pugi::xml_node sc = doc.child("somethingCustom");

  pugi::xml_node vx = recursive_find_child_by_attribute(sc, attrib.c_str(), value.c_str());
  if (vx.empty()) {
    fmt::print("Found nothing.\n");
  } else {
    fmt::print("Found: {}\n", vx.name());
  }
}
