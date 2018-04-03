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

/* Storing best WHT trees
   ======================
*/

#include "spiral_wht.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_line(char * line, int * n, char * wht_string, double * time)
/* line is a line from the file besttrees
  parse_line interprets this line, separating it into its 3 parts:
    n: the size of the tree
    wht_string: a string in wht-syntax representing the best tree for this
     size found on this machine
    time: the time it takes this tree to exectue as reported by measure
  it is assumed that all pointers point to valid objects with sufficient space allocated for the operation

returns:
  1 on success
  0 on partial success -- got n and the tree, but no time
  -1 on failure -- line could not be interpreted

not terribly robust on invalid inputs right now
*/
{
  /* off3 is the length of the line; off1 and off2 are the points
      immediately after the first and second exclamation marks,
      respectively */
  int off1, off2, off3;
  int ret = 1;

  off3 = strlen(line);

  sscanf(line, "%d", n);

  off1 = strcspn(line, "!") + 1;
  if (off1 < off3) off2 = off1 + strcspn(line+off1, "!") + 1; else off2 = off1;

  if (off1 < off3) {
    strncpy(wht_string, line + off1, off2 - off1 - 1);
    wht_string[off2 - off1 - 1] = '\0';
  } else 
    ret = -1;

  if (off2 < off3) {
    sscanf(line + off2, "%lf", time);
  } else {
    if (ret == 1) ret = 0;
    *time = -1;
  }

  return ret;
}


Wht * wht_get_tree(int n)
{
  FILE * besttrees;
  char inputline[1024];
  int input_n;
  char wht_tree_string[1024];
  double time;
  char dump;

  besttrees = fopen(BESTTREE, "r");
  if (besttrees == NULL) return NULL;

  while (fscanf(besttrees, "%[^\n]", inputline)!=EOF) {
    if (parse_line(inputline, &input_n, wht_tree_string, &time) >= 0) {
      if (input_n == n) {
        fclose(besttrees);
        return wht_parse(wht_tree_string);
      }
    }
    fscanf(besttrees, "%c", &dump);
  }

  fclose(besttrees);
  return NULL;
}

#ifdef PARA_ON
Wht * wht_get_p_tree(int n, int thread)
{
  FILE * besttrees;
  char inputline[1024];
  int input_n;
  char wht_tree_string[1024];
  double time;
  char dump;
  char buf[512];

  /* may be subject to buffer overflow attack */
  sprintf(buf,"%s%d", BESTTREE, thread);
  buf[strlen(buf)+1] = '\0';

  besttrees = fopen(buf, "r");
  if (besttrees == NULL) return NULL;

  while (fscanf(besttrees, "%[^\n]", inputline)!=EOF) {
    if (parse_line(inputline, &input_n, wht_tree_string, &time) >= 0) {
      if (input_n == n) {
        fclose(besttrees);
        return wht_parse(wht_tree_string);
      }
    }
    fscanf(besttrees, "%c", &dump);
  }

  fclose(besttrees);
  return NULL;
}

/* wht_get_p_tree_time(int n, int thread) */
/* wht_add_p_tree(Wht * tree, double time, int thread) */

#endif

double wht_get_tree_time(int n)
{
  FILE * besttrees;
  char inputline[1024];
  int input_n;
  char wht_tree_string[1024];
  double time;
  char dump;

  besttrees = fopen(BESTTREE, "r");
  if (besttrees == NULL) return -1;

  while (fscanf(besttrees, "%[^\n]", inputline)!=EOF) {
    if (parse_line(inputline, &input_n, wht_tree_string, &time) >= 0) {
      if (input_n == n) {
        fclose(besttrees);
        return time;
      }
    }
    fscanf(besttrees, "%c", &dump);
  }

  fclose(besttrees);
  return -1;
}

int wht_add_tree(Wht * tree, double time)
{
  int n = tree->n;
  char * wht_tree_string;
  FILE * besttrees;
  char * fileinput;
  char inputline[1024], wht_input_string[1024];
  int input_n;
  int offset = 0, shift;
  int len;
  int found = 0;
  double wht_time;

  besttrees = fopen(BESTTREE, "r");
  if (besttrees == NULL) return 0;

  fileinput = (char *)malloc(10000);
  fscanf(besttrees, "%[^=]", fileinput);
  len = strlen(fileinput);

  fclose(besttrees);

  besttrees = fopen(BESTTREE, "w");

  while (offset < len) {
    sscanf(fileinput + offset, "%[^\n]%n", inputline, &shift);
    offset += shift + 1;
    if (parse_line(inputline, &input_n, wht_input_string, &wht_time) >= 0) {
      if (input_n != n) {
        fprintf(besttrees, "%d ! %s ! %9.3e\n", input_n, wht_input_string, wht_time);
      } else {
	wht_tree_string = wht_print(-1, tree);
	fprintf(besttrees, "%d ! %s ! %9.3e\n", n, wht_tree_string, time);
	free(wht_tree_string);
	found = 1;
      }
    }
  }

  if (found == 0) {
    wht_tree_string = wht_print(-1, tree);
    fprintf(besttrees, "%d ! %s ! %9.3e\n", n, wht_tree_string, time);
    free(wht_tree_string);
  }

  fclose(besttrees);
  return 1;
}




