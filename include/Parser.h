#pragma once

#include "Misc.h"
#include "Semantics.h"

#define BUFFER_SIZE 4096

string readFileToString(char inputPath[MAX_PATH_LENGTH]);
void writeStringToFile(char* filePath, string& content);
void parseVariables(string text, vector<Variable>* variables);
void preProcessCode(string* code);
void processCode(string* code, vector<Operation>* ops, vector<Variable>* vars, int index);