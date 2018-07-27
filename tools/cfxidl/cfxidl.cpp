// cfxidl.cpp

#include "cfxidl.h"
#include "Func.h"
#include "Iface.h"

using namespace std;

ParamList	paramList;
FuncList	funcList;
IfaceList	ifaceList;

string		filename;

/////////////////////////////////////////////////////////////////////////////

const char* Primitives::primList[] = {
	"char", "byte", "int8", "int8_t", "uint8", "uint8_t",
	"short", "int16", "int16_t", "uint16", "uint16_t",
	"int", "int32", "int32_t", "uint32", "uint32_t",
	"long", "int64", "int64_t", "uint64", "uint64_t",
	"float", "double",
	0
};

Primitives* Primitives::instance_ = 0;

Primitives::Primitives()
{
	const char **p = primList;

	while (*p) {
		primSet_.insert(string(*p));
		++p;
	}
}

Primitives* Primitives::instance()
{
	if (instance_ == 0)
		instance_ = new Primitives;
	return instance_;
}

/////////////////////////////////////////////////////////////////////////////

string Value::err_val() const
{
	if (type_ == "void")
		return string();

	if (type_ == "bool")
		return string("false");

	string sret = type_;

    if (is_primitive())
		sret += string("(0)");
	else
		sret += string("()");

	return sret;
}

// --------------------------------------------------------------------------

ostream& operator<<(ostream& os, const Value& v)
{
	os << "Type: " << v.type() << ", Name: " << v.name();
	return os;
}

/////////////////////////////////////////////////////////////////////////////
// 

#include <stdio.h>

extern "C" FILE *yyin;

extern "C" {
	int yyparse(void);
}

// --------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	if (argc == 1) {
		do {
			yyparse();
		}
		while (!feof(yyin));
	}
	else {
		for (int i=1; i<argc; ++i) {
			filename = argv[i];
			yyin = fopen(argv[i], "r");
			if (yyin == 0) {
				cerr << "Error opening file: " << filename << endl;
				return EXIT_FAILURE;
			}
			do {
				yyparse();
			}
			while (!feof(yyin));
		}
	}

	for (IfaceList::const_iterator p=ifaceList.begin(); p!= ifaceList.end(); ++p) {
		//p->write_impl_hdr(cout);
		p->create_files();
	}

	return 0;
}

