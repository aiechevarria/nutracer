#pragma once

#include <string.h>

#include "CLI11.hpp"
#include "GUI.h"
#include "Misc.h"
#include "Parser.h"
#include "Semantics.h"
#include "Interpreter.h"

typedef struct {
    std::string inputFile;
    bool debug = false;
} AppArgs;