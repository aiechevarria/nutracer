#include "Semantics.h"

/**
 * Maps a DataType to a string. 
 * 
 * @param type The DataType.
 * @return string The string that it relates to.
 */
string DataTypeToString(DataType type) {
    switch (type)
    {
        case TYPE_CHAR:   return "char";
        case TYPE_SHORT:  return "short";
        case TYPE_INT:    return "int";
        case TYPE_LONG:   return "long";
        case TYPE_FLOAT:  return "float";
        case TYPE_DOUBLE: return "double";
        default:          return "unknown";
    }
}

/**
 * Maps an OperationType to a string. 
 * 
 * @param type The OperationType.
 * @return string The string that it relates to.
 */
string OperationTypeToString(OperationType type) {
    switch (type)
    {
        case OP_ADD:        return "ADD";
        case OP_SUB:        return "SUB";
        case OP_MUL:        return "MUL";
        case OP_DIV:        return "DIV";
        case OP_EQUAL:      return "STORE";
        case OP_BRANCH:     return "B";
        case OP_END:        return "END";
        default:            return "unknown";
    }
}

/**
 * Maps a BranchType to a string. 
 * 
 * @param type The BranchType.
 * @return string The string that it relates to.
 */
string BranchTypeToString(BranchType type) {
    switch (type)
    {
        case B_AL:          return "AL";
        case B_EQ:          return "EQ";
        case B_NE:          return "NE";
        case B_GT:          return "GT";
        case B_LT:          return "LT";
        case B_GE:          return "GE";
        case B_LE:          return "LE";
        default:            return "unknown";
    }
}

/**
 * Maps a BranchType to an operator. 
 * 
 * @param type The BranchType.
 * @return string The operator string that it relates to.
 */
string BranchTypeToOperator(BranchType type) {
    switch (type)
    {
        case B_EQ:          return "==";
        case B_NE:          return "!=";
        case B_GT:          return ">";
        case B_LT:          return "<";
        case B_GE:          return ">=";
        case B_LE:          return "<=";
        default:            return "unknown";
    }
}

/**
 * Maps a BranchType to the oposite operator. 
 * 
 * @param type The BranchType.
 * @return string The oposite operator string that it relates to.
 */
string BranchTypeToOpositeOperator(BranchType type) {
    switch (type)
    {
        case B_EQ:          return "!=";
        case B_NE:          return "==";
        case B_GT:          return "<=";
        case B_LT:          return ">=";
        case B_GE:          return "<";
        case B_LE:          return ">";
        default:            return "unknown";
    }
}

/**
 * Gets the operator string for an OperationType with a given StatementType
 * 
 * @param opType The OperationType.
 * @param staType The StatementType.
 * @return string The string that it relates to.
 */
string StatementOperatorToString(OperationType opType, StatementType staType) {
    if (staType == STATEMENT_ASSIGNMENT) return "=";

    switch (opType)
    {
        case OP_ADD:
            if (staType == STATEMENT_COMPOUND_ASSIGNMENT) return "+=";
            if (staType == STATEMENT_ASSIGMENT_AFTER_OPERATION) return "+";
        case OP_SUB:
            if (staType == STATEMENT_COMPOUND_ASSIGNMENT) return "-=";
            if (staType == STATEMENT_ASSIGMENT_AFTER_OPERATION) return "-";
        case OP_MUL:
            if (staType == STATEMENT_COMPOUND_ASSIGNMENT) return "*=";
            if (staType == STATEMENT_ASSIGMENT_AFTER_OPERATION) return "*";
        case OP_DIV:
            if (staType == STATEMENT_COMPOUND_ASSIGNMENT) return "/=";
            if (staType == STATEMENT_ASSIGMENT_AFTER_OPERATION) return "/";
        default:          return "?";
    }
}

/**
 * Maps an OperandType to a string. 
 * 
 * @param type The Operand
 * @return string The string that it relates to.
 */
string OperandTypeToString(OperandType type) {
    switch (type)
    {
        case OPR_DESTINATION:   return "Dest";
        case OPR_OP1:           return "OPR1";
        case OPR_OP2:           return "OPR2";
        default:                return "unknown";
    }
}

/**
 * Gets the size of a datatype in bytes
 * 
 * @param dt The datatype
 * @return unsigned long 
 */
unsigned long getDataTypeSize(DataType dt) {
    switch (dt) {
        case TYPE_CHAR: 
        case TYPE_SHORT:
        case TYPE_LONG:
        case TYPE_DOUBLE:
            printf("Warning: Datatypes that are not 4 bytes in size are not currently supported by NuCachis. %s will be treated as 4 bytes in size.\n", DataTypeToString(dt).c_str());
        case TYPE_INT:
        case TYPE_FLOAT:
            return 4;
            
        default:
            return 0;
    }
}

/**
 * Find a variable based on it's name.
 * 
 * @param vars The list of variables that have been identified.
 * @param name The name of the variable to find.
 * @return Variable* Pointer to the variable if found, nullptr if error.
 */
Variable* getVariableByName(vector<Variable>& vars, string name) {
    for (auto& v : vars) {
        if(name == v.name) return &v;
    }

    return nullptr;
}