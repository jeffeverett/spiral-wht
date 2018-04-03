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

#include <stdlib.h>
#include "rand.h"

void init_rand(unsigned int seed)
{
   srandom(seed);
}

/* Returns an integer in the range 0 to "range"-1 inclusive */
/* The following does not work on a Sun, due partially to the
 fact that RAND_MAX is defined relative to rand() (which returns
 an int, not random() (which returns a long) */
/*
long int rand_int(long int range)
{
   long int r;
   r = (long int)
       ( ( (double) random() * (double) range ) / ( (double) RAND_MAX + 1.0 ) );
   return r;
}
*/

/* this produces a slightly different distribution of 
   random numbers than the above -- the above has a slightly
   higher chance of picking some particular numbers in the
   range, depending on the value of range; with this one,
   those numbers are always the first few.  However, for
   small values of range, this difference is negligble at best.

   (For example, range of 1000 and long = 32 bits means that:
      0-295 each have probability    .001000000164
      296-999 each have probability  .000999999931
   For the previous generator, all numbers would still have one of
   these two probabilities; it's just not as easy to pick which
   numbers have which probability.)
*/
long int rand_int(long int range)
{
  return (random() % range);
}

/* Returns a double in the range [0.0, "range") */
/* The following does not work on a Sun (unless "bus error"
   can be considered working.)  This is for the same reasons
   as above
   */
/*
double rand_d(double range)
{
   double r;
   r = ( (double) random() * range ) / ( (double) RAND_MAX + 1.0 );
   return r;
}
*/

double rand_d(double range)
{
  double temp = (double) random() * .31415926535898;
  return temp - ((long int)(temp / range)) * range;
}
