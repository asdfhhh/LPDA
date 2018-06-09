#include "Smooth.h"
#include "TMath.h"
#include <assert.h>     /* assert */

Smooth::Smooth()
{
	ntimes=1;
}

Smooth::~Smooth()
{
;
}

void  Smooth::DoSmooth(double*data,int nbins)
 {
	if (nbins < 3) 
	{
		Error("Smooth","Smooth only supported for array with >= 3 bins. Nbins = %d",nbins);
		return;
	}
	Double_t *xx = new Double_t[nbins];
 	for (int i=0;i<nbins;i++) 
	{
 		xx[i] = data[i];
 	}
 	SmoothArray(nbins,xx);
 	for (int i=0;i<nbins;i++) 
	{
 		data[i]=xx[i];
 	}
 	delete [] xx;
}

void  Smooth::SmoothArray(Int_t nn, Double_t *xx)
{
	Int_t ii;
	Double_t hh[6] = {0,0,0,0,0,0};
	std::vector<double> yy(nn);
	std::vector<double> zz(nn);
	std::vector<double> rr(nn);
	for (Int_t pass=0;pass<ntimes;pass++) 
	{
		// first copy original data into temp array
		std::copy(xx, xx+nn, zz.begin() );
		for (int noent = 0; noent < 2; ++noent) 
		{ // run algorithm two times
 		//  do 353 i.e. running median 3, 5, and 3 in a single loop
			for  (int kk = 0; kk < 3; kk++)  
			{
				std::copy(zz.begin(), zz.end(), yy.begin());
				int medianType = (kk != 1)  ?  3 : 5;
				int ifirst      = (kk != 1 ) ?  1 : 2;
				int ilast       = (kk != 1 ) ? nn-1 : nn -2;
 				//nn2 = nn - ik - 1;
				// do all elements beside the first and last point for median 3
				//  and first two and last 2 for median 5
				for  ( ii = ifirst; ii < ilast; ii++)  
				{
					assert(ii - ifirst >= 0);
					for  (int jj = 0; jj < medianType; jj++)   
					{
						hh[jj] = yy[ii - ifirst + jj ];
					}
					zz[ii] = TMath::Median(medianType, hh);
				}
				if  (kk == 0)  
				{   // first median 3
				// first point
					hh[0] = zz[1];
					hh[1] = zz[0];
					hh[2] = 3*zz[1] - 2*zz[2];
					zz[0] = TMath::Median(3, hh);
					// last point
					hh[0] = zz[nn - 2];
					hh[1] = zz[nn - 1];
					hh[2] = 3*zz[nn - 2] - 2*zz[nn - 3];
					zz[nn - 1] = TMath::Median(3, hh);
				}
				if  (kk == 1)  
				{   //  median 5
					for  (ii = 0; ii < 3; ii++) 
					{
						hh[ii] = yy[ii];
					}
					zz[1] = TMath::Median(3, hh);
					// last two points
					for  (ii = 0; ii < 3; ii++) 
					{
						hh[ii] = yy[nn - 3 + ii];
					}
					zz[nn - 2] = TMath::Median(3, hh);
				}
			}
 			std::copy ( zz.begin(), zz.end(), yy.begin() );
			// quadratic interpolation for flat segments
			for (ii = 2; ii < (nn - 2); ii++) 
			{
				if  (zz[ii - 1] != zz[ii]) continue;
				if  (zz[ii] != zz[ii + 1]) continue;
				hh[0] = zz[ii - 2] - zz[ii];
				hh[1] = zz[ii + 2] - zz[ii];
				if  (hh[0] * hh[1] <= 0) continue;
				int jk = 1;
				if  ( TMath::Abs(hh[1]) > TMath::Abs(hh[0]) ) jk = -1;
				yy[ii] = -0.5*zz[ii - 2*jk] + zz[ii]/0.75 + zz[ii + 2*jk] /6.;
				yy[ii + jk] = 0.5*(zz[ii + 2*jk] - zz[ii - 2*jk]) + zz[ii];
			}
			// running means
			//std::copy(zz.begin(), zz.end(), yy.begin());
			for  (ii = 1; ii < nn - 1; ii++) 
			{
				zz[ii] = 0.25*yy[ii - 1] + 0.5*yy[ii] + 0.25*yy[ii + 1];
			}
			zz[0] = yy[0];
			zz[nn - 1] = yy[nn - 1];
			if (noent == 0) 
			{
			// save computed values
				std::copy(zz.begin(), zz.end(), rr.begin());
			// COMPUTE  residuals
				for  (ii = 0; ii < nn; ii++)  
				{
					zz[ii] = xx[ii] - zz[ii];
				}
			}
		}  // end loop on noent
		double xmin = TMath::MinElement(nn,xx);
		for  (ii = 0; ii < nn; ii++) 
		{
			if (xmin < 0) xx[ii] = rr[ii] + zz[ii];
			// make smoothing defined positive - not better using 0 ?
			else  xx[ii] = TMath::Max((rr[ii] + zz[ii]),0.0 );
		}
	}
}

