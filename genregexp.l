%{
        #include "y.tab.h"
%}

/*"exit"                                    {return exit_command;}
[\n]                                ;
*/

%%
"{"[0-9]+[,]*[0-9]*"}"          {yylval.text = (char *)malloc(yyleng+1);
                                                    strcpy(yylval.text, yytext); return BRA;}
\[(^)?(\\.|[^\]])*\]            {yylval.text = (char *)malloc(yyleng+1);
                                                    strcpy(yylval.text, yytext); return SQR;}
[()\|+*?]                               {return yytext[0];}
"."                                                     {ECHO; yyerror("unexpected character"); exit(EXIT_SUCCESS);}
[^[({\\+*\n.?]                          {yylval.text = (char *)malloc(yyleng+1);
                                                    strcpy(yylval.text, yytext); return CHR;}
\\.                                                     {yylval.text = (char *)malloc(yyleng+1);
                                                    strcpy(yylval.text, yytext+1); return CHR;}
.                                                       {ECHO; yyerror("unexpected character"); exit(EXIT_SUCCESS);}
%%

int yywrap (void) { return 1; }