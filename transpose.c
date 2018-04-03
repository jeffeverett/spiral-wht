/*
 * Written by Neungsoo Park
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "transpose.h"


void transpose(wht_value *xx, int n, int n1)
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
	for(i=1,xbi=n2;i<n1;i++,xbi+=n2) x[i] = (x[0]+xbi);
	
	for(i=0;i<n1;i++)
	  for(j=i+1;j<n1;j++) {
	    wht_value temp;
	    temp = x[i][j];
	    x[i][j] = x[j][i];
	    x[j][i] = temp; 
	  }
	x[0] = x[0]+n1;
      }
    free(x);
    return;
  }

  if (n1 < BlockSize) B = n1;
  
  for(step = 0; step < alpha; step ++) 
    {
      for(i=1,xbi=n2;i<n1;i++,xbi+=n2) x[i] = (x[0]+xbi);

      for(xbj=0; xbj<n1; xbj+=B)
	{
	  xbi = xbj;
	  for(i=0; i<B; i++)
	    {
	      for(j=i; j<B;j++)
		{
		  ii = xbi+i;
		  jj = xbj+j;
		  temp0 = x[jj][ii];
		  x[jj][ii] =  x[ii][jj];
		  x[ii][jj] = temp0;
		}
	    }
	  xbi+=B;
	  for(;xbi<n1;xbi+=B)
	    {
	      for(j=0; j<B; j++)
		{
		  jj = xbj+j;

		  for(i=0;i<B;i+=NoUnroll)
		    {
		      ii = xbi+i;
		      
		      temp0 = x[ii][jj];
		      temp1 = x[ii+1][jj];
		      temp2 = x[ii+2][jj];
		      temp3 = x[ii+3][jj];
		      
		      x[ii][jj]   = x[jj][ii];
		      x[ii+1][jj] = x[jj][ii+1];
		      x[ii+2][jj] = x[jj][ii+2];
		      x[ii+3][jj] = x[jj][ii+3];
		      
		      x[jj][ii]  = temp0;
		      x[jj][ii+1]= temp1;
		      x[jj][ii+2]= temp2;
		      x[jj][ii+3]= temp3;
		    }
		}
	    }
	}
      x[0] = x[0]+n1;
    }
  
  free(x);
}

