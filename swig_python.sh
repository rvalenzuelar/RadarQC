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
swig -python -c++ -Wall $IFILE


# # determine python interpreter used
# PYINT="$(which python)"

# # parse correct include python directory
# BINDIR=${PYINT: -11} #extract string
# INCDIR=/include/python2.7
# PYPATH=${PYINT/$BINDIR/$INCDIR}


# # include QT directory
# QTPATH=/usr/include/qt5

# # include geotiff directory
# GFPATH=/usr/include/geotiff


# # compile
# g++ -c -fPIC "$MODNAME.cpp" "${MODNAME}_wrap.cxx" "-I$PYPATH" "-I$QTPATH" "-I$GFPATH"

# # link libraries
# OFILES="$(ls *.o)"
# g++ -shared -fPIC -o "_${MODNAME}.so" $OFILES

# # remove unncessary files
# rm *.o *wrap*

qmake -o Makefile AirborneRadarQC.pro

make 

ln -s ./libAirborneRadarQC.so.1.0.0 _AirborneRadarQC.so