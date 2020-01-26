%option noyywrap
%option yylineno

%x NEW_FILE
%x NAMESPACE
%x IMPORT

%{
#include <string.h>

#include "../import/SourceFile.h"
#include "../import/SourceFileManager.h"

extern SourceFileManager *sourceFileManager;
extern void printLine();

%}

NAMESPACE ([a-zA-Z]+(\.[a-zA-Z]+)*)

%%

<NEW_FILE>^[ \t]*namespace[ \t]+    {   BEGIN NAMESPACE; }
<NEW_FILE>^\n                       {}
<NEW_FILE>^.                        {   unput(*yytext); yy_set_bol(1); BEGIN INITIAL; }


<NAMESPACE>{NAMESPACE};             {
                                        int n = yyleng-1;
                                        char *name = (char *)malloc(sizeof(char) * n);
                                        strncpy(name, yytext, n);
                                        printf("defined namespace: '%s'", name);
                                        BEGIN INITIAL;
                                    }
<NAMESPACE>.|\n                     {
                                        fprintf(stderr, "%4d bad namespace definition\n", yylineno);
                                        yyterminate();
                                    }


^[ \t]*import[ \t]*[\"]     {   BEGIN IMPORT; }
<IMPORT>[^ \t\n\"]+         {
                                {
                                    int c;
                                    while ((c = yyinput()) && c != '\n') ;
                                }

                                SourceFile *sourceFile = new SourceFile(yytext);
                                if (!sourceFileManager->import(sourceFile)) {}
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
^.                          {   printLine(); }
^\n                         {   printLine(); }
\n                          {   ECHO; }
.                           {   ECHO; }

%%


void printLine() {
    fprintf(yyout, "%s::%d\t%s", sourceFileManager->getCurrentFileName(), yylineno, yytext);
}


void activateInitialState() {
    BEGIN INITIAL;
}

void activateNewFileState() {
    BEGIN NEW_FILE;
}