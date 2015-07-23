#!/usr/bin/env python

# Script for running RadarQC
#
# Raul Valenzuela
# July, 2015

from os.path import expanduser
from glob import glob
from datetime import datetime

import os
import platform

home = expanduser("~")


if platform.machine()=='x86_64':
	os.environ['QMAKESPEC']=home+'/miniconda/mkspecs/linux-g++-64'	
else:
	os.environ['QMAKESPEC']=home+'/miniconda/mkspecs/linux-g++-32'	


print "\nCompiling radarqc"
print "-------------------------------"

if glob('*.o'):
	for f in glob('*.o'):
		os.remove(f)
run_qmake='qmake -o Makefile AirborneRadarQC.pro'
os.system(run_qmake)
os.system('make')


print "\nRunning radarqc"
print "-------------------------------"

DTMFILE=home+'/Github/RadarQC/merged_dem_38-39_123-124_extended.tif'

INDIR=home+'/P3_v2/dorade/c03/leg14_cor'
CFACDIR=home+'/P3_v2/cfac/c03/l03_r07_galt_ve'
LEGTYPE='onshore'
OUTDIR=home+'/P3_v2/qced_prod/c03/leg14'

run_radarqc='./radarqc '+INDIR+' '+OUTDIR+' '+CFACDIR+' '+DTMFILE+' '+LEGTYPE
var=os.system(run_radarqc)

print "\nFinished at:"
print datetime.now().strftime('%H:%M:%S')



for f in glob('*.o'):
	os.remove(f)
os.remove('radarqc')
os.remove('Makefile')


