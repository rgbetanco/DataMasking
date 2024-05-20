# -Weffc++
COMPILER_FLAGS = -Wall -Wextra -Wsign-conversion -Werror -Wno-noexcept-type -std=c++11
LINKER_DEPENDENCIES = -lre2 -lsodium `mysql_config --cflags --libs`
GEN_DIR = string_generator
MYSQL_DIR = db_connect/mysql
OBJS = $(GEN_DIR)/obj/*.o $(MYSQL_DIR)/obj/*.o

all:
	bash $(MYSQL_DIR)/build.sh
	bash $(GEN_DIR)/build.sh
	g++ -fPIC *.cpp ${OBJS} ${COMPILER_FLAGS} ${LINKER_DEPENDENCIES} -o main.out

clean:
	rm *.out $(GEN_DIR)/lex.yy.* $(GEN_DIR)/y.tab.* $(GEN_DIR)/tree_parser_* ${OBJS}
