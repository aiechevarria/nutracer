#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctype.h>
#include <regex>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <math.h>

using namespace std;

// App config
#define APP_NAME "NuTracegen"
#define APP_VERSION "1.0r0"
#define APP_DESC "A simple trace generator for NuCachis"
#define APP_WEB "https://www.atc.unican.es"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define MAX_PATH_LENGTH 512

// Info messages
#define INFO_FILE_READ              "Info: File successfully read\n"
#define INFO_FILE_SAVED             "Info: File has successfully been saved!\n"

// Error Messages
// Files
#define ERROR_FILE_OPEN             "Error: Failed to open file or file descriptor.\n"
#define ERROR_FILE_READ             "Error: Could not read the full file.\n"
#define ERROR_FILE_EMPTY            "Error: The input file is empty.\n"
#define ERROR_FILE_SAVE             "Error: Something went wrong while trying to write the file. Do you have write permissions ?.\n"
#define ERROR_FILE_SAVE_EMPTY       "Error: The save path is empty. Please, configure a save path first.\n"

// Parser
#define ERROR_PARSE_NOVAR           "Error: No variables have been found on the input file.\nCaused by: "
#define ERROR_PARSE_FOR             "Error: Malformed or unfinished for loop detected.\nCaused by: "
#define ERROR_MISSING_CLOSE_BRACKET "Error: Missing close bracket in indexed variable\nCaused by: " 
#define ERROR_MISSING_OPEN_BRACKET  "Error: Missing open bracket in indexed variable\nCaused by: " 
#define ERROR_INDEXING_SCALAR       "Error: Cannot index scalar with brackets.\nCaused by: " 
#define ERROR_MISSING_INDEX         "Error: Missing index inside of brackets.\nCaused by: " 
#define ERROR_UNIDENTIFIED_INDEX    "Error: Unidentified index inside of brackets.\nCaused by: " 
#define ERROR_MALFORMED_FOR         "Error: Malformed for arguments detected.\nCaused by: " 
#define ERROR_FOR_BRACKETS          "Error: Malformed for brackets detected.\nCaused by: " 
#define ERROR_UNIDENTIFIED_VAR      "Error: Unidentified variable. Are all variables declared ?\nCaused by: " 

// Misc
#define ERROR_ASSIST_GENERAL        "Error: Something went wrong in the GUI assistant, please try again.\n"

extern bool debug;

// State in which the program / assistant is in
typedef enum {
    PICK_FILE,
    READ_FILE,
    PARSE_VARIABLES,
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