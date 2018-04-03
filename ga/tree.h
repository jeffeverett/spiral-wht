#ifndef TREE_H
#define TREE_H


struct node {
   int size;
   int parent_index;
   int num_children;
   struct node* parent;
   struct node** children;
};

struct node_info {
   int num;
   int* sizes;
   struct node** ptrs;
};


struct node* create_node(int size, int num_children);
struct node* copy_tree( struct node* orig );

void free_node( struct node* node );
void free_tree( struct node* tree );
void free_children( struct node* node );

char* tree_to_formula( struct node* tree );

void get_node_info( struct node* tree,
                    struct node_info* internal, struct node_info* leaf );
void free_node_info( struct node_info* info );

int trees_equal( struct node* a, struct node* b );

void remove_subtree( struct node** tree, struct node* subtree );
void add_subtree( struct node** tree, struct node* subtree,
                  struct node* place, int child_num );

#endif
