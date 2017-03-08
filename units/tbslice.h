#pragma once

#include "assert.h"
#include "stdint.h"

namespace tblib
{
	template <typename T>
	class slice
	// отрезок чужого массива, за целостность не отвечает
	{
	protected :
		T* m_begin;
		int m_low,m_high;
	public :
		typedef T* iterator;
		typedef const T* const_iterator;

		slice () : m_begin(NULL),m_low(0),m_high(0) {}
		slice (T* first_elem, int low, int high) : m_begin(first_elem-low),m_low(low),m_high(high) {}

		T& operator [] (int index)
		{
			assert(index>=m_low && index<m_high);
			return m_begin[index];
		}
		
		T& operator [] (int index) const
		{
			assert(index>=m_low && index<m_high);
			return m_begin[index];
		}

		iterator begin () const
		{
			return m_begin + m_low;
		}

		iterator end () const
		{
			return m_begin + m_high;
		}

		int low () const
		{
			return m_low;
		}

		int high () const
		{
			return m_high;
		}	

		slice<T> get_slice(int low, int high) const
		{
			assert (low>=m_low && high<=m_high);
			return tblib::slice<T>(m_begin+low, low, high);
		}
	};
};