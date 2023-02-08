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

// Constants /////////////////////////////////////////////////////////////////////////////////////////////////

#define GLVERSION "#version 130"

// SubPlotter ////////////////////////////////////////////////////////////////////////////////////////////////

template<class MessageClass>
struct SubPlotter : Sub<MessageClass, AccumulatingSubListener> {
  using Base = Sub<MessageClass, AccumulatingSubListener>;

  // Constructor =============================================================================================

  SubPlotter(int w, int h) : Base(), width(w), height(h) {
    //
  }

  // Run =====================================================================================================

  inline void run() {
    // ImPlot Setup
    this->setupWindow();
    this->initImgui();

    // ImPlot Loop
    while (!glfwWindowShouldClose(this->win)) {
      glfwPollEvents();
      this->updateImgui();

      // Your stuff goes here
      {
        if (this->listener.queue.size() > 0) {
          // Get stuff from the listener and plot it.
          this->msg = this->listener.queue.pop();
        }
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
      fmt::print("Bad GLFW Init.");
      return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    this->win = glfwCreateWindow(this->width, this->height, "DDS Subscriber Plotter", NULL, NULL);
    if (this->win == NULL) {
      fmt::print("Null window. Bad.");
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

  // Member Variables ========================================================================================
  MessageClass msg;
  GLFWwindow*  win;
  int          width, height;

  ImVector<double> ts, xs, ys, zs, us, vs, ws;
};
