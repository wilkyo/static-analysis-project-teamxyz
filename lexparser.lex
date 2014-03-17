%{
	#include <stdio.h>
	extern int line;
	extern int col;
	void yyerror(const char * msg);
	#include "parser.h"

	void count();
%}
%option noyywrap
integer			[0-9]+
decimal			[0-9]+|[0-9]*\.[0-9]+
identifier		[a-z][a-zA-Z0-9]*
%%
\(			{ count(); return(L_PARENT); }
\)			{ count(); return(R_PARENT); }
\[			{ count(); return(L_BRACKET); }
\]			{ count(); return(R_BRACKET); }

,			{ count(); return(COMMA); }
;			{ count(); return(SEMI); }
:			{ count(); return(COLON); }
\-			{ count(); return(MINUS); }
\+			{ count(); return(PLUS); }
\*			{ count(); return(MULT); }
\/			{ count(); return(DIV); }
:=			{ count(); return(ASSIGN); }
\<\>			{ count(); return(NEQ); }
\<=			{ count(); return(LE); }
\>=			{ count(); return(GE); }
\<			{ count(); return(LT); }
\>			{ count(); return(GT); }
=			{ count(); return(EQ); }
!			{ count(); return(NOT); }
&&			{ count(); return(AND); }
\|\|			{ count(); return(OR); }

"int"			{ count(); return(TYPE_INT); }
"bool"			{ count(); return(TYPE_BOOL); }
"float"		{ count(); return(TYPE_FLOAT); }

"program"		{ count(); return(PROGRAM); }
"begin"			{ count(); return(START); }
"end"			{ count(); return(END); }
"return"		{ count(); return(RETURN); }
"if"			{ count(); return(IF); }
"then"			{ count(); return(THEN); }
"else"			{ count(); return(ELSE); }
"while"			{ count(); return(WHILE); }
"do"			{ count(); return(DO); }
"proc"			{ count(); return(PROCEDURE); }
"res"			{ count(); return(RES); }
"call"			{ count(); return(CALL); }
"skip"			{ count(); return(SKIP); }

"true"			{ count(); return(VALUE_TRUE); }
"false"			{ count(); return(VALUE_FALSE); }
{integer}		{ yylval.intT = atoi(yytext); count(); return(VALUE_INT); }
{decimal}		{ yylval.floatT = atof(yytext); count(); return(VALUE_DECIMAL); }
{identifier}		{ yylval.stringT = calloc(strlen(yytext), sizeof(char)); strcpy(yylval.stringT, yytext); count(); return(IDENTIFIER); }

\n			{ line++; col = 0; }
[[:blank:]]+		{ count(); }

<<EOF>>			{ printf("Lexical analysis successful\n"); return 0; }
.			{ char msg[100]; sprintf(msg,"%d:%d lexical error: %s\n", line, col, yytext); yyerror(msg); return 1; }
%%
void count() {
	col += yyleng;
}
