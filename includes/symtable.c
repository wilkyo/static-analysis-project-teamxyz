#include "symtable.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

const int DUPLICATE_SYM = -1;

const int ILLEGAL_SCOPE = -2;

const int NO_RETVAL = -3;

static const size_t MULTIPLIER = 37;

static const size_t HASHTABLE_INITSZ = 16;

static const double LOAD_FACTOR = 0.75;

typedef struct var_sym_s {
  char *id;
  int reg;
  var_type t;
} var_sym;

typedef struct fun_sym_s {
  char *id;
  int frame;
  var_type rett;
  int argc;
  var_type *argt; 
} fun_sym;

enum sym_type {
  FUN,
  VAR
};

typedef struct {
  union {
    var_sym var;
    fun_sym fun;
  } val;
  enum sym_type type;
} symbol;

typedef struct htable_bucket_s {
  char *key;
  symbol *value;
  struct htable_bucket_s *next;
} htable_bucket;

typedef struct {
  int load;
  int buckets_cnt;
  htable_bucket **buckets;
} hashtable;

typedef struct scope_stack_s {
  hashtable *value;
  struct scope_stack_s *next;
} scope_stack;

typedef struct symtable_s {
  scope_stack *scopes;
  hashtable *fun_table;
  int current_depth;
} symtable;

/**************** Freeing operations ******************/

static void free_symbol(symbol *s) {
  if (s->type == FUN) {
    //free(s->val.fun.argt);
    //free(s->val.fun.id);
  } else {
    //free(s->val.var.id);
  }
  //free(s);
}

static void free_bucket(htable_bucket *b) {
  // TODO this function causes segfaults
  if (b != NULL) {
    //free(b->key);
    free_symbol(b->value);
    free_bucket(b->next);
    //free(b);
  }
}

static void empty_hashtable(hashtable *t) {
  int i;
  for (i=0; i < t->buckets_cnt; i++)
    free_bucket(t->buckets[i]);
}

static void free_hashtable(hashtable *t) {
  int i;
  empty_hashtable(t);
  //free(t);
}

void free_symtable(symtable *t) {
  scope_stack *s = t->scopes;
  while(s != NULL) {
    free_hashtable(s->value);
    s = s->next;
  }
  free_hashtable(t->fun_table);
  //free(t->scopes);
  //free(t);
}

/************** Linked list operations ****************/

static scope_stack *insert_head(scope_stack *l, hashtable *e) {
  scope_stack *nl = (scope_stack *) malloc(sizeof(scope_stack));
  nl->value = e;
  nl->next = l;
  return nl;
}

static scope_stack *remove_head(scope_stack *l) {
  scope_stack *nl = l->next;
  free_hashtable(l->value);
  //free(l);
  return nl;
}

/*************** Hashtable operations *****************/

// exported for tests
inline size_t hashvalue(char *key) {
  size_t h = 0;
  unsigned char *p;

  for (p = (unsigned char *) key; *p != '\0'; p++)
    h = MULTIPLIER * h + *p;
  return h;
}

// used for performance tests
int collision(hashtable *h, char *id) {
  size_t hash = hashvalue(id) % h->buckets_cnt;
  if (h->buckets[hash] != NULL)
    return 1;
  return 0;
}

static void init_hashtable(hashtable *h, size_t s) {
  h->load = 0;
  h->buckets_cnt = s;
  h->buckets = (htable_bucket **) malloc(s * sizeof(htable_bucket *));
}

static void resize(hashtable *h) {
  int i, ncount = h->buckets_cnt * 2;
  htable_bucket **nb = (htable_bucket **) malloc(ncount * sizeof(htable_bucket *));
  for (i=0; i < h->buckets_cnt; i++) {
    htable_bucket *b = h->buckets[i];
    while (b != NULL) {
      htable_bucket *tmp = b->next;
      size_t hash = hashvalue(b->key);
      b->next = nb[hash % ncount];
      nb[hash % ncount] = b;
      b = tmp;
    }
  }
  //free(h->buckets);
  h->buckets = nb;
  h->buckets_cnt = ncount;
}


static int insert_ht(hashtable *h, char *key, symbol *value) {
  size_t hash = hashvalue(key) % h->buckets_cnt;
  htable_bucket *nb, *b = h->buckets[hash];
  while (b != NULL) {
    if (strcmp(b->key, key) == 0)
      return DUPLICATE_SYM;
    b = b->next;
  }
  nb = (htable_bucket *) malloc (sizeof(htable_bucket));
  nb->key = (char *) malloc((strlen(key) + 1) * sizeof(char));
  strcpy(nb->key, key);
  nb->value = value;
  nb->next = h->buckets[hash];
  h->buckets[hash] = nb;
  h->load++;
  if ((float)h->load / h->buckets_cnt > LOAD_FACTOR)
    resize(h);
  return 0;
}

static void *lookup(hashtable *h, char *key) {
  size_t hash = hashvalue(key) % h->buckets_cnt;
  htable_bucket *b = h->buckets[hash];
  if (b == NULL)
    return NULL;
  while (b != NULL) {
    if (strcmp(b->key, key) == 0)
      return b->value;
    else
      b = b->next;
  }
  return NULL;
}

/************** Symbol table operations ***************/

symtable *init_symtable(void) {
  symtable *t = (symtable *) malloc(sizeof(symtable));
  t->scopes = (scope_stack *) malloc(sizeof(scope_stack));
  t->scopes->value = malloc(sizeof(hashtable));
  t->fun_table = (hashtable *) malloc(sizeof(hashtable));
  init_hashtable(t->fun_table, HASHTABLE_INITSZ);
  init_hashtable(t->scopes->value, HASHTABLE_INITSZ);
  t->current_depth = 0;
  
  return t;
}

int insert_var(symtable *t, char *id, var_type type) {
  int ret;
  symbol *s = (symbol *) malloc(sizeof(symbol));
  s->type = VAR;
  s->val.var.id = (char *) malloc((strlen(id) + 1) * sizeof(char));
  strcpy(s->val.var.id, id);
  s->val.var.t = type;
  ret = insert_ht(t->scopes->value, id, s);
  if (ret == DUPLICATE_SYM)
    free_symbol(s);
  return ret;
}

static var_type *cpy(int argc, var_type *argt) {
  var_type *dst = (var_type *) malloc(argc * sizeof(var_type));
  int i;
  for (i=0; i<argc; i++)
    dst[i] = argt[i];
  return dst;
}

int insert_proc(symtable *t, char *id, int argc, var_type *argt) {
  int ret;
  symbol *s = (symbol *) malloc(sizeof(symbol));
  s->type = FUN;
  s->val.fun.id = (char *) malloc((strlen(id) + 1) * sizeof(char));
  strcpy(s->val.fun.id, id);
  s->val.fun.argc = argc;
  s->val.fun.argt = cpy(argc, argt);
  s->val.fun.rett = NO_RETVAL;
  ret = insert_ht(t->fun_table, id, s);
  if (ret == DUPLICATE_SYM)
    free_symbol(s);
  return ret;
}

int insert_fun(symtable *t, char *id, var_type rett, int argc, var_type *argt) {
  int ret;
  symbol *s = (symbol *) malloc(sizeof(symbol));
  s->type = FUN;
  s->val.fun.id = (char *) malloc((strlen(id) + 1) * sizeof(char));
  strcpy(s->val.fun.id, id);
  s->val.fun.argc = argc;
  s->val.fun.argt = cpy(argc, argt);
  s->val.fun.rett = rett;
  ret = insert_ht(t->fun_table, id, s);
  if (ret == DUPLICATE_SYM)
    free_symbol(s);
  return ret;
}

var_sym *lookup_var(symtable *t, char *id) {
  symbol *s;
  var_sym *v;
  scope_stack *scope = t->scopes;
  while (scope != NULL) {
    s = lookup(scope->value, id);
    if (s != NULL)
      return &(s->val.var);
    else
      scope = scope->next;
  }
  return NULL;
}

fun_sym *lookup_fun(symtable *t, char *id) {
  symbol *s = lookup(t->fun_table, id);
  return &(s->val.fun);
}

int enter_nested_scope(symtable *t) {
  hashtable *newscope = (hashtable *) malloc(sizeof(hashtable));
  init_hashtable(newscope, HASHTABLE_INITSZ);
  t->scopes = insert_head(t->scopes, newscope);
  return ++(t->current_depth);
}

int exit_nested_scope(symtable *t) {
  if (t->current_depth == 0)
    return ILLEGAL_SCOPE;
  t->scopes = remove_head(t->scopes);
  return --(t->current_depth);
}

void set_var_register(var_sym *s, int reg) {
	s->reg = reg;
}

int get_var_register(var_sym *s) {
	return s->reg;
}

void set_fun_frame(fun_sym *s, int frame) {
	s->frame = frame;
}

int get_fun_frame(fun_sym *s) {
	return s->frame;
}

var_type get_type(var_sym *s) {
  return s->t;
}

int get_args_count(fun_sym *s) {
  return s->argc;
}

var_type *get_args_type(fun_sym *s) {
  return s->argt;
}

int is_function(fun_sym *s) {
  return (s->rett != NO_RETVAL);
}

var_type get_rett(fun_sym *s) {
  return s->rett;
}
