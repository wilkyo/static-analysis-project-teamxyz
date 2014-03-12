#ifndef SYMTABLE_H
#define SYMTABLE_H

extern const int DUPLICATE_SYM;

extern const int ILLEGAL_SCOPE;

extern const int NO_RETVAL;

typedef enum {
  INTEGER,
  DECIMAL,
  INT_ARRAY,
  DEC_ARRAY
} var_type;

typedef struct symtable_s symtable;

typedef struct var_sym_s var_sym;

typedef struct fun_sym_s fun_sym;

/**
 * \fn symtable *init_symtable(void)
 * \brief Initiates an empty symbol table
 *
 * \return a pointer to an empty symbol table
 */
symtable *init_symtable(void);

/**
 * \fn void free_symtable(symtable *t)
 * \fn Frees the memory allocated to the symbol table and all of its content
 *
 * \param t the symbol table to be destroyed
 */
void free_symtable(symtable *t);

/**
 * \fn int insert_var(symtable *t, char *id, var_type type)
 * \brief Inserts a variable symbol in the symbol t
 *
 * Multiple symbols with the same identifier can be be inserted, provided that
 * they are located in different scopes. The symbol is inserted in the current
 * scope. A copy of the identifier is stored instead of the provided pointer.
 *
 * \param t the symbol table in which to insert the symbol
 * \param id the variable identifier
 * \param type
 * \return 0 in case of success, DUP_SYM if a similar symbol is already present
 * in the table
 */
int insert_var(symtable *t, char *id, var_type type);

/**
 * \fn int insert_proc(symtable *t, char *id, int argc, var_type *argt)
 * \brief Inserts a procedure identifier in the symbol table
 *
 * Unlike variables, functions and procedure can only be declared globally,
 * therefore they all must be declared with different identifiers. A copy of
 * the identifier stored instead of the provided pointer.
 *
 * \param t the symbol table in which to insert the symbol
 * \param id the procedure identifier
 * \param argc the number of arguments required by the procedure
 * \param argt the types of the arguments
 * \return 0 in case of success, DUPLICATE_SYM if a similar symbol is already
 * present in the table
 */
int insert_proc(symtable *t, char *id, int argc, var_type *argt);

/**
 * \fn int insert_fun(symtable *t, char *id, var_type rett, int argc, var_type *argt)
 * \brief Inserts a function identifier in the symbol table
 *
 * Unlike variables, functions and procedure can only be declared globally,
 * therefore they all must be declared with different identifiers. Copies of
 * the identifier and argt array are stored
 *
 * \param t the symbol table in which to insert the symbol
 * \param id the function identifier
 * \param argc the number of arguments required by the function
 * \param argt the types of the arguments
 * \return 0 in case of success, DUPLICATE_SYM if a similar symbol is already
 * present in the table
 */
int insert_fun(symtable *t, char *id, var_type rett, int argc, var_type *argt);

/**
 * \fn var_sym *lookup_var(symtable *t, char *id)
 * \brief Retrieves a variable symbol bound to the identifier id
 *
 * If multiple variables with the same identifier are present in the symbol
 * table, the variable retrieve is the one located in the innermost nested
 * scope. If no variable matches this identifier in any scope, this function
 * returns NULL
 *
 * \param t the symbol table to be searched for
 * \param id the variable identifier
 * \return a variable symbol, or NULL if no symbol matches the identifier
 */
var_sym *lookup_var(symtable *t, char *id);

/**
 * \fn fun_sym *lookup_fun(symtable *t, char *id)
 * \brief Retrieves a function symbol bound to the identifier id
 *
 * \param t the symbol table to be searched for
 * \param id the function identifier
 * \return a function symbol, or NULL if no symbol matches the identifier
 */
fun_sym *lookup_fun(symtable *t, char *id);

/**
 * \fn Creates a new scope, and make it the current scope
 *
 * \return the depth of the newly created scope. 0 is global scope, 1 the first
 * level of nested scope, etc.
 */
int enter_nested_scope(symtable *t);

/**
 * \fn int exit_nested_scope(symtable *t)
 * \brief Changes the current scope to the one outside the current scope
 *
 * \return the depth of the current scope, or ILLEGAL_SCOPE if this function is
 * called while the current scope is the global scope
 */
int exit_nested_scope(symtable *t);

/**
 * \fn var_type get_type(var_sym *s)
 * \brief returns the type of the variable bound to the symbol
 *
 * \param s the symbol
 * \return the type of the variable bound to the symbol
 */
var_type get_type(var_sym *s);

/**
 * \fn int get_args_count(fun_sym *s)
 * \brief returns the number of arguments required by a function or procedure
 *
 * \param s the function symbol
 * \return the number of arguments required by the function or procedure
 */
int get_args_count(fun_sym *s);

/**
 * \fn var_type *get_args_type(fun_sym *s)
 * \brief returns an array of the types of the arguments required by the
 * function bound to the symbol s
 *
 * Use get_args_count(s) to get the size of the array
 *
 * \param s the function symbol
 * \return a pointer to an array of variable type
 */
var_type *get_args_type(fun_sym *s);

/**
 * \fn int is_function(fun_sym *s)
 * \brief returns 1 if the function symbol is bound to a function (i.e. not a
 * procedure)
 *
 * \param s the function symbol
 * \return 1 if the function symbol is bound to a function, 0 if it is bound to
 * a procedure
 */
int is_function(fun_sym *s);

/**
 * \fn var_type get_rett(fun_sym *s)
 * \brief returns the type of the function bound to a symbol s
 *
 * \param s the function symbol
 * \return the type of the function bound to a symbol s, or NO_RETVAL if the
 * symbol is bound to a procedure
 */
var_type get_rett(fun_sym *s);

void set_var_register(var_sym *s, int reg);

int get_var_register(var_sym *s);

void set_fun_frame(fun_sym *s, int frame);

int get_fun_frame(fun_sym *s);

#endif
