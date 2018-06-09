#include "DataProcessing.h"

DataProcessing::DataProcessing()
{
	cutoff=0.001;
	baseline=300;
	polar=-1;
	w_signal=NULL;
	smooth_times=30;
	Q=NULL;
	T=NULL;
	Rt=NULL;
	Pw=NULL;
	peak_num=1;
	width_small=200;
	width_big=2000;
	threshold=10;
}

DataProcessing::~DataProcessing()
{

}
void DataProcessing::DrawWave()
{
	if(!w_signal)return ;
	TFile *outf = new TFile("drawing.root","UPDATE");
        TGraph *gr = new TGraph();
        for(int i=0; i<length; i++)
	{
		Double_t value =w_signal[i];
		gr->SetPoint(i, i, value);
	}
        outf->cd();
        gr->Write("wave");
        outf->Close();
}

double DataProcessing::PeakFinding(double*tmp,int tmp_length)
{
	double tmp_Q=0;
	//printf("tmp_length=%d\n",tmp_length);
	for(int ii=0; ii<tmp_length; ii++)
        {
		//if(!(ii%10))printf("tmp_Q=%f,tmp[%d]=%f\n",tmp_Q,ii,tmp[ii]);
                tmp_Q=tmp[ii]>tmp_Q?tmp[ii]:tmp_Q;
        }
        return tmp_Q;
}


void DataProcessing::BaseSub(TVectorD*data,double*tmp)
{
	double tmp_Q=0;
	//baseline substract
	for(int ii=0; ii<baseline; ii++)
	{
		tmp_Q+=(*data)[ii];
	}
	tmp_Q=tmp_Q/baseline;
	for(int ii=0; ii<length; ii++)
	{
		if(polar<0)tmp[ii] =tmp_Q- (*data)[ii];
		else if(polar>0)tmp[ii] = (*data)[ii]-tmp_Q;
		else tmp[ii]=0;
	}
}
int DataProcessing::CRFilter(TVectorD*data,double daq_t)
{
        peak_num=1;
        if(Q)free(Q);
        Q=(double*) malloc(peak_num*sizeof(double));
        if(T)free(T);
        T=(double*) malloc(peak_num*sizeof(double));
        if(Rt)free(Rt);
        Rt=(int*) malloc(peak_num*sizeof(int));
        if(Pw)free(Pw);
        Pw=(int*) malloc(peak_num*sizeof(int));

	length=data->GetNrows();
	double tmp[length];
	if(w_signal)free(w_signal);
	w_signal=(double*) malloc(length*sizeof(double));
	BaseSub(data,tmp);
	lowp = new LowPassFilter();
	highp = new HighPassFilter();
	lowpstuff lowp_p;
	highpstuff highp_p;
	lowp_p.cutoff = highp_p.cutoff = cutoff;
	lowp->Initial(&lowp_p);
	highp->Initial(&highp_p);

	for (int ii = 0; ii < length; ii++)
	{
		if(ii!=0)highp_p.inm1= tmp[ii-1];
		highp_p.outm1 = highp->Processing(&highp_p, tmp[ii]);
		w_signal[ii] = highp_p.outm1;
	}
	/////////////////////
	//can get time here//
	/////////////////////
	//GetTime(w_signal);
	for (int j = 0; j < 5; j++)
	{
		for (int ii = 0; ii < length; ii++)
		{
			lowp_p.outm1 = lowp->Processing(&lowp_p, w_signal[ii]);
			w_signal[ii] = lowp_p.outm1;
		}
	}
	delete lowp;
	delete highp;
	Q[0]=PeakFinding(w_signal,length);
        T[0]=daq_t;
	Rt[0]=1;
	Pw[0]=1;
        return 1;

}

int DataProcessing::DoSmooth(TVectorD*data,double daq_t)
{
        peak_num=1;
        if(Q)free(Q);
        Q=(double*) malloc(peak_num*sizeof(double));
        if(T)free(T);
        T=(double*) malloc(peak_num*sizeof(double));
        if(Rt)free(Rt);
        Rt=(int*) malloc(peak_num*sizeof(int));
        length=data->GetNrows();
        if(Pw)free(Pw);
        Pw=(int*) malloc(peak_num*sizeof(int));

        //double tmp[length];
	if(w_signal)free(w_signal);
	w_signal=(double*) malloc(length*sizeof(double));
	BaseSub(data,w_signal);
	Smooth*sm=new Smooth();
	sm->SetSTimes(smooth_times);
	sm->DoSmooth(w_signal,length);	
	Q[0]=PeakFinding(w_signal,length);
	T[0]=daq_t;
	Rt[0]=1;
	Pw[0]=1;
	return 1;	
}

int DataProcessing::DoNothing(TVectorD*data,double daq_t)
{
	peak_num=1;
	if(Q)free(Q);
        Q=(double*) malloc(peak_num*sizeof(double));
        if(T)free(T);
        T=(double*) malloc(peak_num*sizeof(double));
        if(Rt)free(Rt);
        Rt=(int*) malloc(peak_num*sizeof(int));
        if(Pw)free(Pw);
        Pw=(int*) malloc(peak_num*sizeof(int));
        length=data->GetNrows();
        double tmp[length];
        if(w_signal)free(w_signal);
        w_signal=(double*) malloc(length*sizeof(double));
        BaseSub(data,w_signal);

	Q[0]=PeakFinding(w_signal,length);
	T[0]=daq_t;
	Rt[0]=1;
	Pw[0]=1;
	return 1;

}

int DataProcessing::SwithD(TVectorD*data,double daq_t)
{
        peak_num=1;
	length=data->GetNrows();
        double tmp[length];
        if(w_signal)free(w_signal);
        w_signal=(double*) malloc(length*sizeof(double));
        BaseSub(data,tmp);
        Smooth*sm=new Smooth();
        sm->SetSTimes(smooth_times);
        sm->DoSmooth(tmp,length);

        lowp = new LowPassFilter();
        highp = new HighPassFilter();
        lowpstuff lowp_p;
        highpstuff highp_p;
        lowp_p.cutoff = highp_p.cutoff = cutoff;
        lowp->Initial(&lowp_p);
        highp->Initial(&highp_p);

        for (int ii = 0; ii < length; ii++)
        {
                if(ii!=0)highp_p.inm1= tmp[ii-1];
                highp_p.outm1 = highp->Processing(&highp_p, tmp[ii]);
                w_signal[ii] = highp_p.outm1;
        }
	Peaking(w_signal);
	//printf("peak_num:%d\n",peak_num);
        if(Q)free(Q);
        Q=(double*) malloc(peak_num*sizeof(double));
        if(T)free(T);
        T=(double*) malloc(peak_num*sizeof(double));
        if(Rt)free(Rt);
        Rt=(int*) malloc(peak_num*sizeof(int));
        if(Pw)free(Pw);
        Pw=(int*) malloc(peak_num*sizeof(int));
	std::vector<int>::iterator it = peak_pos.begin();
        for (int i=0;i<peak_num; i++)
        {
                int tmp_peak=*it;
		it++;
                int tmp_width=*it;
                it++;
		int tmp_pp=*it;
		it++;
		//NOTICE:daq_t unit is 'ps', tmp_peak-tmp_width unit is 'ns'!!!
		T[i]=1000*(tmp_peak-tmp_width)+daq_t;
		Rt[i]=1000*tmp_pp;
		Pw[i]=1000*tmp_width;
		//printf("tmp_peak=%d,tmp_width=%d,T:%f\n",tmp_peak,tmp_width,T[i]);
        }
 
        delete lowp;
        delete highp;
	it = peak_pos.begin();
	for (int i=0;i<peak_num; i++)
        {
                int tmp_peak=*it;
                it++;
                int tmp_width=*it;
                it++;
		int tmp_pp=*it;
                it++;

		//printf("peak_pos:%d\t",tmp_peak);
		//printf("peak_width:%d\n",tmp_width);
		int expend_width=tmp_width;
		
		Q[i]=PeakFinding(w_signal+tmp_peak-tmp_width,expend_width);
		//printf("peak:%f\n",Q[i]);
	} 
	return 1;
}

int DataProcessing::SwithDI(TVectorD*data,double daq_t)
{
        peak_num=1;
	length=data->GetNrows();
        double tmp[length];
        if(w_signal)free(w_signal);
        w_signal=(double*) malloc(length*sizeof(double));
        BaseSub(data,tmp);
        Smooth*sm=new Smooth();
        sm->SetSTimes(smooth_times);
        sm->DoSmooth(tmp,length);

        lowp = new LowPassFilter();
        highp = new HighPassFilter();
        lowpstuff lowp_p;
        highpstuff highp_p;
        lowp_p.cutoff = highp_p.cutoff = cutoff;
        lowp->Initial(&lowp_p);
        highp->Initial(&highp_p);

        for (int ii = 0; ii < length; ii++)
        {
                if(ii!=0)highp_p.inm1= tmp[ii-1];
                highp_p.outm1 = highp->Processing(&highp_p, tmp[ii]);
                w_signal[ii] = highp_p.outm1;
        }
	Peaking(w_signal);
	//printf("peak_num:%d\n",peak_num);
        if(Q)free(Q);
        Q=(double*) malloc(peak_num*sizeof(double));
        if(T)free(T);
        T=(double*) malloc(peak_num*sizeof(double));
        if(Rt)free(Rt);
        Rt=(int*) malloc(peak_num*sizeof(int));
        if(Pw)free(Pw);
        Pw=(int*) malloc(peak_num*sizeof(int));
	std::vector<int>::iterator it = peak_pos.begin();
        for (int i=0;i<peak_num; i++)
        {
                int tmp_peak=*it;
		it++;
                int tmp_width=*it;
                it++;
		int tmp_pp=*it;
		it++;
		//NOTICE:daq_t unit is 'ps', tmp_peak-tmp_width unit is 'ns'!!!
		T[i]=1000*(tmp_peak-tmp_width)+daq_t;
		Rt[i]=1000*tmp_pp;
		Pw[i]=1000*tmp_width;
		//printf("tmp_peak=%d,tmp_width=%d,T:%f\n",tmp_peak,tmp_width,T[i]);
        }
        for (int j = 0; j < 5; j++)
        {
                for (int ii = 0; ii < length; ii++)
                {
                        lowp_p.outm1 = lowp->Processing(&lowp_p, w_signal[ii]);
                        w_signal[ii] = lowp_p.outm1;
                }
        }
        delete lowp;
        delete highp;
	it = peak_pos.begin();
	for (int i=0;i<peak_num; i++)
        {
                int tmp_peak=*it;
                it++;
                int tmp_width=*it;
                it++;
		int tmp_pp=*it;
                it++;

		//printf("peak_pos:%d\t",tmp_peak);
		//printf("peak_width:%d\n",tmp_width);
		int expend_width=0;
		if((tmp_peak+2000)<length)expend_width=tmp_width+2000;
		else expend_width=tmp_width+(length-tmp_peak);
		Q[i]=PeakFinding(w_signal+tmp_peak-tmp_width,expend_width);
		//printf("peak:%f\n",Q[i]);
	} 
	return 1;
}

void DataProcessing::Peaking(double*w_signal)
{
	int keep_flag=0;
	double max=0;
	int p_p=0;
	peak_num=0;
	peak_pos.clear();
	for (int ii = 0; ii < length; ii++)
	{
	//	printf("%f:%f\n",w_signal[ii],threshold);
		if(w_signal[ii]>threshold)
		{
			keep_flag++;
			if(max<w_signal[ii])
			{
				max=w_signal[ii];
				p_p=keep_flag;
			}
		}
		else if(keep_flag)
		{
			if((keep_flag>width_small)&&(keep_flag<width_big))
			{
				peak_pos.push_back(ii);
				peak_pos.push_back(keep_flag);
				peak_pos.push_back(p_p);
				peak_num++;
				//printf("Peak!at:%d peak width:%d\n",ii,keep_flag);
			}
			keep_flag=0;
			max=0;
			p_p=0;
		}
	}
}
