#include <vector>
#include <map>

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

using namespace std;
using namespace llvm;

enum Token {
    token_eof = -1,

    // commands
    token_def = -2,
    token_extern = -3,

    // primary
    token_identifier = -4,
    token_number = -5
};

static string identifierString;    // Used if token_identifier
static double numberValue;         // Used if token_identifier

/// getToken - Returns the next token from the standard input.
static int getToken() {
    static int lastCharacter = ' ';

    // Skip any whitespace
    while (isspace(lastCharacter)) {
        lastCharacter = getchar();
    }

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
        string numberString;

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

/// ExpressionAst - base class for all expression nodes.
class ExpressionAst {
public:
    virtual ~ExpressionAst() {}
    virtual Value *codegen() = 0;
};

/// NumberExpressionAst - Expression class for all numeric literals like "1.0".
class NumberExpressionAst : public ExpressionAst {
    double value;

public:
    NumberExpressionAst(double value) : value(value) {}
    Value *codegen() override;
};

/// VariableExpressionAst - Expression class for referencing a variable like "foo".
class VariableExpressionAst : public ExpressionAst {
    string name;

public:
    VariableExpressionAst(const string &name) : name(name) {}
    Value *codegen() override;
};

/// BinaryExpressionAst - Expression class for a binary operator.
class BinaryExpressionAst : public ExpressionAst {
    char op;
    unique_ptr<ExpressionAst> lhs, rhs;

public:
    BinaryExpressionAst(char op, unique_ptr<ExpressionAst> lhs, unique_ptr<ExpressionAst> rhs)
            : op(op),
              lhs(move(lhs)),
              rhs(move(rhs)) {}
    Value *codegen() override;
};

// CallExpressionAst - Expression class for function calls.
class CallExpressionAst : public ExpressionAst {
    string callee;
    vector<unique_ptr<ExpressionAst>> arguments;

public:
    CallExpressionAst(const string &callee, vector<unique_ptr<ExpressionAst>> arguments)
            : callee(callee),
              arguments(move(arguments)) {}
    Value *codegen() override;
};

/// PrototypeAst - This class represents the "prototype" of a function,
/// which captures its name and its argument names (thus implicitly the number of arguments) it takes.
class PrototypeAst {
    string name;
    vector<string> arguments;

public:
    PrototypeAst(const string &name, vector<string> arguments)
            : name(name),
              arguments(move(arguments)) {}

    const string &getName() const { return name; }
    Function *codegen();
};

/// FunctionAst - This class represents a function definition itself.
class FunctionAst {
    unique_ptr<PrototypeAst> prototype;
    unique_ptr<ExpressionAst> body;

public:
    FunctionAst(unique_ptr<PrototypeAst> prototype, unique_ptr<ExpressionAst> body)
            : prototype(move(prototype)),
              body(move(body)) {}

    Function *codegen();
};

/// currentToken / getNextToken - Provide a simple token buffer. currentToken  is the current token the parser is
/// looking at. getNextToken reads another token from the lexer and updates currentToken with its result.
static int currentToken;

static int getNextToken() {
    return currentToken = getToken();
}

/// Binary Operator Precendence
static map<char, int> binaryOperatorPrecendence;

static int getTokenPrecendence() {
    if (!isascii(currentToken)) {
        return -1;
    }

    int tokenPrecendence = binaryOperatorPrecendence[currentToken];
    if (tokenPrecendence <= 0) {
        return -1;
    }
    return tokenPrecendence;
}

/// logError* - Helper functions for error handling
static unique_ptr<ExpressionAst> logError(const char *str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

static unique_ptr<PrototypeAst> logErrorP(const char *str) {
    logError(str);
    return nullptr;
}

static unique_ptr<ExpressionAst> parseExpression();

/// numberExpression ::= number
static unique_ptr<NumberExpressionAst> parseNumberExpression() {
    auto result = make_unique<NumberExpressionAst>(numberValue);
    getNextToken(); // consume the number
    return move(result);
}

/// parenthesisExpression ::= '(' expression ')'
static unique_ptr<ExpressionAst> parseParenthesisExpression() {
    getNextToken(); // consume '('
    auto expression = parseExpression();

    if (!expression) {
        return nullptr;
    }

    if (currentToken != ')') {
        return logError("expected ')'");
    }
    getNextToken(); // consume ')'

    return expression;
}

/// identifierExpression
///   ::= identifier
///   ::= identifier '(' expression* ')'
static unique_ptr<ExpressionAst> parseIdentifierExpression() {
    string identifierName = identifierString;
    getNextToken(); // consume the identifier

    if (currentToken != '(') {
        // result -> simple variable reference
        return make_unique<VariableExpressionAst>(identifierName);
    }

    // result -> call
    getNextToken(); // consume '('
    vector<unique_ptr<ExpressionAst>> arguments;
    if (currentToken != ')') {
        while (true) {
            if (auto argument = parseExpression()) {
                arguments.push_back(move(argument));
            } else {
                return nullptr;
            }

            if (currentToken == ')') {
                break;
            }

            if (currentToken != ',') {
                return logError("Expected ')' or ',' in argument list");
            }

            getNextToken();
        }
    }

    getNextToken(); // consume ')'

    return make_unique<CallExpressionAst>(identifierName, move(arguments));
}

/// primary
///   ::= identifierExpression
///   ::= numberExpression
///   ::= parenthesisExpression
static unique_ptr<ExpressionAst> parsePrimary() {
    switch (currentToken) {
        case token_identifier:
            return parseIdentifierExpression();

        case token_number:
            return parseNumberExpression();

        case '(':
            return parseParenthesisExpression();

        default:
            return logError("unknown token when expecting an expression");
    }
}

/// binaryOperationRhs
///   ::= ('+', primary)*
static unique_ptr<ExpressionAst> parseBinaryOperationRhs(int expresseionPrecendence, unique_ptr<ExpressionAst> lhs) {
    while (true) {
        // Find operator precendence
        int tokenPrecendence = getTokenPrecendence();

        // If this is a binary operation that binds at least as tightly as the current one,
        // consume it, otherwise we are done.
        if (tokenPrecendence < expresseionPrecendence) {
            return lhs;
        }

        // Okay, we know it is a binary operation
        int binaryOperation = currentToken;
        getNextToken(); // consume binary operation

        // Parse the primary expression after the binary operator
        auto rhs = parsePrimary();
        if (!rhs) {
            return nullptr;
        }

        // If binary operation binds less tightly with RHS than the operator after RHS, let the pending operator take
        // RHS as its LHS.
        int nextPrecendence = getTokenPrecendence();
        if (tokenPrecendence < nextPrecendence) {
            rhs = parseBinaryOperationRhs(tokenPrecendence + 1, move(rhs));
            if (!rhs) {
                return nullptr;
            }
        }

        // Merge LHS and RHS
        lhs = make_unique<BinaryExpressionAst>(binaryOperation, move(lhs), move(rhs));
    }
}

/// prototype
///   ::= id '(' id* ')'
static unique_ptr<PrototypeAst> parsePrototype() {
    if (currentToken != token_identifier) {
        return logErrorP("Expected function name in prototype");
    }

    string functionName = identifierString;
    getNextToken();

    if (currentToken != '(') {
        return logErrorP("Expected '(' in prototype");
    }

    // Read arguments name list
    vector<string> argumentNames;
    while (getNextToken() == token_identifier) {
        argumentNames.push_back(identifierString);
    }

    if (currentToken != ')') {
        return logErrorP("Expected ')' in prototype");
    }
    getNextToken(); // consume ')'

    return make_unique<PrototypeAst>(functionName, move(argumentNames));
}

/// definition
///   ::= 'def' prototype expression
static unique_ptr<FunctionAst> parseDefinition() {
    getNextToken(); // consume def

    auto prototype = parsePrototype();
    if (!prototype) {
        return nullptr;
    }

    if (auto expression = parseExpression()) {
        return make_unique<FunctionAst>(move(prototype), move(expression));
    }

    return nullptr;
}

/// external
///   ::= 'extern' prototype
static unique_ptr<PrototypeAst> parseExternal() {
    getNextToken(); // consume extern
    return parsePrototype();
}

/// topLevelExpression
///   ::= expression
static unique_ptr<FunctionAst> parseTopLevelExpression() {
    if (auto expression = parseExpression()) {
        // Build an anonymous prototype
        auto prototype = make_unique<PrototypeAst>("", vector<string>());
        return make_unique<FunctionAst>(move(prototype), move(expression));
    }

    return nullptr;
}

/// expression
//    ::= primary binarayOperationRhs
static unique_ptr<ExpressionAst> parseExpression() {
    auto lhs = parsePrimary();
    if (!lhs) {
        return nullptr;
    }

    return parseBinaryOperationRhs(0, move(lhs));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////   CODE GENERATION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static unique_ptr<LLVMContext> context;
static unique_ptr<Module> module;
static unique_ptr<IRBuilder<>> builder;
static map<string, Value *>namedValues;

Value * logErrorV(const char *message) {
    logError(message);
    return nullptr;
}

Value *NumberExpressionAst::codegen() {
    return ConstantFP::get(*context, APFloat(value));
}

Value *VariableExpressionAst::codegen() {
    Value *value = namedValues[name];
    if (!value) {
        logErrorV("Unknown variable name");
    }
    return value;
}

Value *BinaryExpressionAst::codegen() {
    Value *lhsValue = lhs->codegen();
    Value *rhsValue = rhs->codegen();

    if (!lhsValue || !rhsValue) {
        return nullptr;
    }

    Value *comparisonResult;
    switch (op) {

        case '+':
            return builder->CreateFAdd(lhsValue, rhsValue, "addtmp");

        case '-':
            return builder->CreateAShr(lhsValue, rhsValue, "subtmp");

        case '*':
            return builder->CreateFMul(lhsValue, rhsValue, "multmp");

        case '<':
            comparisonResult = builder->CreateFCmpULT(lhsValue, rhsValue, "cpmtmp");
            return builder->CreateUIToFP(comparisonResult, Type::getDoubleTy(*context), "booltmp");

        default:
            return logErrorV("Invalid binary operator");
    }
}

Value *CallExpressionAst::codegen() {
    // Look up the name in the global module table.
    Function *calleeFunction = module->getFunction(callee);
    if (!calleeFunction) {
        return logErrorV("Unkown function referenced");
    }

    // Check argument count matching
    if (calleeFunction->arg_size() != arguments.size()) {
        return logErrorV("Mismatching argument count");
    }

    vector<Value *> argumentValues;
    for (unsigned i = 0; i < arguments.size(); i++) {
        argumentValues.push_back(arguments[i]->codegen());
        if (!argumentValues.back()) {
            return nullptr;
        }
    }

    return builder->CreateCall(calleeFunction, argumentValues, "calltmp");
}

Function *PrototypeAst::codegen() {
    // Make the function type double(double, ...)
    vector<Type *> doubles(arguments.size(), Type::getDoubleTy(*context));
    FunctionType *functionType = FunctionType::get(Type::getDoubleTy(*context), doubles, false);
    Function *function = Function::Create(functionType, Function::ExternalLinkage, name, module.get());

    // Set names for all arguments
    unsigned i = 0;
    for (auto &argument : function->args()) {
        argument.setName(arguments[i]);
        i++;
    }

    return function;
}

Function *FunctionAst::codegen() {
    // Check for an existing function from a previous 'extern' declaration.
    Function *function = module->getFunction(prototype->getName());

    if (!function) {
        // Otherwise generate the function prototyp
        function = prototype->codegen();
    }

    if (!function) {
        return nullptr;
    }

    // Create a new basic block to start instertion into
    BasicBlock *basicBlock = BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(basicBlock);

    // Record the function arguments in the namedValues map.
    namedValues.clear();
    for (auto &argument : function->args()) {
        namedValues[string(argument.getName())] = &argument;
    }

    if (Value *returnValue = body->codegen()) {
        builder->CreateRet(returnValue);

        // Validate the generated code, checking for consistency.
        verifyFunction(*function);
        return function;
    }

    // Error reading body, remove function.
    function->eraseFromParent();
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////   TOP LEVEL PARSING
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void initializeModule() {
    // Open a new context and module.
    context = make_unique<LLVMContext>();
    module = make_unique<Module>("my cool jit", *context);

    // Create a new builder for the module.
    builder = std::make_unique<IRBuilder<>>(*context);
}

static void handleDefinition() {
    if (auto functionAst = parseDefinition()) {
        if (auto *functionIr = functionAst->codegen()) {
            fprintf(stderr, "Read function definition:\n");
            functionIr->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

static void handleExtern() {
    if (auto prototypeAst = parseExternal()) {
        if (auto *prototypeIr = prototypeAst->codegen()) {
            fprintf(stderr, "Read extern:\n");
            prototypeIr->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

static void handleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (auto expressionAst = parseTopLevelExpression()) {
        if (auto *expressionIr = expressionAst->codegen()) {
            fprintf(stderr, "Read top-level expression:\n");
            expressionIr->print(errs());
            fprintf(stderr, "\n");

            // Remove the anonymous expression.
            expressionIr->eraseFromParent();
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

/// top
///   ::= definition | external | expression | ';'
static void mainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");

        switch (currentToken) {

            case token_eof:
                return;

            case ';': // Ignore top level semicolons
                getNextToken();
                break;

            case token_def:
                handleDefinition();
                break;

            case token_extern:
                handleExtern();
                break;

            default:
                handleTopLevelExpression();
                break;
        }
    }
}

int main() {
    // Install standard binary operators.
    // 1 is lowest precedence.
    binaryOperatorPrecendence['<'] = 10;
    binaryOperatorPrecendence['+'] = 20;
    binaryOperatorPrecendence['-'] = 30;
    binaryOperatorPrecendence['*'] = 40; // highest.

    // Prime the first token.
    fprintf(stderr, "ready> ");
    getNextToken();

    // Make the module, which holds all the code.
    initializeModule();

    // Run the main loop
    mainLoop();

    // Print out all of the generated code.
    module->print(errs(), nullptr);

    return 0;
}
