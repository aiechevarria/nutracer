#include "Semantics.h"

std::string DataTypeToString(DataType type) {
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