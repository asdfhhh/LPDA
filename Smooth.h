#ifndef SMOOTH_H
#define SMOOTH_H 1

class Smooth
{
public:
	Smooth();
	~Smooth();
	void DoSmooth(double*,int);
	inline void SetSTimes(int setv){ntimes=setv;}
private:
	void SmoothArray(int nn, double *xx);
	int ntimes;
};	
#endif
