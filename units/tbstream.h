#pragma once
#include <istream>
#include <ostream>
#include "string.h"
#include "stdint.h"

namespace tblib
{
	class ibstream
	{
		std::istream& stream;
		ibstream(const ibstream&);
		ibstream&operator=(const ibstream&);
		mutable size_t file_version;
		mutable size_t cur_version;
		mutable size_t processed;
	public :
		ibstream (std::istream& stream) : stream(stream), cur_version(0), file_version(0), processed(0) {}

		void SetFileVersion (size_t version) const
		{
			file_version = version;
		}

		inline const ibstream& ReadVersion () const;

		const ibstream& Version (size_t version) const
		{
			cur_version = version;
			return *this;
		}

		size_t Processed () const
		{
			return processed;
		}
		
		template <typename T>
		T BinaryRead () const
		{
			if (cur_version <= file_version) // формат не старый 
			{
				T t;
				stream.read(reinterpret_cast<char*>(&t), std::streamsize(sizeof(t)));
				processed += sizeof(t);
				return t;
			} else
			{
				return T();
			}
		}	
	};
	
	class obstream
	{
		std::ostream& stream;
		obstream(const obstream&);
		obstream&operator=(const obstream&);
		mutable size_t processed;
	public :
		obstream (std::ostream& stream) : stream(stream), processed(0) {}
		
		size_t Processed () const
		{
			return processed;
		}

		template <typename T>
		void BinaryWrite(const T& t) const
		{			
			stream.write(reinterpret_cast<const char*>(&t), std::streamsize(sizeof(t)));
			processed += sizeof(t);
		}
	};

	template <typename T>
	struct To
	{
		static const ibstream& From(const ibstream&s) 
		{ return s; }
	};

	template <typename T>
	struct From;
	// стандартные читатели из класса 
	// для них нельзя писать конструктор из потока!
	// для любого типа - либо FromStream, либо конструктор, но не и то и другое!!!
};


#define CAN_BINARY_SERIALIZATION(T)\
namespace tblib\
{\
	template <>\
	struct To<T>\
	{\
		static T From(const ibstream&s)\
		{ return s.BinaryRead<T>(); }\
	};\
	inline const obstream& operator << (const obstream&s, const T& t)\
	{ s.BinaryWrite(t); return s; }\
};

CAN_BINARY_SERIALIZATION(uint64_t)
CAN_BINARY_SERIALIZATION(int64_t)
CAN_BINARY_SERIALIZATION(uint32_t)
CAN_BINARY_SERIALIZATION(int32_t)
CAN_BINARY_SERIALIZATION(uint16_t)
CAN_BINARY_SERIALIZATION(int16_t)
CAN_BINARY_SERIALIZATION(char)
CAN_BINARY_SERIALIZATION(bool)


namespace tblib
{
	inline const ibstream& ibstream::ReadVersion () const
	{
		int version = tblib::To<int>::From(*this);
		SetFileVersion(version);
		return *this;
	}

	template <>
	struct To<std::string>
	{
		static std::string From (const ibstream& is)
		{	
			std::string s;
			size_t ressize = To<size_t>::From(is);
			s.resize(ressize);
			for (size_t i=0; i<s.size(); ++i)
				s[i] = To<char>::From(is);
			return s;
		}
	};

	inline void FuckMSVS2003 (const obstream& s, const size_t& t)
	{
		s<<t;
	}
	
	inline const obstream& operator << (const obstream& s, const std::string &t)
	{
		FuckMSVS2003(s, t.size());
		for (size_t i=0; i<t.size(); ++i)
			s << t[i];
		return s;
	}

	template <typename T>
	struct To<std::vector<T> >
	{
		static std::vector<T> From (const ibstream& is)
		{	
			std::vector<T> s;
			size_t ressize = To<size_t>::From(is);
			s.resize(ressize);
			for (size_t i=0; i<s.size(); ++i)
				s[i] = To<T>::From(is);
			return s;
		}
	};
	
	template <typename T>
	inline const obstream& operator << (const obstream& s, const std::vector<T> &t)
	{
		FuckMSVS2003(s, t.size());
		for (size_t i=0; i<t.size(); ++i)
			s << t[i];
		return s;
	}
};