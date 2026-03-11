#pragma once

#include "Misc.h"
#include "Semantics.h"

/*
 * Takes code operations, processes them and puts results in the trace.
 */

void interpretCode(std::string code, std::string* trace, std::vector<Variable>* variables, GeneratorSettings* settings);

// Compound assignment operators
void addVars(std::string* trace, GeneratorSettings settings, Variable v1, Variable v2);         // v1 += v2 
void subVars(std::string* trace, GeneratorSettings settings, Variable v1, Variable v2);         // v1 -= v2
void mulVars(std::string* trace, GeneratorSettings settings, Variable v1, Variable v2);         // TODO
void divVars(std::string* trace, GeneratorSettings settings, Variable v1, Variable v2);

// Equality
void equalScalar(std::string* trace, GeneratorSettings settings, Variable v1, long scalar);
void equalVariable(std::string* trace, GeneratorSettings settings, Variable v1, Variable v2);