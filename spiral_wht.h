/* Fast Walsh-Hadamard Transform (.h-file)
  ========================================
  MP, from 26.08.99

  The package wht implements the Walsh-Hadamard transform
  of 2-power size N = 2^n (WHT_N). The files included in 
  the package are given in the README file.
*/


/* is this necessary? */
#if !defined(WHT_H)
#define WHT_H

/* data type for the signal */
typedef double wht_value;

/* symbolic constant for the wht-type */
#define wht_null      0
#define wht_direct    1
#define wht_small     2
#define wht_split     3

/* #ifdef DDL_ON    modified by Bo Hong */
#define wht_splitddl  4
/* #endif */
#define wht_small_il  5
#define wht_p_split    6
#define wht_p_splitddl 7


/* maximal numbers of factors for split */
#define SPLIT_MAX_FACTORS 30
#ifdef DDL_ON
#define SPLITddl_MAX_FACTORS 2
#endif

#ifdef PARA_ON
#define P_SPLIT_MAX_FACTORS 2
#define P_SPLITddl_MAX_FACTORS 2
#endif

/* data type for the wht */
typedef struct wht {
  int type,                                          /* the method */
      N,                                          /* signal length */
      n;                                               /*  N = 2^n */
#ifdef IL_ON
  int nILNumber;
  void (*apply)();
#else
  void (*apply)(struct wht *W, long S, wht_value *x);
#endif
  void (*deleteObj)(struct wht *W);     /* delete is a C++ keyword */
  union tagPriv {
    struct tagSplit {
      int nn;                                 /* number of factors */
      int ns[SPLIT_MAX_FACTORS];                /* size of factors */
      struct wht *Ws[SPLIT_MAX_FACTORS];      /* the smaller wht's */
    } split;
  } priv;
#ifdef PCL_PROFILE
  long pcl;
#endif
} Wht;

/* creation functions for the specific methods */
extern Wht *wht_new_null(int n);
extern Wht *wht_new_direct(int n);
extern Wht *wht_new_small(int n);
extern Wht *wht_new_split(int nn, Wht *whts[]);
#ifdef DDL_ON
extern Wht *wht_new_splitddl(int nn, Wht *whts[]);
#endif

#ifdef IL_ON
extern Wht *wht_new_smallil(int n, int nILNumber);
#endif

#ifdef PARA_ON   
extern Wht *wht_new_p_split(int nn, Wht *whts[]);
extern Wht *wht_new_p_splitddl(int nn, Wht *whts[]);
#endif

/* headers for unrolled whts */
#if !defined(MAX_UNROLLED)
#define MAX_UNROLLED 8
#endif

void apply_small1(Wht *W, long S, wht_value *x);
void apply_small2(Wht *W, long S, wht_value *x);
void apply_small3(Wht *W, long S, wht_value *x);
void apply_small4(Wht *W, long S, wht_value *x);
void apply_small5(Wht *W, long S, wht_value *x);
void apply_small6(Wht *W, long S, wht_value *x);
void apply_small7(Wht *W, long S, wht_value *x);
void apply_small8(Wht *W, long S, wht_value *x);

/* macros */
#define wht_apply(wht, S, x) ((wht->apply)(wht, S, x))
#define wht_delete(wht) (wht->deleteObj)(wht)
#define wht_error(msg) { fprintf(stderr, "error, %s\n", msg); \
  exit(-1); }

#ifdef DDL_ON
void transpose(wht_value *xx, int n, int n1);
void transpose_stride(wht_value *xx, int n, int n1, long S);
#endif

#define wht_apply_4_para(wht, S, D, x) ((wht->apply)(wht, S, D, x))

#ifdef IL_ON

#ifdef IL_LEVEL1
void apply_il2_small1(Wht *W, long S, long D, wht_value *x);
void apply_il2_small2(Wht *W, long S, long D, wht_value *x);
void apply_il2_small3(Wht *W, long S, long D, wht_value *x);
void apply_il2_small4(Wht *W, long S, long D, wht_value *x);
void apply_il2_small5(Wht *W, long S, long D, wht_value *x);
void apply_il2_small6(Wht *W, long S, long D, wht_value *x);
void apply_il2_small7(Wht *W, long S, long D, wht_value *x);
#endif

#ifdef IL_LEVEL2
void apply_il4_small1(Wht *W, long S, long D, wht_value *x);
void apply_il4_small2(Wht *W, long S, long D, wht_value *x);
void apply_il4_small3(Wht *W, long S, long D, wht_value *x);
void apply_il4_small4(Wht *W, long S, long D, wht_value *x);
void apply_il4_small5(Wht *W, long S, long D, wht_value *x);
void apply_il4_small6(Wht *W, long S, long D, wht_value *x);
#endif

#ifdef IL_LEVEL3
void apply_il8_small1(Wht *W, long S, long D, wht_value *x);
void apply_il8_small2(Wht *W, long S, long D, wht_value *x);
void apply_il8_small3(Wht *W, long S, long D, wht_value *x);
void apply_il8_small4(Wht *W, long S, long D, wht_value *x);
void apply_il8_small5(Wht *W, long S, long D, wht_value *x);
#endif

#ifdef IL_LEVEL4
void apply_il16_small1(Wht *W, long S, long D, wht_value *x);
void apply_il16_small2(Wht *W, long S, long D, wht_value *x);
void apply_il16_small3(Wht *W, long S, long D, wht_value *x);
void apply_il16_small4(Wht *W, long S, long D, wht_value *x);
#endif

#ifdef IL_LEVEL5
void apply_il32_small1(Wht *W, long S, long D, wht_value *x);
void apply_il32_small2(Wht *W, long S, long D, wht_value *x);
void apply_il32_small3(Wht *W, long S, long D, wht_value *x);
#endif

#endif

#ifdef PARA_ON
void p_transpose(wht_value *xx, int n, int n1, int pll);
void p_transpose_stride(wht_value *xx, int n, int n1, long S, int pll);
#endif

extern Wht *wht_parse(char *in);
extern char *wht_print(int indstep, Wht *W);

Wht * wht_get_tree(int n);
/* Given the log (base 2) of the size of the matrix, returns
  the fastest WHT tree for computing it found on this system, if
  a tree of this size has been found.  Returns NULL on failure --
  either the tree is not in the file, not properly specified, or
  the table of trees does not exist. */

#ifdef PARA_ON
Wht * wht_get_p_tree(int n, int threadnum);
/* Given the log (base 2) of the size of the matrix, returns
  the fastest WHT tree for computing it found on this system with 
  the number of thread, if a tree of this size has been found.  
  Returns NULL on failure -- either the tree is not in the file, 
  not properly specified, or  the table of trees does not exist. */
#endif

double wht_get_tree_time(int n);
/* Gets the time required to do WHT using the fastest tree of size n.
  Returns -1 if the file is not found, no tree of this size is in the
  table, or there is no time associated with a tree of this size. */

int wht_add_tree(Wht * tree, double time);
/* Updates the tree table, adding this tree, and removing the previous one
  of this size if it exists.  It does not check to make sure that this
  new tree is faster than the old one.  Returns 0 on failure, 1 on success.
  */

#endif /* !defined(WHT_H) */
