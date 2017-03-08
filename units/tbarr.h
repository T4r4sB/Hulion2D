#pragma once

#include "stdint.h"
#include "assert.h"
#include <algorithm>
#include "tbslice.h"
#include "align.h"

#include "tbal.h"
#include <sstream>
#include <typeinfo>
#include "tbstream.h"

// FUCK THE EXCEPTIONS

template <typename T>
void construct (T* first, T* last) {
	while (first!=last)	{
		new(first) T;
		++first;
	}
}

template <typename T>
void destroy (T* first, T* last)	{
	while (last!=first)	{
		--last;
		last->~T();
	}
}


#ifndef NDEBUG
#define ARRDEBUG
#endif

namespace tblib
{	
	template <typename T, int N>
	class base_array
	{
		union
		{
			char memory [sizeof(T[N])];
			align<T> aligner;
		};
#ifdef ARRDEBUG
		T* m_begin;
#endif
	public :
		typedef T value_type;
		typedef value_type * iterator;
		typedef const value_type * const_iterator;
		typedef value_type & reference;
		typedef const value_type & const_reference;
		
		iterator begin () {
			return reinterpret_cast<T*> (memory);
		}
		
		const_iterator begin () const {
			return reinterpret_cast<const T*> (memory);
		}

		reference front () {
			return *begin();
		}

		const_reference front () const {
			return *begin();
		}

#ifdef ARRDEBUG
		base_array () : m_begin(begin()) {}
#endif


		reference operator [] (int index) {
			assert (index>=0 && index<N);			
			return begin()[index];
		}

		const_reference operator [] (int index) const {
			assert (index>=0 && index<N);			
			return begin()[index];
		}

		static const int capacity = N;
	};

	template <typename T, int N>
	class carray : public base_array <T,N>
	{
	public :
		typename base_array<T,N>::iterator end ()  {
			return carray::begin()+N;
		}
		
		typename base_array<T,N>::const_iterator end () const {
			return carray::begin()+N;
		}		
		
		typename base_array<T,N>::reference back () {
			return base_array<T,N>::operator [](N-1);
		}
		
		typename base_array<T,N>::const_reference back () const {
			return base_array<T,N>::operator [](N-1);
		}		

		carray () {
			construct(carray::begin(), carray::end());
		}
		
		carray (const T& t) {
			std::uninitialized_fill(carray::begin(), carray::end(), t);
		}

		carray (const T a[N]) {
			std::uninitialized_copy(a, a+N, carray::begin());
		}

		carray (const carray& a)
		{
			std::uninitialized_copy(a.begin(), a.end(), carray::begin());
		}

		~carray () 
		{
			destroy(carray::begin(), carray::begin()+N);
		}

		carray& operator = (const carray &a) {
			if (this!=&a)	{
				std::copy(a.begin(), a.end(), carray::begin());
			}
			return *this;
		}		

		carray& operator = (const T a[N]) {
			std::copy(a, a+N, carray::begin());
			return *this;
		}

		slice<T> get_slice(int low, int high)	{
			assert (low>=0 && high<=N);
			return tblib::slice<T>(&(*this)[low], low, high);
		}

		int size() const { 
			return N; 
		};
		
		carray (const ibstream& s) 
		{
			for (int i=0; i<size(); ++i)
				new (carray::begin()+i) T(tblib::To<T>::From(s));
		}

		friend const obstream& operator << (const obstream& s, const carray& t)
		{
			for (int i=0; i<t.size(); ++i)
				s << t[i];
			return s;
		}
	};
	
	template <typename T, int N>
	class modarray : public carray <T,N>
	{
		static int mod (int index, int n)
		{
			if (n&(n-1))
			{
				index %= n;
				if (index<0) index += n;
				return index;
			} else
				return index & (n-1);
		}
	public :		
		modarray () : carray<T,N> () {}		
		modarray (const T& t) : carray<T,N> (t) {}
		modarray (const carray<T,N> &a) : carray<T,N> (a) {}
		modarray (const T a[N]) : carray<T,N> (a) {}

		T& operator [] (int index) {
			return this->carray<T,N>::operator[] (mod(index,N));
		}

		const T& operator [] (int index) const {
			return this->carray<T,N>::operator[] (mod(index,N));
		}
	};

	template <typename T, int N>
	class array : public base_array <T,N>
	{
		int m_size;
	public :
		typename base_array<T,N>::iterator end ()  {
			return array::begin()+size();
		}
		
		typename base_array<T,N>::const_iterator end () const {
			return array::begin()+size();
		}				
		
		typename base_array<T,N>::reference back () {
			return base_array<T,N>::operator [](size()-1);
		}
		
		typename base_array<T,N>::const_reference back () const {
			return base_array<T,N>::operator [](size()-1);
		}		

		array () : m_size(0) {}

		array (int a_size) : m_size(a_size) {
			construct(array::begin(), array::end());
		}		
		
		array (int a_size, const T& t) : m_size(a_size) {
			std::uninitialized_fill(array::begin(), array::end(), t);
		}		

		~array () {
			shrink(0);
		}

		void array_assert(bool condition)
		{
			if (!condition)
			{
				assert(false);
				std::stringstream ss;
				ss << "Array overflow!!! T= " << typeid(T).name() << " N= " << N;
				tbal::LogW(ss.str().c_str());
				exit(0);
			}
		}

		template <int M>
		array (const array<T, M> &a)
		{
			array_assert (a.size()<=N);
			std::uninitialized_copy(a.begin(), a.end(), array::begin());
			m_size = a.size();
		}		

		template <int M>
		array (const T a[M])
		{
			array_assert (M<=N);
			std::uninitialized_copy(a, a+M, array::begin());
			m_size = M;
		}		

		template <int M>
		array (const carray<T, M> &a)
		{
			array_assert (M<=N);
			std::uninitialized_copy(a, a+M, array::begin());
			m_size = M;
		}		

		template <int M>
		array& operator = (const array<T, M> &a)
		{
			if (this!=&a) {
				array_assert (a.size()<=N);
				if (size()<a.size()) { // увеличить
					std::copy(a.begin(), a.begin()+size(), array::begin());
					std::uninitialized_copy(a.begin()+size(), a.end(), array::end());
				} else { // уменьшить
					std::copy(a.begin(), a.end(), array::begin());
					destroy(array::begin()+a.size(), array::end());
				}
				m_size = a.size(); 
			}
			return *this;
		}		

		array& operator = (const array& a)
		{
			return operator = <N> (a);
		}

		template <int M>
		array& operator = (const T a[M])
		{
			array_assert (M<=N);
			if (size()<M) { // увеличить
				std::copy(a, a+size(), array::begin());
				std::uninitialized_copy(a+size(), a+M, array::end());
			} else { // уменьшить
				std::copy(a, a+M, array::begin());
				destroy(array::begin()+M, array::end());
			}
			m_size = M; 
			return *this;
		}		

		template <int M>
		array& operator = (const carray<T, M> &a)
		{
			array_assert (M<=N);
			if (size()<M) { // увеличить
				std::copy(a, a+size(), array::begin());
				std::uninitialized_copy(a+size(), a+M, array::end());
			} else { // уменьшить
				destroy(array::begin()+M, array::end());
				std::copy(a, a+M, array::begin());
			}
			m_size = M; 
			return *this;
		}

		int push_back (const T& t)
		{
			array_assert (m_size<N);
			std::uninitialized_fill_n(array::end(), 1, t);
			return m_size++; 
		}

		int  emplace_back ()	
			{	array_assert (m_size<N);	new (end()) T; return m_size++;	}
		template<typename A1> int  emplace_back (const A1&a1) 
			{	array_assert (m_size<N);	new (end()) T(a1);	return m_size++;	}
		template<typename A1, typename A2> int  emplace_back (const A1&a1, const A2&a2) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2);	return m_size++;	}
		template<typename A1, typename A2, typename A3> int  emplace_back (const A1&a1, const A2&a2, const A3&a3) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4, typename A5> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4,a5);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4,a5,a6);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7& a7) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4,a5,a6,a7);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7&a7, const A8&a8) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4,a5,a6,a7,a8);	return m_size++;	}
		template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9> int  emplace_back (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5, const A6&a6, const A7&a7, const A8&a8, const A9& a9) 
			{	array_assert (m_size<N);	new (end()) T(a1,a2,a3,a4,a5,a6,a7,a8,a9);	return m_size++;	}

		int size() const {
			return m_size;
		}

		void shrink (int a_size) {
			assert (a_size<=m_size);
			destroy(array::begin()+a_size, array::end());
			m_size=a_size;
		}		

		void resize (int a_size) {
			assert (a_size<=N);
			if (a_size<size())
				shrink(a_size);
			else
				construct(array::end(), array::begin()+a_size);
			m_size=a_size;
		}		

		slice<T> get_slice(int low, int high)	{
			assert (low>=0 && high<=m_size);
			return tblib::slice<T>(&(*this)[low], low, high);
		}		

		T& operator [] (int index) {
			assert (index>=0 && index<m_size);			
			return this->base_array<T,N>::operator[] (index);
		}

		const T& operator [] (int index) const {
			assert (index>=0 && index<m_size);			
			return this->base_array<T,N>::operator[] (index);
		}

		array (const ibstream& s) : m_size(0)
		{
			int new_size = tblib::To<int>::From(s);
			for (int i=0; i<new_size; ++i)
				emplace_back(tblib::To<T>::From(s));
		}

		friend const obstream& operator << (const obstream& s, const array& t)
		{
      s << t.size();
			for (int i=0; i<t.size(); ++i)
				s << t[i];
			return s;
		}
	};	
};
