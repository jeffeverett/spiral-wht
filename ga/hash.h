#ifndef MYHASH_H
#define MYHASH_H

#include "indiv.h"

void init_hash_table(void);

struct indiv* hash_lookup( char* key );

void hash_add( struct indiv* indiv );

#endif
