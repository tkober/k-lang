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

const std::string &SourceFile::getFilename() const {
    return filename;
}

void SourceFile::setFilename(const std::string &filename) {
    SourceFile::filename = filename;
}

FILE *SourceFile::getFile() const {
    return file;
}

void SourceFile::setFile(FILE *file) {
    SourceFile::file = file;
}

int SourceFile::getLineNumber() const {
    return lineNumber;
}

void SourceFile::setLineNumber(int lineNumber) {
    SourceFile::lineNumber = lineNumber;
}

const yy_buffer_state *SourceFile::getBufferState() const {
    return bufferState;
}

void SourceFile::setBufferState(const yy_buffer_state *bufferState) {
    SourceFile::bufferState = bufferState;
}
