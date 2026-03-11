#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctype.h>
#include <regex>
#include <cassert>
#include <unordered_map>
#include <math.h>

// App config
#define APP_NAME "NuTracegen"
#define APP_VERSION "1.0r0"
#define APP_DESC "A simple trace generator for NuCachis"
#define APP_WEB "https://www.atc.unican.es"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define MAX_PATH_LENGTH 512
#define TRACE_BUFFER_ADDITIONAL_SPACE   1000            // TODO Does this even work ?

// Info messages
#define INFO_FILE_READ              "Info: File successfully read\n"
#define INFO_FILE_SAVED             "File has successfully been saved!\n"

// Error Messages
// Files
#define ERROR_FILE_OPEN             "Error: Failed to open file or file descriptor.\n"
#define ERROR_FILE_READ             "Error: Could not read the full file.\n"
#define ERROR_FILE_EMPTY            "Error: The input file is empty.\n"
#define ERROR_FILE_GENERAL          "Something went wrong while trying to read the file. Check the console for more information.\n"
#define ERROR_FILE_SAVE             "Something went wrong while trying to write the file. Do you have permissions to write the file ?.\n"
#define ERROR_FILE_SAVEPATH         "The save path is empty. Please, configure a save path first.\n"

// Parser
#define ERROR_PARSE_NOVAR           "Error: No variables have been found on the input file.\n"
#define ERROR_PARSE_NOVAR2          "No variables have been found on the input file. Check that the variables have a datatype assigned to them\n"

// Misc
#define ERROR_ASSIST_GENERAL        "Error: Something went wrong in the GUI assistant, please try again.\n"

extern bool debug;

// State in which the program / assistant is in
typedef enum {
    PICK_FILE,
    READ_FILE,
    VALIDATE_FILE,
    PARSE_VARIABLES,
    VALIDATE_VARIABLES,
    MAIN_WORKSPACE,
    GENERATE_TRACE,
    SAVE_TRACE,
    PROGRAM_STATE_COUNT
} ProgramState;

typedef struct {
    unsigned long baseAddr;     // NuCachis inits memory addresses with incremental numbers starting from the base address. This ensures correctness with the results
    char* destPath;
    bool addComments;
} GeneratorSettings;