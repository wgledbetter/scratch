#include <arpa/inet.h>
#include <fmt/format.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

int main() {
  // CLI =====================================================================================================
  CLI::App app("showip");

  std::string hostname = "localhost";
  app.add_option("--host,-H", hostname, "");

  CLI11_PARSE(app);

  // Net =====================================================================================================
  struct addrinfo hints, *res, *p;
  int             status;
  char            ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family   = AF_UNSPEC;  // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(hostname.c_str(), NULL, &hints, &res)) != 0) {
    fmt::print("getaddrinfo: {}.\n", gai_strerror(status));
    return 2;
  }
  fmt::print("IP addresses for {}:\n\n", hostname.c_str());
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) {  // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
      addr                     = &(ipv4->sin_addr);
      ipver                    = "IPv4";
    } else {  // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
      addr                      = &(ipv6->sin6_addr);
      ipver                     = "IPv6";
    }

    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    fmt::print(" {}: {}\n", ipver, ipstr);
  }

  freeaddrinfo(res);  // free the linked list

  return 0;
}
