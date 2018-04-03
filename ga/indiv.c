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

#include <stdio.h>
#include <stdlib.h>
#include "indiv.h"
#include "hash.h"

void init_indiv( struct indiv** indiv, struct node* tree )
{
   struct indiv* hash_indiv;
   char* formula;

   formula = tree_to_formula( tree );

   hash_indiv = hash_lookup( formula );

   if( hash_indiv != NULL ) {
      *indiv = hash_indiv;
      free( formula );
      free_tree( tree );
   }
   else {
      *indiv = (struct indiv*) malloc( sizeof(struct indiv) );
      if( *indiv == NULL ) {
	 fprintf(stderr, "Out of memory!\n");
	 exit(5);
      }

      (*indiv)->tree = tree;
      (*indiv)->formula = formula;
      (*indiv)->time = -1.0;
      (*indiv)->fitness = -1.0;

      hash_add( *indiv );
   }
}

/* HELP
void update_indiv( struct indiv* indiv )
{
   indiv->formula = tree_to_formula(indiv->tree);
   indiv->time = -1.0;
   indiv->fitness = -1.0;
}

void copy_indiv( struct indiv* from, struct indiv** to )
{
   *to = (struct indiv*) malloc( sizeof(struct indiv) );
   if( *to == NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }
   
   (*to)->tree = copy_tree( from->tree );
   (*to)->formula = from->formula;
   (*to)->time = -1.0;
   (*to)->fitness = -1.0;
}
*/


int compare_fitness( const struct indiv** a, const struct indiv** b )
{
   if( (*a)->fitness > (*b)->fitness )
      return -1;
   else if( (*a)->fitness < (*b)->fitness )
      return 1;
   return 0;
}
