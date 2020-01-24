%option noyywrap
%option yylineno

%x IMPORT

%{

#include "../import/SourceFile.h"
#include "../import/SourceFileManager.h"

extern SourceFileManager *sourceFileManager;

%}

%%

^[ \t]*import[ \t]*[\"<]    { BEGIN IMPORT; }

<IMPORT>[^ \t\n\">]+        {
                                {
                                    int c;
                                    while ((c = yyinput()) && c != '\n') ;
                                }

                                yylineno++;
                                SourceFile *sourceFile = new SourceFile(yytext);
                                if (!sourceFileManager->import(sourceFile)) {
                                    printf("<WARN> File '%s' is already included\n", yytext);
                                }
                                BEGIN INITIAL;
                            }

<IMPORT>.|\n                { fprintf(stderr, "%4d bad include line\n", yylineno); yyterminate(); }

<<EOF>>                     {
                                if (!sourceFileManager->next()) {
                                    yyterminate();
                                }
                            }

^.                          { fprintf(yyout, "%4d %s", yylineno, yytext); }
^\n                         { fprintf(yyout, "\n"); yylineno++; }
\n                          { ECHO; yylineno++;}
.                           { ECHO; }

%%

