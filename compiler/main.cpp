#include "import/SourceFileManager.h"

SourceFileManager *sourceFileManager;

extern void activateNewFileState();

extern void activateInitialState();

int main(int argc, char **argv) {
    sourceFileManager = new SourceFileManager(&yylineno, yy_switch_to_buffer,
                                              yy_delete_buffer, activateNewFileState, activateInitialState);

    if (argc < 2) {
        fprintf(stderr, "need filename\n");
        return 1;
    }

    SourceFile *sourceFile = new SourceFile(argv[1]);
    if (sourceFileManager->import(sourceFile)) {
        yylex();
    }
    return 0;
}