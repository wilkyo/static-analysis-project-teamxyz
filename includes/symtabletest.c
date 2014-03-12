#include "symtable.h"

#include <stdio.h>

void assert_equal(char *desc, int expected, int actual) {
  if (expected == actual)
    printf("%s: OK\n", desc);
  else
    printf("%s: failed!!!!\tExpected %d but got %d\n", desc, expected, actual);
}

// redefined here only to be able to access fun_table
typedef struct symtable_s {
  struct scope_stack *scopes;
  struct hashtable_s *fun_table;
  int current_depth;
} symtable;

void hashtable_test(void) {
  int i, collisions = 0, lost = 0;
  symtable *t = init_symtable();
  char id [32];
  for (i=0; i<1000; i++) {
    int hash;
    sprintf(id, "ab_ident%d", i);
    collisions += collision(t->fun_table, id);
    insert_proc(t, id, 0, NULL);
  }
  for(i=0; i<1000; i++) {
    sprintf(id, "ab_ident%d", i);
    if (lookup_fun(t, id) == NULL)
      lost++;
  }
  free_symtable(t);
  printf("Hashtable peformance test\n");
  printf("%d collisions after 1000 insertions\n", collisions);
  printf("%d elements lost\n", lost);
}

void main(void) {
  var_type argt[] = {DEC_ARRAY, INTEGER};
  symtable *t = init_symtable();
  // functions and procedure
  assert_equal("insert fun", 0, insert_fun(t, "my_fun1", INTEGER, 2, argt));
  assert_equal("insert proc", 0, insert_proc(t, "my_proc1", 0, NULL));
  assert_equal("failed insert fun", DUPLICATE_SYM, insert_fun(t, "my_fun1", INTEGER, 0, NULL));
  assert_equal("failed insert fun", DUPLICATE_SYM, insert_proc(t, "my_fun1", 0, NULL));
  assert_equal("lookup", 1, is_function(lookup_fun(t, "my_fun1")));
  assert_equal("lookup", 0, is_function(lookup_fun(t, "my_proc1")));
  assert_equal("failed lookup", (int) NULL, (int) lookup_fun(t, "doesnotexist"));
  // variables
  assert_equal("insert", 0, insert_var(t, "my_ident1", INTEGER));
  assert_equal("duplicate", DUPLICATE_SYM, insert_var(t, "my_ident1", DECIMAL));
  assert_equal("illegal scope", ILLEGAL_SCOPE, exit_nested_scope(t));
  assert_equal("insert", 0, insert_var(t, "my_ident2", DECIMAL));
  assert_equal("lookup", INTEGER, get_type(lookup_var(t, "my_ident1")));
  assert_equal("lookup", DECIMAL, get_type(lookup_var(t, "my_ident2")));
  assert_equal("failed lookup", (int) NULL, (int) lookup_var(t, "doesnotexist"));
  assert_equal("in nested scope", 1, enter_nested_scope(t));
  assert_equal("shadowing insert", 0, insert_var(t, "my_ident1", DECIMAL));
  assert_equal("lookup", DECIMAL, get_type(lookup_var(t, "my_ident1")));
  assert_equal("lookup", DECIMAL, get_type(lookup_var(t, "my_ident2")));
  assert_equal("failed lookup", (int) NULL, (int) lookup_var(t, "doesnotexist"));
  assert_equal("in nested scope", 2, enter_nested_scope(t));
  assert_equal("out nested scope", 1, exit_nested_scope(t));
  assert_equal("out nested scope", 0, exit_nested_scope(t));
  assert_equal("illegal scope", ILLEGAL_SCOPE, exit_nested_scope(t));
  free_symtable(t);
  
  hashtable_test();
}
