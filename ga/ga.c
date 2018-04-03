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
#include "randtree.h"
#include "fitness.h"
#include "cross.h"
#include "mutate.h"
#include "rand.h"
#include "spiral_wht.h"

extern char draw_method;
extern int tournament_size;


void print_stats( struct indiv** pop, int pop_size )
{
   int i;
   double tot_time=0.0;

   printf("\n");
   printf( "   Best Indiv: %s ! %8.2e\n", pop[0]->formula, pop[0]->time );

   for( i=0; i<pop_size; i++ ) {
      tot_time += pop[i]->time;
   }
   printf( "   Ave. Time: %8.2e\n", tot_time / (double) pop_size );
   fflush( stdout );
}

int print_best( struct indiv** pop, struct indiv** tmp_pop,
               int pop_size, int best_kept )
{
   int pop_i, tpop_i, tpop_j;
   int seen_before;

   printf( "\n\n\nSummary:\n" );
   for( pop_i=0, tpop_i=0; pop_i<pop_size && tpop_i<best_kept; pop_i++ ) {
      seen_before=0;
      for( tpop_j=0; tpop_j<tpop_i && !seen_before; tpop_j++ ) {
         if( strcmp( pop[pop_i]->formula, tmp_pop[tpop_j]->formula ) == 0 ) {
	    seen_before=1;
	 }
      }
      if( !seen_before ) {
         tmp_pop[tpop_i] = pop[pop_i];
	 tpop_i++;
	 printf( "   Indiv%3d: %s ! %8.2e\n",
	         tpop_i, pop[pop_i]->formula, pop[pop_i]->time );
      }
   }
   return tpop_i;
}


int find_best( struct indiv** pop, struct indiv** tmp_pop,
               int pop_size, int best_kept )
{
   int pop_i, tpop_i, tpop_j;
   int seen_before;

   for( pop_i=0, tpop_i=0; pop_i<pop_size && tpop_i<best_kept; pop_i++ ) {
      seen_before=0;
      for( tpop_j=0; tpop_j<tpop_i && !seen_before; tpop_j++ ) {
         if( strcmp( pop[pop_i]->formula, tmp_pop[tpop_j]->formula ) == 0 ) {
	    seen_before=1;
	 }
      }
      if( !seen_before ) {
         tmp_pop[tpop_i] = pop[pop_i];
	 tpop_i++;
      }
   }
   return tpop_i;
}


struct indiv*
draw_tournament( struct indiv** pop, int pop_size )
{
   int fastest = pop_size;
   int rand;
   int i;

   for( i=0; i<tournament_size; i++ ) {
      rand = rand_int( pop_size );
      if( rand < fastest ) {
         fastest = rand;
      }
   }
   return pop[fastest];
}

struct indiv*
draw_weighted( struct indiv** pop, int pop_size, double tot_fitness )
{
   int i;
   double r;
   double sum;
   
   r = rand_d( tot_fitness );
   for( i=0, sum=0.0; pop[i]->fitness + sum <= r && i<pop_size; i++ )
      sum += pop[i]->fitness;
   if( i >= pop_size ) {
      fprintf( stderr, "Problem drawing from population!\n" );
      exit(2);
   }
   return pop[i];
}

struct indiv*
draw_from_pop( struct indiv** pop, int pop_size, double tot_fitness )
{
   switch( draw_method ) {
      case 'w': return draw_weighted( pop, pop_size, tot_fitness ); break;
      case 't': return draw_tournament( pop, pop_size ); break;
      default:  fprintf( stderr, "Invalid draw method: %c!\n", draw_method );
                exit(5);
		break;
   }
}


void generate_new_pop( struct indiv** pop, struct indiv** tmp_pop,
		       int pop_size, double tot_fitness, int transform_size,
		       int best_kept, int crossed,
		       int mutated, int injected_rand_trees )
{
   int actual_best_kept;
   int i;

   /* HELP -- this has a memory leak: never free space of dead individuals */

   /* Keep the best "best_kept" individuals alive */
   actual_best_kept = find_best( pop, tmp_pop, pop_size, best_kept );

   /* Inject new random trees into the population */
   for( i=actual_best_kept; i<actual_best_kept+injected_rand_trees; i++ ) {
      init_indiv( &tmp_pop[i], rand_tree(transform_size) );
   }

   /* Draw random individuals not being crossed over */
   for( i=actual_best_kept+injected_rand_trees; i<pop_size - crossed; i++ ) {
      tmp_pop[i] = draw_from_pop( pop, pop_size, tot_fitness );
   }

   /* Cross-over some individuals */
   if( crossed % 2 != 0 ) {
      fprintf( stderr,
               "Number of individuals to cross-over not a multiple of two!\n" );
      exit(1);
   }

   for( i=pop_size-crossed; i<pop_size; i+=2 ) {
      tmp_pop[i] = draw_from_pop( pop, pop_size, tot_fitness );
      tmp_pop[i+1] = draw_from_pop( pop, pop_size, tot_fitness );
      cross_over( &tmp_pop[i], &tmp_pop[i+1] );
   }

   /* Mutate some individuals in new population (except the best-kept ones) */
   for( i=0; i<mutated; i++ ) {
      mutate( &tmp_pop[actual_best_kept+rand_int(pop_size-actual_best_kept)] );
   }

   /* Update population */
   for( i=0; i<pop_size; i++ ) {
      pop[i] = tmp_pop[i];
   }
}


void ga( int transform_size, int pop_size, int num_gens,
         int best_kept, int crossed, int mutated, int injected_rand_trees )
{
   struct indiv** pop;
   struct indiv** tmp_pop;
   int gen;
   double tot_fitness, best_time;
   int i;

   pop = (struct indiv**) malloc( pop_size * sizeof(struct indiv*) );
   tmp_pop = (struct indiv**) malloc( pop_size * sizeof(struct indiv*) );
   for( i=0; i<pop_size; i++ ) {
      init_indiv( &pop[i], rand_tree(transform_size) );
   }

   for( gen=0; gen<num_gens; gen++ ) {
      printf( "Generation%4d:\n", gen );
      tot_fitness = pop_fitness( pop, pop_size );
      qsort( pop, pop_size, sizeof(struct indiv*),
             (int (*)(const void*,const void*))compare_fitness );
      print_stats( pop, pop_size );
      /* save best one if it beats besttrees */

      best_time = wht_get_tree_time(transform_size);
      printf("%e %e\n", best_time, pop[0]->time);
      
      if (pop[0]->time < best_time) wht_add_tree(wht_parse(pop[0]->formula), pop[0]->time);

      if( gen == num_gens-1 ) {
	 print_best( pop, tmp_pop, pop_size, best_kept );
      }
      else {
	 generate_new_pop( pop, tmp_pop, pop_size, tot_fitness, transform_size,
			   best_kept, crossed, mutated, injected_rand_trees );
      }
      printf( "\n\n" );
   }
   free(pop);
   free(tmp_pop);
}
