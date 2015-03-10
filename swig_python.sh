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
swig -python -c++ $IFILE

# determine python interpreter used
PYINT="$(which python)"

# parse correct include python directory
BINDIR=${PYINT: -11} #extract string
PYDIR=/include/python2.7
PYPATH=${PYINT/$BINDIR/$INCDIR}

# include QT directory
QTPATH=/usr/include/qt5

# include geotiff directory
GFPATH=/usr/include/geotiff

# compile
g++ -c -fPIC "$MODNAME.cpp" "${MODNAME}_wrap.cxx" "-I$PYPATH" "-I$QTPATH" "-I$GFPATH"

# link libraries
OFILES="$(ls *.o)"
g++ -shared -fPIC $OFILES -o "_${MODNAME}.so"

# remove unncessary files
rm *.o *wrap.c