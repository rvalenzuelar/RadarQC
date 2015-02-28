#!/bin/bash

# Raul Valenzuela
# February, 2015

# creates makefile using qmake
qmake -o Makefile AirborneRadarQC.pro

# build radarqc
make

# set I/O directories
INDIR="/home/raul/Radardata/ppisubset"
OUTDIR="/home/raul/Radardata/ppisubset/QCed"

# run radarqc
./radarqc $INDIR $OUTDIR

# remove intermedite compiling files
rm *.o radarqc Makefile

# changes to output directory
cd $OUTDIR

# convert qced files to dorade to fix issue
# with solo3
echo " "
echo "Running RadxConvert..."
RadxConvert -f swp* -dorade -outdir $OUTDIR

# retrieve RadxConvert output directory
RDXOUT="$(ls -d */)"

# remove radarqc output files to avoid duplicates
rm swp*

# overwrite qced files with washed files
echo "Moving files..."
cd $RDXOUT

# add QC suffix
find . -type f -exec mv '{}' '{}'.QC \;

# move files to QCed directory
mv swp* $OUTDIR

# goes back to QCed directory and removes RadxConvert
# output directory
cd $OUTDIR
rm -r $RDXOUT
echo "done"
echo " "
