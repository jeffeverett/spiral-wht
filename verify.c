/*
 * Copyright (c) 2000 Carnegie Mellon University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Verification of the WHT package
   ===============================
*/

/* compile with: make wht_verify */

/* verify [-p <threadnum>] -w <wht-method>
   ----------------------
   verifies <wht-method> given as a string in wht-syntax
   by comparing to a direct implementation.
   The '-p <threadnum>' option allows the verification of multiple 
   threads. The default value is the number set during installation.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spiral_wht.h"
#include "parallel.h"

/* random doubles in [0, 1] */
#define RAND() ((wht_value) (rand() + 1.0L))/RAND_MAX

void print(long N, wht_value *x) {
  long i;

  for (i = 0; i < N; i++) {
    printf("%4.2f ", x[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  int n;
  int thread = THREADNUM;
  long N, i;
  char c;
  Wht *W = NULL, *Wdirect = NULL;
  wht_value *x;
  wht_value *y;

  if (argc == 1) {
    printf("usage: verify -w <wht-method> -p <threadnum>\n");
    exit(0);
  }

#define CHECK_ARG() {if (argc == n+1) { \
  fprintf(stderr, "error, argument for option '-%c' missing\n", c); \
  exit(-1); } }

  /* check options and arguments */
  n = 0;
  while (++n < argc) {
    if (*argv[n]++ == '-') {
      c = *argv[n];
      switch (c) {
      case 'w':

        CHECK_ARG();

        /* create wht to be verified */
        W = wht_parse(argv[++n]);
        break;

      case 'p':

        CHECK_ARG();

        /* thread number */
        sscanf(argv[++n], "%d", &thread);
        break;

      default:
        fprintf(stderr, "error, illegal option %c\n", c);
        exit(-1);
      }
    }
    else {
      fprintf(stderr, "error, options must be preceded by '-'\n");
      exit(-1);
    }  
  }

#undef CHECK_ARG

  /* parameters */
  n = W->n;
  N = W->N;

  /* create a direct WHT */
  Wdirect = wht_new_direct(n);
  
  /* vectors */
  x = (wht_value *) malloc(N * sizeof(wht_value));
  y = (wht_value *) malloc(N * sizeof(wht_value));
  for (i = 0; i < N; i++) {
    x[i] = RAND();
    y[i] = x[i];
  }

#ifdef _OPENMP
  omp_set_num_threads(thread);
#endif

  /* transform and compare */
  wht_apply(W, 1, x);
  wht_apply(Wdirect, 1, y);
  for (i = 0; i < N; i++) {
    if (fabs(x[i] - y[i]) > 1e-10) {
      printf("difference at index %ld: %e\n", i, fabs(x[i] - y[i]));
      printf("exiting\n");
      exit(-1);
    }
  }

  /* everything's fine */
  printf("correct\n");   
  return 0;
}


