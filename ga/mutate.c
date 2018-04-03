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
#include "randtree.h"
#include "rand.h"
#include "options.h"

extern int min_leaf, max_leaf;


void mutate_grow( struct node** tree );
void mutate_truncate( struct node** tree );
void mutate_join( struct node** tree );
void mutate_split( struct node** tree );
void mutate_up( struct node** tree );
void mutate_down( struct node** tree );
void mutate_flip( struct node** tree );
void mutate_move( struct node** tree );

void mutate_choice( struct node** tree );


#define NUM_MUTATIONS 8

int mutation_let_to_num[26] =
    /* a   b   c   d   e   f   g   h   i   j   k   l   m */
    { -1, -1, -1,  5, -1,  6,  0, -1, -1,  2, -1, -1,  7,
    /* n   o   p   q   r   s   t   u   v   w   x   y   z */
      -1, -1, -1, -1, -1,  3,  1,  4, -1, -1, -1, -1, -1 };
double mutation_weights[NUM_MUTATIONS] = {1,1,1,1,1,1,1,0};
double mutation_total_weight=7;
void (*mutation_functions[NUM_MUTATIONS])(struct node**) =
     { mutate_grow, mutate_truncate,
       mutate_join, mutate_split,
       mutate_up, mutate_down,
       mutate_flip,
       mutate_move };


/* Turn a leaf into an internal node by growing a subtree */
void mutate_grow( struct node** tree )
{
   struct node_info leaf_info;
   struct node* node;
   struct node* subtree;
   int start;
   int i;

   /* Get leaf node info */
   get_node_info( *tree, NULL, &leaf_info );

   /* Find leaf that could be an internal node */
   start = rand_int( leaf_info.num );
   node=NULL;
   if( leaf_info.ptrs[start]->size >= 2*min_leaf  ) {
      node = leaf_info.ptrs[start];
   }
   else {
      for( i = (start+1) % leaf_info.num;
           node == NULL && i != start;
	   i = (i+1) % leaf_info.num ) {
	 if( leaf_info.ptrs[i]->size >= 2*min_leaf ) {
	    node = leaf_info.ptrs[i];
	 }
      }
   }

   if( node != NULL ) {
      subtree = rand_tree( node->size );
      if( subtree->num_children > 0 ) {
	 subtree->parent = node->parent;
	 subtree->parent_index = node->parent_index;
	 if( node->parent != NULL ) {
	    node->parent->children[node->parent_index] = subtree;
	 }
	 else {
	    *tree = subtree;
	 }
	 free_node( node );
      }
   }

   free_node_info( &leaf_info );
}


/* Truncate an internal node into a leaf */
void mutate_truncate( struct node** tree )
{
   struct node_info internal_info;
   struct node* node;
   int start;
   int i;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, NULL );

   /* Find internal node that could be a leaf */
   start = rand_int( internal_info.num );
   node=NULL;
   if( internal_info.ptrs[start]->size <= max_leaf ) {
      node = internal_info.ptrs[start];
   }
   else {
      for( i = (start+1) % internal_info.num;
           node == NULL && i != start;
	   i = (i+1) % internal_info.num ) {
	 if( internal_info.ptrs[i]->size <= max_leaf ) {
	    node = internal_info.ptrs[i];
	 }
      }
   }

   /* Truncate */
   if( node != NULL ) {
      free_children( node );
   }

   free_node_info( &internal_info );
}


int has_internal_sibling( struct node* node ) {
   int i;

   if( node->parent != NULL ) {
      for( i=0; i<node->parent->num_children; i++ ) {
         if( i != node->parent_index &&
	      node->parent->children[i]->num_children > 0 )
	    return 1;
      }
   }
   return 0;
}

/* Join 2 siblings into 1 node */
void mutate_join( struct node** tree )
{
   struct node_info internal_info;
   int node_index, start_node_index;
   struct node* node;
   int sibling_index, start_sibling_index;
   struct node* sibling;
   struct node** newchildren;
   int i;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, NULL );

   /* Pick a random internal node with an internal sibling and a 3rd sibling */
   start_node_index = rand_int( internal_info.num );
   node = internal_info.ptrs[start_node_index];
   if( !has_internal_sibling(node) || node->parent->num_children < 3 ) {
      for( node_index = (start_node_index+1)%internal_info.num, node=NULL;
           node==NULL && node_index!=start_node_index;
	   node_index = (node_index+1)%internal_info.num ) {
	 node = internal_info.ptrs[node_index];
	 if( !has_internal_sibling(node) || node->parent->num_children < 3 ) {
	    node = NULL;
	 }
      }
   }
   free_node_info(&internal_info);
   if( node==NULL ) {
      /* No internal node with an internal sibling */
      return;
   }

   /* Pick random internal sibling */
   start_sibling_index = rand_int( node->parent->num_children );
   if( start_sibling_index != node->parent_index &&
       node->parent->children[start_sibling_index]->num_children>0 ) {
      sibling = node->parent->children[start_sibling_index];
   }
   else {
      for( sibling_index = (start_sibling_index+1)%node->parent->num_children,
              sibling=NULL;
	   sibling==NULL && sibling_index!=start_sibling_index;
	   sibling_index = (sibling_index+1)%node->parent->num_children ) {
	 if( sibling_index != node->parent_index &&
	     node->parent->children[sibling_index]->num_children>0 ) {
	    sibling = node->parent->children[sibling_index];
         }
      }
   }

   /* Create a new children-list of the correct size */
   newchildren = (struct node**)
      malloc( (node->num_children + sibling->num_children)
              * sizeof(struct node*) );
   if( newchildren == NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }

   /* Copy over "node"'s children */
   for( i=0; i<node->num_children; i++ ) {
      newchildren[i] = node->children[i];
   }
   /* Copy over "sibling"'s children */
   for( ; i<node->num_children + sibling->num_children; i++ ) {
      newchildren[i] = sibling->children[ i - node->num_children ];
      newchildren[i]->parent = node;
      newchildren[i]->parent_index = i;
   }

   /* Stick in the new children-list */
   free( node->children );
   node->children = newchildren;

   /* Update "node"'s size and num_children */
   node->size = node->size + sibling->size;
   node->num_children = node->num_children + sibling->num_children;

   /* Remove "sibling" from parent */
   node->parent->num_children--;
   for( i=sibling->parent_index; i<node->parent->num_children; i++ ) {
      node->parent->children[i] = node->parent->children[i+1];
      node->parent->children[i]->parent_index = i;
   }

   /* Free stuff */
   free_node( sibling );
}


/* Split 1 node into 2 siblings */
void mutate_split( struct node** tree )
{
   struct node_info internal_info;
   int node_index, start_node_index;
   struct node* node;
   struct node* sibling;
   struct node* tmp;
   int num_children;
   int i;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, NULL );

   /* Pick a random non-root internal node with at least 4 children */
   start_node_index = rand_int( internal_info.num );
   node = internal_info.ptrs[start_node_index];
   if( node->num_children < 4 || node->parent == NULL ) {
      for( node_index = (start_node_index+1)%internal_info.num, node=NULL;
           node==NULL && node_index!=start_node_index;
	   node_index = (node_index+1)%internal_info.num ) {
	 if( internal_info.ptrs[node_index]->num_children >= 4 &&
	     internal_info.ptrs[node_index]->parent != NULL ) {
	    node = internal_info.ptrs[node_index];
	 }
      }
   }
   free_node_info(&internal_info);
   if( node==NULL ) {
      /* No non-root internal node with at least 4 children */
      return;
   }

   /* Pick a random splitting of children that preserves at least 2 children
      for each new sibling */
   num_children = rand_int( node->num_children - 3 ) + 2;

   /* Create new sibling, moving in children */
   sibling = create_node( 0, node->num_children - num_children );
   for( i=num_children; i<node->num_children; i++ ) {
      sibling->children[i-num_children] = node->children[i];
      sibling->children[i-num_children]->parent = sibling;
      sibling->children[i-num_children]->parent_index = i-num_children;
      sibling->size += sibling->children[i-num_children]->size;
   }

   /* Update node's num_children, size */
   node->num_children = num_children;
   tmp = node;
   while( tmp != NULL ) {
      tmp->size -= sibling->size;
      tmp = tmp->parent;
   }

   /* Add in new sibling */
   add_subtree( tree, sibling, node->parent, node->parent_index+1 );
}


/* Node's grandparent becomes parent */
void mutate_up( struct node** tree )
{
   struct node_info internal_info, leaf_info;
   int node_index, start_node_index;
   struct node* node;
   struct node* grandparent;
   int grandparent_index;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, &leaf_info );

   /* Pick a random node with a grandparent */
   start_node_index = rand_int( internal_info.num + leaf_info.num );
   if( start_node_index < internal_info.num )
      node = internal_info.ptrs[start_node_index];
   else
      node = leaf_info.ptrs[start_node_index-internal_info.num];
   if( node->parent == NULL || node->parent->parent == NULL ) {
      for( node_index = (start_node_index+1)%internal_info.num+leaf_info.num,
              node=NULL;
           node==NULL && node_index!=start_node_index;
	   node_index = (node_index+1)%(internal_info.num+leaf_info.num) ) {
	 if( node_index < internal_info.num )
	    node = internal_info.ptrs[node_index];
	 else
	    node = leaf_info.ptrs[node_index-internal_info.num];
	 if( node->parent == NULL || node->parent->parent == NULL ) {
	    node = NULL;
	 }
      }
   }
   free_node_info(&internal_info);
   free_node_info(&leaf_info);
   if( node==NULL ) {
      /* No node with a grandparent */
      return;
   }

   /* Move the node */
   grandparent = node->parent->parent;
   grandparent_index = node->parent->parent_index;
   remove_subtree( tree, node );
   if( node->parent_index == 0 )
      add_subtree( tree, node, grandparent, grandparent_index );
   else
      add_subtree( tree, node, grandparent, grandparent_index+1 );
}


/* Node's sibling becomes parent */
void mutate_down( struct node** tree )
{
   struct node_info internal_info, leaf_info;
   int node_index;
   struct node* node;
   int sibling_index;
   struct node* sibling;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, &leaf_info );

   /* Pick a random non-root node */
   node_index = rand_int( internal_info.num + leaf_info.num - 1 ) + 1;
   if( node_index < internal_info.num )
      node = internal_info.ptrs[node_index];
   else
      node = leaf_info.ptrs[node_index-internal_info.num];

   /* Pick a random sibling */
   sibling_index = rand_int( node->parent->num_children-1 );
   if( sibling_index >= node->parent_index )
      sibling_index++;
   sibling = node->parent->children[sibling_index];

   /* Move the node */
   remove_subtree( tree, node );
   add_subtree( tree, node, sibling, rand_int(sibling->num_children+1) );

   /* Free stuff */
   free_node_info( &internal_info );
   free_node_info( &leaf_info );
}


/* Swap 2 siblings */
void mutate_flip( struct node** tree )
{
   int node_index;
   struct node* node;
   struct node_info internal_info;
   int child1, child2;
   struct node* tmp;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get internal node info */
   get_node_info( *tree, &internal_info, NULL );

   /* Pick random node */
   node_index = rand_int( internal_info.num );
   node = internal_info.ptrs[node_index];

   free_node_info(&internal_info);

   /* Pick two random, different children */
   child1 = rand_int( node->num_children );
   child2 = rand_int( node->num_children-1 );
   if( child1 <= child2 )
      child2++;
   if( trees_equal( node->children[child1], node->children[child2] ) ) {
      return;
   }

   /* Swap children */
   tmp = node->children[child1];
   node->children[child1] = node->children[child2];
   node->children[child2] = tmp;

   /* Update parent_index */
   node->children[child1]->parent_index = child1;
   node->children[child2]->parent_index = child2;
}


/* Move a subtree anywhere */
void mutate_move( struct node** tree )
{
   struct node_info internal_info;
   struct node_info leaf_info;
   struct node* subtree;
   struct node* newparent;
   int subtree_index;
   int newparent_index;

   /* Make sure entire tree isn't just a leaf */
   if( (*tree)->num_children == 0 ) {
      return;
   }

   /* Get node info */
   get_node_info( *tree, &internal_info, &leaf_info );

   /* Choose a subtree to move */
   subtree_index = rand_int( internal_info.num + leaf_info.num - 1 );
   if( subtree_index < leaf_info.num ) {
      subtree = leaf_info.ptrs[subtree_index];
   }
   else {
      subtree = internal_info.ptrs[subtree_index-leaf_info.num+1];
   }

   /* Remove subtree from tree */
   remove_subtree( tree, subtree );

   /* Update node info */
   free_node_info( &internal_info );
   free_node_info( &leaf_info );
   get_node_info( *tree, &internal_info, &leaf_info );

   /* Choose a new parent and add the subtree */
   newparent_index = rand_int( internal_info.num + leaf_info.num );
   if( newparent_index < leaf_info.num ) {
      add_subtree( tree, subtree,
                   leaf_info.ptrs[newparent_index], rand_int(2) );
   }
   else {
      newparent = internal_info.ptrs[newparent_index-leaf_info.num];
      add_subtree( tree, subtree,
                   newparent, rand_int(newparent->num_children+1) );
   }

   /* Free node info */
   free_node_info( &internal_info );
   free_node_info( &leaf_info );
}


void mutate_choice( struct node** tree )
{
   double rand;
   double sum;
   int i;

   rand = rand_d( mutation_total_weight );
   for( i=0, sum=0; ((mutation_weights[i]+sum) < rand) && (i < NUM_MUTATIONS); i++ ) {
      sum += mutation_weights[i];
   }
   if( i >= NUM_MUTATIONS ) {
      fprintf( stderr, "Problem chosing mutation!\n" );
      exit(8);
   }
   mutation_functions[i]( tree );
}

void mutate( struct indiv** indiv )
{
   struct node* tree;
   int num_mutation_tries=0;

   tree = copy_tree( (*indiv)->tree );

   do {
      mutate_choice( &tree );
      num_mutation_tries++;
   } while( trees_equal( tree, (*indiv)->tree ) &&
            num_mutation_tries <= MAX_MUTATION_TRIES );

   init_indiv( indiv, tree );
}


int read_mutation_string( char* string )
{
   int loc;
   int consumed=0;
   int mutated=0;
   char type;
   int mutation_num;
   int i;

   sscanf( string, "%d%n", &mutated, &consumed );
   loc = consumed;
   while( sscanf( string+loc, "%c", &type ) == 1 ) {
      mutation_num = mutation_let_to_num[ (int) (type - 'a') ];
      if( mutation_num != -1 ) {
	 sscanf( string+loc+1, "%lf%n",
	         &mutation_weights[mutation_num], &consumed );
	 loc += consumed+1;
      }
      else {
         fprintf( stderr, "Invalid mutation type: %c!\n", type );
	 exit(5);
      }
   }

   mutation_total_weight=0;
   for( i=0; i<NUM_MUTATIONS; i++ ) {
      mutation_total_weight += mutation_weights[i];
   }

   return mutated;
}

void print_mutation_weights( void )
{
   int mutation_num;
   char c;

   printf( "   " );
   for( mutation_num=0; mutation_num<NUM_MUTATIONS; mutation_num++ ) {
      for( c='a'; c<='z'; c++ ) {
	 if( mutation_let_to_num[ (int) (c - 'a') ] == mutation_num ) {
	    printf( "%c: %3.1f  ", c, mutation_weights[mutation_num] );
	 }
      }
   }
   printf( "\n" );
}
