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

"integer"			{ count(); return(TYPE_INT); }
"decimal"		{ count(); return(TYPE_DECIMAL); }
"array"			{ count(); return(TYPE_ARRAY); }

"new"			{ count(); return(NEW); }
"free"			{ count(); return(FREE); }
"return"		{ count(); return(RETURN); }
"if"			{ count(); return(IF); }
"then"			{ count(); return(THEN); }
"else"			{ count(); return(ELSE); }
"while"			{ count(); return(WHILE); }
"do"			{ count(); return(DO); }
"begin"			{ count(); return(START); }
"end"			{ count(); return(END); }
"function"		{ count(); return(FUNCTION); }
"procedure"		{ count(); return(PROCEDURE); }
"is"			{ count(); return(IS); }
"Program"		{ count(); return(PROGRAM); }

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
