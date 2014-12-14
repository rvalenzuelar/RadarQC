#!/bin/bash

qmake -o Makefile AirborneRadarQC.pro

make

./radarqc

rm *.o radarqc Makefile
