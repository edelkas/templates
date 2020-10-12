#include <stdio.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/gl3w.h>  // Initialize with gl3wInit()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

#define NAME   "N++ Control Center"
#define MAJOR  "1"
#define MINOR  "0"
#define PATCH  "0"
#define WIDTH  1280
#define HEIGHT 650

// Win32 exceptions
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void Tooltip(const char* desc) {
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

static void HelpMarker(const char* desc) {
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(?)");
  Tooltip(desc);
}

static void RangeInt(int* counter, int padding, int limitinf, int limitsup, const char* header = "") {
  ImGui::PushButtonRepeat(true);
  if (ImGui::ArrowButton("##left", ImGuiDir_Left) && *counter > limitinf) *counter--;
  ImGui::SameLine();
  ImGui::Text("%s%0*d", header, padding, *counter);
  ImGui::SameLine();
  if (ImGui::ArrowButton("##right", ImGuiDir_Right) && *counter < limitsup) *counter++;
  ImGui::PopButtonRepeat();
}

static void create_window(const char* window_name, int window_x, int window_y, int window_w, int window_h) {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoScrollbar |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_AlwaysUseWindowPadding;
  ImGui::SetNextWindowPos(ImVec2(window_x, window_y), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(window_w, window_h), ImGuiCond_FirstUseEver);
  ImGui::Begin(window_name, NULL, window_flags);
}

static void make_table(const char* name, int rows, int cols, const char** row_headers, const char** col_headers) {
  ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter;
  if (ImGui::BeginTable(name, cols, flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * rows))) {
    for (int i = 0; i < cols; i++) {
      ImGui::TableSetupColumn(col_headers[i]);
    }
    ImGui::TableHeadersRow();
    for (int i = 0; i < rows - 1; i++) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%s", row_headers[i]);
      for (int j = 0; j < cols - 1; j++) {
        ImGui::TableNextColumn();
        ImGui::Text("100");
      }
    }
    ImGui::EndTable();
  }
}

static void make_leaderboard(const char* name, const char** headers) {
  ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg;
  if (ImGui::BeginTable(name, 3, flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 21))) {
    ImGui::TableSetupColumn(headers[0], ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn(headers[1], ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn(headers[2], ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();
    for (int i = 0; i < 20; i++) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%d", i);
      ImGui::TableNextColumn();
      ImGui::Text("%.25s", "EddyMataGallosEddyMataGallos");
      ImGui::TableNextColumn();
      ImGui::Text("100.000");
    }
    ImGui::EndTable();
  }
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

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Don't create ini config file
  io.IniFilename = NULL;

  // Background
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

    const char* s_tabs[6]   = { "SI", "S", "SU", "SL", "?", "!" };
    const char* s_types[3]  = { "Levels", "Episodes", "Stories" };
    const char* s_modes[4]  = { "Solo", "Coop", "Race", "Hardcore" };
    int win1_x = 0;
    int win1_y = 0;
    int win1_w = 640;
    int win1_h = ImGui::GetTextLineHeightWithSpacing() * 36.5;
    int win2_x = win1_x + win1_w;
    int win2_y = 0;
    int win2_w = 640;
    int win2_h = ImGui::GetTextLineHeightWithSpacing() * 36.5;
    int win3_x = 0;
    int win3_y = win1_x + win1_h;
    int win3_w = WIDTH;
    int win3_h = HEIGHT - win1_h;

    {
      create_window("scores", win1_x, win1_y, win1_w, win1_h);
      ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "HIGHSCORE ANALYSIS"); ImGui::SameLine();
      ImGui::Text("Loaded:"); ImGui::SameLine();
      ImGui::Text("None"); ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      HelpMarker("This section will analyze the highscores from the server. \
                  You first have to load some scores, either by downloading them, \
                  or by loading them from a file. You can then save these scores \
                  to be able to load them at a later point (recommended).");
      ImGui::SmallButton("Download scores"); ImGui::SameLine();
      ImGui::SmallButton("Load scores"); ImGui::SameLine();
      ImGui::SmallButton("Save scores");

      char buf[32];
      sprintf(buf, "%d/%d", 0, 2550);
      ImGui::ProgressBar(0.0f, ImVec2(-1.0f, 0.0f), buf);

      ImGui::Separator();

      ImGuiTableFlags flags = 0;
      if (ImGui::BeginTable("highscoring", 2, flags)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::Text("          PERSONAL HIGHSCORING STATS");
        const char* row_headers[7] = { "SI", "S", "SU", "SL", "?", "!", "Total" };
        const char* col_headers[5] = { "Tabs", "Top20", "Top10", "Top5", "0th" };
        ImGuiTabBarFlags tab_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("stat_tabs", tab_flags)) {
          ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip);
          Tooltip("Solo includes both levels and episodes from solo mode, that \
                   is, the standard highscoring metric used in the community.");
          if (ImGui::BeginTabItem("Solo")) {
            make_table("solo", 8, 5, row_headers, col_headers);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Levels")) {
            make_table("levels", 8, 5, row_headers, col_headers);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Episodes")) {
            make_table("episodes", 8, 5, row_headers, col_headers);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Stories")) {
            make_table("stories", 8, 5, row_headers, col_headers);
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }

        const char* col_headers2[5] = { "Tabs", "Level", "Episode", "Story", "Total" };
        if (ImGui::BeginTabBar("varied_tabs", tab_flags)) {
          ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip);
          Tooltip("'Total score' adds up all your scores in each tab. 'Points' \
                   awards points for each highscore you have: 20 points for a 0th, \
                   19 for 1st... up to 1 for 19th.");
          if (ImGui::BeginTabItem("Total score")) {
            make_table("total_score", 8, 5, row_headers, col_headers2);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Points")) {
            make_table("points", 8, 5, row_headers, col_headers2);
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }

        static float arr[] = {
           0.0f,   1.0f,  5.0f, 12.0f, 17.0f,
          35.0f,  56.0f, 43.0f, 43.0f, 30.0f,
          25.0f,  28.0f, 37.0f, 68.0f, 78.0f,
          89.0f, 100.0f, 90.0f, 80.0f, 85.0f
        };
        ImGui::PlotHistogram("", arr, IM_ARRAYSIZE(arr), 0, NULL, 0, 100, ImVec2(ImGui::GetContentRegionAvail().x * 1.0f, 200));

        ImGui::TableNextColumn();

        ImGui::Text("          GLOBAL HIGHSCORING STATS");
        if (ImGui::BeginTabBar("global_tabs", tab_flags)) {
          if (ImGui::BeginTabItem("Leaderboards")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            if (ImGui::BeginTable("g_leaderboards", 2, ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_BordersInnerV)) {
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Type"); ImGui::TableNextColumn();
              static int leaderboard_type = 0;
              ImGui::RadioButton("Level", &leaderboard_type, 0); ImGui::SameLine();
              ImGui::RadioButton("Episode", &leaderboard_type, 1); ImGui::SameLine();
              ImGui::RadioButton("Story", &leaderboard_type, 2);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Tab"); ImGui::TableNextColumn();
              static int leaderboard_tab = 0;
              ImGui::RadioButton("SI", &leaderboard_tab, 0); ImGui::SameLine();
              ImGui::RadioButton("S",  &leaderboard_tab, 1); ImGui::SameLine();
              ImGui::RadioButton("SU", &leaderboard_tab, 2); ImGui::SameLine();
              ImGui::RadioButton("SL", &leaderboard_tab, 3); ImGui::SameLine();
              ImGui::RadioButton("?",  &leaderboard_tab, 4); ImGui::SameLine();
              ImGui::RadioButton("!",  &leaderboard_tab, 5);

              // TODO: Disable this if we're on stories
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Row"); ImGui::TableNextColumn();
              static int leaderboard_row = 0;
              ImGui::RadioButton("A", &leaderboard_row, 0); ImGui::SameLine();
              ImGui::RadioButton("B", &leaderboard_row, 1); ImGui::SameLine();
              ImGui::RadioButton("C", &leaderboard_row, 2); ImGui::SameLine();
              ImGui::RadioButton("D", &leaderboard_row, 3); ImGui::SameLine();
              ImGui::RadioButton("E", &leaderboard_row, 4); ImGui::SameLine();
              ImGui::RadioButton("X", &leaderboard_row, 5);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Column"); ImGui::TableNextColumn();
              static int leaderboard_col = 0;
              ImGui::PushButtonRepeat(true);
              if (ImGui::ArrowButton("##left", ImGuiDir_Left) && leaderboard_col > 0) leaderboard_col--;
              ImGui::SameLine();
              ImGui::Text("%02d", leaderboard_col);
              ImGui::SameLine();
              if (ImGui::ArrowButton("##right", ImGuiDir_Right) && leaderboard_col < 19) leaderboard_col++;
              ImGui::PopButtonRepeat();

              // TODO: Disable this if we're on episodes or stories
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Level"); ImGui::TableNextColumn();
              static int leaderboard_level = 0;
              ImGui::RadioButton("00", &leaderboard_level, 0); ImGui::SameLine();
              ImGui::RadioButton("01", &leaderboard_level, 1); ImGui::SameLine();
              ImGui::RadioButton("02", &leaderboard_level, 2); ImGui::SameLine();
              ImGui::RadioButton("03", &leaderboard_level, 3); ImGui::SameLine();
              ImGui::RadioButton("04", &leaderboard_level, 4);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");

              ImGui::EndTable();
            }
            static int leaderboard_board;
            ImGui::Text(" "); ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.35f);
            ImGui::PushButtonRepeat(true);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left) && leaderboard_board > 0) leaderboard_board--;
            ImGui::SameLine();
            ImGui::Text("%010d", leaderboard_board); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right) && leaderboard_board < 599) leaderboard_board++;
            ImGui::PopButtonRepeat();
            ImGui::PopStyleVar();

            const char* col_headers3[3] = { "Rank", "Player", "Score" };
            make_leaderboard("leaderboards", col_headers3);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Rankings")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            if (ImGui::BeginTable("g_rankings", 2, ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_BordersInnerV)) {
              static bool tabs[6] = { true, true, true, true, true, true };
              static bool types[3] = { true, true, false };
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Types"); ImGui::TableNextColumn();
              ImGui::Checkbox("Levels",   &types[0]); ImGui::SameLine();
              ImGui::Checkbox("Episodes", &types[1]); ImGui::SameLine();
              ImGui::Checkbox("Stories",  &types[2]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Tabs"); ImGui::TableNextColumn();
              ImGui::Checkbox("SI", &tabs[0]); ImGui::SameLine();
              ImGui::Checkbox("S",  &tabs[1]); ImGui::SameLine();
              ImGui::Checkbox("SU", &tabs[2]); ImGui::SameLine();
              ImGui::Checkbox("SL", &tabs[3]); ImGui::SameLine();
              ImGui::Checkbox("?",  &tabs[4]); ImGui::SameLine();
              ImGui::Checkbox("!",  &tabs[5]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Ranking"); ImGui::TableNextColumn();
              static int ranking = 0;
              static int ranking_rank = 3;
              ImGui::RadioButton("0ths",           &ranking, 0); ImGui::SameLine();
              ImGui::RadioButton("Top20s",         &ranking, 1); ImGui::SameLine();
              ImGui::RadioButton("Top10s",         &ranking, 2); ImGui::SameLine();
              ImGui::RadioButton("Top5s",          &ranking, 3);
              ImGui::RadioButton("Total score",    &ranking, 4); ImGui::SameLine();
              ImGui::RadioButton("Total points",   &ranking, 5);
              ImGui::RadioButton("Avg. points",    &ranking, 6); ImGui::SameLine();
              ImGui::RadioButton("Other:",         &ranking, 7); ImGui::SameLine();
              RangeInt(&ranking_rank, 2, 0, 19, "Top "); // TODO: Disable this if ranking_rank != 7

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Ties"); ImGui::TableNextColumn();
              static int ranking_ties = 0;
              ImGui::RadioButton("Yes", &ranking_ties, 0); ImGui::SameLine();
              ImGui::RadioButton("No",  &ranking_ties, 1);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");

              ImGui::EndTable();
            }
            ImGui::PopStyleVar();
            const char* col_headers3[3] = { "Rank", "Player", "Count" };
            make_leaderboard("rankings", col_headers3);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Spreads")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            if (ImGui::BeginTable("g_spreads", 2, ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_BordersInnerV)) {
              static bool tabs[6] = { true, true, true, true, true, true };
              static bool types[3] = { true, true, false };
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Types"); ImGui::TableNextColumn();
              ImGui::Checkbox("Levels",   &types[0]); ImGui::SameLine();
              ImGui::Checkbox("Episodes", &types[1]); ImGui::SameLine();
              ImGui::Checkbox("Stories",  &types[2]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Tabs"); ImGui::TableNextColumn();
              ImGui::Checkbox("SI", &tabs[0]); ImGui::SameLine();
              ImGui::Checkbox("S",  &tabs[1]); ImGui::SameLine();
              ImGui::Checkbox("SU", &tabs[2]); ImGui::SameLine();
              ImGui::Checkbox("SL", &tabs[3]); ImGui::SameLine();
              ImGui::Checkbox("?",  &tabs[4]); ImGui::SameLine();
              ImGui::Checkbox("!",  &tabs[5]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Order"); ImGui::TableNextColumn();
              static int spread_order = 0;
              ImGui::RadioButton("Biggest", &spread_order, 0); ImGui::SameLine();
              ImGui::RadioButton("Smallest",  &spread_order, 1);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Range"); ImGui::TableNextColumn();
              static int spread_range_inf = 0;
              static int spread_range_sup = 19;
              ImGui::Text("From "); ImGui::SameLine();
              RangeInt(&spread_range_inf, 2, 0, 19, ""); ImGui::SameLine();
              ImGui::Text(" to "); ImGui::SameLine();
              RangeInt(&spread_range_sup, 2, 0, 19, "");

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text(" ");

              ImGui::EndTable();
            }
            ImGui::PopStyleVar();
            const char* col_headers3[3] = { "Rank", "Player", "Time" };
            make_leaderboard("spreads", col_headers3);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Lists")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            if (ImGui::BeginTable("g_lists", 2, ImGuiTableFlags_SizingPolicyFixedX | ImGuiTableFlags_BordersInnerV)) {
              static bool tabs[6] = { true, true, true, true, true, true };
              static bool types[3] = { true, true, false };
              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Types"); ImGui::TableNextColumn();
              ImGui::Checkbox("Levels",   &types[0]); ImGui::SameLine();
              ImGui::Checkbox("Episodes", &types[1]); ImGui::SameLine();
              ImGui::Checkbox("Stories",  &types[2]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Tabs"); ImGui::TableNextColumn();
              ImGui::Checkbox("SI", &tabs[0]); ImGui::SameLine();
              ImGui::Checkbox("S",  &tabs[1]); ImGui::SameLine();
              ImGui::Checkbox("SU", &tabs[2]); ImGui::SameLine();
              ImGui::Checkbox("SL", &tabs[3]); ImGui::SameLine();
              ImGui::Checkbox("?",  &tabs[4]); ImGui::SameLine();
              ImGui::Checkbox("!",  &tabs[5]);

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("List"); ImGui::TableNextColumn();
              static int list = 0;
              static int list_rank = 3;
              if (ImGui::BeginTable("g_lists_internal", 2, ImGuiTableFlags_SizingPolicyFixedX)) {
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::RadioButton("Top20s",         &list, 0); ImGui::TableNextColumn();
                ImGui::RadioButton("Missing Top20s", &list, 1);
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::RadioButton("Top10s",         &list, 2); ImGui::TableNextColumn();
                ImGui::RadioButton("Missing Top10s", &list, 3);
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::RadioButton("Top5s",          &list, 4); ImGui::TableNextColumn();
                ImGui::RadioButton("Missing Top5s",  &list, 5);
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::RadioButton("0ths",           &list, 6); ImGui::TableNextColumn();
                ImGui::RadioButton("Missing 0ths",   &list, 7);
                ImGui::EndTable();
              }
              ImGui::RadioButton("Other:",         &list, 8); ImGui::SameLine();
              static int list_range_inf = 0;
              static int list_range_sup = 19;
              ImGui::Text("From "); ImGui::SameLine();
              RangeInt(&list_range_inf, 2, 0, 19, ""); ImGui::SameLine();
              ImGui::Text(" to "); ImGui::SameLine();
              RangeInt(&list_range_sup, 2, 0, 19, ""); // TODO: Disable this if ranking_rank != 8

              ImGui::TableNextRow(); ImGui::TableNextColumn();
              ImGui::Text("Ties"); ImGui::TableNextColumn();
              static int ranking_ties = 0;
              ImGui::RadioButton("Yes", &ranking_ties, 0); ImGui::SameLine();
              ImGui::RadioButton("No",  &ranking_ties, 1);

              ImGui::EndTable();
            }
            ImGui::PopStyleVar();
            const char* col_headers4[3] = { "Rank", "Player", "Score" };
            make_leaderboard("lists", col_headers4);
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }
        ImGui::EndTable();
      }

      ImGui::End();
    }

    {
      /* Data */
      static bool tabs[6]     = { true, true, true, true, true, true };
      static bool states[3]   = { true, true, true };
      static bool types[3]    = { true, false, false };
      static bool modes[4]    = { true, false, false, false };
      static bool orders[7]   = { true, false, false, false, false, false, false };
      static bool rev_order   = false;
      const char* titles[4]   = { "Tabs", "Types", "Modes", "States" };
      const char* s_states[3] = { "Locked", "Unlocked", "Completed" };
      const char* headers[7]  = { "ID", "State", "Atts.", "Vics.", "Gold", "Score", "Rank" };
      int table_lines         = 25;

      /* Header */
      create_window("savefile", win2_x, win2_y, win2_w, win2_h);
      ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SAVEFILE ANALYSIS"); ImGui::SameLine();
      ImGui::Text("Loaded:"); ImGui::SameLine();
      ImGui::Text("None"); ImGui::SameLine(ImGui::GetWindowWidth() - 30);
      HelpMarker("This section will analyze your savefile and provide stats. \
                  You first need to load it by clicking on 'Open savefile'. \
                  If you don't know where the savefile is located, click on the 'Help' menu.");
      ImGui::SmallButton("Open savefile");

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

      /* Table */
      ImGui::Columns(1);
      static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_MultiSortable
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
            | ImGuiTableFlags_ScrollY;
      if (ImGui::BeginTable("blocks", 7, table_flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * table_lines), 0.0f)) {
        /* Create columns */
        ImGui::TableSetupColumn(headers[0], ImGuiTableColumnFlags_DefaultSort          | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn(headers[1], ImGuiTableColumnFlags_NoSort               | ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupColumn(headers[2], ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupColumn(headers[3], ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupColumn(headers[4], ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupColumn(headers[5], ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupColumn(headers[6],                                              ImGuiTableColumnFlags_WidthFixed,   -1.0f);
        ImGui::TableSetupScrollFreeze(0, 1); // Header always visible after scrolling

        /* Sort data */
        if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs()) {
          if (sorts_specs->SpecsDirty) { // Detect if sorting is required
            // Sort here!
            sorts_specs->SpecsDirty = false;
          }
        }

        /* Display data */
        ImGui::TableHeadersRow();
        for (int i = 0; i < 2000; i++) {
          ImGui::TableNextRow();
          for (int j = 0; j < 7; j++) {
            ImGui::TableNextColumn();
            if (j == 0) {
              ImGui::Text("SI-A-00-00");
            } else {
              ImGui::Text(" (%d, %d)", i, j);
            }
          }
        }
        ImGui::EndTable();
      }

      /* Table footer */
      static ImGuiTableFlags footer_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter;
      if (ImGui::BeginTable("footer", 7, footer_flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 2), 0.0f)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, -1.0f);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Total"); ImGui::TableNextColumn(); ImGui::TableNextColumn();
        ImGui::Text("Many"); ImGui::TableNextColumn();
        ImGui::Text("Lots"); ImGui::TableNextColumn();
        ImGui::Text("Abundant"); ImGui::TableNextColumn();
        ImGui::Text("Seconds"); ImGui::TableNextColumn();

        ImGui::TableNextRow(); ImGui::TableNextColumn();
        ImGui::Text("Avg."); ImGui::TableNextColumn(); ImGui::TableNextColumn();
        ImGui::Text("Many"); ImGui::TableNextColumn();
        ImGui::Text("Lots"); ImGui::TableNextColumn();
        ImGui::Text("Abundant"); ImGui::TableNextColumn();
        ImGui::Text("Seconds"); ImGui::TableNextColumn();
        ImGui::Text("0th");

        ImGui::EndTable();
      }

      ImGui::End();
    }

    {
      create_window("footer", win3_x, win3_y, win3_w, win3_h);
      ImGui::Text("%s v%s.%s.%s - Eddy, 2020/10/11.", NAME, MAJOR, MINOR, PATCH); ImGui::SameLine();
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
