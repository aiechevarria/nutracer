#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Font.h"
#include "Misc.h"
#include "Semantics.h"

// Proportions and placement of each window respective to the workspace
// Picker window
#define PICKER_WINDOW_WIDTH  0.70
#define PICKER_WINDOW_HEIGHT 0.30

#define FILE_PICKER_WINDOW_WIDTH  0.5
#define FILE_PICKER_WINDOW_HEIGHT 0.5

// Message window
#define MESSAGE_WINDOW_WIDTH  0.40
#define MESSAGE_WINDOW_HEIGHT 0.23

#define DEFAULT_TABLE_FLAGS ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg

class GUI {
private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    
    // Images
    GLuint logo;

    // Window sizes
    int windowHeight, windowWidth;                                  // App window

    // Draw functions
    GLuint LoadImageFromCSource(const unsigned char* rawData, int width, int height, bool setTaskbarIcon);

public:
    GUI();
    ~GUI();
    SDL_Window* getWindow();
    void renderPicker(GeneratorSettings& settings, ProgramState& state);
    void renderMessage(string message, bool isError = true);
    void renderMainWorkspace(string& code, string& trace, vector<Operation>& ops, vector<Variable>& variables, GeneratorSettings& settings, ProgramState& state);
};