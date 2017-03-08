#pragma once
#include "assert.h"

namespace tblib
{
	class empty {};

	template <typename Sig> 
	struct sig_info
	{
		enum { arg_count=-1 };
		typedef empty R;
		typedef empty A1;
		typedef empty A2;
		typedef empty A3;
		typedef empty A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};
	
	template <typename R> 
	struct sig_info<R()>
	{
		enum { arg_count=0 };
		typedef R     R;
		typedef empty A1;
		typedef empty A2;
		typedef empty A3;
		typedef empty A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};
	
	template <typename R, typename A1> 
	struct sig_info<R(A1)>
	{
		enum { arg_count=1 };
		typedef R     R;
		typedef A1    A1;
		typedef empty A2;
		typedef empty A3;
		typedef empty A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};	
	
	template <typename R, typename A1, typename A2> 
	struct sig_info<R(A1,A2)>
	{
		enum { arg_count=2 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef empty A3;
		typedef empty A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};	
	
	template <typename R, typename A1, typename A2, typename A3> 
	struct sig_info<R(A1,A2,A3)>
	{
		enum { arg_count=3 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef empty A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};	
	
	template <typename R, typename A1, typename A2, typename A3, typename A4> 
	struct sig_info<R(A1,A2,A3,A4)>
	{
		enum { arg_count=4 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef A4    A4;
		typedef empty A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};	
	
	template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5> 
	struct sig_info<R(A1,A2,A3,A4,A5)>
	{
		enum { arg_count=5 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef A4    A4;
		typedef A5    A5;
		typedef empty A6;
		typedef empty A7;
		typedef empty A8;
	};
	
	template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> 
	struct sig_info<R(A1,A2,A3,A4,A5,A6)>
	{
		enum { arg_count=6 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef A4    A4;
		typedef A5    A5;
		typedef A6    A6;
		typedef empty A7;
		typedef empty A8;
	};
	
	template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> 
	struct sig_info<R(A1,A2,A3,A4,A5,A6,A7)>
	{
		enum { arg_count=7 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef A4    A4;
		typedef A5    A5;
		typedef A6    A6;
		typedef A7    A7;
		typedef empty A8;
	};	
	
	template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8> 
	struct sig_info<R(A1,A2,A3,A4,A5,A6,A7,A8)>
	{
		enum { arg_count=8 };
		typedef R     R;
		typedef A1    A1;
		typedef A2    A2;
		typedef A3    A3;
		typedef A4    A4;
		typedef A5    A5;
		typedef A6    A6;
		typedef A7    A7;
		typedef A8    A8;
	};

	template <typename F>
	class fptr
	{
		F* ptr;
	public :
		fptr () : ptr(NULL) {}
		fptr (F* ptr) : ptr(ptr) {}
		fptr (const fptr& f) : ptr(f.ptr) {}
		fptr& operator=(F* ptr) { this->ptr=ptr; return *this; }
		fptr& operator=(const fptr& f) { this->ptr=f.ptr; return *this; }

		typedef typename sig_info<F>::R  R;
		typedef typename sig_info<F>::A1 A1;
		typedef typename sig_info<F>::A2 A2;
		typedef typename sig_info<F>::A3 A3;
		typedef typename sig_info<F>::A4 A4;
		typedef typename sig_info<F>::A5 A5;
		typedef typename sig_info<F>::A6 A6;
		typedef typename sig_info<F>::A7 A7;
		typedef typename sig_info<F>::A8 A8;

		R operator () () { assert (ptr); return ptr(); }		
		R operator () (A1 a1) { assert (ptr); return ptr(a1); }
		R operator () (A1 a1, A2 a2) { assert (ptr); return ptr(a1,a2); }
		R operator () (A1 a1, A2 a2, A3 a3) { assert (ptr); return ptr(a1,a2,a3); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4) { assert (ptr); return ptr(a1,a2,a3,a4); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) { assert (ptr); return ptr(a1,a2,a3,a4,a5); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6,a7); }
		R operator () (A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) { assert (ptr); return ptr(a1,a2,a3,a4,a5,a6,a7,a8); }
	};

};