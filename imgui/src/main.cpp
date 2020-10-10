#include <stdio.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <gl3w/gl3w.h>  // Initialize with gl3wInit()
#include <glfw/glfw3.h> // Include glfw3.h after our OpenGL definitions

#define NAME   "NProfiler"
#define WIDTH  1280
#define HEIGHT 720

// Win32 exceptions
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void create_window(int window_x, int window_y, int window_w, int window_h) {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_AlwaysUseWindowPadding;
  ImGui::SetNextWindowPos(ImVec2(window_x, window_y), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(window_w, window_h), ImGuiCond_FirstUseEver);
  ImGui::Begin(" ", NULL, window_flags);
}

int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
  return 1;

  // Decide GL+GLSL versions
  #ifdef __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
  #else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
  #endif
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, NAME, NULL, NULL);
  if (window == NULL)
  return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader
  if (gl3wInit() != 0)
  {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Don't create ini config file
  io.IniFilename = NULL;

  // Our state
  ImVec4 clear_color = ImVec4(0.0586f, 0.0586f, 0.0586f, 0.9375f);

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      /* Data */
      static bool tabs[6]     = { true, true, true, true, true, true };
      static bool states[3]   = { true, true, true };
      static bool types[3]    = { true, false, false };
      static bool modes[4]    = { true, false, false, false };
      static bool orders[7]   = { true, false, false, false, false, false, false };
      static bool rev_order   = false;
      const char* titles[4]   = { "Tabs", "Types", "Modes", "States" };
      const char* s_tabs[6]   = { "SI", "S", "SU", "SL", "?", "!" };
      const char* s_types[3]  = { "Levels", "Episodes", "Stories" };
      const char* s_modes[4]  = { "Solo", "Coop", "Race", "Hardcore" };
      const char* s_states[3] = { "Locked", "Unlocked", "Completed" };
      const char* headers[7]  = { "ID", "State", "Atts.", "Vics.", "Gold", "Score", "Rank" };
      int window_x            = 0;
      int window_y            = 0;
      int window_w            = 600;
      int window_h            = 500;
      int col_width           = 75;
      int col_offset          = 10;

      /* Header */
      create_window(window_x, window_y, window_w, window_h);
      ImGui::Text("SAVEFILE ANALYSIS"); ImGui::SameLine();
      ImGui::SmallButton("Open savefile"); ImGui::SameLine();
      ImGui::Text("Username");

      /* Checkboxes */
      ImGui::Columns(4);
      ImGui::Separator();
      for (int i = 0; i < 4; i++) {
        ImGui::Text("%s", titles[i]); ImGui::NextColumn();
      }
      ImGui::Separator();
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
      for (int i = 0; i < 6; i++) {
                   ImGui::Checkbox(s_tabs[i],   &tabs[i]);   ImGui::NextColumn();
        if (i < 3) ImGui::Checkbox(s_types[i],  &types[i]);  ImGui::NextColumn();
        if (i < 4) ImGui::Checkbox(s_modes[i],  &modes[i]);  ImGui::NextColumn();
        if (i < 3) ImGui::Checkbox(s_states[i], &states[i]); ImGui::NextColumn();
      }
      ImGui::PopStyleVar(2);

      /* Table headers */
      ImGui::Columns(7);
      ImGui::SetColumnWidth(0, col_width + col_offset);
      ImGui::Separator();
      for (int i = 0; i < 7; i++) {
        bool highlight = orders[i];
        if (highlight) {
          int c = rev_order ? 0 : 2;
          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(c / 7.0f, 0.6f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(c / 7.0f, 0.7f, 0.7f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(c / 7.0f, 0.8f, 0.8f));
        }
        if (ImGui::SmallButton("^")) {
          if (highlight) {
            rev_order = !rev_order;
          } else {
            for (int j = 0; j < 7; j++) orders[j] = false;
            orders[i] = true;
            rev_order = false;
          }
        }
        if (highlight) ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::Text("%s", headers[i]);
        ImGui::NextColumn();
      }

      /* Table contents */
      ImGui::Columns(1);
      ImGui::Separator();
      ImGui::BeginChild(" ", ImVec2(window_w - 20, window_h - 225));
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
      ImGui::Columns(7);
      ImGui::SetColumnWidth(0, col_width);
      for (int i = 0; i < 2000; i++) {
        for (int j = 0; j < 7; j++) {
          if (j == 0) {
            ImGui::Text("SI-A-00-00");
          } else {
            ImGui::Text(" (%d, %d)", i, j);
          }
          ImGui::NextColumn();
        }
      }
      ImGui::PopStyleVar();
      ImGui::EndChild();

      /* Table footer */
      ImGui::Columns(7);
      ImGui::SetColumnWidth(0, col_width + col_offset);
      ImGui::Separator();
      ImGui::Text("Total"); ImGui::NextColumn(); ImGui::NextColumn();
      ImGui::Text("Many"); ImGui::NextColumn();
      ImGui::Text("Lots"); ImGui::NextColumn();
      ImGui::Text("Abundant"); ImGui::NextColumn();
      ImGui::Text("Seconds"); ImGui::NextColumn(); ImGui::NextColumn();
      ImGui::Text("Avg."); ImGui::NextColumn(); ImGui::NextColumn();
      ImGui::Text("Many"); ImGui::NextColumn();
      ImGui::Text("Lots"); ImGui::NextColumn();
      ImGui::Text("Abundant"); ImGui::NextColumn();
      ImGui::Text("Seconds"); ImGui::NextColumn();
      ImGui::Text("0th"); ImGui::NextColumn();
      ImGui::End();
    }

    {
      create_window(0, HEIGHT - 24, WIDTH, 24);
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
