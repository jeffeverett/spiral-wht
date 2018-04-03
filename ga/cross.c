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
#include "indiv.h"
#include "rand.h"

struct node* rand_node_of_size( struct node_info* info, int size )
{
   struct node* node;
   int start;
   int i;

   start = rand_int( info->num );
   if( info->ptrs[start]->size == size ) {
      node = info->ptrs[start];
   }
   else {
      node = NULL;
      for( i = (start+1) % info->num;
	   node == NULL && i!=start;
	   i = (i+1) % info->num ) {
	 if( info->ptrs[i]->size == size ) {
	    node = info->ptrs[i];
	 }
      }
   }

   return node;
}

void swap_a_subtree( struct node* a, struct node* b )
{
   int num_common, chosen_common, chosen_size;
   int rand;
   struct node_info a_internal_info;
   struct node_info a_leaf_info;
   struct node_info b_internal_info;
   struct node_info b_leaf_info;
   struct node* a_node;
   struct node* b_node;
   struct node* tmp_parent;
   int tmp_parent_index;
   int i;

   /* Get node info */
   get_node_info( a, &a_internal_info, &a_leaf_info );
   get_node_info( b, &b_internal_info, &b_leaf_info );

   /* See if two individuals have internal nodes of the same size 
      or a leaf and an internal node of the same size */
   num_common=0;
   for( i=0; i<a->size; i++ ) {
      if(    (    a_internal_info.sizes[i] > 0
               && b_internal_info.sizes[i] > 0 )
          || (    a_leaf_info.sizes[i] > 0
	       && b_internal_info.sizes[i] > 0 )
	  || (    a_internal_info.sizes[i] > 0
	       && b_leaf_info.sizes[i] > 0     ) ) {
         num_common++;
      }
   }

   if( num_common>0 ) {

      /* Pick a random common size */
      chosen_common = rand_int( num_common ) + 1;
      num_common=0;
      for( i=0; num_common < chosen_common && i<a->size; i++ ) {
	 if(    (    a_internal_info.sizes[i] > 0
		  && b_internal_info.sizes[i] > 0 )
	     || (    a_leaf_info.sizes[i] > 0
		  && b_internal_info.sizes[i] > 0 )
	     || (    a_internal_info.sizes[i] > 0
		  && b_leaf_info.sizes[i] > 0     ) ) {
	    num_common++;
	 }
      }
      chosen_size = i-1;

      /* Pick random nodes of chosen size */
      if( b_internal_info.sizes[chosen_size] > 0 ) {
	 rand = rand_int( a_internal_info.sizes[chosen_size] +
			  a_leaf_info.sizes[chosen_size] );
	 if( rand < a_internal_info.sizes[chosen_size] ) {
	    a_node = rand_node_of_size( &a_internal_info, chosen_size );
	    rand = rand_int( b_internal_info.sizes[chosen_size] +
			     b_leaf_info.sizes[chosen_size] );
	    if( rand < b_internal_info.sizes[chosen_size] )
	       b_node = rand_node_of_size( &b_internal_info, chosen_size );
	    else
	       b_node = rand_node_of_size( &b_leaf_info, chosen_size );
	 }
	 else {
	    a_node = rand_node_of_size( &a_leaf_info,     chosen_size );
	    b_node = rand_node_of_size( &b_internal_info, chosen_size );
	 }
      }
      else {
	 a_node = rand_node_of_size( &a_internal_info, chosen_size );
	 b_node = rand_node_of_size( &b_leaf_info,     chosen_size );
      }

      /* Swap nodes */
      a_node->parent->children[a_node->parent_index] = b_node;
      b_node->parent->children[b_node->parent_index] = a_node;
      tmp_parent = a_node->parent;
      a_node->parent = b_node->parent;
      b_node->parent = tmp_parent;
      tmp_parent_index = a_node->parent_index;
      a_node->parent_index = b_node->parent_index;
      b_node->parent_index = tmp_parent_index;

   }

   free_node_info( &a_internal_info );
   free_node_info( &a_leaf_info );
   free_node_info( &b_internal_info );
   free_node_info( &b_leaf_info );
}

void cross_over( struct indiv** a, struct indiv** b )
{
   struct node* atree;
   struct node* btree;

   /* Copy Trees */
   atree = copy_tree( (*a)->tree );
   btree = copy_tree( (*b)->tree );

   /* Do the actual crossover */
   swap_a_subtree( atree, btree );

   /* Update new individuals */
   init_indiv( a, atree );
   init_indiv( b, btree );
}
