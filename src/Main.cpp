#include "Main.h"

bool debug = false;

/**
 * Parses the CLI arguments.
 * @param argc 
 * @param argv 
 * @return AppArgs The arguments packed in an AppArgs struct
 */
AppArgs parseArguments(int argc, char** argv) {
    AppArgs args;

    CLI::App app{APP_NAME " " APP_VERSION "\n" APP_DESC "\n" APP_WEB};
    argv = app.ensure_utf8(argv);

    app.add_option("-i,--input", args.inputFile, "Path to the input pseudocode file")
        ->check(CLI::ExistingFile);

    app.add_flag("-d,--debug", args.debug, "Toggles debug information");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    return args;
}

int main(int argc, char** argv) {
    // File paths for the trace and config
    char inputPath[MAX_PATH_LENGTH] = "\0";
    ProgramState state = PICK_FILE;
    GeneratorSettings settings;
    bool acceptedError = false;
    bool isReady = false;           // If the user has chosen to generate the trace

    // Set up settings
    settings.addComments = true;
    settings.baseAddr = 0x8000000;
    settings.destPath = (char*) malloc(sizeof(char) * MAX_PATH_LENGTH + 1);

    // Pseudocode structures
    std::string code;
    std::string trace;
    std::vector<Variable> variables;

    // Arguments
    AppArgs args = parseArguments(argc, argv);

    // Copy the config and trace files if they were provided as an argument
    if (!args.inputFile.empty()) {
        strncpy(inputPath, args.inputFile.c_str(), MAX_PATH_LENGTH);
        state = READ_FILE;
    }

    debug = args.debug;

    // Create a new GUI
    GUI* gui = new GUI();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    SDL_Window* window = gui->getWindow();

    // Disable imgui.ini config file creation
    io.IniFilename = nullptr;

    // Main loop
    bool running = true;

    while (running) {
        // Mandatory SDL polling on each frame
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Start a new rendering frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Main logic
        switch(state) {
            case PICK_FILE:
                gui->renderPicker(inputPath, &state);
                break;

            case READ_FILE:
                // Read the file
                code = readFileToString(inputPath);
                state = VALIDATE_FILE;
                break;

            case VALIDATE_FILE:
                // If there is no content throw an error and go back to the file selector
                if (code.empty()) {
                    gui->renderError((char*) ERROR_FILE_GENERAL, &acceptedError);
                } else {
                    printf(INFO_FILE_READ);
                    state = PARSE_VARIABLES;
                }

                if (acceptedError) {
                    acceptedError = false;
                    state = PICK_FILE;
                }
                break;

            case PARSE_VARIABLES:
                // Extract the variables
                parseVariables(code, &variables);
                state = VALIDATE_VARIABLES;
                break;

            case VALIDATE_VARIABLES:
                // Validate that there are some variables, if not throw error
                if (variables.empty()) {
                    gui->renderError((char*) ERROR_PARSE_NOVAR2, &acceptedError);
                } else {
                    state = MAIN_WORKSPACE;
                }

                if (acceptedError) {
                    acceptedError = false;
                    state = PICK_FILE;
                }
                break;

            case MAIN_WORKSPACE:
                // Render the main workspace
                gui->renderMainWorkspace(code, &trace, &variables, &settings, &state);
                break;
            case GENERATE_TRACE:
                // Parse all the code and return to the main workspace
                interpretCode(code, &trace, &variables, &settings);
                state = MAIN_WORKSPACE;
                break;

            case SAVE_TRACE:
                if (settings.destPath[0] == '\0') {
                    gui->renderError((char*) ERROR_FILE_SAVEPATH, &acceptedError);
                } else if (!writeStringToFile(settings.destPath, trace)) {
                    gui->renderError((char*) ERROR_FILE_SAVE, &acceptedError);
                } else {
                    gui->renderInfo((char*) INFO_FILE_SAVED, &acceptedError);
                }

                if (acceptedError) {
                    acceptedError = false;
                    state = MAIN_WORKSPACE;
                }
                break;

            default:
                state = PICK_FILE;
                printf(ERROR_ASSIST_GENERAL);
                break;
        }

        // Render the frame afterwards
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    free(settings.destPath);
    
    return 0;
}
