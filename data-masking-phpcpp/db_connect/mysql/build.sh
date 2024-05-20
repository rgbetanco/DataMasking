#!/bin/bash
cd data-masking/db_connect/mysql

# start to compile
g++ -fPIC -c -o obj/connect.o connect.cpp `mysql_config --cflags --libs`
