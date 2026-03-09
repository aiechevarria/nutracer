#include "GUI.h"
#include "Logo.cpp"
#include "imgui.h"


GUI::GUI () {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    }

    // GL context setup
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window
    window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Set the style to dark
    ImGui::StyleColorsDark();

    // Load the images to the GPU
    logo = LoadImageFromCSource(logoData.pixelData, logoData.width, logoData.height, true);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(robottoFont_compressed_data_base85, 16.0f);
}

GUI::~GUI () {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Window* GUI::getWindow() {
    return window;
}

/**
 * Decodes and loads the given image data to the GPU
 * @param rawData The data to decode 
 * @param width The width of the image
 * @param height The height of the image
 * @param setTaskbarIcon If true, also sets the image as the icon of the application in the OS' taskbar.
 * @return unsigned char* Pointer to the RGBA buffer
 */
GLuint GUI::LoadImageFromCSource(const unsigned char* rawData, int width, int height, bool setTaskbarIcon) {
    // To set the icon, raw data can be passed directly
    if (setTaskbarIcon) {
        SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(
            (void*)rawData,
            width, height, 32,                             // Depth
            width * 4,                                     // Pitch (Row size in bytes)
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 // RGBA Masks
        );

        if (iconSurface) {
            SDL_SetWindowIcon(window, iconSurface);
            SDL_FreeSurface(iconSurface); // Free the struct
        }
    }

    // Unpack the texture and upload the pixels to the GPU
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    // Setup filtering so that the image is not blurry
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels directly from the static array
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawData);

    return imageTexture;
}

/**
 * Renders the file picker.
 * @param configPath Pointer to a sufficiently large array of characters for the config path
 * @param tracePath Pointer to a sufficiently large array of characters for the trace path
 * @param freshLaunch If true, displays the app's logo and welcome screen
 * @param clickedLaunch Pointer to a boolean. Toggled true when the user clicks the launch button
 */
void GUI::renderPicker(char inputPath[MAX_PATH_LENGTH]) {
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set a size and position based on the current workspace dimms
    ImVec2 windowSize(windowWidth * PICKER_WINDOW_WIDTH, windowHeight * PICKER_WINDOW_HEIGHT);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImVec2 windowPos((windowWidth / 2 - windowWidth * PICKER_WINDOW_WIDTH / 2), (windowHeight / 2 - windowHeight * PICKER_WINDOW_HEIGHT / 2));
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
    
    // Render the window
    ImGui::Begin("Welcome to NuTracegen", nullptr, ImGuiWindowFlags_NoCollapse);

    // Center the logo
    ImGui::Image((ImTextureID)(intptr_t)logo, ImVec2(logoData.width / 4, logoData.height / 4));

    // Put text on the side with some padding
    ImGui::SameLine(0.0f, 20.0f);

    // Group all text
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text(APP_NAME " (" APP_VERSION ")");
    ImGui::Text(APP_DESC);
    ImGui::Text(APP_WEB);

    // Leave y pixels of vertical separation
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::Text("Please, pick an input file:");

    // Render the file picker button
    if (ImGui::Button("Pick input file", ImVec2(125.0f, 0.0f))) {
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        IGFD::FileDialogConfig config;
        config.path = '.';
		ImGuiFileDialog::Instance()->OpenDialog("ChooseInputFile", "Choose Input File", "*", config);
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);          // Make the input below take all avaiable width
    ImGui::InputText("##InputPicker", inputPath, MAX_PATH_LENGTH);

    // File 
    if (ImGuiFileDialog::Instance()->Display("ChooseInputFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            strncpy(inputPath, ImGuiFileDialog::Instance()->GetFilePathName().c_str(), MAX_PATH_LENGTH);
        }
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::EndGroup();

    ImGui::End();
}

/**
 * Displays an error box with the provided message.
 * 
 * @param message The message to display
 * @param toggle Boolean variable to toggle when the close button is pressed
 */
void GUI::renderError(char* message, bool* toggle) {
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set a size and position based on the current workspace dimms
    ImVec2 windowSize(windowWidth * ERROR_WINDOW_WIDTH, windowHeight * ERROR_WINDOW_HEIGHT);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImVec2 windowPos((windowWidth / 2 - windowWidth * ERROR_WINDOW_WIDTH / 2), (windowHeight / 2 - windowHeight * ERROR_WINDOW_HEIGHT / 2));
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

    ImGui::Begin("Error");
    // Draw a red, 5 times larger than usual exclamation and reset the style after doing so 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); 
    ImGui::SetWindowFontScale(5.0f); 
    ImGui::Text("!");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::Text("%s", message);

    ImGui::Separator();
    if (ImGui::Button("Ok")) {
        *toggle = !*toggle;
    }

    ImGui::End();
}