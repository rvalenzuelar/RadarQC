#
# Run AirborneRadarQC by Bell and Wolff (2011)
# using python
#
# Raul Valenzuela
# 2015


import AirborneRadarQC

from PyQt4 import QtCore as Qt 

# from AirborneRadarQC import AirborneRadarQC as QC

inpath = Qt.QString('/home/raul/Radardata/ppisubset')
outpath = Qt.QString('/home/raul/Radardata/ppisubset/QCed')



# QString inpath = argv[1];
# QString outpath = argv[2];

# # // QString inpath = "/home/raul/Radardata/p3/case04";
# # // QString outpath = "/home/raul/Radardata/p3/case04/QCed";

suffix = Qt.QString('QC')

# QC(inpath, outpath, suffix);

# create object
dum=AirborneRadarQC.AirborneRadarQC(inpath,outpath,suffix)

# # // Process the data
# QC.processSweeps("ground");
# # // QC.processSweeps("airborne");

# # // QC.exportVad("V3");

