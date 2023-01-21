#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

int main() {
  CLI::App app("Crapp");

  double param = 69;
  app.add_option("-p,--parameter", param, "The tuning parameter.");

  CLI11_PARSE(app);

  fmt::print("The parameter is {}.\n", param);

  return 0;
}
