#ifndef BOAAA_DUMP_OSTREAM_H
#define BOAAA_DUMP_OSTREAM_H

#include <ostream>
#include <sstream>

namespace boaaa
{
	/*
	class dump_streambuf : public std::streambuf
	{
	public:
		static dump_streambuf create() { return dump_streambuf(); }
	}; */

	class dump_ostream : public std::ostream
	{

	public:

		dump_ostream(std::_Uninitialized uinit) : std::ostream(uinit) {}

		bool bad() const { return false; }
		void clear(int _State, bool _Reraise = false) {}
		std::ios& copyfmt(const std::ios& _Right) { return *this; }
		bool eof() const { return true; }
		std::ios_base::iostate exceptions() const { return 0; }
		void exceptions(std::ios_base::iostate except) const {}
		bool fail() const { return false; }
		//fill not overridden
		//not override flags
		void flags(std::ios_base::fmtflags flags) {};
		std::ostream& flush() { return *this; }
		//getloc not overriden
		bool good() const { return true; }
		//not override imbue
		long& iword(int _Idx) { static long x; return x; }
		void move(basic_ios&& _Right) {}
		char narrow(char _Ch, char _Dflt = '\0') const { return _Ch; }
		bool opfx() { return true; }
		void osfx() {}
		std::streamsize precision() const { return 2; }
		std::streamsize precision(std::streamsize _Newprecision) { return _Newprecision; }
		basic_ostream& put(char _Ch) { return *this; }
		void*& pword(int _Idx) { static void* x = nullptr; return x; }
		std::streambuf* rdbuf() const { return nullptr; }
		std::ios_base::iostate rdstate() const { return 0; }
		void register_callback(event_callback _Pfn, int _Idx) {}
		basic_ostream& seekp(pos_type _Pos) { return *this; }
		fmtflags setf(fmtflags _Newfmtflags) { return setf(_Newfmtflags, 0); }
		fmtflags setf(fmtflags _Newfmtflags, fmtflags _Mask) { return _Newfmtflags; }
		void setstate(std::ios_base::iostate _State, bool _Reraise = false) {}
		void set_rdbuf(_Mysb* _Strbuf) {}
		void swap(basic_ios& _Right) noexcept {}
		static bool sync_with_stdio(bool _Newsync = true) { return true; }
		std::ostream::pos_type tellp() { return 0; }
		//dont override tie
		void unsetf(fmtflags _Mask) {}
		char widen(char _Byte) const { return _Byte; }
		std::streamsize width(std::streamsize _width) const { return _width; }
		basic_ostream& write(const char* _Str, std::streamsize _Count) { return *this; }
		//not overriden xalloc

	};

}

#endif // !BOAAA_DUMP_OSTREAM_H