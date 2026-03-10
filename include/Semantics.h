#pragma once

#include <string>

/*
 * Representation of various elements related to the pseudocode  
 */

// Supported datatypes
typedef enum {
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_COUNT
} DataType;

// The representation of a variable
typedef struct {
    unsigned long address;  // The address of the variable. Stablished by the user
    std::string name;
    DataType type;          // The number of bytes the array 
} Variable;

std::string DataTypeToString(DataType type);