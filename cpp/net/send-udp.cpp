#include <fmt/format.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include "IpAddr.h"

enum class IpMode {
  IPV4,
  IPV6
};

int main() {
  // CLI =====================================================================================================
  CLI::App app("send-udp");

  std::string urlOrIp = "127.0.0.1";

  // Net Stuff ===============================================================================================
  constexpr IPv4Addr testAddr = "192.168.0.1"_ip4;
  fmt::print("The address is: {}.\n", testAddr.toString());
}
