/* AirborneRadarQC */
/* Copyright 2011 Michael Bell and Cory Wolff */
/* All rights reserved */

#include <iostream>
#include <QApplication>
#include "./ext/QCscript/Dorade.h"
#include "./ext/QCscript/AirborneRadarQC.h"

using namespace std;

int main () {

	// The qc object will read from one directory and write to another
	QString inpath = "/home/raul/Radardata/ppisubset";
	QString outpath = "/home/raul/Radardata/ppisubset/QCed";

	// QString inpath = "/home/raul/Radardata/p3/case04";
	// QString outpath = "/home/raul/Radardata/p3/case04/QCed";

	QString suffix = "QC";
	AirborneRadarQC QC(inpath, outpath, suffix);

	// Process the data
	QC.processSweeps("ground");
	// QC.processSweeps("airborne");

	QC.exportVad("V3");

	return 0;

}
