// Func.h

#ifndef __Func_h
#define __Func_h

#include <iostream>
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////////
// Parameters

class Param : public Value
{
	Dir dir_;

public:
	Param() {}
	Param(Dir d, const std::string& t, const std::string& n) : Value(t, n), dir_(d) {}

	Dir dir() const { return dir_; }
};

/////////////////////////////////////////////////////////////////////////////

typedef std::vector<Param> ParamList;

/////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Param& p);
std::ostream& operator<<(std::ostream& os, const ParamList& pl);

/////////////////////////////////////////////////////////////////////////////
//							Functions (methods)
/////////////////////////////////////////////////////////////////////////////

class Func : public Value
{
	ParamList params_;

public:
	Func() {}
	Func(const std::string& t, const std::string& n) : Value(t, n) {}

	const ParamList& params() const { return params_; }

	void swap(ParamList& pl) { params_.swap(pl); }

	/**
     * Gets a string name for the function constant.
     * This changes the identifier name into a constant-style name, by
     * converting to upper-case.
     * For example my_func() is changed to MY_FUNC_CMD
	 * 
	 * @return string
	 */
	std::string cmd_const_name() const;

	std::string cmd_func_name() const {
		return name_ + "_cmd";
	}
	/**
     * Writes the function declaration.
     * If the className is non-empty, the function name is written as
     * ClassName::FuncName
     * @param os The output stream
     * @param className The name of the class containing the function
     *                  (method)
     * @return ostream& The output stream
	 */
	std::ostream& write_decl(std::ostream& os, const std::string& className="") const;
};

/////////////////////////////////////////////////////////////////////////////

typedef std::vector<Func> FuncList;

/////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Func& f);
std::ostream& operator<<(std::ostream& os, const FuncList& fl);

#endif		// __Func_h

