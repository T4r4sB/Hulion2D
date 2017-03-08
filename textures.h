#pragma once
#include "topology.h"

template <int N, int STEP, bool NEEDREC=(STEP*2<N)>
struct UselessWrapperFillPerlinStep;

template <int N, int STEP>
struct UselessWrapperFillPerlinStep <N, STEP, true>
{
	static void FillPerlinStep(tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
	{
		UselessWrapperFillPerlinStep <N, STEP*2>::FillPerlinStep(texture);

		for (int j=0; j<N; j+=STEP*2)
			for (int i=0; i<N; i+=STEP*2)
				texture[j+STEP][i] = (texture[j][i]+texture[j+STEP*2][i])>>1;
		
		for (int j=0; j<N; j+=STEP)
			for (int i=0; i<N; i+=STEP*2)
				texture[j][i+STEP] = (texture[j][i]+texture[j][i+STEP*2])>>1;	 	

		for (int j=0; j<N; j+=STEP)
			for (int i=0; i<N; i+=STEP)
				texture[j][i] += rnd.frandom()*Fixed(STEP,N);
    
	}
};

template <int N, int STEP>
struct UselessWrapperFillPerlinStep <N, STEP, false>
{
	static void FillPerlinStep(tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
	{		
		for (int j=0; j<N; j+=STEP)
			for (int i=0; i<N; i+=STEP)
				texture[j][i] += rnd.frandom()*Fixed(STEP,N);		
	}
};


template <int N>
void FillPerlin (tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
{
	for (int j=0; j<N; ++j)
		for (int i=0; i<N; ++i)
			texture[j][i] = fx0;
  
	UselessWrapperFillPerlinStep<N,1>::FillPerlinStep(texture);
}


template <int COUNT, int N>
void FillCell (tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
{
	tblib::array<Point, COUNT> centers;
  for (int i=0; i<centers.capacity; ++i)
		centers.emplace_back(rnd.frandom()*Fixed(N), rnd.frandom()*Fixed(N) );
	
	for (int j=0; j<N; ++j) for (int i=0; i<N; ++i)
	{
		Fixed min1 = N, min2 = N;
		for (int k=0; k<centers.size(); ++k)
		{
			Point d = centers[k] - Point(i,j);
			d.x = d.x.Abs();
			tblib::shrink(d.x, Fixed(N)-d.x);
			d.y = d.y.Abs();
			tblib::shrink(d.y, Fixed(N)-d.y);
			Fixed ad = d.Length();
			if (ad<min1)
			{
				min2 = min1;
				min1 = ad;
			} else if (ad<min2)
				min2 = ad;
		}
		texture[j][i] = std::min(fx1, (min2-min1).Abs()>>1);
	}
}

template <int N>
void FillRandom (tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
{	
	for (int j=0; j<N; ++j) for (int i=0; i<N; ++i)
		texture[j][i] = rnd.frandom();
}

struct PD : Point
{
	Fixed phase;
	PD (Fixed x, Fixed y, Fixed phase) : Point(x,y), phase(phase) {}
};

template <int COUNT, int N>
void FillWaves (tblib::modarray <tblib::modarray<Fixed, N>, N> &texture)
{		
	tblib::array<PD, COUNT> centers;
  for (int i=0; i<centers.capacity; ++i)
		centers.emplace_back(rnd.frandom()*Fixed(N), rnd.frandom()*Fixed(N), rnd.frandom()*fxPi );

	
	for (int j=0; j<N; ++j) for (int i=0; i<N; ++i)
	{
		texture[j][i] = fx0;
		for (int k=0; k<centers.size(); ++k)
		{
			Point d = centers[k] - Point(i,j);
			d.x = d.x.Abs();
			tblib::shrink(d.x, Fixed(N)-d.x);
			d.y = d.y.Abs();
			tblib::shrink(d.y, Fixed(N)-d.y);
			Fixed ad = d.Length();
			texture[j][i] += (Sin((ad+centers[k].phase)/fxPi)+fx1);
		}
		texture[j][i] /= (2*k);
	}  
}





