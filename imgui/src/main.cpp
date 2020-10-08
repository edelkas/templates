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

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);

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

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
      int window_x = 0;
      int window_y = 0;
      int window_w = 600;
      int window_h = 500;
      static bool tabs[6] = { true, true, true, true, true, true };
      static bool states[3] = { true, true, true };
      static bool types[3] = { true, false, false };
      ImGui::SetNextWindowPos(ImVec2(window_x, window_y), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSize(ImVec2(window_w, window_h), ImGuiCond_FirstUseEver);
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoCollapse;
      ImGui::Begin(" ", NULL, window_flags);
      ImGui::Columns(7);
      const char* titles[3] = { "Tabs", "Types", "States" };
      const char* s_tabs[6] = { "SI", "S", "SU", "SL", "?", "!" };
      const char* s_types[3] = { "Levels", "Episodes", "Stories" };
      const char* s_states[3] = { "Locked", "Unlocked", "Completed" };
      for (int i = 0; i < 3; i++) {
        ImGui::Text("%s", titles[i]);
        ImGui::NextColumn();
        for (int j = 0; j < 6; j++) {
          if (!(i > 0 && j > 2)) {
            bool entry;
            const char* text;
            switch (i) {
              case 0:
                entry = tabs[j];
                text = s_tabs[j];
                break;
              case 1:
                entry = types[j];
                text = s_types[j];
                break;
              case 2:
                entry = states[j];
                text = s_states[j];
                break;
              default:
                continue;
            }
            ImGui::Checkbox(text, &entry);
            ImGui::NextColumn();
          } else {
            ImGui::NextColumn();
          }
        }
      }
      ImGui::Separator();
      const char* headers[7] = {"ID", "State", "Attempts", "Victories", "Gold", "Score", "Rank"};
      for (int i = 0; i < 7; i++) {
        ImGui::Text("%s", headers[i]);
        ImGui::NextColumn();
      }
      ImGui::Separator();
      for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 7; j++) {
          ImGui::Text("SI-A-00-00");
          ImGui::NextColumn();
        }
      }
      ImGui::Columns(1);
      ImGui::Separator();
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
