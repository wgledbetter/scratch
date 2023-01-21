#include "Pub.h"

#include <fmt/format.h>

#include "MySuperCoolMessage.h"

int main() {
  fmt::print("Entered main.\n");
  Pub<MySuperCoolMessage> pub;
  fmt::print("Constructed publisher.\n");
  pub.init();
  fmt::print("Initialized publisher. Running...\n");
  pub.run(15, 1000, "Heyyyy babe u up?");
}
