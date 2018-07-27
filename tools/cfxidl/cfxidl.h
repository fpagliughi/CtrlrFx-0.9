// cfxidl.h

#ifndef __cfxidl_h
#define __cfxidl_h

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <stdexcept>
#include "cfxidl_def.h"

/////////////////////////////////////////////////////////////////////////////

class cfx_exception : public std::exception
{
	std::string msg_;

public:
	cfx_exception(const std::string& msg) : msg_(msg) {}
	virtual ~cfx_exception() throw() {}
	virtual const char* what() const throw() { return msg_.c_str(); }
};

/////////////////////////////////////////////////////////////////////////////

class Primitives
{
	static const char* primList[];
	static Primitives* instance_;

	std::set<std::string> primSet_;

	Primitives();

public:
	static Primitives* instance();

	static bool contains(const std::string& type) {
		return instance()->primSet_.find(type) != instance()->primSet_.end();
	}
};

/////////////////////////////////////////////////////////////////////////////

class Value
{
protected:
	std::string	type_,
				name_;

public:
	Value() {}
	Value(const std::string& t, const std::string& n) : type_(t), name_(n) {}

	std::string type() const { return type_; }
	std::string name() const { return name_; }

	bool is_primitive() const { return Primitives::contains(type_); }
	std::string err_val() const;
};

/////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Value& v);

#endif		// __cfxidl_h


