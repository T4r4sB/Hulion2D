#pragma once
#include "stdint.h"
namespace tblib 
{
	template <typename T>
	struct check_align
	{
		uint8_t c;
		T t;
		check_align();
		check_align(const check_align&);
		check_align&operator=(const check_align&);
	};

	template <typename T>
	struct align_value
	{
		enum { value = sizeof(check_align<T>)-sizeof(T) };
	};

	template <int N>
	struct align_by_size;

	template <> 
	struct align_by_size<1> { uint8_t guts; };

	template <> 
	struct align_by_size<2> { uint16_t  guts; };

	template <> 
	struct align_by_size<4> { uint32_t guts; };

	template <> 
	struct align_by_size<8> { uint64_t guts; };

	template <typename T>
	struct align
	{
		align_by_size<align_value<T>::value> guts;
	};
};

