#include "GUI.h"
#include "Logo.cpp"
#include "Misc.h"
#include "imgui.h"

ImFont* defaultFont;

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
    defaultFont = io.Fonts->AddFontDefault();
    io.FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(robottoFont_compressed_data_base85, 16.0f);
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
 * @param inputPath Pointer to a sufficiently large array of characters for the input path
 * @param state Pointer to the current state of the program. Switches to the next assistant step when clicked
 */
void GUI::renderPicker(char inputPath[MAX_PATH_LENGTH], ProgramState* state) {
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set a size and position based on the current workspace dimms
    ImVec2 windowSize(windowWidth * PICKER_WINDOW_WIDTH, windowHeight * PICKER_WINDOW_HEIGHT);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    ImVec2 windowPos((windowWidth / 2 - windowWidth * PICKER_WINDOW_WIDTH / 2), (windowHeight / 2 - windowHeight * PICKER_WINDOW_HEIGHT / 2));
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
    
    // Render the window
    ImGui::Begin("Welcome to NuTracegen", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    // Center the logo
    ImGui::Image((ImTextureID)(intptr_t)logo, ImVec2(logoData.width / 4, logoData.height / 4));

    // Put text on the side with some padding
    ImGui::SameLine(0.0f, 20.0f);

    // Group all text
    ImGui::BeginGroup();
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
		ImGuiFileDialog::Instance()->OpenDialog("ChooseInputFile", "Choose Input File", ".ntg", config);
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);          // Make the input below take all avaiable width
    ImGui::InputText("##InputPicker", inputPath, MAX_PATH_LENGTH);

    // File picker dialog
    if (ImGuiFileDialog::Instance()->Display("ChooseInputFile", ImGuiWindowFlags_NoCollapse, ImVec2(windowWidth * FILE_PICKER_WINDOW_WIDTH, windowHeight * FILE_PICKER_WINDOW_HEIGHT))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            strncpy(inputPath, ImGuiFileDialog::Instance()->GetFilePathName().c_str(), MAX_PATH_LENGTH);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Once submit has been clicked
    if (ImGui::Button("Submit", ImVec2(125.0f, 0.0f))) *state = READ_FILE;

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
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("Error");
    // Draw a red, 5 times larger than usual exclamation and reset the style after doing so 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); 
    ImGui::SetWindowFontScale(5.0f); 
    ImGui::Text("!");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::TextWrapped("%s", message);
    ImGui::EndGroup();

    ImGui::Separator();
    if (ImGui::Button("Ok")) {
        *toggle = !*toggle;
    }

    ImGui::End();
}

/**
 * Displays an info box with the provided message.
 * 
 * @param message The message to display
 * @param toggle Boolean variable to toggle when the close button is pressed
 */
void GUI::renderInfo(char* message, bool* toggle) {
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set a size and position based on the current workspace dimms
    ImVec2 windowSize(windowWidth * ERROR_WINDOW_WIDTH, windowHeight * ERROR_WINDOW_HEIGHT);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImVec2 windowPos((windowWidth / 2 - windowWidth * ERROR_WINDOW_WIDTH / 2), (windowHeight / 2 - windowHeight * ERROR_WINDOW_HEIGHT / 2));
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("Information");
    // Draw a red, 5 times larger than usual exclamation and reset the style after doing so 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.3f, 0.9f, 1.0f)); 
    ImGui::SetWindowFontScale(5.0f); 
    ImGui::Text("i");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::TextWrapped("%s", message);
    ImGui::EndGroup();

    ImGui::Separator();
    if (ImGui::Button("Ok")) {
        *toggle = !*toggle;
    }

    ImGui::End();
}

/**
 * Renders the main workspace. 
 * 
 * @param code The code that has been provided with the file, for preview.
 * @param trace The trace that has been generated
 * @param variables The variables that have been parsed from the code.
 * @param settings The settings of the generator.
 * @param run If the user has clicked on the run button or not
 */
void GUI::renderMainWorkspace(std::string code, std::string* trace, std::vector<Variable>* variables, GeneratorSettings* settings, ProgramState* state) {
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Set a size and position based on the current workspace dimms
    ImVec2 windowSize(windowWidth, windowHeight);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);

    ImGui::Begin("Trace generation", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    // Render the left side of the window
    ImGui::BeginChild("LeftPanel", ImVec2(windowWidth / 2, 0), true);
    ImVec2 leftDimms = ImGui::GetContentRegionAvail();
    
    // Render the code in a bordered box
    ImGui::Text("Code Preview:");
    ImGui::PushFont(defaultFont);               // Switch to the default monospace font
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::InputTextMultiline("##code", (char*) code.c_str(), code.size() + 1, ImVec2(leftDimms.x, leftDimms.y / 3), ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleVar();
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    
    // Render the variable table
    ImGui::Text("Variable configuration");
    ImGui::Text("In hexadecimal, set the addresses of the variables listed below before generating a trace:");

    // Render the variable table
    if (ImGui::BeginTable("VariableTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg )) {
        // Headers
        ImGui::TableSetupColumn("Variable Name");
        ImGui::TableSetupColumn("Datatype");
        ImGui::TableSetupColumn("Address (In hexadecimal)");
        ImGui::TableHeadersRow();

        // Populate rows
        for (Variable& var : *variables) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%s", var.name.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", DataTypeToString(var.type).c_str());

            ImGui::TableNextColumn();
            ImGui::Text("0x");
            ImGui::SameLine();
            ImGui::PushFont(defaultFont);               // Switch to the default monospace font
            ImGui::InputScalar(("##" + var.name).c_str(), ImGuiDataType_U64, &var.address, nullptr, nullptr, "%016llX", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::PopFont();
        }

        ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    ImGui::Text("Generation settings");

    ImGui::Text("Page Base Address (must match page_base_address in the Nucachis config) 0x");
    ImGui::SameLine();
    ImGui::InputScalar("## BaseSimAddr", ImGuiDataType_U64, &settings->baseAddr, nullptr, nullptr, "%016llX", ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Checkbox("Add comments to the trace", &settings->addComments);

    if (ImGui::Button("Destination file", ImVec2(125.0f, 0.0f))) {
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
        IGFD::FileDialogConfig config;
        config.path = '.';
		ImGuiFileDialog::Instance()->OpenDialog("ChooseDestFile", "Choose Dest File", ".vca", config);
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);          // Make the input below take all avaiable width
    ImGui::InputText("##DestPicker", settings->destPath, MAX_PATH_LENGTH);

    // File picker dialog
    if (ImGuiFileDialog::Instance()->Display("ChooseDestFile", ImGuiWindowFlags_NoCollapse, ImVec2(0.0f, 0.0f), ImVec2(windowWidth * FILE_PICKER_WINDOW_WIDTH, windowHeight * FILE_PICKER_WINDOW_HEIGHT))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            strncpy(settings->destPath, ImGuiFileDialog::Instance()->GetFilePathName().c_str(), MAX_PATH_LENGTH);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Push the buttons to the bottom
    ImVec2 leftAvail = ImGui::GetContentRegionAvail();
    ImGui::Dummy(ImVec2(0.0f, leftAvail.y - ImGui::GetFrameHeight() - 20.0f));

    if (ImGui::Button("Generate Trace", ImVec2(125.0f, 0.0f))) *state = GENERATE_TRACE;
    ImGui::SameLine();
    ImGui::BeginDisabled(trace->empty()); if (ImGui::Button("Save Trace")) *state = SAVE_TRACE; ImGui::EndDisabled();

    ImGui::EndChild();

    ImGui::SameLine();

    // Render the right side of the window
    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    ImVec2 rightDimms = ImGui::GetContentRegionAvail();
    ImVec2 rightAvail = ImGui::GetContentRegionAvail();


    ImGui::Text("Trace Preview:");
    ImGui::PushFont(defaultFont);               // Switch to the default monospace font
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::InputTextMultiline("##trace", (char*) trace->c_str(), trace->size() + TRACE_BUFFER_ADDITIONAL_SPACE, ImVec2(rightAvail.x, rightAvail.y - 20.0f), ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleVar();
    ImGui::PopFont();

    ImGui::EndChild();

    ImGui::End();
}