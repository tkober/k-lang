//
// Created by Thorsten Kober on 24.01.20.
//

#include "SourceFileManager.h"

SourceFileManager::SourceFileManager(
        int *linenumber,
        BufferStateOperation changeBuffer,
        BufferStateOperation deleteBuffer,
        StateActivation newFileState,
        StateActivation initialState) {

    this->linenumber = linenumber;
    this->changeBuffer = changeBuffer;
    this->deleteBuffer = deleteBuffer;
    this->newFileState = newFileState;
    this->initialState = initialState;

}

bool SourceFileManager::import(SourceFile *sourceFile) {
    // Already parsed or is already on the stack?
    if (this->sourceFileStates.find(sourceFile->getFilename()) != this->sourceFileStates.end()) {
        this->initialState();
        return false;
    }

    // Save the line number of the current source file
    if (!this->sourceFiles.empty()) {
        this->sourceFiles.top()->setLineNumber(*this->linenumber);
    }

    // Push the new source file and mark it as being parsed
    this->sourceFileStates[sourceFile->getFilename()] = PARSING;
    this->sourceFiles.push(sourceFile);

    // Change the actual source file of the scanner
    this->changeSourceFile(sourceFile);
    this->newFileState();

    return true;
}

bool SourceFileManager::next() {
    // No source files on the stack?
    if (this->sourceFiles.empty()) {
        return false;
    }
    // Pop the current one, delete the buffer, mark it as parsed and finally free it
    SourceFile *currentSourceFile = this->sourceFiles.top();
    this->sourceFiles.pop();
    this->deleteBuffer(currentSourceFile->getBufferState());
    this->sourceFileStates[currentSourceFile->getFilename()] = PARSED;
    free(currentSourceFile);

    // Is there a next source file?
    if (this->sourceFiles.empty()) {
        return false;
    }

    // Change the actual source file of the scanner
    SourceFile *nextSourceFile = this->sourceFiles.top();
    this->changeSourceFile(nextSourceFile);
    this->initialState();

    return true;
}

char * SourceFileManager::getCurrentFileName() {
    if (this->sourceFiles.empty()) {
        return NULL;
    } else {
        const std::string filename = this->sourceFiles.top()->getFilename();
        char *result = (char *)malloc(sizeof(char) * filename.size());
        strcpy(result, filename.c_str());
        return result;
    }
}

void SourceFileManager::changeSourceFile(SourceFile *sourceFile) {
    this->changeBuffer(sourceFile->getBufferState());
    *linenumber = sourceFile->getLineNumber();
}