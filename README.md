RadarQC
=======

AirborneRadarQC code implemented with C++ and python via swig wrapper.

Original code developed by Michael Bell and Cory Wolff (2011)

Need installed:

- Qt
- GeographicLib
- libgeotiff
- swig
- PyQt5 (SIP needs to be installed before PyQt)

Add link path to environment variable using .bashrc:

- LD_LIBRARY_PATH=/usr/local/lib

Functions added:

- Thresholding with an AND operation
- Dump a VAD text file
- Use RadxConvert (in bash script) to remove issues with Solo3
