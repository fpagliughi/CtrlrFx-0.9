%{
/**
 * Lexer for the CtrlrFx IDL compiler.
 *  
 */

#include "cfxidl.y.h"
#include "cfxidl_def.h"
#include <string.h>

int lineno = 1;
%}

%%
"interface" { return INTERFACE; }
"in"		{ yylval.ival = IN; return DIR; }
"out"		{ yylval.ival = OUT; return DIR; }
"inout"		{ yylval.ival = INOUT; return DIR; }


([0-9]+|([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?)	{
			yylval.dval = atof(yytext); return NUMBER;
		}

[ \t\r]	;

\n		{ ++lineno; }

[a-zA-Z_][a-zA-Z0-9_]* {
			yylval.sym = strdup(yytext); return NAME;
		}

.		return yytext[0];

