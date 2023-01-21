#include "Pub.h"

#include <fmt/format.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <string>

#include "MySuperCoolMessage.h"

int main() {
  // Get message from command line ===========================================================================
  CLI::App app("DDS Publisher");

  std::string msg = "CLI default message";
  app.add_option("-m,--message", msg, "The message you'd like to send.");

  int nMsg = 5;
  app.add_option("-n,--number", nMsg, "How many times to send the message.");

  int delayMilliseconds = 1000;
  app.add_option("-d,--delay", delayMilliseconds, "How long to wait between messages.");

  CLI11_PARSE(app);

  // Do DDS Stuff ============================================================================================
  fmt::print("Entered main.\n");
  Pub<MySuperCoolMessage> pub;
  fmt::print("Constructed publisher.\n");
  pub.init();
  fmt::print("Initialized publisher. Running...\n");
  pub.run(nMsg, delayMilliseconds, msg);
}
