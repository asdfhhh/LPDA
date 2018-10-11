#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H 1
#include "LowPassFilter.h"
#include "HighPassFilter.h"
#include "TVectorD.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TMarker.h"
#include "Smooth.h"
class DataProcessing
{
public:
	DataProcessing();
	~DataProcessing();
	int CRFilter(TVectorD*,double );
	int DoSmooth(TVectorD*,double );
	int DoNothing(TVectorD*,double );
	int SwithD(TVectorD*,double );
	int SwithDI(TVectorD*,double );
	void DrawWave(int );
	inline void SetPolar(int setv){polar=setv;}
        inline void SetBase(int setv){baseline=setv;}
	inline void SetSmoothTimes(int setv){smooth_times=setv;}
	inline void SetThreshold(int setv){threshold=setv;}
	inline void SetDCut(double setv){Dcutoff=setv;}
        inline void SetICut(double setv){Icutoff=setv;}
	inline double GetQ(int n){ return Q[n];}
	inline double GetT(int n){ return T[n];}
        inline int GetRt(int n){ return Rt[n];}
	inline int GetPw(int n){ return Pw[n];}
	inline int GetPeakNum(){return peak_num;}
        double *w_signal;
	int peak_num;
	double* Q;
	double* T;
	int * Rt;
	int * Pw;
	std::vector<int>::iterator it;
private:
	void BaseSub(TVectorD*,double*);
	double PeakFinding(double*,int);
	void CalThreshold(double*);
        void Peaking(double*);
	int polar;
	int baseline;
	int smooth_times;
	double Dcutoff;
        double Icutoff;
	LowPassFilter*lowp;
	HighPassFilter*highp;
	int length;
	int width_small;
	int width_big;
	double threshold;
	std::vector<int> peak_pos;
};
#endif
