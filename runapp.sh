#!/bin/bash

# Raul Valenzuela
# February, 2015

# creates makefile using qmake
#----------------------------------------------
echo ' '
echo 'Compiling radarqc ... '
echo ' '
# using miniconda Qt
QMAKESPEC=~/miniconda/mkspecs/linux-g++-64
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
# OUTDIR="/home/rvalenzuela/P3/qced/case04"
# CFACDIR="/home/rvalenzuela/Github/RadarQC/cfac_case04"


INDIR="/home/rvalenzuela/P3/dorade/case03/leg01" # <--form original uncompressed files it works
# INDIR="/home/rvalenzuela/P3/dorade/case03_coords_cor" # Seek Error
OUTDIR="/home/rvalenzuela/P3/qced/case03/leg01"
CFACDIR="/home/rvalenzuela/Github/RadarQC/cfac_case03"


# set DTM file
#---------------------
DTMFILE="/home/rvalenzuela/Github/RadarQC/merged_dem_38-39_123-124_extended.tif"

# run radarqc
#------------------
echo ' '
echo 'Running radarqc ... '
echo ' '
./radarqc $INDIR $OUTDIR $CFACDIR $DTMFILE

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

