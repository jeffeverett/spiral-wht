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

/* Runtime measurement for the WHT package
   =======================================
*/

/* measure 
     [ -s <stride> ] # <wht> tensor I_<stride> is measured
     [ -t <num> ]    # sec. to measure, default = 1
     [ -g ]          # output in gap syntax   
     [ -p <threadnum> ]    # thread number
     [ -c ]          # use processor timer
     -w <wht-method>

   measures <wht-method> given as a string in wht-syntax
   applied as
     wht_apply( wht, <stride>, x)
   by repeating it at least <num> (a float) seconds. The time
   returned is for one wht, performed on a vector of length
   <size-wht> x <stride>. If <stride> is > 1 then an entire
   <wht> tensor 1_<stride> is performed and the average time
   of one <wht> returned. Performing only one <wht> with <stride>
   may produce bad timings.
   <stride> must be a 2-power. The default for stride is 1.
   The option '-g' prints the output in gap readable syntax, i.e.
     theInput := <t>
   where <t> is an integer representing the time in us/10.
   The option '-p <threadnum>' indicates the number of threads to 
   be used in this measurement. The default value is the number 
   set during installation.
   The option '-c' allows the program use the CPU timer. 
   The default timer is wall clock timer which is better for 
   measuring the speedup of multiple threads. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "spiral_wht.h"
#include "parallel.h"

double gettime_CPU(void) {
  clock_t t;

  t = clock();
  if (t == -1) {
    fprintf(stderr, "error in clcok()\n");
    exit(-1);
  }
  return (double) t/CLOCKS_PER_SEC;
}

#define  USEC_TO_SEC   1.0e-6    /* to convert microsecs to secs */

double gettime_Clock(void) {
   double time_seconds;
   struct timezone tzp;       /* time zone data */
   struct timeval  time_data; /* seconds since 0 GMT */
   
   tzp.tz_minuteswest = 0;    /* time zone info.  I don't care */
   tzp.tz_dsttime     = 0;    /* this so I just zero them      */
   
   gettimeofday(&time_data,&tzp);
   
   time_seconds  = (double) time_data.tv_sec;
   time_seconds += (double) time_data.tv_usec * USEC_TO_SEC;

   return time_seconds;
}


int main(int argc, char *argv[])
{
  int n;
  long stride = 1;
  float t = 1.0;
  long N, i, j;
  char c;
  int gap = 0;
  int thread = THREADNUM;

  double (*gettime) (void) = gettime_Clock;
  Wht *W = NULL;
  wht_value *x;
  long reps;
  double t1, time;

  if (argc == 1) {
    printf(
      "usage: p_measure\n"
      "  [ -s <stride> ]\n"
      "  [ -t <num> ]\n"
      "  [ -g ]\n"
      "  [ -p <threadnum> ]\n"
      "  [ -c ]\n"
      "  -w <wht-method>\n"
    );
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
      case 's':

        CHECK_ARG();

        /* stride */
        sscanf(argv[++n], "%ld", &stride);
        break;

      case 't':

        CHECK_ARG();

        /* number of seconds to be measured */
        sscanf(argv[++n], "%f", &t);
        break;

      case 'g':

        gap = 1;
        break;

      case 'p':

        CHECK_ARG();

        /* thread number */
        sscanf(argv[++n], "%d", &thread);
        break;

      case 'c':

        /* use processor timer */
        gettime = gettime_CPU;
        break;

      case 'w':

        CHECK_ARG();

        /* create wht to be verified */
        W = wht_parse(argv[++n]);
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

  /* wht present? */
  if (W == NULL) {
    fprintf(stderr, "wht must be given\n");
    exit(-1);
  }

  /* parameters */
  n = W->n;
  N = (W->N) * stride;

  /* allocate vector */
  x = (wht_value *) malloc(N * sizeof(wht_value));

  if ((x==NULL)) {
    fprintf(stderr, "out of memory\n");
    exit(-2);
  }

  for (i = 0; i < N; i++)
    x[i] = 0;  

#ifdef _OPENMP
  omp_set_num_threads(thread);
#endif

  if (stride == 1) {

    /* how many repetitions? */
    reps = 1;
    time = 0;
    while (time < t) {
      t1 = gettime();
      for (i = 0; i < reps; i++) {
        wht_apply(W, stride, x);
      }
      time = gettime() - t1;
      reps *= 2;
    }
    reps /= 2;

  }
  else {  /* stride is > 1 */

    /* how many repetitions? */
    reps = 1;
    time = 0;
    while (time < t) {
      t1 = gettime();
      for (i = 0; i < reps; i++) {
        for (j = 0; j < stride; j++)
          wht_apply(W, stride, x+j);
      }
      time = gettime() - t1;
      reps *= 2;
    }
    reps /= 2;

  }

  if (gap == 0) 
    printf("%9.3e\n", time/(reps * stride));
  else
    printf(
      "theInput := %ld; # us/10\n", 
      (long) ((time/(reps * stride)) * 10000000)
    );

  return 0;
}


