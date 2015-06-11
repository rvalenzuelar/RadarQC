/* AirborneRadarQC */
/* Copyright 2011 Michael Bell and Cory Wolff */
/* All rights reserved */

#include <iostream>
#include <QApplication>
#include "./Dorade.h"
#include "./AirborneRadarQC.h"

using namespace std;

int main (int argc, char *argv[]) {

	// The qc object will read from one directory and write to another
	QString inpath = argv[1];
	QString outpath = argv[2];
	QString cfacpath = argv[3];
	QString dtmFile = argv[4];
	QString legType = argv[5];

	QString suffix = "QC";
	AirborneRadarQC QC(inpath, outpath, cfacpath, suffix,dtmFile,legType);

	// Process the data
	// QC.processSweeps("ground");
	QC.processSweeps("airborne");

	// QC.exportVad("V3");

	return 0;

}
