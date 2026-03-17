#pragma once

#include "Misc.h"
#include "Semantics.h"
#include "Parser.h"

/*
 * Takes code operations, processes them and puts results in the trace.
 */

// Groups the memory, trace and settings for ease of use
typedef struct {
    unordered_map<uint64_t, uint64_t>* memMap;
    GeneratorSettings settings;
    string* trace;
} TraceData;

void interpretCode(string code, string& trace, vector<Operation>& ops, vector<Variable>& variables, GeneratorSettings settings);