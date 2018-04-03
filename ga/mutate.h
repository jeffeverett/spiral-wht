#ifndef MUTATE_H
#define MUTATE_H

#include "indiv.h"

void mutate( struct indiv** indiv );

int read_mutation_string( char* string );
void print_mutation_weights( void );

#endif
