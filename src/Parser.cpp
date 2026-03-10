#include "Parser.h"

/**
 * Get all the lines that match a given regex
 * 
 * @param text 
 * @param pattern 
 * @return std::vector<std::string> 
 */
std::vector<std::string> getMatchingLines(const std::string& text, const std::string& pattern) {
    // Parse the regular expression and create a container for all the matches
    std::regex re(pattern);
    std::vector<std::string> matches;

    std::istringstream stream(text);
    std::string line;

    // Iterate over all the liens in the stream
    while (std::getline(stream, line)) {
        // If the line matches the regex, store it in the matches bin
        if (std::regex_search(line, re)) matches.push_back(line);
    }

    return matches;
}

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

/**
 * Extracts the variables from the pseudocode.
 * @param text The text to extract the variables from
 * @param variables A vector that stores all variables in the order they have been parsed
 * @return True if the process suceeded, false if it failed.
 */
bool parseVariables(std::string text, std::vector<Variable>* variables) {
    // For a given datatype, check if the text has any occurences
    // For instance, get all the lines that have "int "
    // Once all the matches have been fetched, crop the datatype and the space until a ; [ = or space is met. That is the variable name

    // Iterate over each datatype 
    for (int d = TYPE_CHAR; d < TYPE_COUNT; d++) {
        DataType type = static_cast<DataType>(d);
        std::string typeStr = DataTypeToString(type);

        // Match for datatype + space + variable name (until space, ;, =, or [)
        std::string pattern = typeStr + R"(\s+([a-zA-Z_]\w*))";

        // Get all the matching lines for this regex
        std::vector<std::string> lines = getMatchingLines(text, pattern);

        // Iterate over every line
        for (const auto& line : lines) {
            std::smatch match;

            // Split the line into groups
            if (std::regex_search(line, match, std::regex(pattern))) {
                if (match.size() >= 2) {
                    // Capture the name and store the variable
                    std::string varName = match[1];
                    variables->push_back(Variable{0, varName, type});

                    if(debug) printf("Debug: Found variable with name=%s, type=%s\n", varName.c_str(), DataTypeToString(type).c_str());
                }
            }
        }
    }

    if (variables->empty()) {
        printf(ERROR_PARSE_NOVAR);
        return false;
    } 

    return true;
}