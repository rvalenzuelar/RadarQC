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

	QString suffix = "QC";
	AirborneRadarQC QC(inpath, outpath, suffix);

	// Process the data
	// QC.processSweeps("ground");
	QC.processSweeps("airborne");

	// QC.exportVad("V3");

	return 0;

}
