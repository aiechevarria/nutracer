#pragma once

#include "Misc.h"
#include "Semantics.h"

#define BUFFER_SIZE 4096

std::string readFileToString(char inputPath[MAX_PATH_LENGTH]);
bool parseVariables(std::string text, std::vector<Variable>* variables);