# Tentative de Makefile

########################################################################
# MACROS                                                               #
########################################################################

# Exécutables indépendants
YACC=bison
LEX=flex
IMAGE_VIEWER=eog

# Sources
LEX_FILE=lexparser.lex
YACC_FILE=synparser.y
LIBS=includes/node.c includes/dotoutput.c includes/symtable.c
SRC=exemples/f5ex1.src

# Générés
AUTOMATON_FILE=afd.c
SYNTAXIC_ANALYSER=parser
TREE_FILE=ast
TREE_FILE_TYPE=svg

########################################################################
# CIBLES                                                               #
########################################################################


# make sans paramètre compile, lance et affiche l'arbre
all: $(SYNTAXIC_ANALYSER) run show

# make afd.c génère l'automate d'états finis déterministe de Lex
$(AUTOMATON_FILE): $(LEX_FILE)
	@echo "===== "$(LEX)" is running ====="
	flex -o $(AUTOMATON_FILE) $(LEX_FILE)

# make parser.c génère l'analyseur syntaxique de Yacc
$(SYNTAXIC_ANALYSER).c: $(YACC_FILE)
	@echo "===== "$(YACC)" is running ====="
	bison -d -o $(SYNTAXIC_ANALYSER).c $(YACC_FILE)

# make parser compile l'analyseur final
$(SYNTAXIC_ANALYSER): $(AUTOMATON_FILE) $(SYNTAXIC_ANALYSER).c
	@echo "===== Compiling "$(SYNTAXIC_ANALYSER)" ====="
	gcc -o $(SYNTAXIC_ANALYSER) $(LIBS) $(SYNTAXIC_ANALYSER).c $(AUTOMATON_FILE) -DPROD_INTER=0

# make run exécute l'analyseur sur le fichier source
run: $(SYNTAXIC_ANALYSER) $(SRC)
	@echo "===== Running "$(SYNTAXIC_ANALYSER)" ====="
	./$(SYNTAXIC_ANALYSER) $(SRC)

# make ast.svg génère l'image au format svg du fichier dot
$(TREE_FILE).$(TREE_FILE_TYPE): $(TREE_FILE).dot
	@echo "===== Generating image from dot ====="
	dot -T$(TREE_FILE_TYPE) $(TREE_FILE).dot > $(TREE_FILE).$(TREE_FILE_TYPE)

# make show affiche le fichier svg généré
show: $(TREE_FILE).$(TREE_FILE_TYPE)
	@echo "===== Showing tree ====="
	$(IMAGE_VIEWER) $(TREE_FILE).$(TREE_FILE_TYPE) 2> /dev/null &


# make clean nettoie les fichiers générés
clean:
	@echo "===== Cleaning ====="
	rm -f $(SYNTAXIC_ANALYSER) $(SYNTAXIC_ANALYSER).c $(SYNTAXIC_ANALYSER).h $(AUTOMATON_FILE) $(TREE_FILE).dot $(TREE_FILE).$(TREE_FILE_TYPE) &\
	rm -f includes/*~


