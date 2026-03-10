#include "Misc.h"
#include "Parser.h"

/**
 * Extracts the content of the input file to a string.
 * 
 * @param inputPath The path to the file
 * @return std::string The parsed file
 */
std::string readFileToString(char inputPath[MAX_PATH_LENGTH]) {
    char buffer[BUFFER_SIZE];
    std::string result;
    size_t bytesRead;

    // Open the file
    FILE* file = fopen(inputPath, "r");
    
    // Validate the file has been opened
    if (!file) {
        printf(ERROR_FILE_OPEN);
        return result;
    }

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        result.append(buffer, bytesRead);
    }

    // Validate no error has happened
    if (ferror(file)) {
        printf(ERROR_FILE_READ);
        result.clear();
        fclose(file);
        return result;
    }

    // Validate the file is not empty
    if (result.empty()) printf(ERROR_FILE_EMPTY);

    fclose(file);
    return result;
}