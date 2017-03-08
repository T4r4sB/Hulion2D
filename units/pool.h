#pragma once

#include "tbarr.h"

namespace tblib
{
	template <typename T, int N> 
	class pool : base_array <T,N>
	{
		tblib::carray <int, N> next_free;
		int m_size;
		int m_free;
		int m_count;

		int reserve_index ()
		{
			assert (!full());

			int index;
			if (m_free>=0) // есть свободные элементы
			{
				index = m_free;
				m_free = next_free[index]; // следующий элемент списка
			} else
			{
				index = m_size++; // новый элемент, а свободных всё ещё нет
			}

			next_free[index] = -2; // он занят
			++m_count;
			return index;
		}		

		pool (const pool&);
		pool& operator=(const pool&);

	public :
		pool () : m_size(0), m_free(-1), m_count(0) {}

		int size() { return m_size; }
		int count () { return m_count;}
		int capacity () { return N;}

		bool full ()
		{
			return m_free<0 && m_size>=N;
		}

		bool valid (int index) { return index>=0 && index<m_size && next_free[index]<-1;}

		int push ()
		{ 
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T();
			return index;
		}

		int push (const T& t)
		{ 
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(t);
			return index;
		}

		int emplace ()	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T();
			return index;	}
			
		template<typename A1>
		int emplace (const A1&a1)	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(a1);
			return index;	}
			
		template<typename A1,typename A2>
		int emplace (const A1&a1, const A2&a2)	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(a1,a2);
			return index;}
						
		template<typename A1,typename A2,typename A3>
		int emplace (const A1&a1, const A2&a2, const A3&a3)	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(a1,a2,a3);
			return index;}
						
		template<typename A1,typename A2,typename A3,typename A4>
		int emplace (const A1&a1, const A2&a2, const A3&a3, const A4&a4)	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(a1,a2,a3,a4);
			return index;}
						
		template<typename A1,typename A2,typename A3,typename A4,typename A5>
		int emplace (const A1&a1, const A2&a2, const A3&a3, const A4&a4, const A5&a5)	
			{	
			int index = reserve_index();
			new (&(base_array<T,N>::operator[](index))) T(a1,a2,a3,a4,a5);
			return index;}

		void free_index (int index)
		{
			assert (valid(index));
			next_free[index] = m_free; // -1 для последнего инвалида, -2 для свободного
			m_free = index;
			typename base_array<T,N>::iterator fe = &(base_array<T,N>::operator[](index));
			destroy(fe, fe+1);
			--m_count;
		}

		T& operator [] (int index)
		{
			assert (valid(index));
			return base_array<T,N>::operator[](index);
		}

		const T& operator [] (int index) const
		{
			assert (valid(index));
			return base_array<T,N>::operator[](index);
		}

		~pool ()
		{
			for (int i=0; i<size(); ++i)
				if (valid(i))
				{
					typename base_array<T,N>::iterator fe = &(base_array<T,N>::operator[](i));
					destroy(fe, fe+1);
				}
		}
	};
};