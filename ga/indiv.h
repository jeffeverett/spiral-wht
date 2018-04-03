#ifndef INDIV_H
#define INDIV_H

#include "tree.h"

struct indiv {
   double time;
   double fitness;
   struct node* tree;
   char* formula;
};

void init_indiv( struct indiv** indiv, struct node* tree );
void update_indiv( struct indiv* indiv );
void copy_indiv( struct indiv* from, struct indiv** to );

int compare_fitness( const struct indiv** a, const struct indiv** b );

#endif
