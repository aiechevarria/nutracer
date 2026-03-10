#pragma once

#include <string.h>

#include "CLI11.hpp"
#include "GUI.h"

typedef struct {
    std::string inputFile;
    bool debug = false;
} AppArgs;