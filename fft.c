/*
 * C based implementtion of the FFT algorithm published in Numerical Recipes and other sources
 */
#include <math.h>

#define PI 3.1415296

#define SWAP(a,b) { double tempr = (a); (a)=(b); (b)=tempr; }

//data -> float array that represent the array of complex samples
//number_of_complex_samples -> number of samples (N^2 order number)
//isign -> 1 to calculate FFT and -1 to calculate Reverse FFT
void FFT (float data[], unsigned long number_of_complex_samples, int isign)
{
//variables for trigonometric recurrences
unsigned long n,mmax,m,j,istep,i;
double wtemp,wr,wpr,wpi,wi,theta,tempi, tempr;

	//the complex array is real+complex so the array
	//as a size n = 2* number of complex samples
	// real part is the data[index] and the complex part is the data[index+1]
	n=number_of_complex_samples * 2;

	//binary inversion (note that
	//the indexes start from 1 witch means that the
	//real part of the complex is on the odd-indexes
	//and the complex part is on the even-indexes
	j = 1;
	for (i = 1; i < n; i += 2)
	{
		if (j > i)
		{
			//swap the real part
			SWAP(data[j], data[i]);
			//swap the complex part
			SWAP(data[j+1], data[i+1]);
		}
		m = n / 2;
		while (m >= 2 && j > m)
		{
			j -= m;
			m = m / 2;
		}
		j += m;
	}

	//the complex array is real+complex so the array
	//as a size n = 2* number of complex samples
	// real part is the data[index] and
	//the complex part is the data[index+1]
	n = number_of_complex_samples * 2;

	//binary inversion (note that the indexes
	//start from 0 witch means that the
	//real part of the complex is on the even-indexes
	//and the complex part is on the odd-indexes
	j=0;
	for (i = 0; i < n / 2; i += 2)
	{
		if (j > i)
		{
			//swap the real part
			SWAP(data[j], data[i]);
			//swap the complex part
			SWAP(data[j+1], data[i+1]);
			// checks if the changes occurs in the first half
			// and use the mirrored effect on the second half
			if ((j / 2) < (n / 4))
			{
				//swap the real part
				SWAP(data[(n-(i+2))], data[(n-(j+2))]);
				//swap the complex part
				SWAP(data[(n-(i+2))+1], data[(n-(j+2))+1]);
			}
		}
	    m = n / 2;
	    while (m >= 2 && j >= m)
	    {
		j -= m;
		m = m / 2;
	    }
	    j += m;
	}

	//Danielson-Lanzcos routine
	mmax = 2;
	//external loop
	while (n > mmax)
	{
	    istep = mmax<<  1;
	    theta = isign*(2*PI / mmax);
	    wtemp = sin(0.5 * theta);
	    wpr = -2.0 * wtemp * wtemp;
	    wpi = sin(theta);
	    wr = 1.0;
	    wi = 0.0;
	    //internal loops
	    for (m = 1; m < mmax; m += 2)
	    {
		for (i = m; i <= n; i += istep)
		{
		    j = i + mmax;
		    tempr = wr * data[j-1] - wi * data[j];
		    tempi = wr * data[j] + wi * data[j-1];
		    data[j-1] = data[i-1] - tempr;
		    data[j] = data[i] - tempi;
		    data[i-1] += tempr;
		    data[i] += tempi;
		}
		wr = (wtemp = wr) * wpr - wi * wpi + wr;
		wi = wi * wpr + wtemp * wpi + wi;
	    }
	    mmax = istep;
	}
}
