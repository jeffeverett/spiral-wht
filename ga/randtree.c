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

#include "randtree.h"
#include "rand.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>

extern int min_leaf, max_leaf;

int num_1bits( long vect )
{
   int num=0;
   while( vect != 0 ) {
      if( (vect & 1) == 1 )
         num++;
      vect = vect >> 1;
   }
   return num;
}

struct node* rec_uniform_splitting( int size )
{
   struct node* tree;
   long splitting;
   int num_children;
   int child_size;
   int child_index;

   do {
      splitting = rand_int( 1 << (size-1) );
      splitting |= 1 << (size-1);
      num_children = num_1bits(splitting);
   } while( num_children == 1 && size > max_leaf );

   if( num_children == 1 ) {
      tree = create_node( size, 0 );
   }
   else {
      tree = create_node( size, num_children );
      child_size=1;
      child_index=0;
      while( splitting != 0 ) {
         if( (splitting & 1) == 1 ) {
	    tree->children[child_index] = rec_uniform_splitting( child_size );
	    tree->children[child_index]->parent = tree;
	    tree->children[child_index]->parent_index = child_index;
	    child_index++;
	    child_size=1;
	 }
	 else {
	    child_size++;
	 }
	 splitting = splitting >> 1;
      }
   }

   return tree;

}


struct node* rec_series_splitting( int size )
{
   struct node* tree;
   int num_children;
   int children_sum;
   int children[MAX_NODE_SIZE];
   int i;

   /* Sanity check */
   if( size < min_leaf ) {
     fprintf( stderr,
               "Tried to create a node of size smaller than min_leaf!\n" );
      exit(13);
   }

   /* If we are within twice the min leaf size, make it a leaf */
   if( size < 2*min_leaf ) {
      return create_node( size, 0 );
   }

   /* If we are at a possible leaf size,
      give a 50-50 chance of making it a leaf */
   if( size <= max_leaf && rand_int(2) >= 1 ) {
      return create_node( size, 0 );
   }

   children[0] = rand_int( size-2*min_leaf+1 ) + min_leaf;
   num_children=1;
   children_sum=children[0];
   while( children_sum < size ) {
      if( size-children_sum < 2*min_leaf ) {
         children[num_children] = size-children_sum;
      }
      else {
	 children[num_children] =
	    rand_int( size-children_sum-2*min_leaf+1 ) + min_leaf;
      }
      children_sum += children[num_children];
      num_children++;
   }

   tree = create_node( size, num_children );
   for( i=0; i<num_children; i++ ) {
      tree->children[i] = rec_series_splitting( children[i] );
      tree->children[i]->parent = tree;
      tree->children[i]->parent_index = i;
   }

   return tree;
}


#define min(a,b) ((a) < (b)) ? (a) : (b)

int factorial( int n )
{
   int i;
   int prod;

   prod=1;
   for( i=2; i<=n; i++ ) {
      prod *= i;
   }

   return prod;
}

int** n_choose_l=NULL;

struct node* leaf_joining( int size )
{
   int leaf_size;
   struct node* subtrees[MAX_NODE_SIZE];
   struct node* new_subtree;
   int num_subtrees;
   int num_children;
   int leaf_sum;
   int rand;
   int n,i;

   /* create leaves */
   num_subtrees=0;
   leaf_sum=0;
   while( leaf_sum < size ) {
      do {
         leaf_size = 
	    rand_int( min(size-leaf_sum,max_leaf)-min_leaf+1 ) + min_leaf;
      } while( leaf_size > size-leaf_sum-min_leaf &&
               leaf_size != size-leaf_sum );
      subtrees[num_subtrees] = create_node( leaf_size, 0 );
      leaf_sum += leaf_size;
      num_subtrees++;
   }

   /* Set-up n choose l table if not done already */
   if( n_choose_l == NULL ) {
      n_choose_l = (int**) malloc( MAX_NODE_SIZE * sizeof(int*) );
      for( n=0; n<MAX_NODE_SIZE; n++ ) {
	 n_choose_l[n] = (int*) malloc( MAX_NODE_SIZE * sizeof(int) );
	 n_choose_l[n][1] = 0;
	 for( i=2; i<MAX_NODE_SIZE; i++ ) {
	    n_choose_l[n][i] = n_choose_l[n][i-1] +
	                       (factorial(n)/factorial(n-i))/factorial(i);
	 }
      }
   }

   /* join leaves and subtrees together */
   while( num_subtrees > 1 ) {
      /* HELP FIX THIS */
      rand = rand_int( (1<<num_subtrees) - num_subtrees - 1 );
      for( i=2; rand > n_choose_l[num_subtrees][i] && i<num_subtrees; i++ )
         ;
      num_children = i;
      new_subtree = create_node( 0, num_children );
      for( i=0; i<num_children; i++ ) {
	 rand = rand_int( num_subtrees );
         new_subtree->children[i] = subtrees[rand];
	 new_subtree->size += subtrees[rand]->size;
	 subtrees[rand]->parent = new_subtree;
	 subtrees[rand]->parent_index = i;
	 num_subtrees--;
	 subtrees[rand] = subtrees[num_subtrees];
      }
      subtrees[num_subtrees] = new_subtree;
      num_subtrees++;
   }

   return subtrees[0];
}


extern char rand_tree_method;

struct node* rand_tree( int size )
{
   switch( rand_tree_method ) {
      case 'u': return rec_uniform_splitting(size);
      case 's': return rec_series_splitting(size);
      case 'j': return leaf_joining(size);
      case 'r': switch( rand_int(3) ) {
		   case 0: return rec_uniform_splitting(size);
		   case 1: return rec_series_splitting(size);
		   case 2: return leaf_joining(size);
                }
      default:  fprintf( stderr, "No such rand tree method: %c\n",
                          rand_tree_method );
	        exit(7);
   }
}
