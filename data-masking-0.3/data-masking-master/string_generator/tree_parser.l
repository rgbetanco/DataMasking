%{
        #include "tree_parser_yacc.tab.h"
%}

%option nounput noinput noyywrap
%option reentrant bison-bridge

ASC     [\x00-\x7f]
ASCN    [\x00-\t\v-\x7f]
U       [\x80-\xbf]
U2      [\xc2-\xdf]
U3      [\xe0-\xef]
U4      [\xf0-\xf4]

%%
"{"[0-9]+[,]*[0-9]*"}"          {yylval->text = strdup(yytext); return BRA;}

\[(^)?(\\.|[^\]])*\]            {yylval->text = strdup(yytext); return SQR;}

[()\|+*?]                       {return yytext[0];}

"."                             {return DOT;}

[^[({\\+*\n.?]                  {yylval->text = strdup(yytext); return CHR;}

\\.                             {yylval->text = strdup(yytext); return CMD;}

\\*				{yylval->text = strdup(yytext); return CMD;}
                                
\\{U3}{U}{U}                    {yylval->text = strdup(yytext); return CMD;}

{ASC}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U} {
                                        yylval->text = strdup(yytext);
                                        return UTF8;
                                }
%%