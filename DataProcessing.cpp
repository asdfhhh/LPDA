#include "DataProcessing.h"
#include "TMath.h"
DataProcessing::DataProcessing()
{
	Dcutoff=0.001;
	Icutoff=0.001;
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
	width_big=5000;
	threshold=0;
}

DataProcessing::~DataProcessing()
{

}
void DataProcessing::DrawWave(int num)
{
	if(!w_signal)return ;
	TFile *outf = new TFile("drawing.root","UPDATE");
        TMultiGraph*mg = new TMultiGraph();
	TGraph *gr = new TGraph();
        for(int i=0; i<length; i++)
	{
		Double_t value =w_signal[i];
		gr->SetPoint(i, i, value);
	}
	mg->Add(gr);
	TGraph *th = new TGraph();
	th->SetPoint(0,0,threshold);
	th->SetPoint(1,length,threshold);
	th->SetLineStyle(4);
	th->SetLineColor(2);
	mg->Add(th);
	it = peak_pos.begin();
	TGraph *PM[peak_num];
        for (int i=0;i<peak_num; i++)
        {
                int tmp_peak=*it;
                it++;
                int tmp_width=*it;
                it++;
                int tmp_pp=*it;
                it++;
		PM[i]=new TGraph();
		PM[i]->SetPoint(0,tmp_peak-tmp_width+tmp_pp,Q[i]);
		PM[i]->SetMarkerStyle(23);
		PM[i]->SetMarkerColor(2);
		mg->Add(PM[i]);
	}
        outf->cd();
	char name[10];
	sprintf(name,"wave%d",num);
        mg->Write(name);
        outf->Close();
}

double DataProcessing::PeakFinding(double*tmp,int tmp_length)
{
	int base=0;
	if(tmp[0]>threshold)base=tmp[0];
	double tmp_Q=0;
	//printf("tmp_length=%d\n",tmp_length);
	for(int ii=0; ii<tmp_length; ii++)
        {
		//if(!(ii%10))printf("tmp_Q=%f,tmp[%d]=%f\n",tmp_Q,ii,tmp[ii]);
                tmp_Q=tmp[ii]>tmp_Q?tmp[ii]:tmp_Q;
        }
        return tmp_Q-base;
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

void DataProcessing::CalThreshold(double*tmp)
{
        //double tmp_Q=0;
        //
        //for(int ii=0; ii<baseline; ii++)
        //{
        //        tmp_Q+=tmp[ii];
        //}
        //tmp_Q=tmp_Q/baseline;

	//make the threshold with 3 sigma
        for(int ii=0; ii<baseline; ii++)
        {
                //threshold+=(tmp[ii]-tmp_Q)*(tmp[ii]-tmp_Q);
                threshold+=tmp[ii]*tmp[ii];
        }
        threshold=5*TMath::Sqrt(threshold/baseline);
        //printf("TH:%f\n",threshold);

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
	lowp_p.cutoff = Icutoff;
	highp_p.cutoff = Dcutoff;
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
	CalThreshold(w_signal);
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
        CalThreshold(w_signal);
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
        CalThreshold(w_signal);

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

        lowp_p.cutoff = Icutoff;
        highp_p.cutoff = Dcutoff;

        lowp->Initial(&lowp_p);
        highp->Initial(&highp_p);

        for (int ii = 0; ii < length; ii++)
        {
                if(ii!=0)highp_p.inm1= tmp[ii-1];
                highp_p.outm1 = highp->Processing(&highp_p, tmp[ii]);
                w_signal[ii] = highp_p.outm1;
        }
        delete lowp;
        delete highp;
        CalThreshold(w_signal);
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
	it = peak_pos.begin();
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
		//printf("peak_pos:%d\t",tmp_peak);
		//printf("peak_width:%d\n",tmp_width);
		int expend_width=tmp_width;
		
		Q[i]=PeakFinding(w_signal+tmp_peak-tmp_width-1,expend_width+1);
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
        lowp_p.cutoff = Icutoff;
        highp_p.cutoff = Dcutoff;

        lowp->Initial(&lowp_p);
        highp->Initial(&highp_p);

        for (int ii = 0; ii < length; ii++)
        {
                if(ii!=0)highp_p.inm1= tmp[ii-1];
                highp_p.outm1 = highp->Processing(&highp_p, tmp[ii]);
                w_signal[ii] = highp_p.outm1;
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
        CalThreshold(w_signal);
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
	it = peak_pos.begin();
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
		//printf("peak_pos:%d\t",tmp_peak);
		//printf("peak_width:%d\n",tmp_width);
		int expend_width=0;
		if((tmp_peak+2000)<length)expend_width=tmp_width+2000;
		else expend_width=tmp_width+(length-tmp_peak);
		Q[i]=PeakFinding(w_signal+tmp_peak-tmp_width-1,expend_width+1);
		//printf("peak:%f\n",Q[i]);
	} 
	return 1;
}

void DataProcessing::Peaking(double*w_signal)
{
	int keep_flag=0;
	double max=0;
	int p_p=0;
	int t_t=0;
	double tail=0;
	peak_num=0;
	peak_pos.clear();
	for (int ii = 0; ii < length; ii++)
	{
		if(w_signal[ii]>(threshold+tail))
		{
			//printf("%d: %f %f\n",ii,w_signal[ii],threshold+tail);
			keep_flag++;
			if(max<w_signal[ii])
			{
				max=w_signal[ii];
				p_p=keep_flag;
			}
			else if(w_signal[ii]>(max-threshold))
			{
				t_t++;
			}
			else 
			{
				//if((keep_flag>width_small)&&(keep_flag<width_big))
				{
					peak_pos.push_back(ii);
					peak_pos.push_back(keep_flag);
					peak_pos.push_back(p_p);
					peak_num++;
					//printf("Peak!at:%d peak %d\n",ii,max-tail);
				}
				keep_flag=0;
				max=0;
				p_p=0;
				tail=w_signal[ii];

			}
		}
		else if(w_signal[ii]>threshold)
		{
			tail=w_signal[ii]<tail?w_signal[ii]:tail;
			t_t++;
		}
		else 
		{
			tail=0;
			t_t=0;
		}

	}
}
