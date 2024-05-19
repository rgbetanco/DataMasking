#!/bin/bash
cd db_connect/mysql

# start to compile
g++ -c -o obj/connect.o connect.cpp `mysql_config --cflags --libs`