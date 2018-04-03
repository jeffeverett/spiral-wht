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
#include <ctype.h>
#include "hash.h"
#include "rand.h"
#include "ga.h"
#include "mutate.h"
#include "tester.h"
#include "options.h"
#include "spiral_wht.h"

char rand_tree_method='j';
int min_leaf=2;
int max_leaf=6;

char draw_method='w';
int tournament_size=10;

char fitness_function='r';
char* measure_exe=DEFAULT_MEASURE_EXE;

int main( int argc, char* argv[] )
{
   int transform_size;
   int pop_size = 100;
   int num_gens = 100;
   int best_kept = 10;
   int crossed = 30;
   int mutated = 20;
   int injected_rand_trees = 10;
   int seed = 8349;
   int testing_mode_only=0;
   int i;

   if( argc < 2 || !isdigit(argv[1][0]) ) {
      printf( "Usage: %s transform_size\n"
              "       [-p population_size] [-g generations]\n"
	      "       [-b best_kept] [-c crossed] "
	             "[-m mutated[g#t#j#s#u#d#f#m#]]\n"
	      "       [-i injected_rand_trees]\n"
	      "       [-tree_gen uniform|series|joining|rand] "
	             "[-leaf min,max]\n"
	      "       [-draw weight|tournament=size] "
	             "[-fitness reciprocal|measured]\n"
	      "       [-e 'measure_executable measure_options']\n"
	      "       [-s random_seed]\n"
	      "       [-z testing_mode_only]\n",
	      argv[0] );
      exit(1);
   }

   transform_size = atoi( argv[1] );
   i=2;
   while( i<argc ) {
      switch( argv[i][1] ) {
         case 'p': pop_size   = atoi( argv[i+1] );  break;
         case 'g': num_gens = atoi( argv[i+1] );  break;
         case 'b': best_kept  = atoi( argv[i+1] );  break;
         case 'c': crossed    = atoi( argv[i+1] );  break;
         case 'm': mutated = read_mutation_string( argv[i+1] ); break;
	 case 'i': injected_rand_trees = atoi( argv[i+1] ); break;
	 case 't': rand_tree_method = argv[i+1][0]; break;
	 case 'l': sscanf( argv[i+1], "%d,%d", &min_leaf, &max_leaf ); break;
	 case 'd': draw_method = argv[i+1][0];
	           if( draw_method == 't' ) {
		      sscanf( argv[i+1], "%*[^=]%d", &tournament_size );
		   }
		   break;
	 case 'f': fitness_function = argv[i+1][0]; break;
	 case 'e': measure_exe = argv[i+1]; break;
         case 's': seed       = atoi( argv[i+1] );  break;
	 case 'z': testing_mode_only = 1; break;
	 default : fprintf( stderr, "Invalid option: %s!\n", argv[i] );
	           exit(5);
		   break;
      }
      i+=2;
   }

   printf( "transform_size = %d\n", transform_size );
   printf( "pop_size = %d\n", pop_size );
   printf( "num_gens = %d\n", num_gens );
   printf( "best_kept = %d\n", best_kept );
   printf( "crossed = %d\n", crossed );
   printf( "mutated = %d\n", mutated );
   print_mutation_weights();
   printf( "injected_rand_trees = %d\n", injected_rand_trees );
   printf( "rand_tree_method = %c\n", rand_tree_method );
   printf( "min_leaf = %d, max_leaf = %d\n", min_leaf, max_leaf );
   printf( "draw_method = %c", draw_method );
   if( draw_method == 't' ) {
      printf( " with size %d", tournament_size );
   }
   printf( "\n" );
   printf( "fitness_function = %c\n", fitness_function );
   printf( "measure_exe = '%s'\n", measure_exe );
   printf( "seed = %d\n", seed );
   printf( "\n" );

   if( pop_size < best_kept + crossed + injected_rand_trees ||
       pop_size < best_kept + mutated ) {
      fprintf( stderr,
               "Population size too small for number of\n"
               "   best_kept, mutated, crossed, and injected_rand_trees!\n" );
      return 3;
   }

   init_rand(seed);
   init_hash_table();

   if( !testing_mode_only )
      ga( transform_size, pop_size, num_gens,
          best_kept, crossed, mutated, injected_rand_trees );
   else
      tester( transform_size, pop_size, num_gens, crossed, mutated );

   return 0;
}
