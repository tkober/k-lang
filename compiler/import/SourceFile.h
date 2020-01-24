//
// Created by Thorsten Kober on 23.01.20.
//

#ifndef COMPILER_SOURCEFILE_H
#define COMPILER_SOURCEFILE_H


#include <cstdio>
#include <string>
#include "../scanner/scanner.h"

class SourceFile {

public:
    SourceFile(const char *path);

private:
    std::string filename;
    FILE *file;
    int lineNumber;
    YY_BUFFER_STATE bufferState;
};


#endif //COMPILER_SOURCEFILE_H
