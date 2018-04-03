
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "transpose.h"

void p_transpose_stride(wht_value *xx, int n, int n1, long S, int pll)
{
  wht_value **x;
  int B = BlockSize;
  int i, j, ii, jj, xbi, xbj;
  wht_value temp0, temp1, temp2, temp3;
  int n2 = n * S / n1;
  int id, total;

  /*#pragma omp parallel shared(xx)*/
  {
  x = (wht_value **) malloc(n1*sizeof(wht_value *));
  if (pll) {
#ifdef _OPENMP    
    total = omp_get_num_threads() * n1 * S;
    id = omp_get_thread_num() * n1 * S;  
#else
    total = n1 * S;
    id = 0;
#endif
  } else {
    total = n1 * S;
    id = 0;
  }
  
  if (n1 < BlockSize) 
    B = n1;

  while (id < n2) {
    x[0] = &(xx[0]) + id;
    id += total;
    
    for(i = 1; i < n1; ++ i) 
      x[i] = x[i-1] + n2;

    for(xbj = 0; xbj < n1; xbj += B) {
      xbi = xbj;
      for(i = xbi; i < B + xbi; ++ i) {
        for(j = i + 1; j < B + xbj; ++ j) {
          temp0 = x[i][j * S];
          x[i][j * S] = x[j][i * S];
          x[j][i * S] = temp0;
        }
      }
      xbi += B;
      for( ; xbi < n1; xbi += B) {
        int jjs = xbj * S;
        j = xbj + B;
        for(jj = xbj; jj < j; ++ jj) {
          int iis = xbi * S;
          jjs += S;
          i = xbi + B;
          for(ii = xbi; ii < i; ii += NoUnroll) {
            iis += S;
            
            temp0 = x[ii][jjs];
            temp1 = x[ii + 1][jjs];
            temp2 = x[ii + 2][jjs];
            temp3 = x[ii + 3][jjs];
                      
            x[ii][jjs]   = x[jj][iis];
            x[ii+1][jjs] = x[jj][iis + S];
            x[ii+2][jjs] = x[jj][iis + 2*S];
            x[ii+3][jjs] = x[jj][iis + 3*S];
                      
            x[jj][iis]      = temp0;
            x[jj][iis + S]  = temp1;
            x[jj][iis + 2*S]= temp2;
            x[jj][iis + 3*S]= temp3;
          }
        }
      }
    }
  }
  free(x);
  
  } /*end of omp pragma*/
}

