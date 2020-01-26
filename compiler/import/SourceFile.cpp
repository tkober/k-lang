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
    this->nameSpace = "\"" + this->filename + "\"";
}

std::string &SourceFile::getNameSpace() {
    return nameSpace;
}

void SourceFile::setNameSpace(std::string &nameSpace) {
    SourceFile::nameSpace = nameSpace;
}

void SourceFile::setNameSpace(char *nameSpace) {
    std::string name = nameSpace;
    SourceFile::setNameSpace(name);
}

std::string &SourceFile::getFilename() {
    return filename;
}

void SourceFile::setFilename(std::string &filename) {
    SourceFile::filename = filename;
}

FILE *SourceFile::getFile() {
    return file;
}

void SourceFile::setFile(FILE *file) {
    SourceFile::file = file;
}

int SourceFile::getLineNumber() {
    return lineNumber;
}

void SourceFile::setLineNumber(int lineNumber) {
    SourceFile::lineNumber = lineNumber;
}

yy_buffer_state *SourceFile::getBufferState() {
    return bufferState;
}

void SourceFile::setBufferState(yy_buffer_state *bufferState) {
    SourceFile::bufferState = bufferState;
}
