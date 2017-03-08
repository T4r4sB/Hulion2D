namespace tblib
{
	template <typename T, int size>
	class ring
	{
		tblib::carray<T, size> elements;
		unsigned int b,e;
	public:
		ring() : b(0), e(0) {}

		bool empty () {return b==e;}
		
		void push(const T& t)
		{
			elements[b] = t;
			b = (b+1)%size;
			assert (!empty());
		}

		T pop()
		{
			assert (!empty());
			int oe = e;
			e = (e+1)%size;
			return elements[oe];
		}
	};
};