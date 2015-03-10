#!/bin/bash

# Compiles and export C module to python 
# using SWIG
# 
# Raul Valenzuela
# 2015

# locate interface file name
IFILE="$(ls *.i)"

# parse module name
MODNAME=${IFILE%.i}

# runs swig for python with the inteface file
swig -python $IFILE

# determine python interpreter used
PYINT="$(which python)"

# parse correct include directory
BINDIR=${PYINT: -11} #extract string
INCDIR=/include/python2.7
INCPATH=${PYINT/$BINDIR/$INCDIR}

# compile
gcc -c "$MODNAME.cpp" "${MODNAME}_wrap.c" "-I$INCPATH" 

# link libraries
OFILES="$(ls *.o)"
ld -shared $OFILES -o "_${MODNAME}.so"

# remove unncessary files
rm *.o *wrap.c
