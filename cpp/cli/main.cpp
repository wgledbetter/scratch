#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <Eigen/Eigen>
#include <array>
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

  int verbosity = 0;
  app.add_flag("-v", verbosity, "Verbosity level.");

  auto flagBack = [](int nFlag) { fmt::print("I was called because you passed '-c' {} times.\n", nFlag); };
  app.add_flag_function("-c", flagBack, "Pass this flag to execute some callback.");

  std::array<double, 3> initState = {1, 2, 3.5};
  app.add_option("-i,--initState", initState, "The inital state where to start.")->delimiter(',');

  // Execute =================================================================================================

  CLI11_PARSE(app);

  // Handle ==================================================================================================

  Eigen::Matrix<double, 3, 1> eigenState(initState.data());

  // Tell me what you've got =================================================================================

  fmt::print("The parameter is {}.\n", param);
  fmt::print("The option is {}.\n", magic_enum::enum_name<SomeOptions>(so));
  fmt::print("The verbosity level is {}.\n", verbosity);
  fmt::print("The initial state is {}.\n", initState);

  fmt::print("The inferred Eigen matrix is {}.\n", eigenState);

  return 0;
}
