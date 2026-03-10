#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "Font.h"
#include "Misc.h"

// Proportions and placement of each window respective to the workspace
// Picker window
#define PICKER_WINDOW_WIDTH  0.70
#define PICKER_WINDOW_HEIGHT 0.27

#define FILE_PICKER_WINDOW_WIDTH  0.5
#define FILE_PICKER_WINDOW_HEIGHT 0.5

// Error window
#define ERROR_WINDOW_WIDTH  0.40
#define ERROR_WINDOW_HEIGHT 0.23

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
    void renderPicker(char inputPath[MAX_PATH_LENGTH], ProgramState* state);
    void renderError(char* message, bool* toggle);
};
