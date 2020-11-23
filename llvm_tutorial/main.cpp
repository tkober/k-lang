#include <iostream>

enum Token {
    token_eof = -1,

    // commands
    token_def = -2,
    token_extern = -3,

    // primary
    token_identifier = -4,
    token_number = -5
};

static std::string identifierString;    // Used if token_identifier
static double numberValue;              // Used if token_identifier

/// getToken - Returns the next token from the standard input.
static int getToken() {
    static int lastCharacter = ' ';

    // Skip any whitespace
    while (isspace(lastCharacter)) {
        lastCharacter = getchar();

        // [a-zA-Z][a-zA-Z0-9]*
        if (isalpha(lastCharacter)) {
            identifierString = lastCharacter;

            while (isalnum((lastCharacter = getchar()))) {
                identifierString += lastCharacter;
            }

            if (identifierString == "def") {
                return token_def;
            }

            if (identifierString == "extern") {
                return token_extern;
            }

            return token_identifier;
        }

        // [0-9.]+
        if (isdigit(lastCharacter) || lastCharacter == '.') {
            std::string numberString;

            do {
                numberString += lastCharacter;
                lastCharacter = getchar();
            } while (isdigit(lastCharacter) || lastCharacter == '.');

            numberValue = strtod(numberString.c_str(), 0);
            return token_number;
        }

        // comments
        if (lastCharacter == '#') {
            do {
                lastCharacter = getchar();
            } while (lastCharacter != EOF && lastCharacter != '\n' && lastCharacter != '\r');

            if (lastCharacter != EOF) {
                return getToken();
            }
        }

        // EOF
        if (lastCharacter == EOF) {
            return token_eof;
        }

        // Everything else as its ascii value
        int thisCharacter = lastCharacter;
        lastCharacter = getchar();
        return thisCharacter;
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
