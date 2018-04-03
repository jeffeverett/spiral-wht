/*
 * Written by Neungsoo Park
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "transpose.h"

void transpose_stride(wht_value *xx, int n, int n1, long S)
{


  wht_value **x;
  int B=BlockSize;

  int i, j, xbi, xbj, step;

  register int ii,jj;
  wht_value temp0, temp1, temp2, temp3;
  /* n = n1*n2 & n2 = alpha*n1 */
  int n2, alpha;

  n2 = n/n1;
  alpha = n2/n1;

  x = (wht_value **) malloc(n1*sizeof(wht_value *));
  x[0] = &(xx[0]);

  if (n1 <NoUnroll){
    for(step = 0; step < alpha; step ++) 
      {
	int iS, jS;
	for(i=1,xbi=n2;i<n1;i++,xbi+=n2) x[i] = (x[0]+xbi*S);
	
	for(iS=i=0;i<n1;i++,iS+=S)
	  for(j=i+1, jS=j*S;j<n1;j++,jS+=S) {
	    wht_value temp;
	    temp = x[i][jS];
	    x[i][jS] = x[j][iS];
	    x[j][iS] = temp; 
	  }
	x[0] = x[0]+n1*S;
      }
    free(x);
    return;
  }

  if (n1 < 32) B = n1;
  
  for(step = 0; step < alpha; step ++) 
    {
      for(i=1,xbi=n2;i<n1;i++,xbi+=n2) x[i] = (x[0]+xbi*S);

      for(xbj=0; xbj<n1; xbj+=B)
	{
	  xbi = xbj;
	  for(i=0; i<B; i++)
	    {
	      for(j=i; j<B;j++)
		{
		  ii = xbi+i;
		  jj = xbj+j;
		  temp0 = x[jj][ii*S];
		  x[jj][ii*S] =  x[ii][jj*S];
		  x[ii][jj*S] = temp0;
		}
	    }
	  xbi+=B;
	  for(;xbi<n1;xbi+=B)
	    {
	      int jjs;

	      for(j=0,jj=xbj,jjs=xbj*S; j<B; j++,jj++,jjs+=S)
		{
		  int iis;

		  for(i=0, ii=xbi, iis=xbi*S;i<B;i+=NoUnroll,ii++, iis+=S)
		    {
		      
		      temp0 = x[ii][jjs];
		      temp1 = x[ii+1][jjs];
		      temp2 = x[ii+2][jjs];
		      temp3 = x[ii+3][jjs];
		      
		      x[ii][jjs]   = x[jj][iis];
		      x[ii+1][jjs] = x[jj][iis+S];
		      x[ii+2][jjs] = x[jj][iis+2*S];
		      x[ii+3][jjs] = x[jj][iis+3*S];
		      
		      x[jj][iis]  = temp0;
		      x[jj][iis+S]= temp1;
		      x[jj][iis+2*S]= temp2;
		      x[jj][iis+3*S]= temp3;
		    }
		}
	    }
	}
      x[0] = x[0]+n1*S;
    }
  
  free(x);
}

