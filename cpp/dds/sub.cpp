#include "Sub.h"

#include <fmt/format.h>

#include "MySuperCoolMessage.h"

int main() {
  fmt::print("Entered main.\n");
  Sub<MySuperCoolMessage> sub;
  fmt::print("Constructed subscriber.\n");
  sub.init();
  fmt::print("Initialized subscriber. Running...\n");
  sub.run();
}
