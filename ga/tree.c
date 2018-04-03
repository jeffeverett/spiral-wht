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
#include <string.h>
#include "tree.h"
#include "options.h"


struct node* create_node(int size, int num_children)
{
   int i;
   struct node* np;

   np = (struct node*) malloc( sizeof(struct node) );
   if( np==NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }

   np->size=size;
   np->parent=NULL;
   np->parent_index=-1;
   np->num_children=num_children;

   if( num_children == 0 ) {
      np->children=NULL;
   }
   else {
      np->children = (struct node**) malloc(num_children*sizeof(struct node*));
      if( np->children==NULL ) {
	 fprintf(stderr, "Out of memory!\n");
	 exit(5);
      }
      for( i=0; i<num_children; i++ )
	 np->children[i]=NULL;
   }

   return np;
}


struct node* copy_tree( struct node* orig )
{
   struct node* new;
   int i;

   new = create_node( orig->size, orig->num_children );
   for( i=0; i<orig->num_children; i++ ) {
      new->children[i] = copy_tree( orig->children[i] );
      new->children[i]->parent = new;
      new->children[i]->parent_index = i;
   }

   return new;
}

/* don't call, except from free_tree */
void free_node( struct node* node )
{
   if( node->children != NULL ) {
      free( node->children );
   }
   free( node );
}

void free_tree( struct node* tree )
{
   int i;

   for( i=0; i<tree->num_children; i++ ) {
      free_tree( tree->children[i] );
   }
   free_node( tree );
}

void free_children( struct node* node )
{
   int i;

   for( i=0; i<node->num_children; i++ ) {
      free_tree( node->children[i] );
   }
   if( node->children != NULL ) {
      free( node->children );
      node->children=NULL;
   }

   node->num_children = 0;
}


void tree_to_formula_rec( struct node* tree, char* formula )
{
   int i;

   if( tree->num_children == 0 ) {
      sprintf( formula, "small[%d]", tree->size );
   }
   else {
      strcpy( formula, "split[ " );
      formula += 7;
      for( i=0; i<tree->num_children; i++ ) {
	 tree_to_formula_rec( tree->children[i], formula );
	 if( i<tree->num_children-1 )
	    strcat( formula, ", " );
	 formula += strlen(formula);
      }
      strcpy( formula, " ]" );
   }
}

char* tree_to_formula( struct node* tree )
{
   char *formula;
   char buff[MAX_FORMULA_LEN];
  
   /*   formula = (char*) malloc( MAX_FORMULA_LEN * sizeof(char) );
   if( formula==NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }*/
   tree_to_formula_rec( tree, buff );
   formula = (char *) malloc ((strlen(buff) + 1) * sizeof(char));
   strcpy(formula, buff);
   return formula;
}


void get_node_info_rec( struct node* tree,
                        struct node_info* internal, struct node_info* leaf )
{
   int i;

   if( tree->num_children != 0 ) {
      if( internal != NULL ) {
	 internal->sizes[ tree->size ]++;
	 internal->ptrs[internal->num] = tree;
	 internal->num++;
      }
      for( i=0; i<tree->num_children; i++ ) {
	 get_node_info_rec( tree->children[i], internal, leaf );
      }
   }
   else {
      if( leaf != NULL ) {
	 leaf->sizes[ tree->size ]++;
	 leaf->ptrs[leaf->num] = tree;
	 leaf->num++;
      }
   }
}

void malloc_node_info( struct node_info* info )
{
   int i;

   if( info == NULL )
      return;

   info->num=0;

   info->sizes = (int*) malloc( MAX_NODE_SIZE * sizeof(int) );
   if( info->sizes == NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }
   for( i=0; i<MAX_NODE_SIZE; i++ ) {
      info->sizes[i] = 0;
   }

   info->ptrs = 
      (struct node**) malloc( MAX_NUM_NODES * sizeof(struct node*) );
   if( info->ptrs == NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }
   for( i=0; i<MAX_NUM_NODES; i++ ) {
      info->ptrs[i] = NULL;
   }
}

void free_node_info( struct node_info* info )
{
   free( info->sizes );
   free( info->ptrs );
}

void get_node_info( struct node* tree,
                    struct node_info* internal, struct node_info* leaf )
{
   malloc_node_info( internal );
   malloc_node_info( leaf );

   get_node_info_rec( tree, internal, leaf );
}


int trees_equal( struct node* a, struct node* b )
{
   int equal=1;
   int i;

   if( a->size == b->size && a->num_children == b->num_children ) {
      for( i=0; i<a->num_children && equal; i++ ) {
         if( ! trees_equal( a->children[i], b->children[i] ) ) {
	    equal=0;
	 }
      }
      return equal;
   }
   return 0;
}


void update_sizes( struct node* node, int amount )
{
   while( node != NULL ) {
      node->size += amount;
      node = node->parent;
   }
}

void remove_subtree( struct node** tree, struct node* subtree )
{
   int i;

   if( subtree == *tree ) {
      *tree = NULL;
   }
   else {
      /* if removing subtree would create an only child ... */
      if( subtree->parent->num_children == 2 ) {
	 /* if subtree is a child of the root ... */
	 if( subtree->parent->parent == NULL ) {
	    /* then the tree becomes the subtree's only sibling */
	    *tree = subtree->parent->children[1 - subtree->parent_index];
	    (*tree)->parent = NULL;
	    free_node( subtree->parent );
	 }
	 else {
	    /* else the subtree's only sibling
	     *      takes the place of the subtree's parent */
	    subtree->parent->parent->children[subtree->parent->parent_index] =
	       subtree->parent->children[1-subtree->parent_index];
	    subtree->parent->children[1-subtree->parent_index]->parent =
	       subtree->parent->parent;
	    subtree->parent->children[1-subtree->parent_index]->parent_index =
	       subtree->parent->parent_index;
	    /* and update the sizes of all parents */
	    update_sizes( subtree->parent->parent, -subtree->size );
	    free_node( subtree->parent );
	 }
      }
      else {
	 /* otherwise just remove the subtree
	  *           from the list of the parent's children */
	 subtree->parent->num_children--;
	 for( i=subtree->parent_index; i<subtree->parent->num_children; i++ ) {
	    subtree->parent->children[i] = subtree->parent->children[i+1];
	    subtree->parent->children[i]->parent_index = i;
	 }
	 /* and update the sizes of all parents */
	 update_sizes( subtree->parent, -subtree->size );
      }
   }
}

void add_subtree( struct node** tree, struct node* subtree,
                  struct node* place, int child_num )
{
   struct node* newparent;
   struct node* newsibling;
   struct node** newchildren;
   int i;

   /* If adding to a leaf ... */
   if( place->num_children == 0 ) {

      /* The leaf will become subtree's sibling */
      newsibling = place;

      /* Create a new node and attach it to the tree */
      newparent = create_node( newsibling->size + subtree->size, 2 );
      if( newsibling->parent == NULL ) {
         *tree = newparent;
      }
      else {
         newsibling->parent->children[newsibling->parent_index] = newparent;
         newparent->parent = newsibling->parent;
         newparent->parent_index = newsibling->parent_index;
         update_sizes( newsibling->parent, subtree->size );
      }

      /* Add the subtree and its new sibling as children to the new node */
      if( child_num < 1 ) {
         newparent->children[0] = newsibling;
         newsibling->parent = newparent;
         newsibling->parent_index = 0;
         newparent->children[1] = subtree;
         subtree->parent = newparent;
         subtree->parent_index = 1;
      }
      else {
         newparent->children[1] = newsibling;
         newsibling->parent = newparent;
         newsibling->parent_index = 1;
         newparent->children[0] = subtree;
         subtree->parent = newparent;
         subtree->parent_index = 0;
      }
   }

   /* Else if adding to an internal node ... */
   else {

      /* Subtree's new parent will be "place" */
      newparent = place;

      /* Create a new children-list of the correct size */
      newchildren = (struct node**)
         malloc( (newparent->num_children+1) * sizeof(struct node*) );
      if( newchildren == NULL ) {
         fprintf(stderr, "Out of memory!\n");
         exit(5);
      }

      /* Copy over the first portion of children */
      for( i=0; i<child_num; i++ ) {
         newchildren[i] = newparent->children[i];
      }

      /* Add in the subtree */
      newchildren[i] = subtree;
      subtree->parent = newparent;
      subtree->parent_index = i;

      /* Copy rest of the old children */
      for( i++; i<=newparent->num_children; i++ ) {
         newchildren[i] = newparent->children[i-1];
         newchildren[i]->parent_index = i;
      }

      /* Stick in the new children-list */
      free( newparent->children );
      newparent->children = newchildren;

      /* Update num_children and sizes */
      newparent->num_children++;
      update_sizes( newparent, subtree->size );
   }
}
