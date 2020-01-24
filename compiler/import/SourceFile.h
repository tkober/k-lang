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

    const std::string &getFilename() const;
    void setFilename(const std::string &filename);

    FILE *getFile() const;
    void setFile(FILE *file);

    int getLineNumber() const;
    void setLineNumber(int lineNumber);

    const yy_buffer_state *getBufferState() const;
    void setBufferState(const yy_buffer_state *bufferState);

private:
    std::string filename;
    FILE *file;
    int lineNumber;
    YY_BUFFER_STATE bufferState;
};


#endif //COMPILER_SOURCEFILE_H
