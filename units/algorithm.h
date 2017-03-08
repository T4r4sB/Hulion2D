#pragma once
#include <string>

namespace tblib
{
	template <typename T, typename U>
	void enlarge(T& t, const U& u)
	{ if (t<u) t=u; }
	
	template <typename T, typename U>
	void shrink(T& t, const U& u)
	{ if (t>u) t=u; }
	
	template <typename T, typename U>
	void inbound(T& t, const U& u1, const U& u2)
	{ if (t<u1) t=u1; else if (t>u2) t=u2; }

	inline std::string to_string (int i)
	{
		char buf[32];
		sprintf(buf, "%d", i);
		return std::string(buf);
	}
	
	inline std::string to_string (uint32_t i)
	{
		char buf[32];
		sprintf(buf, "%u", i);
		return std::string(buf);
	}
	
	inline std::string to_string (long double f)
	{
		char buf[32];
		sprintf(buf, "%f", f);
		return std::string(buf);
	}

	template <typename T> 
	class tmpsaver
	{
		T& ref;
		T val;
		bool mustsave;
		tmpsaver (const tmpsaver&);
		tmpsaver& operator = (const tmpsaver&);

	public :
		tmpsaver (T& ref) : ref(ref), val(ref), mustsave(true) {}
		~tmpsaver () { if (mustsave) ref=val; }
		void approve () { mustsave=false; }
	};

	template <typename T> void recreate (T& t) 
		{ t.~T(); new(&t) T(); }

	template <typename T, typename A1> void recreate (T& t, const A1& a1)
		{ t.~T(); new(&t) T(a1); }
		
	template <typename T, typename A1, typename A2> void recreate (T& t, const A1& a1, const A2& a2)
		{ t.~T(); new(&t) T(a1, a2); }
		
	template <typename T, typename A1, typename A2, typename A3> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3)
		{ t.~T(); new(&t) T(a1, a2, a3); }
		
	template <typename T, typename A1, typename A2, typename A3, typename A4> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
		{ t.~T(); new(&t) T(a1, a2, a3, a4); }
		
	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6); }
	
	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10> void recreate (T& t, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
		{ t.~T(); new(&t) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
};