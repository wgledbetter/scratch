#include "map-demo.h"


// Constants /////////////////////////////////////////////////////////////////////////////////////////////////

#define WIDTH  1280
#define HEIGHT 720

// Main //////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  // Class setup =============================================================================================

  MapApp ma("Maple", WIDTH, HEIGHT);

  ma.init();

  double t0 = ImGui::GetTime();

  while (!glfwWindowShouldClose(ma.win)) {
    glfwPollEvents();
    ma.update();
    ma.render();

    glfwSwapBuffers(ma.win);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  return 0;
}
