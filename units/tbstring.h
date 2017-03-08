#pragma once

#include "tbslice.h"
#include <string>

namespace tblib
{
	typedef tblib::slice<const char> base_stringref;

	class stringref : public base_stringref
	{
	public :
		stringref () : base_stringref() {}
		stringref (const base_stringref& bs) : base_stringref(bs) {}
		stringref (const char* m_begin, int m_low, int m_high) : base_stringref(m_begin, m_low, m_high) {}    
		stringref (const std::string& s) : base_stringref (s.c_str(), 0, int(s.length())) {}
		stringref (const char* c) : base_stringref(c, 0, int(strlen(c))) {}

		operator std::string () const
		{
			return (high()>low()) ? std::string(begin(),end()) : std::string();
		}
	};	
		
	inline bool operator == (stringref sl, const char* c)
	{
		for (int i=sl.low(); i<sl.high(); ++i)
			if (sl[i] != c[i-sl.low()])
				return false;
		return c[sl.high()-sl.low()] == 0;	
	}
};
	
inline std::ostream& operator << (std::ostream& ss, const tblib::stringref& sl)
{
	for (int i=sl.low(); i<sl.high(); ++i)
		ss << sl[i];
	return ss;
}