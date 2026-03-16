#pragma once

#include <string.h>

#include "CLI11.hpp"
#include "GUI.h"
#include "Misc.h"
#include "Parser.h"
#include "Semantics.h"
#include "Interpreter.h"

typedef struct {
    string inputPath, configPath, savePath;
    vector<string> variableAddresses;
    bool debug = false;
    bool addComments = false;
    bool noGui = false;
} AppArgs;