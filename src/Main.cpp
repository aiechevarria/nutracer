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
        exit(app.exit(e));
    }

    return args;
}

int main(int argc, char** argv) {
    // File paths for the trace and config
    char inputPath[MAX_PATH_LENGTH] = "\0";
    ProgramState state = PICK_FILE;
    GeneratorSettings settings;
    string errorMessage;
    bool errorHappened = false;
    bool isReady = false;           // If the user has chosen to generate the trace

    // Set up settings
    settings.addComments = true;
    settings.baseAddr = 0x8000000;  // TODO Read this from the config file
    settings.destPath = (char*) malloc(sizeof(char) * MAX_PATH_LENGTH + 1);

    // Pseudocode structures
    string code;
    string trace;
    vector<Variable> variables;

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
                if (!errorHappened) {
                    try {
                        code = readFileToString(inputPath);
                        state = PARSE_VARIABLES;
                    } catch (const runtime_error& e) {
                        errorMessage = e.what();
                        fprintf(stderr, "%s", errorMessage.c_str());
                        errorHappened = true;
                    }
                } else {
                    gui->renderError(errorMessage.c_str(), &errorHappened);

                    // If the user has toggled the error message, return to the previous menu
                    if (!errorHappened) state = PICK_FILE;
                }
                break;

            case PARSE_VARIABLES:
                // Parse the variables
                if (!errorHappened) {
                    try {
                        parseVariables(code, &variables);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        errorMessage = e.what();
                        fprintf(stderr, "%s", errorMessage.c_str());
                        errorHappened = true;
                    }
                } else {
                    gui->renderError(errorMessage.c_str(), &errorHappened);
                    if (!errorHappened) state = PICK_FILE; // If the user has toggled the error message, return to the previous menu
                }
                break;

            case MAIN_WORKSPACE:
                // Render the main workspace
                gui->renderMainWorkspace(code, &trace, &variables, &settings, &state);
                break;
            case GENERATE_TRACE:
                if (!errorHappened) {
                    // Try generating the trace
                    try {
                        interpretCode(code, &trace, &variables, &settings);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        errorMessage = (char*) e.what();
                        fprintf(stderr, "%s", errorMessage.c_str());
                        errorHappened = true;
                    }
                } else {
                    gui->renderError(errorMessage.c_str(), &errorHappened);
                    trace.clear();
                }
                break;

            case SAVE_TRACE:
                if (!errorHappened) {
                    // Try generating the trace
                    try {
                        writeStringToFile(settings.destPath, trace);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        errorMessage = (char*) e.what();
                        fprintf(stderr, "%s", errorMessage.c_str());
                        errorHappened = true;
                    }
                } else {
                    gui->renderError(errorMessage.c_str(), &errorHappened);
                    if (!errorHappened) state = MAIN_WORKSPACE; // If the user has toggled the error message, return to the previous menu
                }
                break;

            default:
                errorHappened = true;
                gui->renderError(ERROR_ASSIST_GENERAL, &errorHappened);
                if (!errorHappened) state = PICK_FILE;
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
