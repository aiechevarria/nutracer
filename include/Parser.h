#pragma once

#include "Misc.h"
#include "Semantics.h"

#define BUFFER_SIZE 4096

bool hasString(string unit, string content);
string readFileToString(string inputPath);
void writeStringToFile(string filePath, string& content);
void parseVariableArgs(vector<Variable>& vars, vector<string> args);
void parseVariables(string text, vector<Variable>& variables);
void preProcessCode(string& code);
void processCode(string code, vector<Operation>& ops, vector<Variable>& vars, int index);