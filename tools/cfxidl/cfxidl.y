%{
/**
 * YACC/Bison Parser for the CtrlrFx IDL compiler.
 * 
 * This uses a C++ output target language.
 * 
 * Frank Pagliughi
 * SoRo Systems, Inc.
 * 
 * July 2007
 * 
 */

#include <iostream>
#include <map>
#include <string>
#include "cfxidl.h"
#include "Func.h"
#include "Iface.h"

using namespace std;

extern int lineno;
extern char* yytext;
extern string filename;

extern "C" {

	int yyparse(void);
	int yylex(void);

	int yyerror(char *s) {
		if (filename.empty()) 
			cerr << "Line ";
		else
			cerr << filename << ":";
		cerr << lineno << ": " << s 
			<< " at '" << yytext << "'" << endl;
	}

	int yywrap() {
		return 1;
	}
}

extern ParamList	paramList;
extern FuncList		funcList;
extern IfaceList	ifaceList;
%}

%union {
	double dval;
	int ival;
	char *sym;
}

%token INTERFACE
%token <sym> NAME
%token <dval> NUMBER
%token <ival> DIR

%left '-' '+'
%left '*' '/'
%nonassoc UMINUS

%%
interface
	: INTERFACE NAME '{' func_decl_list '}' {
			Iface iface($2);
			iface.swap(funcList);
			ifaceList.add(iface);
			cout << filename << ":interface: " << $2 << endl; 
			free($2); 
		}
	;

func_decl_list
	:	func_decl
	|	func_decl_list func_decl
	;

func_decl
	: NAME NAME '(' ')'	';'	{ 
			funcList.push_back(Func($1, $2));
			free($1); free($2);
		}
	| NAME NAME '(' param_list ')'	';'	{
			Func func($1, $2);
			func.swap(paramList);
			funcList.push_back(func);
			free($1); free($2);
		}
	;

param_list
	: param
	| param_list ',' param

param
	: DIR NAME NAME { 
			paramList.push_back(Param(Dir($1), $2, $3));
			free($2); free($3);
		}
	| NAME NAME { 
			paramList.push_back(Param(IN, $1, $2));
			free($1); free($2);
		}
	;

