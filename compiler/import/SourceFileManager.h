//
// Created by Thorsten Kober on 24.01.20.
//

#ifndef COMPILER_SOURCEFILEMANAGER_H
#define COMPILER_SOURCEFILEMANAGER_H

#include <map>
#include <stack>
#include "SourceFile.h"

#ifndef FLEX_SCANNER
#import "../scanner/scanner.h"
#endif

enum SourceFileState {
    PARSING,
    PARSED
};

typedef void (*BufferStateOperation)(yy_buffer_state *);

class SourceFileManager {

public:
    SourceFileManager(int *pInt, BufferStateOperation changeBuffer, BufferStateOperation deleteBuffer);

    bool import(SourceFile *sourceFile);
    bool next();

private:
    int *linenumber;
    BufferStateOperation changeBuffer;
    BufferStateOperation deleteBuffer;

    std::map<std::string, SourceFileState> sourceFileStates;
    std::stack<SourceFile *> sourceFiles;

    void changeSourceFile(SourceFile *sourceFile);
};


#endif //COMPILER_SOURCEFILEMANAGER_H
