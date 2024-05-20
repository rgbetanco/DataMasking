COM_COLOR   = \033[0;34m
OBJ_COLOR   = \033[0;36m
OK_COLOR    = \033[0;32m
ERROR_COLOR = \033[0;31m
WARN_COLOR  = \033[0;33m
NO_COLOR    = \033[m

# -Weffc++ -Werror 
# -Wno-noexcept-type ---- for c++ only
# COMPILER_FLAGS = -Wall -Wextra -Wsign-conversion
# DEBUG_FLAG = -g
LINKER_DEPENDENCIES = -lsodium
DIR = chinese_generator

all:
	@echo "$(OBJ_COLOR)Compiling Data Masking ...$(NO_COLOR)"
	@echo "$(OK_COLOR)Generate y.tab.c y.tab.h$(NO_COLOR)"
	yacc -d word_parser.y
	@echo "$(OK_COLOR)Generate lex.yy.c lex.yy.h$(NO_COLOR)"
	lex --header-file=lex.yy.h word_parser.l
	@echo "$(OK_COLOR)Generate tree_parser_yacc.tab.c tree_parser_yacc.tab.h$(NO_COLOR)"
	yacc -d --file-prefix=tree_parser_yacc --name-prefix=treeB_ tree_parser.y
	@echo "$(OK_COLOR)Generate tree_parser_lex.c tree_parser_lex.h$(NO_COLOR)"
	lex --header-file=tree_parser_lex.h --outfile=tree_parser_lex.c --prefix=treeB_ tree_parser.l
	@echo "$(OK_COLOR)Cross compile all c files$(NO_COLOR)"
	gcc -fPIC ${DEBUG_FLAG} *.c ${DIR}/*.c ${COMPILER_FLAGS} ${LINKER_DEPENDENCIES} -o main.out

clean:
	rm lex.yy.c lex.yy.h y.tab.c y.tab.h *.out tree_parser_*
