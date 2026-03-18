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

    app.add_option("-i,--input", args.inputPath, "Path to the input pseudocode file")
        ->check(CLI::ExistingFile);

    app.add_option("-c,--config", args.configPath, "Path to the NuCachis configuration file")
        ->check(CLI::ExistingFile);

    app.add_option("-o,--output", args.savePath, "Path to the output file");
    
    app.add_option("-v, --variable", args.variableAddresses, "Define a variable's base address (NAME=0x80)")->take_all();

    app.add_option("-f, --frequency", args.variableAccessFrequency, "Define a variable's access frequency (always, once, never) (NAME=always)")->take_all();

    app.add_flag("-g,--nogui", args.noGui, "Disable the GUI");

    app.add_flag("-m, --comments", args.addComments, "Add comments to the trace");

    app.add_flag("-d,--debug", args.debug, "Toggles debug information");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        exit(app.exit(e));
    }

    return args;
}

/**
 * Converts the code to operations and interprets them
 * 
 * @param code 
 * @param trace 
 * @param ops 
 * @param variables 
 * @param settings 
 */
void generateTrace(string code, string& trace, vector<Operation>& ops, vector<Variable>& variables, GeneratorSettings settings) {
    // Clear previous trace instances
    trace.clear();
    for (Variable& var: variables) var.hasBeenAccessed = false;

    // Preprocess the code prior to extracting units
    preProcessCode(code);

    if (debug) {
        printf("Debug: Pre-processed output:\n");
        printf("%s\n\n", code.c_str());
        fflush(stdout);
    }

    // Process the code and extract the operations
    ops.clear();
    processCode(code, ops, variables, 0);

    // Terminate the list of operations with a OP_END
    Operation lastOp;
    lastOp.opType = OP_END;
    lastOp.oprState[OPR_DESTINATION] = OPRS_UNUSED;
    lastOp.oprState[OPR_OP1] = OPRS_UNUSED;
    lastOp.oprState[OPR_OP2] = OPRS_UNUSED;
    ops.emplace_back(lastOp);

    // Run the interpretation
    interpretCode(code, trace, ops, variables, settings);
}

/**
 * Handles an exception in the GUI
 * 
 * @param rt The exception
 * @param errorMessage Where to store the error text
 * @param state The current state
 * @param setToState The state to set
 */
void handleGUIError(runtime_error rt, string& errorMessage, ProgramState& state, ProgramState setToState) {
    errorMessage = rt.what();
    fprintf(stderr, "%s", errorMessage.c_str());
    state = setToState;
}

int main(int argc, char** argv) {
    // Program state and GUI
    ProgramState state = PICK_FILE;
    GeneratorSettings settings;
    string errorMessage;
    bool isReady = false;           // If the user has chosen to generate the trace
    bool cli = false;

    // Pseudocode structures
    string code, ogCode, config, trace;
    vector<Variable> variables;             // The list of variables that will be extracted from the code
    vector<Operation> opList;               // The list of operations that will get interpreted

    // Arguments
    AppArgs args = parseArguments(argc, argv);

    // Set up the settings
    settings.addComments = args.addComments;
    settings.baseAddr = 0x8000000;  // TODO Read this from the config file
    settings.wordWidth = 32;  // TODO Read this from the config file
    settings.pageSize = 1024;  // TODO Read this from the config file
    debug = args.debug;
    cli = args.noGui;

    // Copy the filepaths if they were provided as an argument
    if (!args.inputPath.empty()) settings.inputPath = args.inputPath;
    if (!args.configPath.empty()) settings.configPath = args.configPath;
    if (!args.savePath.empty()) settings.savePath = args.savePath;

    // If running in CLI only mode
    if (cli) {
        // Validate that files have been provided
        if (settings.inputPath.empty() || settings.configPath.empty() || settings.savePath.empty()) {
            fprintf(stderr, ERROR_CLI_FILES);
            return 1;
        }

        // Read the trace, parse the variables, generate the trace and save it 
        try {
            code = readFileToString(settings.inputPath);
            parseVariables(code, variables);

            // Validate that all variables have an address
            try {
                parseAddressArgs(variables, args.variableAddresses);
                parseFrequencyArgs(variables, args.variableAccessFrequency);
            } catch (const runtime_error& e) {
                fprintf(stderr, e.what());
                return 1;
            }

            generateTrace(code, trace, opList, variables, settings);
            writeStringToFile(settings.savePath, trace);
            return 0;
        } catch (const runtime_error& e) {
            errorMessage = e.what();
            fprintf(stderr, "%s", errorMessage.c_str());
            return 1;
        }
    } else {
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
                    // Render the picker and errors, if they happened
                    gui->renderPicker(settings, state);
                    if (!errorMessage.empty()) gui->renderMessage(errorMessage);
                    break;

                case READ_FILE:
                    // Read the files
                    try {
                        code = readFileToString(settings.inputPath);
                        config = readFileToString(settings.configPath);
                        ogCode = code;
                        state = PARSE_VARIABLES;
                    } catch (const runtime_error& e) {
                        // If an error happened, store it and kick the user back to the file picker.
                        handleGUIError(e, errorMessage, state, PICK_FILE);
                    }
                    break;

                case PARSE_VARIABLES:
                    // Parse the variables
                    try {
                        parseVariables(code, variables);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        // If an error happened, store it and kick the user back to the file picker.
                        handleGUIError(e, errorMessage, state, PICK_FILE);
                    }
                    break;

                case MAIN_WORKSPACE:
                    // Render the main workspace
                    gui->renderMainWorkspace(ogCode, trace, opList, variables, settings, state);
                    break;

                case GENERATE_TRACE:
                    // Try generating the trace
                    try {
                        trace.clear();
                        generateTrace(code, trace, opList, variables, settings);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        // If an error happened, store it and kick the user back to the main workspace
                        handleGUIError(e, errorMessage, state, MAIN_WORKSPACE);
                    }
                    break;

                case SAVE_TRACE:
                    // Save the trace
                    try {
                        writeStringToFile(settings.savePath, trace);
                        state = MAIN_WORKSPACE;
                    } catch (const runtime_error& e) {
                        // If an error happened, store it and kick the user back to the main workspace
                        handleGUIError(e, errorMessage, state, MAIN_WORKSPACE);
                    }
                    break;

                default:
                    errorMessage = ERROR_ASSIST_GENERAL;
                    state = PICK_FILE;
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
    }

    return 0;
}
