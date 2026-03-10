#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <cassert>
#include <math.h>

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

// Error Messages
// Files
#define ERROR_FILE_OPEN             "Error: Failed to open file\n"
#define ERROR_FILE_READ             "Error: Could not read the full file.\n"
#define ERROR_FILE_EMPTY            "Error: The input file is empty\n"
#define ERROR_FILE_GENERAL          "Something went wrong while trying to read the file. Check the console for more information\n"

// Misc
#define ERROR_ASSIST_GENERAL        "Error: Something went wrong in the GUI assistant, please try again.\n"

typedef enum {
    FILE_NOT_SELECTED,
    FILE_SELECTED,
    FILE_READ,
    FILE_VALIDATED,
    VARIABLES_SET,
} ProgramState;