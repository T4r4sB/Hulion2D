/*
Copyright (c) 2006 Henry Strickland & Ryan Seto

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

        (* http://www.opensource.org/licenses/mit-license.php *)
*/

#pragma once 

#include "assert.h"
#include "tbstream.h"

inline int ilog2(uint32_t n)
{
#ifdef __GNUC__
	return __builtin_clz(n) ^ 31;
#else
	int res = 0;
	for (int ord = 16; ord; ord /= 2)
		if (n >= ((uint32_t)1 << ord)) 
			{ res += ord, n >>= ord;  }
   return res;
#endif
}


struct Fixed {
	int	g; // the guts

	const static int BP= 16;  // how many low bits are right of Binary Point
	const static int BP2= BP*2;  // how many low bits are right of Binary Point
	const static int BPhalf= BP/2;  // how many low bits are right of Binary Point

	static double STEP() { return 1.0 / (1<<BP); }  // smallest step we can represent

	// for private construction via guts
	enum FixedRaw { RAW };
	Fixed(FixedRaw, int guts) : g(guts) {}

	static bool IsOk(long long i) {
	const long long max=(long long)(1<<BP)*(long long)(1<<BP)*(long long)(1<<(BP-1));
	return (i<max && -i<max); }

	Fixed() : g(0) {}
	Fixed(const Fixed& a) : g( a.g ) {}
	Fixed(int a) : g( a << BP ) {}
	Fixed(int a, int b) : g( (int)((((long long)a << BP) + (b/2))/b) ) {}
	inline Fixed& operator =(const Fixed& a) { g= a.g; return *this; }
	inline Fixed& operator =(int a) { g= Fixed(a).g; return *this; }

	inline operator float()  const { return g * (float)STEP(); }
	inline operator double() const { return g * (double)STEP(); }
	inline operator int()    const { return g>>BP; }
	inline operator long()   const { return g>>BP; }

	inline int Round () const { return (g+(1<<(BP-1)))>>BP; }

	inline Fixed operator +() const { return Fixed(RAW,g); }
	inline Fixed operator -() const { return Fixed(RAW,-g); }

	inline Fixed operator +(const Fixed& a) const { return Fixed(RAW, g + a.g); }
	inline Fixed operator -(const Fixed& a) const { return Fixed(RAW, g - a.g); }
	// more acurate, using long long
	inline Fixed operator *(const Fixed& a) const {
		assert (IsOk((long long)g * (long long)a.g));		
		return Fixed(RAW,  (int)( ((long long)g * (long long)a.g ) >> BP)); }
	// faster, but with only half as many bits right of binary point
	//Fixed operator *(const Fixed& a) const { return Fixed(RAW, (g>>BPhalf) * (a.g>>BPhalf) ); }
	inline Fixed operator /(const Fixed& a) const { return Fixed(RAW, int( (((long long)g << BP2) / (long long)(a.g)) >> BP) ); }
//
	inline Fixed Abs  () const { return g>0 ? *this : Fixed(RAW, -g); }
	inline Fixed Frac () const { return Fixed(RAW, g&((1<<BP)-1)); }

	inline Fixed operator >>(int a) const { return Fixed(RAW, g>>a); }
	inline Fixed operator <<(int a) const { return Fixed(RAW, g<<a); }

	inline Fixed& operator >>=(int a) { g>>=a; return *this; }
	inline Fixed& operator <<=(int a) { g<<=a; return *this; }

	inline Fixed& operator +=(Fixed a) { return *this = *this + a; }
	inline Fixed& operator -=(Fixed a) { return *this = *this - a; }
	inline Fixed& operator *=(Fixed a) { return *this = *this * a; }
	inline Fixed& operator /=(Fixed a) { return *this = *this / a; }

	inline Fixed& operator +=(int a) { return *this = *this + (Fixed)a; }
	inline Fixed& operator -=(int a) { return *this = *this - (Fixed)a; }
	inline Fixed& operator *=(int a) { return *this = *this * (Fixed)a; }
	inline Fixed& operator /=(int a) { return *this = *this / (Fixed)a; }

	inline bool operator ==(const Fixed& a) const { return g == a.g; }
	inline bool operator !=(const Fixed& a) const { return g != a.g; }
	inline bool operator <=(const Fixed& a) const { return g <= a.g; }
	inline bool operator >=(const Fixed& a) const { return g >= a.g; }
	inline bool operator  <(const Fixed& a) const { return g  < a.g; }
	inline bool operator  >(const Fixed& a) const { return g  > a.g; }

	inline int ILog2 () const { return ilog2(abs(g)|1)-BP; }

	Fixed (const tblib::ibstream& s) : g(tblib::To<int>::From(s)) {}
	friend const tblib::obstream& operator << (const tblib::obstream& s, const Fixed &f) 
		{ return s<<f.g; }
};

inline int& operator +=(int& a, const Fixed& b) { a = (Fixed)a + b; return a; }
inline int& operator -=(int& a, const Fixed& b) { a = (Fixed)a - b; return a; }
inline int& operator *=(int& a, const Fixed& b) { a = (Fixed)a * b; return a; }
inline int& operator /=(int& a, const Fixed& b) { a = (Fixed)a / b; return a; }

inline Fixed MulDiv (Fixed a, Fixed b, Fixed c)
{
	return Fixed(Fixed::RAW, int((((long long)a.g)*((long long)b.g))/c.g));
}

inline Fixed DoubleToFixed (double a) { return Fixed(Fixed::RAW, int(a*double(1<<Fixed::BP))); }

inline Fixed fxd(int a) { return Fixed(a,10); }
inline Fixed fxh(int a) { return Fixed(a,100); }
inline Fixed fxt(int a) { return Fixed(a,1000); }

const Fixed 
	fx0 = 0, 
	fx1 = 1, 
	fx2 = 2, 
	fx3 = 3, 
	fx4 = 4, 
	fx5 = 5, 
	fx6 = 6, 
	fx7 = 7, 
	fx8 = 8, 
	fx9 = 9, 
	fx10 = 10, 
	fxEps(Fixed::RAW,1), 
	fxHalf(1,2);
