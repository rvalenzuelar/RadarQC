#!/bin/bash

# Raul Valenzuela
# February, 2015

# creates makefile using qmake
echo ' '
echo 'Compiling radarqc ... '
echo ' '
qmake -o Makefile AirborneRadarQC.pro

# build radarqc
make

# set I/O directories
# INDIR="/home/rvalenzuela/P3/dorade/case04"
INDIR='/home/rvalenzuela/P3/dorade/case04_coords_cor'
OUTDIR="/home/rvalenzuela/P3/qced/case04"

# run radarqc
echo ' '
echo 'Running radarqc ... '
echo ' '
./radarqc $INDIR $OUTDIR

# remove intermedite compiling files
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
