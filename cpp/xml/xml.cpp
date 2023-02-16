#include <fmt/format.h>
#include <fmt/ostream.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <string>

#include "pugixml.hpp"

int main() {
  // Get file from command line ==============================================================================
  CLI::App app("XML");

  std::string fName = "xml.xml";
  app.add_option("-f,--file", fName, "The xml file to fuck with.");

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
  pugi::xml_node xn = sc.find_child_by_attribute("a", "b");
  fmt::print("Found: {}\n", xn.name());
}
