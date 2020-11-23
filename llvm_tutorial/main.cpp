#include <iostream>
#include <vector>

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

/// ExpressionAst - base class for all expression nodes.
class ExpressionAst {
public:
    virtual ~ExpressionAst() {}
};

/// NumberExpressionAst - Expression class for all numeric literals like "1.0".
class NumberExpressionAst : public ExpressionAst {
    double value;

public:
    NumberExpressionAst(double value) : value(value) {}
};

/// VariableExpressionAst - Expression class for referencing a variable like "foo".
class VariableExpressionAst : public ExpressionAst {
    std::string name;

public:
    VariableExpressionAst(const std::string &name) : name(name) {}
};

/// BinaryExpressionAst - Expression class for a binary operator.
class BinaryExpressionAst : public ExpressionAst {
    char op;
    std::unique_ptr<ExpressionAst> lhs, rhs;

public:
    BinaryExpressionAst(char op, std::unique_ptr<ExpressionAst> lhs, std::unique_ptr<ExpressionAst> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// CallExpressionAst - Expression class for function calls.
class CallExpressionAst : public ExpressionAst {
    std::string callee;
    std::vector<std::unique_ptr<ExpressionAst>> arguments;

public:
    CallExpressionAst(const std::string &callee, std::vector<std::unique_ptr<ExpressionAst>> arguments) : callee(callee), arguments(std::move(arguments)) {}
};

/// PrototypeAst - This class represents the "prototype" of a function,
/// which captures its name and its argument names (thus implicitly the number of arguments) it takes.
class PrototypeAst {
    std::string name;
    std::vector<std::string> arguments;

public:
    PrototypeAst(const std::string &name, std::vector<std::string> arguments) : name(name), arguments(std::move(arguments)) {}

    const std::string &getName() const { return name; }
};

/// FunctionAst - This class represents a function definition itself.
class FunctionAst {
    std::unique_ptr<PrototypeAst> prototype;
    std::unique_ptr<ExpressionAst> body;

public:
    FunctionAst(std::unique_ptr<PrototypeAst> prototype, std::unique_ptr<ExpressionAst> body) : prototype(std::move(prototype)), body(std::move(body)) {}
};

/// currentToken / getNextToken - Provide a simple token buffer. currentToken  is the current token the parser is
/// looking at. getNextToken reads another token from the lexer and updates currentToken with its result.
static int currentToken;

static int getNextToken() {
    return currentToken = getToken();
}

/// logError* - Helper functions for error handling
std::unique_ptr<ExpressionAst> logError(const char *str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

std::unique_ptr<PrototypeAst> logErrorP(const char *str) {
    logError(str);
    return nullptr;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
