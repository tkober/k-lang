//
// Created by Thorsten Kober on 23.01.20.
//

#include "SourceFile.h"

SourceFile::SourceFile(const char *path) {
    this->file = fopen(path, "r");
    if (!this->file) {
        perror(path);
        exit(-1);
    }

    YY_BUFFER_STATE  bufferState = yy_create_buffer(this->file, YY_BUF_SIZE);

    this->bufferState = bufferState;
    this->filename = path;
    this->lineNumber = 1;

}