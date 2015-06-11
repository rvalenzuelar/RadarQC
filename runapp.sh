#!/bin/bash

# Raul Valenzuela
# February, 2015

# REMEMBER COMMENT/UNCOMMENT LINES FOR
# RUNNING ON NOAA_LINUX AND LAPTOP_LINUX

# NOTES:
#	- For Soloii edited sweep files use pragma pack (4) in read_dorade.h
#	- For Solo3 edited sweep files use pragma pack (8) in read_dorade.h
#	- After using RadxConvert, "Seek Error" can be solved by copying a field (dummy var) using Soloii/Solo3
#	- For some reason, changes due to navigation correction are visualized only in Solo3. Opening QCed files with
#	  Soloii would give the impression of no nav correction. However, running any editing comman will updated 
#	  the sweep files and deploy the data with the correct navigation.

# creates makefile using qmake
#----------------------------------------------
echo ' '
echo 'Compiling radarqc ... '
echo ' '
# using miniconda Qt
QMAKESPEC=~/miniconda/mkspecs/linux-g++-64 # for noaa linux
# QMAKESPEC=~/miniconda/mkspecs/linux-g++-32 # for laptop linux
export QMAKESPEC
qmake -o Makefile AirborneRadarQC.pro

# build radarqc
#----------------------
make

# set I/O directories
#----------------------------------------------
# # INDIR="/home/rvalenzuela/P3/dorade/case04_all/" # <--form original uncompressed files it works
# INDIR="/home/rvalenzuela/P3/dorade/case04" # <--after copying from case04_all it works
#  # INDIR="/home/rvalenzuela/P3/dorade/case04_coords_cor" # <-- Seek Error..aborting..
# OUTDIR="/home/rvalenzuela/P3/qced_test/case04"
# CFACDIR="/home/rvalenzuela/Github/RadarQC/cfac_case04"


# INDIR="$HOME/P3/dorade/case03_coords_cor" 
# OUTDIR="$HOME/P3/qced_test/case03/leg01"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg01"

# INDIR="$HOME/P3/dorade/case03_coords_cor/leg03" 
# OUTDIR="$HOME/P3/qced_test/case03/leg03"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg03"

# INDIR="$HOME/P3/dorade/case03_coords_cor/leg02" 
# OUTDIR="$HOME/P3/qced_test/case03/leg02"
# # CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg02"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg03"

# INDIR="$HOME//P3/dorade/case03_all/leg01" 
# OUTDIR="$HOME/P3/qced_test/velocity"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg02"

# INDIR="$HOME/P3/dorade/case03_coords_cor/leg02" 
# OUTDIR="$HOME/P3/qced_test/case03/leg02"
# # CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg02"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg03"

# INDIR="$HOME/P3/dorade/case03_coords_cor/leg03_new" 
# OUTDIR="$HOME/P3/qced_test/case03/leg03_new"
# CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg03_new"
# # CFACDIR="$HOME/Github/RadarQC/cfac_case03/leg03"


# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_cfac_leg01_run01"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg01_run01"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/l01_leg03run02"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run02"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/l03_leg03run02"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run02"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/l03_leg03run02"
# # CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run02_altcor"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run02"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/l03_leg03run03"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run03"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/l03_leg03run01"
# # CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run01"
# CFACDIR="$HOME/P3_v2/cfac/c03/leg03_run01_altcor"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r06_galt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r06_galt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r05_galt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r05_galt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r02_galt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r02_galt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r05_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r05_galt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r03_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r03_galt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r02_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r02_galt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r01_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r01_galt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r06_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r06_galt_ve"


# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r01_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r01_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r02_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r02_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r03_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r03_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r04_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r04_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r05_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r05_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r06_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r06_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r07_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r07_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r08_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r08_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg03_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg03_l03_r07_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l03_r07_galt_ve"
# LEGTYPE="onshore"

# ---------- LEG 01 -----------------

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r01_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r01_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r02_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r02_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r03_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r03_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r04_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r04_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r05_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r05_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r06_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r06_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r07_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r07_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r08_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r08_palt_ve"


# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r01_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r01_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r02_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r02_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r03_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r03_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r04_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r04_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r05_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r05_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r06_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r06_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r07_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r07_palt_vg"

# INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r08_palt_vg"
# CFACDIR="$HOME/P3_v2/cfac/c03/l01_r08_palt_vg"

INDIR="$HOME/P3_v2/dorade/c03/leg01_cor" 
OUTDIR="$HOME/P3_v2/qced_test/c03/leg01_l01_r07_galt_ve"
CFACDIR="$HOME/P3_v2/cfac/c03/l01_r07_galt_ve"
LEGTYPE="offshore"


# ---------- LEG 05 -----------------

# INDIR="$HOME/P3_v2/dorade/c03/leg05_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg05_l05_r07_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l05_r07_palt_ve"

# INDIR="$HOME/P3_v2/dorade/c03/leg05_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg05_l05_r07_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l05_r07_galt_ve"

# ---------- LEG 14 -----------------

# INDIR="$HOME/P3_v2/dorade/c03/leg14_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c03/leg14_l14_r07_palt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c03/l14_r07_palt_ve"


# ---------- C04 - LEG 10 -----------------

# INDIR="$HOME/P3_v2/dorade/c04/leg10_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c04/leg10_l10_r07_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c04/l10_r07_galt_ve"

# ---------- C04 - LEG 11 -----------------

# INDIR="$HOME/P3_v2/dorade/c04/leg11_cor" 
# OUTDIR="$HOME/P3_v2/qced_test/c04/leg11_l11_r07_galt_ve"
# CFACDIR="$HOME/P3_v2/cfac/c04/l11_r07_galt_ve"


# set DTM file
#---------------------
#  over ocean comment next line
DTMFILE="$HOME/Github/RadarQC/merged_dem_38-39_123-124_extended.tif"

# run radarqc
#------------------
echo ' '
echo 'Running radarqc ... '
echo ' '
./radarqc $INDIR $OUTDIR $CFACDIR $DTMFILE $LEGTYPE

# remove intermedite compiling files and executable
#--------------------------------------------------------------------------
rm *.o radarqc Makefile 




# # changes to output directory
# cd $OUTDIR

# # convert qced files to dorade to fix issue
# # with solo3
# echo " "
# echo "Cleaning files with RadxConvert..."
# RadxConvert -f swp* -dorade -outdir $OUTDIR

# # retrieve RadxConvert output directory
# RDXOUT="$(ls -d */)"

# # remove radarqc output files to avoid duplicates
# rm swp*

# # changes to RadxConvert output directory
# cd $RDXOUT

# # add QC suffix
# find $RDXOUT -type f -exec mv '{}' '{}'.QC \;

# # move files to QCed directory
# echo "Moving cleaned files to $OUTDIR"
# mv swp* $OUTDIR

# # goes back to QCed directory and removes RadxConvert
# # output directory
# cd $OUTDIR
# rm -r $RDXOUT
# echo "done"
# echo " "

