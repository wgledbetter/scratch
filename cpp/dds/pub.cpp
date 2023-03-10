#include "Pub.h"

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
  PUB_CLI_SETUP("DDS Publisher");

  std::string msg = "CLI default message";
  app.add_option("-m,--message", msg, "The message you'd like to send.");

  int nMsg = 5;
  app.add_option("-n,--number", nMsg, "How many times to send the message. (-1 for infinite)");

  CLI11_PARSE(app);

  // Argument Comprehension ==================================================================================
  if ((mode == DDSExampleModes::TCPv6 || mode == DDSExampleModes::UDPv6) && whitelist[0] == "127.0.0.1") {
    whitelist.erase(whitelist.begin());
  }

  // Do DDS Stuff ============================================================================================
  fmt::print("Entered main.\n");
  Pub<MySuperCoolMessage> pub;
  fmt::print("Constructed publisher.\n");

  std::string ip = "";

  bool goodInit = false;
  if (mode == DDSExampleModes::Default) {
    fmt::print("Trying to initialize default publisher.\n");
    goodInit = pub.init();
  } else if (mode == DDSExampleModes::TCPv4 || mode == DDSExampleModes::TCPv6
             || mode == DDSExampleModes::UDPv4 || mode == DDSExampleModes::UDPv6) {
    fmt::print("Trying to initialize {} publisher.\n", magic_enum::enum_name(mode));
    goodInit = pub.initNetwork(mode, ip, port, whitelist, historyLength);
  } else {
    fmt::print("BAD MODE.\n");
    return 1;
  }

  if (goodInit) {
    fmt::print("Good Init! Running...\n");
    pub.run(nMsg, delayMilliseconds, msg);
  } else {
    fmt::print("BAD INIT!\n");
    return 1;
  }

  return 0;
}
