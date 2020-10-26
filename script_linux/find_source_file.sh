#! /bin/bash
find . -name *.cpp -o -name *.h |less | xargs -n 1 basename | sort 

