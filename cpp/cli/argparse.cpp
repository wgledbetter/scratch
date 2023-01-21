#include <fmt/format.h>

#include <argparse/argparse.hpp>

// Main //////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
  // Initialization ==========================================================================================
  argparse::ArgumentParser app("Crapp");

  // Registering Flags =======================================================================================

  app.add_argument("-p", "--parameter")
      .help("The tuning parameter.")
      .nargs(1)
      .default_value(69)
      .scan<'g', double>();

  // Execute =================================================================================================

  try {
    app.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    fmt::print("Error: {}.\n", err.what());
    exit(1);
  }

  // Tell me what you've got =================================================================================

  fmt::print("The parameter is {}.\n", app.get<double>("-p"));
}
