#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot_demo.cpp>
#include <memory>

// Constants /////////////////////////////////////////////////////////////////////////////////////////////////

#define WIDTH  1280
#define HEIGHT 720

#define GLVERSION "#version 130"

// Class from tutorial video /////////////////////////////////////////////////////////////////////////////////

struct DoImGuiStuff {

  // Constructor =============================================================================================

  DoImGuiStuff(GLFWwindow* win_) : win(win_) {};

  // init ====================================================================================================

  inline void init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(this->win, true);
    ImGui_ImplOpenGL3_Init(GLVERSION);

    ImGui::StyleColorsDark();
  }

  // update ==================================================================================================

  inline void update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  // render ==================================================================================================

  inline void render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  // shutdown ================================================================================================

  inline void shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

 protected:
  // Member Variables ========================================================================================
  GLFWwindow* win;
};

// Main //////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  // Window Setup ============================================================================================

  if (!glfwInit()) {
    fmt::print("Bad init.\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "Just show me something plz...", NULL, NULL);
  if (win == NULL) {
    fmt::print("Null window.\n");
    return 1;
  }
  glfwMakeContextCurrent(win);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0, 0.0, 0.0, 1.0);

  int width, height;
  glfwGetFramebufferSize(win, &width, &height);
  glViewport(0, 0, width, height);

  // Class setup =============================================================================================

  DoImGuiStuff digs(win);

  digs.init();

  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();

    digs.update();
    ImPlot::ShowDemoWindow();
    digs.render();

    glfwSwapBuffers(win);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  digs.shutdown();

  return 0;
}
