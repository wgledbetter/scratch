#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "Sub.h"
#include "SubListeners/AccumulatingSubListener.h"
#include "common/glfwErrorCallback.h"
#include "common/utcTime.h"

// Constants /////////////////////////////////////////////////////////////////////////////////////////////////

#define GLVERSION "#version 130"

// SubPlotter ////////////////////////////////////////////////////////////////////////////////////////////////

template<class MessageClass>
struct SubPlotter : Sub<MessageClass, AccumulatingSubListener> {
  using Base = Sub<MessageClass, AccumulatingSubListener>;

  // Constructor =============================================================================================

  SubPlotter(int w, int h) : width(w), height(h) {};

  // Accessors ===============================================================================================

  inline int getWidth() const {
    return this->width;
  }

  inline int getHeight() const {
    return this->height;
  }

  inline GLFWwindow* getWindowPtr() const {
    return this->win;
  }

  inline double getHistory() const {
    return this->history;
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void setWidth(int w) {
    this->width = w;
  }

  inline void setHeight(int h) {
    this->height = h;
  }

  inline void setWindowPtr(GLFWwindow* w) {
    this->win = w;
  }

  inline void setHistory(double h) {
    this->history = h;
  }

  // Run =====================================================================================================

  inline void run() {
    glfwSetErrorCallback(printing_callback);
    // ImPlot Setup
    this->setupWindow();
    if (this->getWindowPtr() == NULL) {
      fmt::print("Failed to set up window.\n");
      return;
    }
    fmt::print("Set up the window.\n");
    this->initImgui();
    fmt::print("Initialized ImGui.\n");

    double t0 = utcTime();

    // ImPlot Loop
    while (!glfwWindowShouldClose(this->win)) {
      glfwPollEvents();
      this->updateImgui();

      // Your stuff goes here
      {
        bool   boolTrue = true;
        double tNow     = utcTime();

        // fmt::print("GUI Time is {}.\n", tNow);

        this->readMessagesToImVectors();

        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
        ImGui::Begin("DDS Plot", &boolTrue, ImGuiWindowFlags_MenuBar);

        ImGui::Spacing();

        if (ImPlot::BeginPlot("DDS Message X", ImVec2(-1, -1))) {
          ImPlot::SetupAxes("t", "x");

          if (this->ts.size() > 0) {
            ImPlot::SetupAxisLimits(ImAxis_X1, this->ts[0], this->ts.back(), ImGuiCond_Always);
          } else {
            ImPlot::SetupAxisLimits(
                ImAxis_X1, std::max(0.0, tNow - t0 - this->history), tNow - t0, ImGuiCond_Always);
          }

          ImPlot::PlotLine("x", this->ts.begin(), this->xs.begin(), this->xs.size());

          ImPlot::EndPlot();
        }

        ImGui::End();
      }

      this->renderImgui();
      glfwSwapBuffers(this->win);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    this->shutdownImgui();
  }

 protected:
  // Functions nobody else should call =======================================================================

  inline void setupWindow() {
    if (!glfwInit()) {
      fmt::print("Bad GLFW Init.\n");
      return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    this->win = glfwCreateWindow(this->width, this->height, "DDS Subscriber Plotter", NULL, NULL);
    if (this->win == NULL) {
      fmt::print("Null window. Bad.\n");
      return;
    }
    glfwMakeContextCurrent(this->win);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    int w, h;
    glfwGetFramebufferSize(this->win, &w, &h);
    glViewport(0, 0, w, h);
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void initImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(this->win, true);
    ImGui_ImplOpenGL3_Init(GLVERSION);

    ImGui::StyleColorsDark();
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void updateImgui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void renderImgui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void shutdownImgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void readMessagesToImVectors() {
    MessageClass msg;
    while (this->listener.queue.size() > 0) {
      msg = this->listener.queue.pop();

      // fmt::print("Got message: {}.\n", msg.toString());

      this->ts.push_back(msg.getTime());
      this->xs.push_back(msg.getX());
      this->ys.push_back(msg.getY());
      this->zs.push_back(msg.getZ());
      this->us.push_back(msg.getU());
      this->vs.push_back(msg.getV());
      this->ws.push_back(msg.getW());
    }
    // fmt::print("Got all messages for this GUI loop.");
  }

  // Member Variables ========================================================================================
  GLFWwindow* win;
  int         width, height;
  double      history = 20;

  ImVector<double> ts, xs, ys, zs, us, vs, ws;
};
