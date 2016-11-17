#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#ifndef _MSC_VER
#include <boost/locale.hpp>
#else
#include <boost/scoped_array.hpp>
#include "windows.h"
#include "winnls.h" // unicode-multibyte conversion
#endif

inline std::string wide2utf8(const std::wstring& text) {
#ifndef _MSC_VER
	return boost::locale::conv::to_utf<wchar_t>(text, "UTF-8");
#else
	const size_t len_needed = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (UINT)(text.length()) , NULL, 0, NULL, NULL) ;
	boost::scoped_array<char> buff(new char[len_needed+1]) ;
	const size_t num_copied = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (UINT)(text.length()) , buff.get(), len_needed+1, NULL, NULL) ;
	return std::string(buff.get(), num_copied) ;
#endif
}

namespace std {

	inline ostream& operator<<(ostream& out, const wchar_t* value)
	{
		wstring text(value) ;
		out << wide2utf8(text);
		return out;
	}

	inline ostream& operator<<(ostream& out, const wstring& value)
	{
		out << wide2utf8(value);
		return out;
	}
}


