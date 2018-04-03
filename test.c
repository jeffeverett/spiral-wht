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

/* A Test File for the WHT package
   ===============================
*/

#include <stdio.h>
#include "spiral_wht.h"
#include "parallel.h"

int main(void) 
{
  Wht *Wht_tree;
  wht_value input[16384];
  int x;

  /* initialize somehow */
  for (x = 0; x < 16384; x++) 
    input[x] = (x & 1) * 2 - 1; 

  /* get wht tree for size 2^2 */
  Wht_tree = wht_get_tree(2);

  if (Wht_tree == NULL) {
    printf("Could not find tree of size 2^2 in wht_trees file,\nDid you do \"make install\"?\n");
    exit (-1);
  } 
  else {
    wht_apply(Wht_tree, 1, input);  /* stride is 1 */
    printf("Computation for sequential WHT successful.\n");
  }
  wht_delete(Wht_tree);

#ifdef PARA_ON
  omp_set_num_threads(THREADNUM);
#pragma omp parallel
  {
    int id = omp_get_thread_num();
    if (id == 0)
      printf("Default number of threads = %d\n",omp_get_num_threads());
    printf("Thread %d works.\n", id);
  }

  /* get parallel wht tree for size 2^2 */
  Wht_tree = wht_get_p_tree(2, THREADNUM);

  if (Wht_tree == NULL) {
    printf("Could not find tree of size 2^2 in wht_trees%d file,\nDid you do \"make install\"?\n", THREADNUM);
    exit (-1);
  } 
  else {
    wht_apply(Wht_tree, 1, input);  /* stride is 1 */
    printf("Computation for parallel WHT successful.\n");
  }
#endif
  return(0);
}

