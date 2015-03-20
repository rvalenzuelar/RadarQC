RadarQC
=======

AirborneRadarQC code implemented only with C++. Extra functions are being added.

Original code developed by Michael Bell and Cory Wolff (2011)

Need installed:

- Qt
- GeographicLib
- libgeotiff

Add link path to environment variable using .bashrc:

- LD_LIBRARY_PATH=/usr/local/lib

Functions added:

- Thresholding with an AND operation
- Dump a VAD text file
- Use RadxConvert (in bash script) to remove issues with Solo3

Folder has to have cfac files to apply them.