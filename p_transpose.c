#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "transpose.h"

void p_transpose(wht_value *xx, int n, int n1, int pll)
{
  wht_value **x;
  int B = BlockSize, Bi, Bj;
  int i, j, k, xbi, xbj;
  int band_id = 0, load, beta, shift, totalload, row = 0, col = 0;
  wht_value temp0, temp1, temp2, temp3;
  int n2 = n / n1;
  int id, total;

  if (n1 < BlockSize) {
    B = n1;
    beta = 1;
  } else {
    beta = n1 / BlockSize;
  }

  if (pll) {
#ifdef _OPENMP
    total = omp_get_num_threads();
    id = omp_get_thread_num();
#else
    total = 1;
    id = 0;
#endif
  } else {
    total = 1;
    id = 0;
  }

  x = (wht_value **) malloc(n1 * sizeof(wht_value *));
  
  /* determine total blocks (work) for each thread */
  totalload = beta * (beta + 1) / 2;
  shift = totalload % total;
  (shift) ? (load = totalload / total + 1) : (load = totalload / total);

  /*fprintf(stderr,"id%d beta%d load%d blocksize%d\n",id, beta, load, B);*/
  
  while (band_id < n2) {
    x[0] = &(xx[0]) + band_id;
    band_id += n1;

    for (i = 1; i < n1; ++ i) 
      x[i] = x[i-1] + n2;

    col = load * id;
    if (col >= totalload) {
      id = (id + total - shift) % total;
      continue;
    }
    row = 0;
    temp0 = beta;
    while (col >= temp0) {
      col -= temp0;
      temp0 --;
      row ++;
    }
    xbj = row * B;
    xbi = (col + row) * B;

    for (k = 0; k < load; ++ k) {
      if (xbj >= n1) break;
      /*fprintf(stderr, "id%d load%d band%d n1_%d n2_%d xbi%d xbj%d\n", 
        id, load, band_id, n1, n2, xbi, xbj);*/
      Bi = B + xbi;
      Bj = B + xbj;
      if (xbi == xbj) {
        for(i = xbi; i< Bi; ++ i) {
          for (j = i + 1; j < Bj; ++ j) {
            temp0 = x[i][j];
            x[i][j] =  x[j][i];
            x[j][i] = temp0;
          }
        }
      } else {
        for(j = xbj; j < Bj; ++ j) {
          for(i = xbi; i < Bi; i += NoUnroll) {
            temp0 = x[i][j];
            temp1 = x[i+1][j];
            temp2 = x[i+2][j];
            temp3 = x[i+3][j]; 
                      
            x[i][j]   = x[j][i];
            x[i+1][j] = x[j][i+1];
            x[i+2][j] = x[j][i+2];
            x[i+3][j] = x[j][i+3]; 
                      
            x[j][i]  = temp0;
            x[j][i+1]= temp1;
            x[j][i+2]= temp2;
            x[j][i+3]= temp3; 
          }
        }
      }
      xbi += B;
      if (xbi >= n1) {
        xbj += B;
	xbi = xbj;
      }
    } 
    id = (id + total - shift) % total;
  }
  free(x);
}

