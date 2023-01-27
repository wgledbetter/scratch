#include "Pub.h"

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
  CLI::App app("DDS Publisher");

  std::string msg = "CLI default message";
  app.add_option("-m,--message", msg, "The message you'd like to send.");

  int nMsg = 5;
  app.add_option("-n,--number", nMsg, "How many times to send the message.");

  int delayMilliseconds = 1000;
  app.add_option("-d,--delay", delayMilliseconds, "How long to wait between messages (in milliseconds).");

  DDSExampleModes                        mode = DDSExampleModes::Default;
  std::map<std::string, DDSExampleModes> modeMap;
  for (auto m: magic_enum::enum_values<DDSExampleModes>()) {
    modeMap[std::string(magic_enum::enum_name(m))] = m;
  }
  app.add_option("--mode", mode, "The transmission mode.")
      ->transform(CLI::CheckedTransformer(modeMap, CLI::ignore_case));

  std::string ip = "";
  // app.add_option("--ip", ip, "If network mode, the ip address to use.");

  unsigned short port = 5749;
  app.add_option("-p,--port", port, "If network mode, the port to use.");

  std::vector<std::string> whitelist = {"127.0.0.1"};
  app.add_option("-w,--whitelist",
                 whitelist,
                 "If network mode, the IP addresses of this machine on which to allow DDS.")
      ->delimiter(',');

  CLI11_PARSE(app);

  // Do DDS Stuff ============================================================================================
  fmt::print("Entered main.\n");
  Pub<MySuperCoolMessage> pub;
  fmt::print("Constructed publisher.\n");

  if (mode == DDSExampleModes::Default) {
    pub.init();
    fmt::print("Initialized default publisher. Running...\n");
  } else if (mode == DDSExampleModes::TCPv4) {
    pub.initTCPv4(ip, port, whitelist);
    fmt::print("Initialized TCPv4 publisher. Running...\n");
  } else {
    fmt::print("BAD MODE.\n");
    return 1;
  }

  pub.run(nMsg, delayMilliseconds, msg);
}
