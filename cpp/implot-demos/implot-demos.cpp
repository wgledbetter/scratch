#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cmath>
#include <implot_demo.cpp>
#include <memory>
#include <random>

#include "map-demo.h"

// Constants /////////////////////////////////////////////////////////////////////////////////////////////////

#define WIDTH  1280
#define HEIGHT 720

#define GLVERSION "#version 130"

#define PLOT_HISTORY 10

// Functions to plot /////////////////////////////////////////////////////////////////////////////////////////

inline double f(double x) {
  return std::sin(x);
}

// Custom ImPlot Function ////////////////////////////////////////////////////////////////////////////////////

inline void myImPlotStuff(double t0, ImVector<double>& xVec, ImVector<double>& yVec) {
  bool boolTrue = true;

  double tNow = ImGui::GetTime();

  // Window setup
  ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
  ImGui::Begin("PMP", &boolTrue, ImGuiWindowFlags_MenuBar);

  ImGui::Spacing();

  // Plot Core
  if (ImPlot::BeginPlot("Plotty McPlotface", ImVec2(-1, -1))) {
    // Plot Name, Plot Size (-1 = fit window)

    ImPlot::SetupAxes("t", "f(t)");
    ImPlot::SetupAxisLimits(ImAxis_X1, std::max(0.0, tNow - t0 - PLOT_HISTORY), tNow - t0, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1, ImGuiCond_Always);
    ImPlot::PlotLine("f", xVec.begin(), yVec.begin(), xVec.size());
    ImPlot::EndPlot();
  }

  ImGui::End();
}

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

  // Plot data setup =========================================================================================

  std::random_device               rd;
  std::mt19937                     rng(rd());
  std::uniform_real_distribution<> dist(-1, 1);

  ImVector<double> x, y;
  x.push_back(0);
  y.push_back(0);

  // Class setup =============================================================================================

  DoImGuiStuff digs(win);
  // MapApp ma("Maple", WIDTH, HEIGHT);

  digs.init();
  // ma.init();

  double t0 = ImGui::GetTime();

  // while (!glfwWindowShouldClose(ma.win)) {
  while (!glfwWindowShouldClose(win)) {
    if (dist(rng) > 0.6) {
      // Simulates weirdly-timed data
      x.push_back(ImGui::GetTime() - t0);
      y.push_back(f(x.back()));
    }

    glfwPollEvents();

    digs.update();
    // ma.update();

    // ImPlot stuff
    ImPlot::ShowDemoWindow();
    myImPlotStuff(t0, x, y);

    digs.render();
    // ma.render();

    // glfwSwapBuffers(ma.win);
    glfwSwapBuffers(win);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  digs.shutdown();

  return 0;
}
