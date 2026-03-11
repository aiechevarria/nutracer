#include "Interpreter.h"
#include "Misc.h"

// Keeps modified addresses for future calculations
std::unordered_map<unsigned long, unsigned long> memMap;

/**
 * Parses the code and generates trace.
 * 
 * @param code The code. 
 * @param trace Pointer to the string that will store the trace
 * @param variables Pointer to the list of variables and their configurations
 * @param settings Pointer to the settings of the generator
 */
void interpretCode(std::string code, std::string* trace, std::vector<Variable>* variables, GeneratorSettings* settings) {
    // Clear the memory map
    memMap.clear();

    // Print the starting comment to the trace
    trace->append("# Trace generated with ");
    trace->append(APP_NAME);
    trace->append(" ");
    trace->append(APP_VERSION);
    trace->append("\n");

    

    // printf("%s", trace->c_str());
}
