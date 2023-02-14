#include "Sub.h"

#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <magic_enum.hpp>
#include <string>
#include <vector>

#include "MySuperCoolMessage.h"
#include "cli.def"
#include "settings.h"

int main() {
  // Get command line options ================================================================================
  SUB_CLI("DDS Subscriber");

  // Argument Comprehension ==================================================================================
  if ((mode == DDSExampleModes::TCPv6 || mode == DDSExampleModes::UDPv6) && whitelist[0] == "127.0.0.1") {
    whitelist.erase(whitelist.begin());
  }

  // Do DDS Stuff ============================================================================================
  fmt::print("Entered main.\n");
  Sub<MySuperCoolMessage> sub;
  fmt::print("Constructed subscriber.\n");

  bool goodInit = false;
  if (mode == DDSExampleModes::Default) {
    fmt::print("Trying to initialize default subscriber.\n");
    goodInit = sub.init();
  } else if (mode == DDSExampleModes::TCPv4 || mode == DDSExampleModes::TCPv6
             || mode == DDSExampleModes::UDPv4 || mode == DDSExampleModes::UDPv6) {
    fmt::print("Trying to initialize {} subscriber.\n", magic_enum::enum_name(mode));
    goodInit = sub.initNetwork(mode, ip, port, whitelist, historyLength);
  } else {
    fmt::print("BAD MODE.\n");
    return 1;
  }

  if (goodInit) {
    fmt::print("Good Init! Running...\n");
    sub.run();
  } else {
    fmt::print("BAD INIT\n");
    return 1;
  }

  return 0;
}
