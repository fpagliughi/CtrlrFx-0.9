// Iface.h

#ifndef __Iface_h
#define __Iface_h

#include <iostream>
#include <string>
#include "Func.h"

/////////////////////////////////////////////////////////////////////////////

struct Iface
{
	std::string name_;
	FuncList	funcs_;

	static const std::string HDR_EXT, SRC_EXT;

public:
	Iface() {}
	Iface(const std::string& n) : name_(n) {}

	std::string name() const { return name_; }
	std::string iface_name() const { return std::string("I") + name_; }

	void swap(FuncList& fl) { funcs_.swap(fl); }

	std::ostream& write_func_sep(std::ostream& os) const;
	std::ostream& write_class_sep(std::ostream& os) const;

	// ----- Output Methods -----

	std::ostream& write_impl_hdr(std::ostream& os) const;
	
	void create_files() const;

	bool create_iface() const;

	bool create_stub() const;
	bool create_stub_hdr() const;
	bool create_stub_src() const;

	bool create_skel() const;
	bool create_skel_hdr() const;
	bool create_skel_src() const;

	bool create_impl() const;
	bool create_impl_hdr() const;
	bool create_impl_src() const;

};

/////////////////////////////////////////////////////////////////////////////

class IfaceList : public std::vector<Iface>
{
public:
	void add(const Iface& inf) { push_back(inf); }
};

std::ostream& operator<<(std::ostream& os, const Iface& inf);
std::ostream& operator<<(std::ostream& os, const IfaceList& infl);

#endif		// __Iface_h

