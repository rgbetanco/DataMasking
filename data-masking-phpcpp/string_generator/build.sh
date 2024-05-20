#!/bin/bash
cd data-masking/string_generator

yacc -d word_parser.y
lex --header-file=lex.yy.h word_parser.l
yacc -d --file-prefix=tree_parser_yacc --name-prefix=treeB_ tree_parser.y
lex --header-file=tree_parser_lex.h --outfile=tree_parser_lex.c --prefix=treeB_ tree_parser.l
# start to compile
gcc -fPIC -c -o obj/lex.yy.o lex.yy.c
gcc -fPIC -c -o obj/y.tab.o y.tab.c
gcc -fPIC -c -o obj/tree_parser_lex.o tree_parser_lex.c
gcc -fPIC -c -o obj/tree_parser_yacc.tab.o tree_parser_yacc.tab.c
gcc -fPIC -c -o obj/queue.o queue.c
gcc -fPIC -c -o obj/treebuilder.o treebuilder.c
gcc -fPIC -c -o obj/string_builder.o string_builder.c
gcc -fPIC -c -o obj/chinese_generator.o chinese_generator/chinese_generator.c
