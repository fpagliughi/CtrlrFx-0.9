// Func.cpp

#include "cfxidl.h"
#include "Func.h"
#include <algorithm>
#include <iterator>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//							Parameters
/////////////////////////////////////////////////////////////////////////////

// Writes the parameter as compatible with a C++ function declaration or
// definition.

ostream& operator<<(ostream& os, const Param& p)
{
	if (p.dir() == IN) {
		if (p.is_primitive())
			os << p.type() << " " << p.name();
		else
			os << "const " << p.type() << "& " << p.name();
	}
	else
		os << p.type() << "* " << p.name();

	return os;
}

// --------------------------------------------------------------------------
// Writes the parameter list compatible with a C++ function declaration or
// definition.

ostream& operator<<(ostream& os, const ParamList& pl)
{
	ParamList::const_iterator last = pl.end();
	if (last != pl.begin()) {
		copy(pl.begin(), --last, ostream_iterator<Param>(os, ", "));
		os << *last;
	}
	return os;
}

/////////////////////////////////////////////////////////////////////////////
//							Functions
/////////////////////////////////////////////////////////////////////////////

string Func::cmd_const_name() const
{
	string cmdName = name_;
	for (string::iterator p=cmdName.begin(); p!=cmdName.end(); ++p)
		*p = ::toupper(*p);
	cmdName += "_CMD";
	return cmdName;
}

// --------------------------------------------------------------------------

ostream& Func::write_decl(ostream& os, const string& className /*=""*/) const
{
	if (className.empty())
		os << (*this);
	else {
		os << type_ << " " << className << "::" 
			<< name_ << "(" << params_ << ")";
	}
	return os;
}

/////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const Func& f)
{
	os << "virtual " << f.type() << " " 
		<< f.name() << "(" << f.params() << ")";
	return os;
}

// --------------------------------------------------------------------------

ostream& operator<<(ostream& os, const FuncList& fl)
{
	for (FuncList::const_iterator p=fl.begin(); p!=fl.end(); ++p)
		os << (*p) << endl;
	return os;
}


