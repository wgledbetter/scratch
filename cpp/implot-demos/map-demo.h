#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <implot_demos/common/Fonts/IconsFontAwesome5.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "implot-image.h"

#define TILE_SERVER "https://a.tile.openstreetmap.org/"
#define TILE_SIZE   256
#define MAX_ZOOM    19
#define MAX_THREADS 2
#define USER_AGENT  "ImMaps/0.1"

#define PI 3.14

namespace fs = std::filesystem;

int long2tilex(double lon, int z) {
  return (int) (floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z) {
  double latrad = lat * PI / 180.0;
  return (int) (floor((1.0 - asinh(tan(latrad)) / PI) / 2.0 * (1 << z)));
}

double tilex2long(int x, int z) {
  return x / (double) (1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z) {
  double n = PI - 2.0 * PI * y / (double) (1 << z);
  return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}

struct TileCoord {
  int z;  // zoom    [0......20]
  int x;  // x index [0...z^2-1]
  int y;  // y index [0...z^2-1]

  inline std::string subdir() const {
    return std::to_string(z) + "/" + std::to_string(x) + "/";
  }
  inline std::string dir() const {
    return "tiles/" + subdir();
  }
  inline std::string file() const {
    return std::to_string(y) + ".png";
  }
  inline std::string path() const {
    return dir() + file();
  }
  inline std::string url() const {
    return TILE_SERVER + subdir() + file();
  }
  inline std::string label() const {
    return subdir() + std::to_string(y);
  }
  std::tuple<ImPlotPoint, ImPlotPoint> bounds() const {
    double n = std::pow(2, z);
    double t = 1.0 / n;
    return {{x * t, (1 + y) * t}, {(1 + x) * t, (y) *t}};
  }
};

bool operator<(const TileCoord &l, const TileCoord &r) {
  if (l.z < r.z)
    return true;
  if (l.z > r.z)
    return false;
  if (l.x < r.x)
    return true;
  if (l.x > r.x)
    return false;
  if (l.y < r.y)
    return true;
  if (l.y > r.y)
    return false;
  return false;
}

enum class TileState {
  Unavailable = 0,
  Loaded,
  Downloading,
  OnDisk
};

using TileImage = Image;

struct Tile {
  Tile() : state(TileState::Unavailable) {};
  Tile(TileState s) : state(s) {};
  TileState state;
  TileImage image;
};

size_t curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
  FILE *stream = (FILE *) userdata;
  if (!stream) {
    printf("No stream\n");
    return 0;
  }
  size_t written = fwrite((FILE *) ptr, size, nmemb, stream);
  return written;
}

class TileManager {
 public:
  TileManager() {
    start_workers();
  }

  inline ~TileManager() {
    {
      std::unique_lock<std::mutex> lock(m_queue_mutex);
      m_stop = true;
    }
    m_condition.notify_all();
    for (std::thread &worker: m_workers)
      worker.join();
  }

  const std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>> &get_region(ImPlotRect view, ImVec2 pixels) {
    double min_x  = std::clamp(view.X.Min, 0.0, 1.0);
    double min_y  = std::clamp(view.Y.Min, 0.0, 1.0);
    double size_x = std::clamp(view.X.Size(), 0.0, 1.0);
    double size_y = std::clamp(view.Y.Size(), 0.0, 1.0);

    double pix_occupied_x   = (pixels.x / view.X.Size()) * size_x;
    double pix_occupied_y   = (pixels.y / view.Y.Size()) * size_y;
    double units_per_tile_x = view.X.Size() * (TILE_SIZE / pix_occupied_x);
    double units_per_tile_y = view.Y.Size() * (TILE_SIZE / pix_occupied_y);

    int    z = 0;
    double r = 1.0 / pow(2, z);
    while (r > units_per_tile_x && r > units_per_tile_y && z < MAX_ZOOM)
      r = 1.0 / pow(2, ++z);

    m_region.clear();
    if (!append_region(z, min_x, min_y, size_x, size_y) && z > 0) {
      append_region(--z, min_x, min_y, size_x, size_y);
      std::reverse(m_region.begin(), m_region.end());
    }
    return m_region;
  }

  std::shared_ptr<Tile> request_tile(TileCoord coord) {
    std::lock_guard<std::mutex> lock(m_tiles_mutex);
    if (m_tiles.count(coord))
      return get_tile(coord);
    else if (fs::exists(coord.path()))
      return load_tile(coord);
    else
      download_tile(coord);
    return nullptr;
  }

  int tiles_loaded() const {
    return m_loads;
  }
  int tiles_downloaded() const {
    return m_downloads;
  }
  int tiles_cached() const {
    return m_loads - m_downloads;
  }
  int tiles_failed() const {
    return m_fails;
  }
  int threads_working() const {
    return m_working;
  }

 private:
  bool append_region(int z, double min_x, double min_y, double size_x, double size_y) {
    int    k     = pow(2, z);
    double r     = 1.0 / k;
    int    xa    = min_x * k;
    int    xb    = xa + ceil(size_x / r) + 1;
    int    ya    = min_y * k;
    int    yb    = ya + ceil(size_y / r) + 1;
    xb           = std::clamp(xb, 0, k);
    yb           = std::clamp(yb, 0, k);
    bool covered = true;
    for (int x = xa; x < xb; ++x) {
      for (int y = ya; y < yb; ++y) {
        TileCoord             coord {z, x, y};
        std::shared_ptr<Tile> tile = request_tile(coord);
        m_region.push_back({coord, tile});
        if (tile == nullptr || tile->state != TileState::Loaded)
          covered = false;
      }
    }
    return covered;
  }

  void download_tile(TileCoord coord) {
    auto dir = coord.dir();
    fs::create_directories(dir);
    if (fs::exists(dir)) {
      m_tiles[coord] = std::make_shared<Tile>(TileState::Downloading);
      {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.emplace(coord);
      }
      m_condition.notify_one();
    }
  }

  std::shared_ptr<Tile> get_tile(TileCoord coord) {
    if (m_tiles[coord]->state == TileState::Loaded)
      return m_tiles[coord];
    else if (m_tiles[coord]->state == TileState::OnDisk)
      return load_tile(coord);
    return nullptr;
  }

  std::shared_ptr<Tile> load_tile(TileCoord coord) {
    auto path = coord.path();
    if (!m_tiles.count(coord))
      m_tiles[coord] = std::make_shared<Tile>();
    if (m_tiles[coord]->image.LoadFromFile(path.c_str())) {
      m_tiles[coord]->state = TileState::Loaded;
      m_loads++;
      return m_tiles[coord];
    }
    m_fails++;
    printf("TileManager[00]: Failed to load \"%s\"\n", path.c_str());
    if (!fs::remove(path))
      printf("TileManager[00]: Failed to remove \"%s\"\n", path.c_str());
    printf("TileManager[00]: Removed \"%s\"\n", path.c_str());
    m_tiles.erase(coord);
    return nullptr;
  }

  void start_workers() {
    for (int thrd = 1; thrd < MAX_THREADS + 1; ++thrd) {
      m_workers.emplace_back([this, thrd] {
        printf("TileManager[%02d]: Thread started\n", thrd);
        CURL *curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        for (;;) {
          TileCoord coord;
          {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_condition.wait(lock, [this] { return m_stop || !m_queue.empty(); });
            if (m_stop && m_queue.empty()) {
              curl_easy_cleanup(curl);
              printf("TileManager[%02d]: Thread terminated\n", thrd);
              return;
            }
            coord = std::move(m_queue.front());
            m_queue.pop();
          }
          m_working++;
          bool  success = true;
          auto  dir     = coord.dir();
          auto  path    = coord.path();
          auto  url     = coord.url();
          FILE *fp      = fopen(coord.path().c_str(), "wb");
          if (fp) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            CURLcode cc = curl_easy_perform(curl);
            fclose(fp);
            if (cc != CURLE_OK) {
              printf("TileManager[%02d]: Failed to download: \"%s\"\n", thrd, url.c_str());
              long rc = 0;
              curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
              if (!((rc == 200 || rc == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
                printf("TileManager[%02d]: Response code: %d\n", thrd, rc);
              fs::remove(coord.path());
              success = false;
            }
          } else {
            printf("TileManager[%02d]: Failed to open or create file \"%s\"\n", thrd, path.c_str());
            success = false;
          }
          if (success) {
            m_downloads++;
            std::lock_guard<std::mutex> lock(m_tiles_mutex);
            m_tiles[coord]->state = TileState::OnDisk;
          } else {
            m_fails++;
            std::lock_guard<std::mutex> lock(m_tiles_mutex);
            m_tiles.erase(coord);
          }
          m_working--;
        }
      });
    }
  }

  std::atomic<int>                                         m_loads     = 0;
  std::atomic<int>                                         m_downloads = 0;
  std::atomic<int>                                         m_fails     = 0;
  std::atomic<int>                                         m_working   = 0;
  std::map<TileCoord, std::shared_ptr<Tile>>               m_tiles;
  std::mutex                                               m_tiles_mutex;
  std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>> m_region;
  std::vector<std::thread>                                 m_workers;
  std::queue<TileCoord>                                    m_queue;
  std::mutex                                               m_queue_mutex;
  std::condition_variable                                  m_condition;
  bool                                                     m_stop = false;
};


struct MapApp {
  // Constructor =============================================================================================
  MapApp(std::string title, int w, int h) {
    if (!glfwInit()) {
      abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    this->win = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (this->win == NULL) {
      fmt::print("Failed to make map window.");
      abort();
    }
    glfwMakeContextCurrent(this->win);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glfwGetFramebufferSize(this->win, &w, &h);
    glViewport(0, 0, w, h);
  }

  // Destructor ==============================================================================================
  ~MapApp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->win);
    glfwTerminate();
  }

  // init ====================================================================================================
  inline void init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    const char *glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(this->win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark();
  }

  // update ==================================================================================================
  inline void update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static int  renders = 0;
    static bool debug   = false;
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
      debug = !debug;

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetWindowSize());
    ImGui::Begin("Map", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    if (debug) {
      int wk = mngr.threads_working();
      int dl = mngr.tiles_downloaded();
      int ld = mngr.tiles_loaded();
      int ca = mngr.tiles_cached();
      int fa = mngr.tiles_failed();
      ImGui::Text(
          "FPS: %.2f    Working: %d    Downloads: %d    Loads: %d    Caches: %d    Fails: %d    Renders: %d",
          ImGui::GetIO().Framerate,
          wk,
          dl,
          ld,
          ca,
          fa,
          renders);
    }

    ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels
                               | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Foreground;
    if (ImPlot::BeginPlot("##Map", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
      ImPlot::SetupAxes(NULL, NULL, ax_flags, ax_flags | ImPlotAxisFlags_Invert);
      ImPlot::SetupAxesLimits(0, 1, 0, 1);

      auto  pos    = ImPlot::GetPlotPos();
      auto  size   = ImPlot::GetPlotSize();
      auto  limits = ImPlot::GetPlotLimits();
      auto &region = mngr.get_region(limits, size);
      renders      = 0;
      if (debug) {
        float ys[] = {1, 1};
        ImPlot::SetNextFillStyle({1, 0, 0, 1}, 0.5f);
        ImPlot::PlotShaded("##Bounds", ys, 2);
      }
      for (auto &pair: region) {
        TileCoord             coord = pair.first;
        std::shared_ptr<Tile> tile  = pair.second;
        auto [bmin, bmax]           = coord.bounds();
        if (tile != nullptr) {
          auto col = debug
                         ? ((coord.x % 2 == 0 && coord.y % 2 != 0) || (coord.x % 2 != 0 && coord.y % 2 == 0))
                               ? ImVec4(1, 0, 1, 1)
                               : ImVec4(1, 1, 0, 1)
                         : ImVec4(1, 1, 1, 1);
          ImPlot::PlotImage("##Tiles", (void *) (intptr_t) tile->image.ID, bmin, bmax, {0, 0}, {1, 1}, col);
        }
        if (debug)
          ImPlot::PlotText(coord.label().c_str(), (bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2);
        renders++;
      }
      ImPlot::PushPlotClipRect();
      static const char *label      = "OpenStreetMap Contributors";
      auto               label_size = ImGui::CalcTextSize(label);
      auto               label_off  = ImPlot::GetStyle().MousePosPadding;
      ImPlot::GetPlotDrawList()->AddText(
          {pos.x + label_off.x, pos.y + size.y - label_size.y - label_off.y}, IM_COL32_BLACK, label);
      ImPlot::PopPlotClipRect();
      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  // render ==================================================================================================
  inline void render() {
    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(this->win, &w, &h);
    glViewport(0, 0, w, h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  // Member Variables ========================================================================================
  TileManager mngr;

  GLFWwindow *win;
};
