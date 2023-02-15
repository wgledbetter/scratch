#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <magic_enum.hpp>
#include <string>
#include <vector>

#include "DatafulMessage.h"
#include "PubData.h"
#include "cli.def"
#include "settings.h"

int main() {
  // Get command line options ================================================================================
  PUB_CLI_SETUP("DDS Data Publisher");

  CLI11_PARSE(app);

  // Argument Comprehension ==================================================================================
  if ((mode == DDSExampleModes::TCPv6 || mode == DDSExampleModes::UDPv6) && whitelist[0] == "127.0.0.1") {
    whitelist.erase(whitelist.begin());
  }

  // Do DDS Stuff ============================================================================================
  PubData<DatafulMessage> pub;

  std::string ip = "";

  bool goodInit = false;
  if (mode == DDSExampleModes::Default) {
    fmt::print("Trying to initialize default data publisher.\n");
    goodInit = pub.init();
  } else if (mode == DDSExampleModes::TCPv4 || mode == DDSExampleModes::TCPv6
             || mode == DDSExampleModes::UDPv4 || mode == DDSExampleModes::UDPv6) {
    fmt::print("Trying to initialize {} data publisher.\n");
    goodInit = pub.initNetwork(mode, ip, port, whitelist, historyLength);
  } else {
    fmt::print("BAD MODE.\n");
    return 1;
  }

  if (goodInit) {
    fmt::print("Good Init! Running...\n");
    pub.run(delayMilliseconds);
  } else {
  }
}
