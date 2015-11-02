#!/usr/bin/env python

# Script for running RadarQC
#
# Example
#-------------
#
# 	$ ./runapp.py c03/leg16 onshore
#
# First argument is the directory where swp are located.
# Second argument choose a cfac file
#
# Raul Valenzuela
# July, 2015



from os.path import expanduser
from glob import glob
from datetime import datetime

import os
import platform
import sys

startime=datetime.now().strftime('%H:%M:%S')

""" define home directory """
home = expanduser("~")

""" get system information """
uname = platform.uname()
machine=platform.machine()

""" setup for 64-bit """
if machine=='x86_64':
	os.environ['QMAKESPEC']=home+'/miniconda/mkspecs/linux-g++-64'	
else:
	os.environ['QMAKESPEC']=home+'/miniconda/mkspecs/linux-g++-32'	


print "\nCompiling radarqc"
print "-------------------------------"
if glob('*.o'):
	for f in glob('*.o'):
		os.remove(f)
if uname[1] == 'psdapp.psd.esrl.noaa.gov':
	qt_project='AirborneRadarQC1.pro'
else:
	qt_project='AirborneRadarQC2.pro'
run_qmake='qmake -o Makefile '+qt_project
os.system(run_qmake)
os.system('make')


print "\nRunning radarqc"
print "-------------------------------"
# DTMFILE='/home/dkingsmill/Aircraft_NavCor_QC/geotiff_merge-master/merged_dem_127to121W_35to41N.tif'
DTMFILE=home+'/Github/RadarQC/merged_dem_38-39_123-124_extended.tif'
# DTMFILE=home+'/Github/RadarQC/merged_dem_127to121W_35to41N.tif'
INDIR=home+'/P3_v2/dorade/'+sys.argv[1]+'_cor'
LEGTYPE=sys.argv[2]
if LEGTYPE=='onshore':
	CFACDIR=home+'/P3_v2/cfac/c03/l03_r07_galt_ve'	
elif LEGTYPE=='offshore':
	CFACDIR=home+'/P3_v2/cfac/c03/l01_r07_galt_ve'	
OUTDIR=home+'/P3_v2/qced_prod/'+sys.argv[1]
# OUTDIR=home+'/P3_v2/foo'
print "Input dir: "+INDIR
print "Output dir: "+OUTDIR
run_radarqc='./radarqc '+INDIR+' '+OUTDIR+' '+CFACDIR+' '+DTMFILE+' '+LEGTYPE
var=os.system(run_radarqc)


print "\nRunning times:"
print "-------------------------------"
print ("Started at: %s"% startime)
endtime=datetime.now().strftime('%H:%M:%S')
print ("Finished at: %s"% endtime)

""" remove unnecesary files """
for f in glob('*.o'):
	os.remove(f)
os.remove('radarqc')
os.remove('Makefile')


