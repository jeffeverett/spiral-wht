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
#include "indiv.h"
#include "options.h"


struct hash_node {
   struct indiv* indiv;
   struct hash_node* next;
};


struct hash_node hash_table[HASH_SIZE];


void init_hash_table(void)
{
   int i;

   for( i=0; i<HASH_SIZE; i++ ) {
      hash_table[i].indiv = NULL;
      hash_table[i].next = NULL;
   }
}


int hash_val( char* key )
{
   long value;

   value=0;
   while( *key != '\0' ) {
      value = ( (value+1) * ((long) *key + 1) ) % HASH_SIZE;
      key++;
   }
   return value;
}


struct indiv* hash_lookup( char* key )
{
   struct hash_node* ptr;

   ptr = &hash_table[hash_val(key)];

   do {
      if( ptr->indiv != NULL && strcmp( ptr->indiv->formula, key ) == 0 )
         return ptr->indiv;
      ptr = ptr->next;
   } while( ptr != NULL );

   return NULL;
}


void hash_add( struct indiv* indiv )
{
   struct hash_node* ptr;

   ptr = &hash_table[hash_val(indiv->formula)];

   if( ptr->indiv == NULL ) {
      ptr->indiv = indiv;
      return;
   }

   while( ptr->next != NULL ) {
      ptr = ptr->next;
   }
   ptr->next = (struct hash_node*) malloc( sizeof(struct hash_node) );
   if( ptr->next == NULL ) {
      fprintf(stderr, "Out of memory!\n");
      exit(5);
   }
   ptr=ptr->next;
   ptr->indiv = indiv;
   ptr->next = NULL;
}
