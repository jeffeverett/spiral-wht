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

/* PCL investigation for the WHT package
   =====================================
*/

/* compile with: make pclprof */

/* This function needs the pcl library 1.2 or later to be installed:
     http://www.fz-juelich.de/zam/PCL/
*/

/* pclprof -w <wht-method>
     [ -p ]         
     [ -e <event> ]
     -w <wht-method>
   measures <event> for <wht-method> using the pcl library 
   which must be installed. The default for <event> is PCL_CYCLES.
   To check which events can be monitored, run 'hpm -s'.
   The respective number is printed to stdout. If the option '-p'
   is supplied then also the name for the event monitored is printed.
*/

#include <pcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spiral_wht.h"

/* size of L2 Cache (in Bytes) is needed for cache flushing 
   512KB = 2^19B = 524288B
*/
#define L2CACHE 524288

/* flush cache by initializing an array of
   four times the size (suggested by Padua and Jianxin)
*/
#define FLUSH_CACHE() {long l2c4 = 4 * L2CACHE/sizeof(wht_value), il2c4; \
  wht_value *l2cf; \
  l2cf = (wht_value *) malloc(l2c4 * sizeof(wht_value)); \
  for (il2c4 = 0; il2c4 < l2c4; il2c4++) l2cf[il2c4] = 1.0; }

/* random doubles in [0, 1] */
#define RAND() ((double) (rand() + 1.0L))/RAND_MAX

/* the PCL constant for an event given as a string
   i.e. returns PCL_CYCLES for the string "PCL_CYCLES",
   the code is taken from pcl/hpm.c
*/

static int map_eventnames(char *name)
{
  int i;

  /* search all names */
  for (i = 0; i < PCL_MAX_EVENT; ++i)
    if (!strcmp(name, PCLeventname(i)))
      return i;

  /* not found */
  return -1;
}

/* print either integer value or double value,
   code is taken from pcl/pcl_test.c
*/

static void print_result(int event, PCL_CNT_TYPE i_result, PCL_FP_CNT_TYPE fp_result)
{
  if (event < PCL_MFLOPS)
    /* use %f format as we don't know what format we must use for 64-bit ints */
    printf("%13.0f", (double) i_result);
  else
    printf("%13f", fp_result);
}

/* the following function is only needed for pcl profiling */
#ifdef PCL_PROFILE
void print_pcl_tree(Wht* W, int indent)
{
   int i;
   long child_pcl = 0;

   for (i = 0; i < indent; i++) 
     printf("  ");
   printf("Node %2d: %10ld", W->n, W->pcl);
   if (W->priv.split.nn > 0) {
      for(i = 0; i < W->priv.split.nn; i++) {
         child_pcl += W->priv.split.Ws[i]->pcl;
      }
      printf(",  overhead: %8ld pcl", W->pcl - child_pcl);
   }
   printf( "\n" );
   indent++;
   for (i = 0; i < W->priv.split.nn; i++) {
      print_pcl_tree(W->priv.split.Ws[i], indent);
   }
}
#endif


int main(int argc, char *argv[])
{
  int n;
  long N, i;
  char c;
  int printev = 0;
  int event = PCL_CYCLES;
  Wht *W = NULL;
  wht_value *x;

  /* pcl declarations */
  int counter[1];
  PCL_CNT_TYPE result[1];
  PCL_FP_CNT_TYPE fp_result[1];
  unsigned int mode;


  if (argc == 1) {
    printf(
      "usage: pclprof\n"
      "  [ -p ]          # print event\n"
      "  [ -e <event> ]  # default: PCL_CYCLES, try: hpm -s\n"
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
      case 'p':

        printev = 1;
        break;

      case 'e':

        CHECK_ARG();

        /* event to be monitored */
        event = map_eventnames(argv[++n]);
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
  N = W->N;

  /* define count mode */
  mode       = PCL_MODE_USER;
  counter[0] = event;

  /* check if event can be measured */
  if (PCLquery(counter, 1, mode) != PCL_SUCCESS) {
    fprintf(stderr, "event ");
    fprintf(stderr, "%s", PCLeventname(event));
    fprintf(stderr, " cannot be measured\n");
    exit(-1);
  }

  /* allocate and initialize vector */
  x = (wht_value *) malloc(N * sizeof(wht_value));
  for (i = 0; i < N; i++)
    x[i] = RAND();

  /* performing WHT */
  FLUSH_CACHE();
  PCLstart(counter, 1, mode);
  wht_apply(W, 1, x);
  PCLstop(result, fp_result, 1);
#ifdef PCL_PROFILE
  W->pcl += result[0];
#endif

  /* print result */
  if (printev) 
    printf("%s: ", PCLeventname(event));
  print_result(event, result[0], fp_result[0]);
  printf("\n");

#ifdef PCL_PROFILE
  /* print profile */
  printf("Rightmost child is lowest and computed first!\n");
  printf("Event: %s\n", PCLeventname(event));
  printf("Tree profile:\n");
  print_pcl_tree(W, 0);
#endif

  return 0;
}
