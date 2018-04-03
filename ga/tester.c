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
#include "randtree.h"
#include "cross.h"
#include "mutate.h"

void tester( int size, int pop_size, int num_gens, int crossed, int mutated )
{
   struct indiv* a;
   struct indiv* b;
   int i, j;

   for( i=0; i<pop_size; i++) {
      init_indiv( &a, rand_tree(size) );
      init_indiv( &b, rand_tree(size) );
      printf( "Rand   A: %s\n",   a->formula );
      printf( "Rand   B: %s\n\n", b->formula );

      for( j=0; j<num_gens; j++ ) {
         if( crossed > 0 ) {
	    cross_over( &a, &b );
	    printf( "Cross  A: %s\n",   a->formula );
	    printf( "Cross  B: %s\n\n", b->formula );
	 }
	 if( mutated > 0 ) {
	    mutate( &a );
	    printf( "Mutate A: %s\n", a->formula );
	    if( mutated > 1 ) {
	       mutate( &b );
	       printf( "Mutate B: %s\n\n", b->formula );
	    }
	    else {
	       printf( "\n" );
	    }
	 }
      }

      printf( "\n\n" );
   }
}
