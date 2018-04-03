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
#include "options.h"

extern char* measure_exe;
extern char fitness_function;

double time_formula( char* formula )
{
   char command[MAX_COMMAND_LEN];
   FILE* pp;
   double time;

   printf( "   %s ! ", formula );
   fflush( stdout );

   sprintf( command, "%s -w '%s'", measure_exe, formula );
   pp = popen( command, "r" );
   fscanf( pp, "%lf", &time );
   pclose( pp );

   printf( "%8.2e\n", time );
   fflush( stdout );

   return time;
}

void time_pop( struct indiv** pop, int pop_size )
{
   int i;

   for( i=0; i<pop_size; i++ ) {
      if( pop[i]->time <= 0.0 ) {
	 pop[i]->time = time_formula( pop[i]->formula );
      }
   }
}


double reciprocal_fitness( struct indiv* indiv )
{
   return (1.0 / indiv->time);
}

double measured_fitness( struct indiv* indiv )
{
   return (indiv->time);
}

double indiv_fitness( struct indiv* indiv )
{
   if( indiv->fitness > 0.0 ) {
      return indiv->fitness;
   }
   else {
     switch( fitness_function ) {
        case 'r': return reciprocal_fitness( indiv ); break;
	case 'm': return measured_fitness( indiv ); break;
	default:
	   fprintf( stderr,
	            "Invalid fitness_function: %c!\n", fitness_function );
	   exit(5);
	   break;
     }
   }
}

double pop_fitness( struct indiv** pop, int pop_size )
{
   int i;
   double tot_fitness = 0.0;

   time_pop( pop, pop_size );

   for( i=0; i<pop_size; i++ ) {
      tot_fitness += pop[i]->fitness = indiv_fitness( pop[i] );
   }

   return tot_fitness;
}
