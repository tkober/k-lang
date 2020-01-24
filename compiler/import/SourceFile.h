//
// Created by Thorsten Kober on 23.01.20.
//

#ifndef COMPILER_SOURCEFILE_H
#define COMPILER_SOURCEFILE_H


#include <cstdio>
#include <string>

#ifndef FLEX_SCANNER
#include "../scanner/scanner.h"
#endif

class SourceFile {

public:
    SourceFile(const char *path);

    std::string &getFilename();
    void setFilename(std::string &filename);

    FILE *getFile();
    void setFile(FILE *file);

    int getLineNumber();
    void setLineNumber(int lineNumber);

    yy_buffer_state *getBufferState();
    void setBufferState(yy_buffer_state *bufferState);

private:
    std::string filename;
    FILE *file;
    int lineNumber;
    YY_BUFFER_STATE bufferState;
};


#endif //COMPILER_SOURCEFILE_H
