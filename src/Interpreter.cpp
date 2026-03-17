#include "Interpreter.h"

/**
 * Inits the memory map in the same way NuCachis does. Required by stores to have the actual proper value
 * 
 * @param memMap The memory map
 * @param settings The settings of the simulator
 */
void initMemory(TraceData& td) {
    uint64_t currentWord = td.settings.baseAddr;
    uint64_t iter = 0;
    uint32_t wordWidth = td.settings.wordWidth / 8;        // Word width converted to bytes 

    // Fill the memory with values until the whole simulated page size is initialized
    while (currentWord <= td.settings.baseAddr + td.settings.pageSize) {
        (*td.memMap)[currentWord] = iter;

        // Increment the iterator and move to the next word
        currentWord += wordWidth;
        iter++;
    }
}

/**
 * Reads from memory and appends to the trace.
 * 
 * @param td The trace data
 * @param addr The address to read
 * @param comment Optional comment to add to the line 
 * @return uint64_t The value of that address
 */
uint64_t readMemory(TraceData& td, uint64_t addr, string comment = "") {
    char buffer[32];
    // Add the access to the trace, and a comment if available
    sprintf(buffer, "L 0x%lx D", addr);
    td.trace->append(buffer);
    (!comment.empty() && td.settings.addComments) ? td.trace->append("\t\t # " + comment + "\n") : td.trace->append("\n");

    // Lastly, return the memory address' content
    return (*td.memMap)[addr];
}

/**
 * Writes to memory and appends to the trace.
 * 
 * @param td The trace data
 * @param addr The address to read
 * @param comment Optional comment to add to the line 
 */
void writeMemory(TraceData& td, uint64_t addr, uint64_t value, string comment = "") {
    char buffer[32];
    // Add the access to the trace, and a comment if available
    sprintf(buffer, "S 0x%lx D %lu", addr, value);
    td.trace->append(buffer);
    (!comment.empty() && td.settings.addComments) ? td.trace->append("\t # " + comment + "\n") : td.trace->append("\n");

    // Lastly, update the memory
    (*td.memMap)[addr] = value;
}

/**
 * Returns the operand's indexed address. The operand should be a varaible.
 * 
 * @param td 
 * @param op 
 * @param type 
 * @return uint64_t 
 */
uint64_t fetchOperandAddress(TraceData& td, Operation& op, OperandType type) {
    uint64_t index;
    Variable* indexVar = (Variable*) op.indexes[type];
    Variable* oprVar = (Variable*) op.operands[type];

    // Get the index depending on the indexing type and apply the datatype size
    if (op.indexState[type] == OPRS_SCALAR) {
        index = ((uint64_t) op.indexes[type]); 
    } else if (op.indexState[type] == OPRS_VARIABLE) {
        // If the variable is indexed by another variable, get the index first and then address the content of the var
        index = readMemory(td, indexVar->address, indexVar->name);
    } else {
        index = 0;
    }

    // Calculate the address with the offset applied
    return oprVar->address + index * getDataTypeSize(oprVar->type);
}

/**
 * Fetches the content from the specified operand. 
 * 
 * @param memMap The map of modified memory addresses 
 * @param op The operation
 * @param type The operand to check
 * @return uint64_t The content of the operand, 0 if it is not used. It is up to the caller to check if the result is valid by comparing to OPRS_UNUSED.
 */
uint64_t fetchOperandValue(TraceData& td, Operation& op, OperandType type) {
    switch (op.oprState[type]) {
        case OPRS_SCALAR:   return (uint64_t) op.operands[type];
        case OPRS_VARIABLE: return readMemory(td, fetchOperandAddress(td, op, type), ((Variable*) op.operands[type])->name);
        default:            return 0;       // Does not matter, the caller should check if this is valid
    }
}

/**
 * Parses the code and generates a trace.
 * 
 * @param code The code. 
 * @param ops Pointer to the list of operations
 * @param variables Pointer to the list of variables and their configurations
 * @param settings Pointer to the settings of the generator
 */
void interpretCode(string code, string& trace, vector<Operation>& ops, vector<Variable>& variables, GeneratorSettings settings) {
    // Because stores need to have an actual value to store and the contents of memory/caches cannot be viewed at this point,
    // we need to store all modified data in a struct.
    unordered_map<uint64_t, uint64_t> memMap;
    uint64_t result;
    uint64_t opr1, opr2;                                // The two operands
    uint32_t pc = 0;                                    // The program counter
    bool takeBranch;
    TraceData td;
    

    // Group all the interpretation data and trace in a single struct
    td.memMap = &memMap;
    td.settings = settings;
    td.trace = &trace;

    // Init the memory
    initMemory(td);
    
    // Print the starting comment to the trace
    trace.append("# Trace generated with ");
    trace.append(APP_NAME);
    trace.append(" ");
    trace.append(APP_VERSION);
    trace.append("\n\n");

    // Interpret until the end of the instructions is reached
    while (ops[pc].opType != OP_END) {
        if (settings.addComments) trace.append("# " + ops[pc].comments + "\n");
        // Fetch the operands' values
        opr1 = fetchOperandValue(td, ops[pc], OPR_OP1);
        opr2 = fetchOperandValue(td, ops[pc], OPR_OP2);
        
        // If the operation is of arithmetic type, calculate the result
        if (ops[pc].opType != OP_BRANCH) {
            switch (ops[pc].opType) {
                case OP_ADD: result = opr1 + opr2; break;
                case OP_SUB: result = opr1 - opr2; break;
                case OP_MUL: result = opr1 * opr2; break;
                case OP_DIV: result = opr1 / opr2; break;
                case OP_EQUAL: result = opr1; break;
                default: break;
            }

            // Store in memory the result of the operation and increment the pc
            writeMemory(td, fetchOperandAddress(td, ops[pc], OPR_DESTINATION), result, ops[pc].comments);
            trace.append("\n");

            pc++;
        } else {
            // If it is a branch, calculate if it should be taken or not
            switch (ops[pc].bType) {
                case B_AL: takeBranch = true; break;
                case B_EQ: takeBranch = (opr1 == opr2); break;
                case B_NE: takeBranch = (opr1 != opr2); break;
                case B_GE: takeBranch = (opr1 >= opr2); break;
                case B_LE: takeBranch = (opr1 <= opr2); break;
                case B_GT: takeBranch = (opr1 > opr2); break;
                case B_LT: takeBranch = (opr1 < opr2); break;
                default: takeBranch = false; break;
            }

            // Update the PC
            if (takeBranch) {
                pc = ops[pc].operands[OPR_DESTINATION];
            } else {
                pc++;
            }
        }
    }

    trace.append("\n# Trace end\n");
}