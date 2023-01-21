#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <magic_enum.hpp>
#include <map>
#include <string>

// Miscellaneous Stuff ///////////////////////////////////////////////////////////////////////////////////////

enum class SomeOptions {
  First,
  Middle,
  Last,
  Kevin,
  AnythingReally
};

// Main //////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  // Initialization ==========================================================================================
  CLI::App app("Crapp");

  // Registering Flags =======================================================================================

  double param = 69;
  app.add_option("-p,--parameter", param, "The tuning parameter.");

  SomeOptions                        so = SomeOptions::Middle;
  std::map<std::string, SomeOptions> soMap;
  for (auto op: magic_enum::enum_values<SomeOptions>()) {
    soMap[std::string(magic_enum::enum_name(op))] = op;
  }
  app.add_option("-o,--option", so, "The option you'd like to use.")
      ->transform(CLI::CheckedTransformer(soMap, CLI::ignore_case));

  // Execute =================================================================================================

  CLI11_PARSE(app);

  // Tell me what you've got =================================================================================

  fmt::print("The parameter is {}.\n", param);
  fmt::print("The option is {}.\n", magic_enum::enum_name<SomeOptions>(so));

  return 0;
}
