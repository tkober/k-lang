#include "import/SourceFileManager.h"

SourceFileManager *sourceFileManager;

int main(int argc, char **argv) {
    sourceFileManager = new SourceFileManager(&yylineno, yy_switch_to_buffer, yy_delete_buffer);

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