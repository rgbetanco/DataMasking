#!/bin/bash
cd data-masking

# start to compile
g++ -fPIC -c -o datamasking.o datamasking.cpp
g++ -fPIC -c -o lookup.o lookup.cpp
