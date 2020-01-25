%option noyywrap
%option yylineno

%x IMPORT

%{

#include "../import/SourceFile.h"
#include "../import/SourceFileManager.h"

extern SourceFileManager *sourceFileManager;
extern void printLine();

%}

%%

^[ \t]*import[ \t]*[\"]     {
                                printLine();
                                BEGIN IMPORT;
                            }

<IMPORT>[^ \t\n\"]+         {
                                printf("%s\n", yytext);
                                {
                                    int c;
                                    while ((c = yyinput()) && c != '\n') ;
                                }

                                SourceFile *sourceFile = new SourceFile(yytext);
                                if (!sourceFileManager->import(sourceFile)) {}

                                BEGIN INITIAL;
                            }

<IMPORT>.|\n                {
                                fprintf(stderr, "%4d bad include line\n", yylineno);
                                yyterminate();
                            }

<<EOF>>                     {
                                if (!sourceFileManager->next()) {
                                    yyterminate();
                                }
                                printf("\n");
                            }

^.                          { printLine(); }
^\n                         { printLine(); }
\n                          { ECHO; }
.                           { ECHO; }

%%


void printLine() {
    fprintf(yyout, "%s::%d\t%s", sourceFileManager->getCurrentFileName(), yylineno, yytext);
}
