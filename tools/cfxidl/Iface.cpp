// Iface.cpp

#include "cfxidl.h"
#include "Iface.h"
#include <fstream>

using namespace std;

const string Iface::HDR_EXT = ".h";
const string Iface::SRC_EXT = ".cpp";

// --------------------------------------------------------------------------

ostream& Iface::write_func_sep(ostream& os) const
{
	os << "// ";
	for (int i=0; i<74; ++i)
		os << '-';
	os << "\n\n";
	return os;
}

// --------------------------------------------------------------------------

ostream& Iface::write_class_sep(ostream& os) const
{
	for (int i=0; i<77; ++i)
		os << '/';
	os << "\n\n";
	return os;
}

// --------------------------------------------------------------------------

void Iface::create_files() const
{
	create_iface();
    create_stub();
	create_skel();
	create_impl();
}

// --------------------------------------------------------------------------
//								Interface File
// --------------------------------------------------------------------------

bool Iface::create_iface() const
{
	string	ifaceName = iface_name(),
			filename = ifaceName + HDR_EXT;

	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open interface header.");

	string fdef = string("__") + filename;
	replace(fdef.begin(), fdef.end(), '.', '_');

	os << "// " << filename << endl;
	os << "// Implementation for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n\n";
	os << "#ifndef " << fdef << endl;
	os << "#define " << fdef << "\n\n";

	os << "class " << ifaceName << "\n{\n";

	os << "protected:\n";
	os << "\t// TODO: Assign a class number\n";
	os << "\tstatic const uint16_t OBJ_CLASS = 0;\n\n";

	if (!funcs_.empty()) {
		FuncList::const_iterator p, last = funcs_.end()-1;

		os << "\tenum { ";
		for (p=funcs_.begin(); p!=funcs_.end(); ++p)
			os << p->cmd_const_name() << ", ";
		os << "N_CMD };\n";

		os << "\npublic:\n";
		for (p=funcs_.begin(); p!=funcs_.end(); ++p)
			os << "\t" << (*p) << " =0;\n";
	}

	os << "\n\tvirtual ~" << ifaceName << "() {}\n";
	os << "};\n";

	os << "\n#endif\t\t// " << fdef << endl;
	return !(!os);
}

// --------------------------------------------------------------------------
//								Stub Files
// --------------------------------------------------------------------------

bool Iface::create_stub() const
{
	return create_stub_hdr() &&
			create_stub_src();
}

// --------------------------------------------------------------------------

bool Iface::create_stub_hdr() const
{
	string	stubName = name_ + "_stub",
			filename = stubName + HDR_EXT;

	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open stub header");

	string fdef = string("__") + filename;
	replace(fdef.begin(), fdef.end(), '.', '_');

	os << "// " << filename << endl;
	os << "// Stub for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n" << endl;
	os << "#ifndef " << fdef << endl;
	os << "#define " << fdef << '\n' << endl;

	os << "#include \"" << iface_name() << HDR_EXT << "\"\n" << endl;

	os << "class " << stubName << " : virtual public " << iface_name() << "\n{\n";

	os << "\ttypedef CtrlrFx::BinNativeEncoder Encoder;\n";
	os << "\ttypedef CtrlrFx::BinNativeDecoder Decoder;\n";
	// TODO: Allow for variable size buffers
	os << "\ttypedef CtrlrFx::FixedBuffer<byte, CtrlrFx::DistObjSrvr::DFLT_CMD_RSP_SIZE> CmdBuffer;\n";
	os << "\ttypedef CtrlrFx::IDistObj::op_t op_t;\n";
	os << "\n";

	os << "\tCtrlrFx::DistObjSrvr* objSrvr_;\n";
	os << "\n";

	os << "\tvoid put_cmd_hdr(CtrlrFx::BufEncoder& enc, op_t cmd) const;\n";

	os << "\npublic:\n";
	os << "\tCtrlrFx::DistObjSrvr* obj_srvr() const { return objSrvr_; }\n";
	os << "\tvoid obj_srvr(CtrlrFx::DistObjSrvr* srvr) { objSrvr_ = srvr; }\n";

	os << "\n";
	for (FuncList::const_iterator p=funcs_.begin(); p!=funcs_.end(); ++p)
		os << "\t" << (*p) << ";\n";
	os << "};" << endl;

	os << "\n#endif\t\t// " << fdef << endl;
	return !(!os);
}

// --------------------------------------------------------------------------

bool Iface::create_stub_src() const
{
	string	stubName = name_ + "_stub",
			filename = stubName + SRC_EXT,
			hdrname = stubName + HDR_EXT;

	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open stub source");

	os << "// " << filename << endl;
	os << "// Stub for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n\n";
	os << "#include \"CtrlrFx/CtrlrFx.h\"\n";
	os << "#include \"CtrlrFx/DistObjSrvr.h\"\n";
	os << "#include \"CtrlrFx/BufCodec.h\"\n";
	os << "#include \"CtrlrFx/FixedBuffer.h\"\n";
	os << "#include \"" << hdrname << "\"\n\n";

	os << "using namespace CtrlrFx;\n\n";

	write_func_sep(os);
	os << "void " << stubName << "::put_cmd_hdr(BufEncoder& enc, op_t cmd) const\n{\n";
	os << "\tenc.put_uint32(objSrvr_->next_msg_id());\n";
	// TODO: Enumerate objects
	os << "\tenc.put_uint32(tgt_obj(OBJ_CLASS, uint16_t(0)));\n";
	os << "\tenc.put_uint32(cmd);\n";
	os << "}\n\n";

	for (FuncList::const_iterator p=funcs_.begin(); p!=funcs_.end(); ++p) {
		write_func_sep(os);
        p->write_decl(os, stubName);
		os << "\n{\n";
		os << "\tCmdBuffer cmd, rsp;\n";
		os << "\tEncoder cmdEnc(cmd);\n";
		os << "\n";

		os << "\tput_cmd_hdr(cmdEnc, " << p->cmd_const_name() << ");\n";
		os << "\n";

		const ParamList& pl = p->params();
		ParamList::const_iterator ppl;
		int nin = 0, nout = 0;

		for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
			Dir dir = ppl->dir();
			if (dir == IN || dir == INOUT)
				++nin;
			if (dir == OUT || dir == INOUT)
				++nout;
		}

		if (nin > 0) {
			os << "\tcmdEnc";
			for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
				if (ppl->dir() == IN)
					os << " << " << ppl->name();
				else if (ppl->dir() == INOUT)
					os << " << *" << ppl->name();
			}
			os << ";\n\n";
		}

		if (p->type() == "void" && nout == 0) {
			os << "\tobjSrvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp);\n";
		}
		else {
			os << "\tint _n = objSrvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp);\n";
			os << "\tif (_n < 0)\n\t\t";
			os << "return " << p->err_val() << ";\n\n";

			if (p->type() != "void")
				os << "\t" << p->type() << " _ret;\n";

			os << "\tDecoder rspDec(rsp);\n";
			os << "\trspDec";

			for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
				if (ppl->dir() == OUT || ppl->dir() == INOUT)
					os << " >> *" << ppl->name();
			}

			if (p->type() != "void") {
				os << " >> _ret;\n";
				os << "\treturn (rspDec) ? _ret : " << p->err_val();
			}
			os << ";\n";
		}
		os << "}\n" << endl;
	}

	return !(!os);
}

// --------------------------------------------------------------------------
//								Skel Files
// --------------------------------------------------------------------------

bool Iface::create_skel() const
{
	return create_skel_hdr() &&
			create_skel_src();
}

// --------------------------------------------------------------------------

bool Iface::create_skel_hdr() const
{
	string	skelName = name_ + "_skel",
			filename = skelName + HDR_EXT;

	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open skel header");

	string fdef = string("__") + filename;
	replace(fdef.begin(), fdef.end(), '.', '_');

	os << "// " << filename << endl;
	os << "// Skel for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n" << endl;
	os << "#ifndef " << fdef << endl;
	os << "#define " << fdef << '\n' << endl;

	os << "#include \"" << iface_name() << HDR_EXT << "\"\n";
	os << "#include \"CtrlrFx/DistObj.h\"\n";
	os << "\n";

	write_class_sep(os);

	os << "class " << skelName << " : virtual public " << iface_name() 
		<< ",\n\t\t\tvirtual public CtrlrFx::IDistObj\n{\n";

	os << "\ttypedef CtrlrFx::BinNativeEncoder Encoder;\n";
	os << "\ttypedef CtrlrFx::BinNativeDecoder Decoder;\n";
	os << "\ttypedef CtrlrFx::IDistObj::op_t op_t;\n";
	os << "\n";

	os << "\ttypedef int (" << skelName << "::* CmdPtr)(Decoder& param, Encoder& rsp);\n";
	os << "\n\tstatic const CmdPtr cmdTbl_[];\n";
	os << "\n";

	os << "\t// ----- Distributed callback methods -----\n";
	for (FuncList::const_iterator p=funcs_.begin(); p!=funcs_.end(); ++p)
		os << "\tint " << p->cmd_func_name() << "(Decoder& param, Encoder& rsp);\n";

	os << "\npublic:\n";
	os << "\tvirtual int invoke(op_t op, Decoder& param, Encoder& rsp);\n";

	os << "};" << endl;
	os << "\n#endif\t\t// " << fdef << endl;

	return !(!os);
}

// --------------------------------------------------------------------------

bool Iface::create_skel_src() const
{
	string	skelName = name_ + "_skel",
			filename = skelName + SRC_EXT,
			hdrname = skelName + HDR_EXT;

	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open skel source");

	FuncList::const_iterator p;

	os << "// " << filename << endl;
	os << "// Skel for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n\n";
	os << "#include \"CtrlrFx/CtrlrFx.h\"\n";
	os << "#include \"" << hdrname << "\"\n\n";

	os << "using namespace CtrlrFx;\n\n";

	write_class_sep(os);

	os << "const " << skelName << "::CmdPtr " << skelName << "::cmdTbl_[] = {\n";
	for (p=funcs_.begin(); p!=funcs_.end(); ++p)
		os << "\t&" << skelName << "::" << p->cmd_func_name() << ",\n";
	os << "\t0\n};\n\n"; 

	for (p=funcs_.begin(); p!=funcs_.end(); ++p) {
		const ParamList& pl = p->params();
		ParamList::const_iterator ppl, last = pl.end()-1;
		int nin = 0, nout = 0;

		for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
			Dir dir = ppl->dir();
			if (dir == IN || dir == INOUT)
				++nin;
			if (dir == OUT || dir == INOUT)
				++nout;
		}

		bool	anyIn = (nin != 0),
				anyRsp = (nout > 0 || p->type() != "void");

		write_func_sep(os);
        os << "int " << skelName << "::" << p->cmd_func_name() 
			<< "(Decoder& param, Encoder& rsp)\n{\n";

		// ----- Parameter List -----

		for (ppl=pl.begin(); ppl!=pl.end(); ++ppl)
			os << "\t" << ppl->type() << " " << ppl->name() << ";\n";

		// ----- Read inputs into in vars -----

		if (anyIn) {
			os << "\tparam";
			for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
				if (ppl->dir() == IN || ppl->dir() == INOUT)
					os << " >> " << ppl->name();
			}
			os << ";\n";
		}

		// ----- Call the method -----

		os << "\t";
		if (p->type() != "void")
			os << p->type() << " _ret = ";
		os << p->name() << "(";

		for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
			if (ppl->dir() == OUT || ppl->dir() == INOUT)
				os << '&';
			os << ppl->name();
			if (ppl != last)
				os << ", ";
		}
		os << ");\n";

		// ----- Encode the response(s) -----

		if (anyRsp) {
			os << "\trsp";
			for (ppl=pl.begin(); ppl!=pl.end(); ++ppl) {
				if (ppl->dir() == OUT || ppl->dir() == INOUT)
					os << " << " << ppl->name();
			}
			if (p->type() != "void")
				os << " << _ret";
			os << ";\n";
		}

		os << "\treturn 0;\n";
		os << "}\n\n";
	}

	// ----- The invoke() method -----

	write_func_sep(os);
	os << "int " << skelName 
		<< "::invoke(op_t op, Decoder& param, Encoder& rsp)\n{\n";
	os << "\tif (unsigned(op) > N_CMD)\n\t\treturn -1;\n\n";
	os << "\tCmdPtr p = cmdTbl_[unsigned(op)];\n";
	os << "\treturn (p == 0) ? -1 : ((this->*p)(param, rsp));\n}";

	os << endl;
	return !(!os);
}

// --------------------------------------------------------------------------
//							Implementation Files
// --------------------------------------------------------------------------

bool Iface::create_impl() const
{
	return create_impl_hdr() &&
			create_impl_src();
}

// --------------------------------------------------------------------------

bool Iface::create_impl_hdr() const
{
	string filename = name_ + HDR_EXT;
	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open implementation header");

	string fdef = string("__") + filename;
	replace(fdef.begin(), fdef.end(), '.', '_');

	os << "// " << filename << endl;
	os << "// Implementation for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n" << endl;
	os << "#ifndef " << fdef << endl;
	os << "#define " << fdef << '\n' << endl;

	write_impl_hdr(os);

	os << "\n#endif\t\t// " << fdef << endl;

	return !(!os);
}

// --------------------------------------------------------------------------

bool Iface::create_impl_src() const
{
	string filename = name_ + SRC_EXT;
	ofstream os(filename.c_str());

	if (!os)
		throw cfx_exception("Unable to open implementation source");

	os << "// " << filename << endl;
	os << "// Implementation for interface " << name_ << endl;
	os << "// Generated by cfxidl compiler.\n" << endl;
	os << "#include \"CtrlrFx/CtrlrFx.h\"" << endl;
	os << "#include \"" << name_ << ".h\"\n" << endl;

	for (FuncList::const_iterator p=funcs_.begin(); p!=funcs_.end(); ++p) {
		write_func_sep(os);
        p->write_decl(os, name_);
		os << "\n{\n\t// TODO: Implement this" << endl;
		if (p->type() != "void") {
			os << "\treturn 0" << endl;
		}
		os << "}\n" << endl;
	}

	return !(!os);
}

// --------------------------------------------------------------------------
// Writes the header for the implementation class.

ostream& Iface::write_impl_hdr(ostream& os) const
{
	os << "class " << name_ << "\n{\npublic:\n";
	for (FuncList::const_iterator p=funcs_.begin(); p!=funcs_.end(); ++p)
		os << "\t" << (*p) << ';' << endl;
	os << "};" << endl;

	return os;
}

/////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const Iface& inf)
{
	os << inf.name_ << '\n' << inf.funcs_;
	return os;
}

/////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const IfaceList& infl)
{
	for (IfaceList::const_iterator p=infl.begin(); p!=infl.end(); ++p)
		os << (*p) << endl;
	return os;
}

