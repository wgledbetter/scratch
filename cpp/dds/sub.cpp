#include "Sub.h"

#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <magic_enum.hpp>
#include <string>
#include <vector>

#include "MySuperCoolMessage.h"
#include "settings.h"

int main() {
  // Get command line options ================================================================================
  CLI::App app("DDS Subscriber");

  DDSExampleModes                        mode = DDSExampleModes::Default;
  std::map<std::string, DDSExampleModes> modeMap;
  for (auto m: magic_enum::enum_values<DDSExampleModes>()) {
    modeMap[std::string(magic_enum::enum_name(m))] = m;
  }
  app.add_option("--mode", mode, "The transmission mode.")
      ->transform(CLI::CheckedTransformer(modeMap, CLI::ignore_case));

  std::string ip = "";
  app.add_option("--ip", ip, "If network mode, the ip address to use.");

  unsigned short port = 5749;
  app.add_option("-p,--port", port, "If network mode, the port to use.");

  std::vector<std::string> whitelist = {"127.0.0.1"};
  app.add_option("-w,--whitelist", whitelist, "If network mode, the ips to whitelist.")->delimiter(',');

  CLI11_PARSE(app);

  // Do DDS Stuff ============================================================================================
  fmt::print("Entered main.\n");
  Sub<MySuperCoolMessage> sub;
  fmt::print("Constructed subscriber.\n");

  if (mode == DDSExampleModes::Default) {
    sub.init();
    fmt::print("Initialized default subscriber. Running...\n");
  } else if (mode == DDSExampleModes::TCPv4) {
    sub.initTCPv4(ip, port, whitelist);
    fmt::print("Initialized TCPv4 subscriber. Running...\n");
  }

  sub.run();
}
