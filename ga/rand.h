#ifndef MYRAND_H
#define MYRAND_H

void init_rand(unsigned int seed);

/* Returns an integer in the range 0 to range-1 inclusive */
long int rand_int(long int range);

/* Returns a double in the range [0.0, "range") */
double rand_d(double range);

#endif
