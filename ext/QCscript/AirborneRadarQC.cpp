/*
 *  AirborneRadarQC.cpp
 *
 *  Copyright 2011 Michael Bell and Cory Wolff
 *  All rights reserved.
 *
 */

#include "AirborneRadarQC.h"
#include "RecursiveFilter.h"
#include "DEM.h"
#include <GeographicLib/TransverseMercator.hpp>
#include <iterator>
#include <fstream>
#include <iostream>
#include <QFile>
#include <QTextStream>

using namespace std;
using namespace GeographicLib;

AirborneRadarQC::AirborneRadarQC(const QString& in, const QString& out, const QString& suffix)
{

	// Setup the data path
	dataPath.setPath(in);
	outPath.setPath(out);
	swpSuffix = suffix;
	readSwpDir();

}

AirborneRadarQC::AirborneRadarQC()
{
	// Setup the data path
	dataPath.setPath(".");
	outPath.setPath(".");
	swpSuffix = "QC";
}

AirborneRadarQC::~AirborneRadarQC()
{
}

QString AirborneRadarQC::getInputPath()
{
	return dataPath.absolutePath();
}

bool AirborneRadarQC::setInputPath(const QString& in)
{
	dataPath.setPath(in);
	if (dataPath.isReadable()) {
		readSwpDir();
		return true;
	}
	return false;
}

QString AirborneRadarQC::getOutputPath()
{
	return outPath.absolutePath();
}

bool AirborneRadarQC::setOutputPath(const QString& out)
{
	outPath.setPath(out);
	if (outPath.exists()) return true;
	return false;
}

// This is the main driver for the data processing
bool AirborneRadarQC::processSweeps(const QString& typeQC)
{

	// Do some QC
	if (!getfileListsize()) {
		std::cout << "No swp files exist in " << dataPath.dirName().toStdString() << "\n";
		return false;
	}

	for (int f = 0; f < getfileListsize(); ++f) {
	//for (int f = 0; f < 10; ++f) {
		if (load(f)) {

			std::cout << "Processing " << swpfile.getFilename().toStdString() << "\n";

			if (typeQC == "ground"){

				/* syntax: copyField("oldField",'newField')*/
				 copyField("VV", "V3");

				/* syntax: thresholdData("targetField","criteriaField","criteria",criteriaValue,)*/
				thresholdData("V3","CC","below", 0.85);
				thresholdData("V3","DC","above",1.8);
				thresholdData("V3","W0","above",30.);
				thresholdData("V3","W0","below",0.);
				/* note: it gives similar results to editing in Solo3 but is 
						more agressive than thresholdDataAND*/

				/* syntax:thresholdDataAND("targetField","criteriaField1", "direction1", "thresholds1",
															"criteriaField2", "direction2", "thresholds2",
															"criteriaField3", "direction3", "thresholds3")*/
				// directions "below" and "above" use only first value of threshold
				// float threshold1 [2] = {0.85,0};
				// float threshold2 [2] = {1.8,0.0};
				// float threshold3 [2] = {0.,30.};
				// thresholdDataAND("V3","CC","below", threshold1,
				// 						 "DC","above", threshold2,
				// 						 "W0","outside", threshold3);

				/* syntax: despeckleRadial("targetField", #gates)*/
				// despeckleRadial("V3", 3);
				// despeckleAzimuthal("V3", 3);

				/* syntax: histogram("targetField", binMin, binMax, binInterval, fileNumber)*/
        		// histogram("VE", -20, 20, 1,f);

				/* Write it back out*/
			  	saveQCedSwp(f);

			  	/* syntax: exportVad("targetField",NyquistVel)*/
				exportVad("V3",f);
				/* note: it has to be after saveQCedSwp, otherwise
						creates a Segmentation Fault error when 
						click over data in solo3*/

			}
			else {


			// Use these to apply navigation corrections
			// setNavigationCorrections("rf12.cfac.aft", "TA-ELDR");
			// setNavigationCorrections("rf12.cfac.fore", "TF-ELDR");
			 setNavigationCorrections("cfac.aft", "TA43P3");
			 setNavigationCorrections("cfac.fore", "TF43P3");

			// Make a backup of the original dBZ
			// syntax: copyField("oldField",'newField')
			 copyField("DZ", "ZBK");


			// removeAircraftMotion("VR", "VG");

			// thresholdData("NCP", "VG", 0.2, "below");

			//Flat terrain
			// probGroundGates("ZZ", "GG", 1.8);
			//probGroundGates("ZBK", "ZG", 2.0);

			// Complex terrain
			//syntax: probGroundGates("originalFieldName","newFieldName",beamWidth,"demFileName")
			//probGroundGates("ZBK", "ZG", 2.0, "ASTGTM2_N38W124_dem.tif"); //<--correct for leg01

			///SW/Z thresholding
			// calcRatio("SW", "ZZ", "SWZ", true);
			// thresholdData("SWZ","VG", 0.6, "above");

			/* Calculate KDP
			despeckleRadial("PHIDP", 2);
			GaussianSmooth("PHIDP", "KDP2", 3);
			calc1stRadialDerivative("KDP2", "KDP3", 2); */

			/* Calculate various gradients
			calcStdDev("VV","VSTD");
			calcLaplacian("VV","VLP");
			calcMixedPartial("VV","VMP");
			calcGradientMagnitude("VV","VGR",2); */

			/* Histograms of QC parameters
            		histogram("NCP", 0.0, 1.0, 0.05);
			histogram("VSD", 0.0, 30.0, 1.0);
            		histogram("SWZ", 0.0, 1.0, 0.05);
            		histogram("VLP", -10.0, 10.0, 1.0);
			histogram("VMP", -10.0, 10.0, 1.0);
			histogram("GG", 0.0, 1.0, 0.05);
			histogram("VGR", 0.0, 20.0, 1.0); */
			// histogram("NCP", 0.0, 1.0, 0.05);

			/* WxProbability
			float weights[7];
			for (int i =0; i<7; i++) weights[i] = 1.0;
			wxProbability("VG","WXP",weights); */

			/* Skill statistics on individual sweeps
			 BrierSkillScore();
			 RelativeOperatingCharacteristic();
			 ReliabilityDiagram();
			 soloiiScriptROC(); */

	 		// Copy edits to reflectivity
			// copyEdits("VG","DBZ");

			// Write it back out
		    saveQCedSwp(f);

			// Dump the data to compare the flight level wind and radar data
			//dumpFLwind();

			// Write out everything to a (big) CSV file
			//writeToCSV();
		    	}
		}
	}

	/* Verification statistics over all sweeps*/
	//QC.verify();
	//QC.soloiiScriptVerification();

	return true;

}

/****************************************************************************************
 ** readSwpDir : This function reads a directory of sweep filenames into a list
 ****************************************************************************************/
bool AirborneRadarQC::readSwpDir()
{
	dataPath.setNameFilters(QStringList("swp.*"));
	dataPath.setFilter(QDir::Files);
	dataPath.setSorting(QDir::Name);
	QStringList filenames = dataPath.entryList();

	// Read in the list sweepfiles
	for (int i = 0; i < filenames.size(); ++i) {
		QStringList fileparts = filenames.at(i).split(".");
		if (fileparts.size() != 6) {
			std::cout << "Warning! Editing previously modified file " << filenames.at(i).toStdString() << "\n";
		}
		swpfileList.append(filenames.at(i));

	}

	if (swpfileList.isEmpty()) {
		return false;
	} else {
		return true;
	}

}

/****************************************************************************************
** load : This function loads the information from an individual swp file given the index.
****************************************************************************************/
bool AirborneRadarQC::load(const int& swpIndex)
{

	QString filename = dataPath.absolutePath() + "/" + getswpfileName(swpIndex);
	swpfile.setFilename(filename);
	std::cout << filename.toStdString() << "\n";
	// Read in the swp file
	if(swpfile.readSwpfile())
		return true;

	return false;

}

/****************************************************************************************
 ** loadAuxSwp : This function loads the information from an individual swp file given a filename
 ****************************************************************************************/
bool AirborneRadarQC::loadAuxSwp(const int& swpIndex)
{
	QString filename = dataPath.absolutePath() + "/" + getswpfileName(swpIndex);
	auxSwpfile.setFilename(filename);
	// Read in the swp file
	if(auxSwpfile.readSwpfile())
		return true;

	return false;

}

/****************************************************************************************
 ** copyAuxField : This function copies an existing field from the auxiliary sweep file
 ** to the current sweep for editing.
 ****************************************************************************************/
bool AirborneRadarQC::copyAuxField(const QString& oldFieldName,const QString& newFieldName,
								   const QString& newFieldDesc,const QString& newFieldUnits)
{
	// First copy an old field to allocate the memory
	if(!swpfile.copyField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) return false;
	for (int i=0; i < swpfile.getNumRays(); i++) {
		// Get the data
		float* data = swpfile.getRayData(i, newFieldName);
		float* aux = auxSwpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			data[n] = aux[n];
		}
	}
	return true;
}

/****************************************************************************************
** save : This function saves a modified swp file to a Dorade file.
****************************************************************************************/
bool AirborneRadarQC::saveDorade(const QString& doradeFilename)
{

	if (swpfile.writeDoradefile(doradeFilename))
		return true;

	return false;
}

/****************************************************************************************
 ** save : This function saves a modified swp file with suffix appended to the end.
 ****************************************************************************************/
bool AirborneRadarQC::saveQCedSwp(const int& swpIndex)
{
	QString qcfilename = outPath.absolutePath() + "/" + getswpfileName(swpIndex) + "." + swpSuffix;
	if (swpfile.writeSwpfile(qcfilename)) return true;

	return false;
}


/****************************************************************************************
** newField : This function copies an existing field to a new field for editing.
****************************************************************************************/
bool AirborneRadarQC::newField(const QString& oldFieldName,const QString& newFieldName,
							   const QString& newFieldDesc,const QString& newFieldUnits)
{

	return swpfile.copyField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits);

}

bool AirborneRadarQC::copyField(const QString& oldFieldName,const QString& newFieldName)
{

	return swpfile.copyField(oldFieldName, newFieldName);

}


/****************************************************************************************
 ** thresholdData : Threshold a field on another field above or below the given value
 ****************************************************************************************/
void AirborneRadarQC::thresholdData(const QString& fldname, const QString& threshfield,
					const QString& direction, const float& threshold)
{
	for (int i=0; i < swpfile.getNumRays(); i++) {
		// Get the data
		float* threshdata = swpfile.getRayData(i, threshfield);
		float* data = swpfile.getRayData(i, fldname);

		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (direction == "below") {
				if (threshdata[n] <= threshold) data[n] = -32768.0;
			} else {
				if (threshdata[n] >= threshold) data[n] = -32768.0;
			}
		}
	}
}

/****************************************************************************************
 ** thresholdDataAND : Threshold a field based on other fields using
 						  an AND operation (RV)
 ****************************************************************************************/
void AirborneRadarQC::thresholdDataAND(const QString& fldname,
	const QString& threshfield1, const QString& direction1, const float threshold1[],
	const QString& threshfield2, const QString& direction2, const float threshold2[],
	const QString& threshfield3, const QString& direction3, const float threshold3[])
{
	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();

	for (int i=0; i < rays; i++) {
		// Get the data
		float* threshdata1 = swpfile.getRayData(i, threshfield1);
		float* threshdata2 = swpfile.getRayData(i, threshfield2);
		float* threshdata3 = swpfile.getRayData(i, threshfield3);
		float* data = swpfile.getRayData(i, fldname);

		// Creates a mask using thresholding
		for (int n=0; n < gates; n++) {

			int mask = 0;

			// field 1
			if (direction1 == "below"){
				if (threshdata1[n] <= threshold1[0]) mask += 1;
			}
			else if (direction1 == "above")	{
				if (threshdata1[n] >= threshold1[0]) mask += 1;
			}
			else if (direction1 == "outside"){
				if (threshdata1[n] <= threshold1[0] && threshdata1[n] >= threshold1[1]) mask += 1;
			}

			// field 2
			if (direction2 == "below"){
				if (threshdata2[n] <= threshold2[0]) mask += 1;
			}
			else if (direction2 == "above")	{
				if (threshdata2[n] >= threshold2[0]) mask += 1;
			}
			else if (direction1 == "outside"){
				if (threshdata2[n] <= threshold2[0] && threshdata2[n] >= threshold2[1]) mask += 1;
			}

			// field 3
			if (direction3 == "below"){
				if (threshdata3[n] <= threshold3[0]) mask += 1;
			}
			else if (direction3 == "above"){
				if (threshdata3[n] >= threshold3[0]) mask += 1;
			}
			else if (direction1 == "outside"){
				if (threshdata3[n] <= threshold3[0] && threshdata3[n] >= threshold3[1]) mask += 1;
			}

			// it has to pass all the conditions (i.e. AND operation) to keep its value,
			// otherwise is NaN
			if (mask !=0) data[n] = -32768.0;
		}

	}
}


/****************************************************************************************
 ** despeckle : Flag isolated gates less than speckle along a ray
 ****************************************************************************************/
void AirborneRadarQC::despeckleRadial(const QString& fldname, const int& speckle)
{
	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	for (int i=0; i < rays; i++) {
		// Get the data
		float* data = swpfile.getRayData(i, fldname);
		int n = 0;
		while (n < gates) {
			// Found a good gate, go forward to look for bad flag
			if (data[n] != -32768.) {
				int m = 0;
				int s = n;
				while ((n < gates) and (data[n] != -32768.)) {
					n++; m++;
				}
				// Found a bad flag, check m
				if (m > speckle) continue;
				// Found a speckle
				while((s < gates) and (s <= n)) {
					data[s] = -32768.;
					s++;
				}
			} else { n++; }
		}
	}
}

/****************************************************************************************
 ** despeckleAzimuth : Flag isolated gates less than speckle in the azimuthal direction
 ****************************************************************************************/
void AirborneRadarQC::despeckleAzimuthal(const QString& fldname, const int& speckle)
{

	// Allocate memory for the despeckling field
	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	float** data = new float*[rays];
	for (int i=0; i < rays; i++)  {
		data[i] = new float[gates];
	}

	this->swpField2array(fldname, data);

	for (int n=0; n < gates; n++) {
		int i = 0;
		while (i < rays) {
			// Found a good gate, go forward to look for bad flag
			if (data[i][n] != -32768.) {
				int m = 0;
				int s = i;
				while ((i < rays) and (data[i][n] != -32768.)) {
					i++; m++;
				}
				// Found a bad flag, check m
				if (m > speckle) continue;
				// Found a speckle
				while((s < rays) and (s <= i)) {
					data[s][n] = -32768.;
					s++;
				}
			} else { i++; }
		}
	}

	/* For some strange reason, this causes an intermittent crash
	 but the following always appears to crash despite the fact that
	 the only difference is the local rays and gates variables.
	 There is a bug somewhere that I don't understand - MMB */
	/* for (int i=0; i < rays; i++) {
		// Get the data
		float* despeckled = swpfile.getRayData(i, fldname);
		for (int n=0; n < gates; n++) {
			despeckled[n] = data[i][n];
		}
	} */

	for (int i=0; i < swpfile.getNumRays(); i++) {
		// Get the data
		float* despeckled = swpfile.getRayData(i, fldname);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			despeckled[n] = data[i][n];
		}
	}

	for (int i=0; i < rays; i++)  {
		delete[] data[i];
	}
	delete[] data;

}

/****************************************************************************************
 ** GaussianSmooth : Smooth the field according to a Gaussian function
 ****************************************************************************************/
void AirborneRadarQC::GaussianSmooth(const QString& oriFieldName, const QString& newFieldName, const int& scale)
{

	QString newFieldDesc = "Smoothed";
	QString newFieldUnits = "m/s2";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	RecursiveFilter* filter = new RecursiveFilter(4,scale);
	float* temp = new float[swpfile.getNumGates()];
	// First along the radials
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* oridata = swpfile.getRayData(i, oriFieldName);
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			temp[n] = newdata[n];
		}
		filter->filterArray(temp, swpfile.getNumGates());
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if ((oridata[n] == -32768.) or (fabs(temp[n]) > 1000)) {
				newdata[n] = -32768.;
			} else {
				newdata[n] = temp[n];
			}
		}
	}
	delete[] temp;

	/* Now along the beams
	float* data = new float[swpfile.getNumRays()];
	for (int n=0; n < swpfile.getNumGates(); n++)  {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* temp = swpfile.getRayData(i, fieldName);
			data[i] = temp[n];
		}
		filter->filterArray(data, swpfile.getNumRays());
	}

	delete[] data; */
	delete filter;

}


/****************************************************************************************
 ** GaussianSmooth : Smooth the field according to a Gaussian function
 ****************************************************************************************/
void AirborneRadarQC::GaussianSmooth(const QString& oriFieldName, float** field, const int& scale)
{

	RecursiveFilter* filter = new RecursiveFilter(4,scale);
	float* temp = new float[swpfile.getNumGates()];
	// First along the radials
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		// int lastgate=0;
		float* oridata = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) temp[n] = 0;
		for (int n=0; n < swpfile.getNumGates(); n++) {
			temp[n] = oridata[n];
			/*if ((oridata[n] == -32768.) and (!lastgate)) {
				// End of good data
				lastgate = n;
			} */
		}
		filter->filterArray(temp, swpfile.getNumGates());
		//float* newdata = field[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if ((oridata[n] == -32768.) or (fabs(temp[n]) > 1000)) {
				field[i][n] = -32768.;
			} else {
				field[i][n] = temp[n];
			}
		}
	}
	delete[] temp;

	/* Now along the beams
	temp = new float[swpfile.getNumRays()];
	for (int n=0; n < swpfile.getNumGates(); n++)  {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* oridata = swpfile.getRayData(i, oriFieldName);
			temp[i] = oridata[n];
			if (oridata[n] == -32768.) {
				// End of good data, break out;
				lastgate = n;
			}
		}
		filter->filterArray(temp, swpfile.getNumRays());
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			field[i][n] = temp[i];
		}
	}
	delete[] temp; */
	delete filter;

}

void AirborneRadarQC::swpField2array(const QString& oriFieldName, float** field)
{

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* oridata = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			field[i][n] = oridata[n];
		}
	}

}

void AirborneRadarQC::array2swpField(float** field, const QString& oriFieldName, const QString& newFieldName)
{
	QString newFieldDesc = "Array";
	QString newFieldUnits = "Unknown";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			newdata[n] = field[i][n];
		}
	}

}

void AirborneRadarQC::array2swpField(float** field, const QString& oriFieldName)
{

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* oridata = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			oridata[n] = field[i][n];
		}
	}

}

void AirborneRadarQC::copyEdits(const QString& oriFieldName,const QString& newFieldName)
{

	for (int i=0; i < swpfile.getNumRays(); i++) {
		// Get the data
		float* threshdata = swpfile.getRayData(i, oriFieldName);
		float* data = swpfile.getRayData(i, newFieldName);

		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (threshdata[n] == -32768.0) data[n] = -32768.0;
		}
	}

}

void AirborneRadarQC::calcRatio(const QString& topFieldName, const QString& bottomFieldName,
							const QString& newFieldName, const bool& zflag)
{

	QString newFieldDesc = "Ratio";
	QString newFieldUnits = "na";
	if(!newField(topFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	float z = 0;
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* top = swpfile.getRayData(i, topFieldName);
		float* bottom =  swpfile.getRayData(i, bottomFieldName);
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if ((top[n] != -32768.) and (bottom[n] != -32768.)) { // and (bottom[n] != 0)) {
				if (zflag) {
					z = pow(10.0,(bottom[n]*0.1));
				} else {
					z = bottom[n];
				}
				if (z > 0.0) {
					float ratio = top[n]/z;
					if (ratio > 100) ratio = 100;
					newdata[n] = log10(ratio) / 2.0;
				} else {
					newdata[n] = -32768.;
				}
			} else {
				newdata[n] = -32768.;
			}
		}
	}

}

/****************************************************************************************
 ** calcTexture : This subroutine calculates the texture of the input field by
 ** computing the sum of the square of the difference between values in a gateWindow x
 ** rayWindow box and dividing by the number of valid points inside that box.
 ****************************************************************************************/
void AirborneRadarQC::calcTexture(const QString& oriFieldName, const QString& newFieldName)
{
	float sum;
	int ray_index, N;
	int gateWindow = 2;
	int rayWindow = 2;
	float minval = -999;

	QString newFieldDesc = "Texture";
	QString newFieldUnits = "";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (data[n] < minval) continue;
			sum = 0.0;
			N = 0;
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, swpfile.getNumRays());
				float* oridata = swpfile.getRayData(ray_index, oriFieldName);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					if ((gi >= 1) && (gi < swpfile.getNumGates())
						&& (oridata[gi] > minval) && (oridata[gi-1] > minval)) {
						N++;
						sum = sum + pow((oridata[gi] - oridata[gi-1]), 2.0);
					}
				}
			}
			if (N > 0)
				data[n] = sum / float(N);
			else
				data[n] = -32768.0;
		}
	}
}


/****************************************************************************************
** calcRefSpinSteiner : This subroutine calculates the reflectivity SPIN using the method
** described in Steiner and Smith (2002).  It does this by calculating the reflectivity
** differences between rays in the gateWindow x rayWindow box.  If the difference exceeds
** spinthresh then the count (spinchange) is incremented.  The final value is a percentage
** of total possible times the threshold could have been exceeded in that box.
****************************************************************************************/
void AirborneRadarQC::calcSpinSteiner(const QString& oriFieldName, const QString& fldname) {
	float diff;
	int ray_index, N, spinchange;
	int gateWindow = 4;
	int rayWindow = 4;
	float minref = -999;
	float spinthresh = 2;

	QString newFieldName = fldname;
	QString newFieldDesc = "Spin Steiner";
	QString newFieldUnits = "dBZ";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, fldname);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (data[n] < minref)  // Make sure the data are valid
				continue;
			N = 0;
			spinchange = 0;
			for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
				// Make sure we're on the ray
				if ((gi >= 0) && (gi < swpfile.getNumGates())) {
					for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
						ray_index = getRayIndex(ri, swpfile.getNumRays());
						float* refdata1 = swpfile.getReflectivity(ray_index);
						ray_index = getRayIndex(ri-1, swpfile.getNumRays());
						float* refdata2 = swpfile.getReflectivity(ray_index);
						// Check the validity of the data
						if ((refdata1[gi] > minref) && (refdata2[gi] > minref)) {
							N++;
							diff = refdata1[gi] - refdata2[gi];  // Take the difference
							// Check the "spin" of the difference.  If it's outside the sping
							// threshold then increment the spinchange variable
							if ((diff > spinthresh) || (diff < (spinthresh*-1.0)))
								spinchange++;
						}
					}
				}
			}
			if (N > 0)
				data[n] = (float(spinchange) / float(N)) * 100.0;  // Express as a percentage
			else
				data[n] = -32768.0;
		}
	}
}

/****************************************************************************************
** calcSpinKessinger : This subroutine calculates the reflectivity SPIN using the method
** described in Kessinger et al. (2003).  It does this by calculating the reflectivity
** difference along the beams within the gateWindow x rayWindow box.  If the difference
** is above the threshold and the direction has changed (i.e. reflectivity has gone from
** increasing or neutral to decreasing and vice versa.  The final value is a percentage
** of total possible times the threshold could have been exceeded in that box.
****************************************************************************************/
void AirborneRadarQC::calcSpinKessinger(const QString& oriFieldName, const QString& fldname) {
	float diff;
	int ray_index, N, spinchange, dir;
	int gateWindow = 4;
	int rayWindow = 4;
	float minref = -999;
	float spinthresh = 7;

	QString newFieldName = fldname;
	QString newFieldDesc = "Reflectivity Spin Kessinger";
	QString newFieldUnits = "dBZ";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, fldname);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (data[n] < minref)  // Make sure the data are valid
				continue;
			N = 0;
			spinchange = 0;
			dir = 0; // The direction of spin: -1 is negative, 0 is neutral, 1 is positive
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, swpfile.getNumRays());
				float* refdata = swpfile.getReflectivity(ray_index);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					// Make sure it's still on the ray and that the data are valid
					if ((gi >= 1) && (gi < swpfile.getNumGates())) {
						if ((refdata[gi] > minref) && (refdata[gi-1] > minref)) {
							N++;
							diff = refdata[gi] - refdata[gi-1];
							// If the difference exceeds the threshold and the "spin" direction changes
							// then increment spinchange.
							if ((diff > spinthresh) && (dir <= 0)) {
								dir = 1;
								spinchange++;
							}
							else if ((diff < (spinthresh*-1.0)) && (dir >= 0)) {
								dir = -1;
								spinchange++;
							}
						}
					}
				}
			}
			if (N > 0)
				data[n] = (float(spinchange) / float(N)) * 100.0;  // Express as a percentage
			else
				data[n] = -32768.0;
		}
	}
}

/****************************************************************************************
** calcStdDev : This subroutine calculates the standard deviation of the velocity field
** by first calculating the mean over the gateWindow x rayWindow box, then summing the
** squares of the deviations from the mean, dividing by one less than the number of values
** and finally taking the square root.
****************************************************************************************/
void AirborneRadarQC::calcStdDev(const QString& oriFieldName, const QString& fldname) {
	float sum, mean;
	int ray_index, N;
	int gateWindow = 2;
	int rayWindow = 2;
	float minvel = -999;

	QString newFieldName = fldname;
	QString newFieldDesc = "Std Deviation";
	QString newFieldUnits = "Unknown";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, fldname);
		for (int n=0; n < swpfile.getNumGates(); n++) {

			// Calculate the mean velocity
			sum = 0.0;
			N = 0;
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, swpfile.getNumRays());
				float* veldata = swpfile.getRayData(ray_index, oriFieldName);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					if ((gi >= 0) && (gi < swpfile.getNumGates())) {
						if (veldata[gi] > minvel) {
							N++;
							sum = sum + veldata[gi];
						}
					}
				}
			}
			// If a mean is able to be calculated then go through the rest of the steps
			// At least two good values are required in order to calculate a STD
			if (N > 1) {
				mean = sum / float(N);

				// Now calculate the deviations, square them, and sum them
				sum = 0.0;
				for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
					ray_index = getRayIndex(ri, swpfile.getNumRays());
					float* veldata = swpfile.getRayData(ray_index, oriFieldName);
					for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
						if ((gi >= 0)  && (gi < swpfile.getNumGates())) {
							if (veldata[gi] > minvel) {
								// Sum the square of the differences
								sum = sum + (pow(mean - veldata[gi], 2.0));
							}
						}
					}
				}

				// Divide the sum by one less than the number of values and take the square root
				data[n] = pow(sum / float(N-1), 0.5);
			} else data[n] = -32768.0;
		}
	}
}

/****************************************************************************************
** calcStdDev : Same as above but to a 2-D array
****************************************************************************************/
void AirborneRadarQC::calcStdDev(const QString& oriFieldName, float** field) {
	float sum, mean;
	int ray_index, N;
	int gateWindow = 2;
	int rayWindow = 2;
	float minvel = -999;
	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();

	float** orifield = new float*[rays];
	for (int i=0; i < rays; i++)  {
		orifield[i] = new float[gates];
		float* data = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < gates; n++) {
			orifield[i][n] = data[n];
		}
	}

	for (int i=0; i < rays; i++)  {
		for (int n=0; n < gates; n++) {
			// Calculate the mean velocity
			sum = 0.0;
			N = 0;
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, rays);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					if ((gi >= 0) && (gi < gates)) {
						if (orifield[ray_index][gi] > minvel) {
							N++;
							sum = sum + orifield[ray_index][gi];
						}
					}
				}
			}
			// If a mean is able to be calculated then go through the rest of the steps
			// At least two good values are required in order to calculate a STD
			if (N > 1) {
				mean = sum / float(N);

				// Now calculate the deviations, square them, and sum them
				sum = 0.0;
				for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
					ray_index = getRayIndex(ri, rays);
					for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
						if ((gi >= 0)  && (gi < gates)) {
							if (orifield[ray_index][gi] > minvel) {
								// Sum the square of the differences
								sum = sum + (pow(mean - orifield[ray_index][gi], 2.0));
							}
						}
					}
				}

				// Divide the sum by one less than the number of values and take the square root
				field[i][n] = pow(sum / float(N-1), 0.5);
			} else field[i][n] = -32768.0;
		}
	}
	for (int i=0; i < rays; i++)  {
		delete[] orifield[i];
	}
	delete[] orifield;

}

/****************************************************************************************
** calcStdDev : Same as above but from and to a 2-D array
****************************************************************************************/
void AirborneRadarQC::calcStdDev(float** orifield, float** field) {
	float sum, mean;
	int ray_index, N;
	int gateWindow = 2;
	int rayWindow = 2;
	float minvel = -999;
	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();

	for (int i=0; i < rays; i++)  {
		for (int n=0; n < gates; n++) {
			// Calculate the mean velocity
			sum = 0.0;
			N = 0;
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, rays);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					if ((gi >= 0) && (gi < gates)) {
						if (orifield[ray_index][gi] > minvel) {
							N++;
							sum = sum + orifield[ray_index][gi];
						}
					}
				}
			}
			// If a mean is able to be calculated then go through the rest of the steps
			// At least two good values are required in order to calculate a STD
			if (N > 1) {
				mean = sum / float(N);

				// Now calculate the deviations, square them, and sum them
				sum = 0.0;
				for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
					ray_index = getRayIndex(ri, rays);
					for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
						if ((gi >= 0)  && (gi < gates)) {
							if (orifield[ray_index][gi] > minvel) {
								// Sum the square of the differences
								sum = sum + (pow(mean - orifield[ray_index][gi], 2.0));
							}
						}
					}
				}

				// Divide the sum by one less than the number of values and take the square root
				field[i][n] = pow(sum / float(N-1), 0.5);
			} else field[i][n] = -32768.0;
		}
	}
	for (int i=0; i < rays; i++)  {
		delete[] orifield[i];
	}
	delete[] orifield;

}

/****************************************************************************************
 ** calcSpatialMean : This subroutine calculates the mean value over the gateWindow x
 ** rayWindow box.
 ****************************************************************************************/
void AirborneRadarQC::calcSpatialMean(const QString& oriFieldName, const QString& newFieldName, const int& gateWindow, const int& rayWindow) {
	float sum;
	int ray_index, N;
	float mindata = -999;

	QString newFieldDesc = "Mean";
	QString newFieldUnits = "dBZ";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			N = 0;
			for (int ri=i-rayWindow/2; ri <= i+rayWindow/2; ri++) {
				ray_index = getRayIndex(ri, swpfile.getNumRays());
				float* oridata = swpfile.getRayData(ray_index, oriFieldName);
				for (int gi=n-gateWindow/2; gi <= n+gateWindow/2; gi++) {
					if ((gi >= 0) && (gi < swpfile.getNumGates())) {
						if (oridata[gi] > mindata) {
							N++;
							sum = sum + oridata[gi];
						}
					}
				}
			}
			if (N > 0) {
				newdata[n] = sum / float(N);
			} else newdata[n] = -32768.0;
		}
	}
}

/* Load a bunch of sweeps and take the average */
void AirborneRadarQC::calcTemporalMean(const QString& oriFieldName, const QString& newFieldName)
{

	if (this->getfileListsize()) {
		this->loadAuxSwp(0);
		int rays = 1300;
		int gates = 1500;
		float** mean = new float*[rays];
		float** count = new float*[rays];
		for (int i=0; i < rays; i++)  {
			mean[i] = new float[gates];
			count[i] = new float[gates];
		}

		for (int i=0; i < rays; i++)  {
			for (int n=0; n < gates; n++) {
				mean[i][n] = 0;
				count[i][n] = 0;
			}
		}

		for (int f = 0; f < this->getfileListsize(); ++f) {
			if (this->load(f)) {
				printf("\n\nCalculating mean from file %d\n", f);
				for (int i=0; i < swpfile.getNumRays(); i++)  {
					float* oridata = swpfile.getRayData(i, oriFieldName);
					for (int n=0; n < swpfile.getNumGates(); n++) {
						if (oridata[n] != -32768.) {
							mean[i][n] += oridata[n];
						}
						count[i][n] ++;
					}
				}
			}
		}

		if (this->load(0)) {
			// Copy to a new field first
			QString newFieldDesc = "Time Mean";
			QString newFieldUnits = "na";
			if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
				printf("Error creating new field!!!\n");
				return;
			}
			for (int i=0; i < swpfile.getNumRays(); i++)  {
				float* newdata = swpfile.getRayData(i, newFieldName);
				for (int n=0; n < swpfile.getNumGates(); n++) {
					if (count[i][n] != 0.) {
						newdata[n] = mean[i][n]/count[i][n];
					}
				}
			}
		}
		this->saveQCedSwp(0);
		for (int i=0; i < rays; i++)  {
			delete[] mean[i];
			delete[] count[i];
		}
		delete[] mean;
		delete[] count;
	}
}

void AirborneRadarQC::calcGradientMagnitude(const QString& oriFieldName, const QString& newFieldName, const int& order){

	// Copy to a new field first
	QString newFieldDesc = "Gradient";
	QString newFieldUnits = "na";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	// Allocate memory for the gradient fields
	float** gs = new float*[rays];
	float** r1 = new float*[rays];
	float** a1 = new float*[rays];
	for (int i=0; i < rays; i++)  {
		gs[i] = new float[gates];
		r1[i] = new float[gates];
		a1[i] = new float[gates];
	}

	this->swpField2array(oriFieldName, gs);

	this->calc1stRadialDerivative(gs,r1,order);

	this->calc1stAzimuthalDerivative(gs,a1,order);

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (newdata[n] != -32768.) {
				newdata[n] = r1[i][n]*r1[i][n] + a1[i][n]*a1[i][n];
			}
		}
	}

	for (int i=0; i < rays; i++)  {
		delete[] gs[i];
		delete[] r1[i];
		delete[] a1[i];
	}
	delete[] gs;
	delete[] r1;
	delete[] a1;

}

void AirborneRadarQC::calcGradientMagnitude(const QString& oriFieldName, float** field, const int& order){

	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	// Allocate memory for the gradient fields
	float** gs = new float*[rays];
	float** r1 = new float*[rays];
	float** a1 = new float*[rays];
	for (int i=0; i < rays; i++)  {
		gs[i] = new float[gates];
		r1[i] = new float[gates];
		a1[i] = new float[gates];
	}

	this->swpField2array(oriFieldName, gs);

	this->calc1stRadialDerivative(gs,r1,order);

	this->calc1stAzimuthalDerivative(gs,a1,order);

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* orifield = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (orifield[n] != -32768.) {
				field[i][n] = (r1[i][n]*r1[i][n] + a1[i][n]*a1[i][n]);
			}
		}
	}

	for (int i=0; i < rays; i++)  {
		delete[] gs[i];
		delete[] r1[i];
		delete[] a1[i];
	}
	delete[] gs;
	delete[] r1;
	delete[] a1;

}

void AirborneRadarQC::calcGradientMagnitude(float** orifield, float** field, const int& order){

	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	// Allocate memory for the gradient fields
	float** gs = new float*[rays];
	float** r1 = new float*[rays];
	float** a1 = new float*[rays];
	for (int i=0; i < rays; i++)  {
		gs[i] = new float[gates];
		r1[i] = new float[gates];
		a1[i] = new float[gates];
	}

	this->calc1stRadialDerivative(orifield,r1,order);

	this->calc1stAzimuthalDerivative(orifield,a1,order);

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* oridata = orifield[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (oridata[n] != -32768.) {
				field[i][n] = (r1[i][n]*r1[i][n] + a1[i][n]*a1[i][n]);
			}
		}
	}

	for (int i=0; i < rays; i++)  {
		delete[] gs[i];
		delete[] r1[i];
		delete[] a1[i];
	}
	delete[] gs;
	delete[] r1;
	delete[] a1;

}

/****************************************************************************************
 ** calc1stAzimuthGrad : This subroutine calculates the reflectivity gradient along a
 ** constant azimuth by taking the reflectivity difference from numrays to the left and right
 ** of the point in question and dividing by the number of points used.
 ****************************************************************************************/
void AirborneRadarQC::calc1stAzimuthalDerivative(const QString& oriFieldName, const QString& newFieldName,const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	QString newFieldDesc = "Azimuthal Gradient";
	QString newFieldUnits = "m/s2";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 1) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				float* oridata2 = swpfile.getRayData(i, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n]);
				} else newdata[n] = -32768.0;

			} else if (order == 2) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = swpfile.getRayData(i, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n])*0.5;
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = swpfile.getRayData(ray_index, oriFieldName);
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc1stRadialDerivative : This subroutine calculates the first derivative
 along a ray using a 1st, 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc1stRadialDerivative(const QString& oriFieldName, const QString& newFieldName,const int& order) {
	float sum;
	float mindata = -99999;

	QString newFieldDesc = "Radial Gradient";
	QString newFieldUnits = "m/s";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	if (order == 1) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = swpfile.getRayData(i, newFieldName);
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] - oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = swpfile.getRayData(i, newFieldName);
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = (oridata[n+1] - oridata[n-1])*0.5;
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = swpfile.getRayData(i, newFieldName);
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}
/****************************************************************************************
 ** calc2ndAzimuthDerivative : This subroutine calculates the 2nd derivative of a field
 ****************************************************************************************/
void AirborneRadarQC::calc2ndAzimuthalDerivative(const QString& oriFieldName, const QString& newFieldName,const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	QString newFieldDesc = "Azimuthal Gradient";
	QString newFieldUnits = "m/s2";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 2) {
				float* oridata = swpfile.getRayData(i, oriFieldName);
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = swpfile.getRayData(ray_index, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = oridata1[n]+oridata2[n] -2*oridata[n];
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = swpfile.getRayData(ray_index, oriFieldName);
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc2ndRadialDerivative : This subroutine calculates the 2nd derivative
 along a ray using a 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc2ndRadialDerivative(const QString& oriFieldName, const QString& newFieldName,const int& order) {
	float sum;
	float mindata = -99999;

	QString newFieldDesc = "Radial Gradient";
	QString newFieldUnits = "m/s";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = swpfile.getRayData(i, newFieldName);
			float* oridata = swpfile.getRayData(i, oriFieldName);
			newdata[0] = newdata[swpfile.getNumGates()-1] = -32768.;
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] + oridata[n-1] - 2*oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = swpfile.getRayData(i, newFieldName);
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}

/****************************************************************************************
 ** calcLaplacian : This subroutine calculates Laplacian of a field								0  1 0
 ** by adding the values to the immediate left, right, front, and back of the point in			1 -4 1
 ** question and then subtracting 4 times the value at the point in question.  The shape		0  1 0
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcLaplacian(const QString& oriFieldName, const QString& newFieldName) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	QString newFieldDesc = "Laplacian";
	QString newFieldUnits = "m/s2";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		float* oridata = swpfile.getRayData(i, oriFieldName);
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridata[n-1] + oridata[n+1] + oridatal[n] + oridatar[n];

			newdata[n] = sum - (4.0 * oridata[n]);  // Subtract 4 * the center point
		}
	}
}


/****************************************************************************************
 ** calcMixedPartial : This subroutine calculates the mixed derivative of a field			   -.25  0 .25
 ** by adding the corner values of the point using the stencil to the right           			 0   0  0
 ** question and then subtracting 4 times the value at the point in question.  The shape		.25  0 -.25
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcMixedPartial(const QString& oriFieldName, const QString& newFieldName) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	QString newFieldDesc = "Mixed Partial";
	QString newFieldUnits = "m/s2";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = swpfile.getRayData(i, newFieldName);
		float* oridata = swpfile.getRayData(i, oriFieldName);
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridatal[n-1] - oridatal[n+1] - oridatar[n-1] + oridatar[n+1];

			newdata[n] = sum/4;
			if (newdata[n] == 0.0)
				newdata[n] = -32768.;

		}
	}
}

/****************************************************************************************
 ** calc1stAzimuthGrad : This subroutine calculates the reflectivity gradient along a
 ** constant azimuth by taking the reflectivity difference from numrays to the left and right
 ** of the point in question and dividing by the number of points used.
 ****************************************************************************************/
void AirborneRadarQC::calc1stAzimuthalDerivative(const QString& oriFieldName, float** field,const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 1) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				float* oridata2 = swpfile.getRayData(i, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n]);
				} else newdata[n] = -32768.0;

			} else if (order == 2) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = swpfile.getRayData(i, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n])*0.5;
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = swpfile.getRayData(ray_index, oriFieldName);
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc1stRadialDerivative : This subroutine calculates the first derivative
 along a ray using a 1st, 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc1stRadialDerivative(const QString& oriFieldName, float** field,const int& order) {
	float sum;
	float mindata = -99999;

	if (order == 1) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] - oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = (oridata[n+1] - oridata[n-1])*0.5;
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}
/****************************************************************************************
 ** calc2ndAzimuthDerivative : This subroutine calculates the 2nd derivative of a field
 ****************************************************************************************/
void AirborneRadarQC::calc2ndAzimuthalDerivative(const QString& oriFieldName, float** field, const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 2) {
				float* oridata = swpfile.getRayData(i, oriFieldName);
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = swpfile.getRayData(ray_index, oriFieldName);
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = swpfile.getRayData(ray_index, oriFieldName);
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = oridata1[n]+oridata2[n] -2*oridata[n];
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = swpfile.getRayData(ray_index, oriFieldName);
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc2ndRadialDerivative : This subroutine calculates the 2nd derivative
 along a ray using a 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc2ndRadialDerivative(const QString& oriFieldName, float** field, const int& order) {
	float sum;
	float mindata = -99999;

	if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = swpfile.getRayData(i, oriFieldName);
			newdata[0] = newdata[swpfile.getNumGates()-1] = -32768.;
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] + oridata[n-1] - 2*oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = swpfile.getRayData(i, oriFieldName);
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}

/****************************************************************************************
 ** calcLaplacian : This subroutine calculates Laplacian of a field								0  1 0
 ** by adding the values to the immediate left, right, front, and back of the point in			1 -4 1
 ** question and then subtracting 4 times the value at the point in question.  The shape		0  1 0
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcLaplacian(const QString& oriFieldName, float** field) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		float* oridata = swpfile.getRayData(i, oriFieldName);
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridata[n-1] + oridata[n+1] + oridatal[n] + oridatar[n];

			newdata[n] = sum - (4.0 * oridata[n]);  // Subtract 4 * the center point
		}
	}
}


/****************************************************************************************
 ** calcMixedPartial : This subroutine calculates the mixed derivative of a field			   -.25  0 .25
 ** by adding the corner values of the point using the stencil to the right           			 0   0  0
 ** question and then subtracting 4 times the value at the point in question.  The shape		.25  0 -.25
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcMixedPartial(const QString& oriFieldName, float** field) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		float* oridata = swpfile.getRayData(i, oriFieldName);
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = swpfile.getRayData(ray_index, oriFieldName);
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridatal[n-1] - oridatal[n+1] - oridatar[n-1] + oridatar[n+1];

			newdata[n] = sum/4;
			if (newdata[n] == 0.0)
				newdata[n] = -32768.;

		}
	}
}

/****************************************************************************************
 ** calc1stAzimuthGrad : This subroutine calculates the reflectivity gradient along a
 ** constant azimuth by taking the reflectivity difference from numrays to the left and right
 ** of the point in question and dividing by the number of points used.
 ****************************************************************************************/
void AirborneRadarQC::calc1stAzimuthalDerivative(float** orifield, float** field,const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 1) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = orifield[ray_index];
				float* oridata2 = orifield[i];
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n]);
				} else newdata[n] = -32768.0;

			} else if (order == 2) {
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = orifield[ray_index];
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = orifield[i];
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = (oridata1[n]-oridata2[n])*0.5;
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = orifield[ray_index];
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc1stRadialDerivative : This subroutine calculates the first derivative
 along a ray using a 1st, 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc1stRadialDerivative(float** orifield, float** field,const int& order) {
	float sum;
	float mindata = -99999;

	if (order == 1) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = orifield[i];
			for (int n=0; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] - oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = orifield[i];
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = (oridata[n+1] - oridata[n-1])*0.5;
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { 1./12., -2./3., 0, 2./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = orifield[i];
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}
/****************************************************************************************
 ** calc2ndAzimuthDerivative : This subroutine calculates the 2nd derivative of a field
 ****************************************************************************************/
void AirborneRadarQC::calc2ndAzimuthalDerivative(float** orifield, float** field, const int& order) {
	float sum;
	int ray_index;
	float mindata = -999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		for (int n=0; n < swpfile.getNumGates(); n++) {
			sum = 0.0;
			newdata[n] = -32768.;
			if (order == 2) {
				float* oridata = orifield[i];
				ray_index = i+1;
				if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
				float* oridata1 = orifield[ray_index];
				ray_index = i-1;
				if (ray_index < 0) ray_index += swpfile.getNumRays();
				float* oridata2 = orifield[ray_index];
				if ((oridata1[n] > mindata) && (oridata2[n] > mindata)) {
					newdata[n] = oridata1[n]+oridata2[n] -2*oridata[n];
				} else newdata[n] = -32768.0;

			} else if (order == 4) {
				double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
				sum = 0;
				for (int m = i-2; m < i+3; m++) {
					ray_index = m;
					if (ray_index < 0) ray_index += swpfile.getNumRays();
					if (ray_index >= swpfile.getNumRays()) ray_index -= swpfile.getNumRays();
					float* oridata = orifield[ray_index];
					if (oridata[n] > mindata) {
						sum += weights[m-i+2]*oridata[n];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			} else {
				printf("Order %d not supported!",order);
			}
		}
	}
}

/****************************************************************************************
 ** calc2ndRadialDerivative : This subroutine calculates the 2nd derivative
 along a ray using a 2nd, or 4th order finite difference approximation
 ****************************************************************************************/
void AirborneRadarQC::calc2ndRadialDerivative(float** orifield, float** field, const int& order) {
	float sum;
	float mindata = -99999;

	if (order == 2) {
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = orifield[i];
			newdata[0] = newdata[swpfile.getNumGates()-1] = -32768.;
			for (int n=1; n < swpfile.getNumGates()-1; n++) {
				if ((oridata[n-1] > mindata) and (oridata[n+1] > mindata)) {
					newdata[n] = oridata[n+1] + oridata[n-1] - 2*oridata[n];
				} else {
					newdata[n] = -32768.;
				}
			}
		}
	} else if (order == 4) {
		double weights[5] = { -1./12., 4./3., -5./2., 4./3., -1./12. };
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* newdata = field[i];
			float* oridata = orifield[i];
			for (int n=0; n < swpfile.getNumGates(); n++) {
				sum = 0;
				newdata[n] = -32768.;
				for (int m = n-2; m < n+3; m++) {
					if ((m > 0) and (m < swpfile.getNumGates()) and (oridata[m] > mindata)) {
						sum += weights[m-n+2]*oridata[m];
					} else {
						sum = -32768.;
						break;
					}
				}
				if (sum != -32768.)
					newdata[n] = sum;
			}
		}
	} else {
		printf("Order %d not supported!",order);
	}

}

/****************************************************************************************
 ** calcLaplacian : This subroutine calculates Laplacian of a field								0  1 0
 ** by adding the values to the immediate left, right, front, and back of the point in			1 -4 1
 ** question and then subtracting 4 times the value at the point in question.  The shape		0  1 0
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcLaplacian(float** orifield, float** field) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		float* oridata = orifield[i];
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = orifield[ray_index];
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = orifield[ray_index];
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridata[n-1] + oridata[n+1] + oridatal[n] + oridatar[n];

			newdata[n] = sum - (4.0 * oridata[n]);  // Subtract 4 * the center point
		}
	}
}


/****************************************************************************************
 ** calcMixedPartial : This subroutine calculates the mixed derivative of a field			   -.25  0 .25
 ** by adding the corner values of the point using the stencil to the right           			 0   0  0
 ** question and then subtracting 4 times the value at the point in question.  The shape		.25  0 -.25
 ** is shown at the right.  If any of the values are bad then the calculation cannot be done.
 ****************************************************************************************/
void AirborneRadarQC::calcMixedPartial(float** orifield, float** field) {
	float sum;
	int ray_index, ri;
	float mindata = -99999;

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* newdata = field[i];
		float* oridata = orifield[i];
		// For this one we only care about the gates from 1 to 1 before the end because if
		// we try to do the math with less than 4 points it will break down.
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			sum = 0.0;
			// If any of the data are bad then we can't do the calculation
			// First, get the gates above and below the center point.
			if ((oridata[n-1] < mindata) || (oridata[n+1] < mindata)) {
				newdata[n] = -32768.0;
				continue;
			}

			// Next, get the gates to the right and left of the center point
			ri = i-1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatal = orifield[ray_index];
			if (oridatal[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}

			ri = i+1;
			ray_index = getRayIndex(ri, swpfile.getNumRays());
			float* oridatar = orifield[ray_index];
			if (oridatar[n] < mindata) {
				newdata[n] = -32768.0;
				continue;
			}
			sum = oridatal[n-1] - oridatal[n+1] - oridatar[n-1] + oridatar[n+1];

			newdata[n] = sum/4;
			if (newdata[n] == 0.0)
				newdata[n] = -32768.;

		}
	}
}


/****************************************************************************************
 ** flagGroundGates : This subroutine flags all gates potentially contaminated by
 ground clutter and below ground with a binary flag (1 = ground contamination possible)
 ****************************************************************************************/
void AirborneRadarQC::flagGroundGates(const QString& fldname, const float& eff_beamwidth) {

	float earth_radius=6366805.6;
	QString oldFieldName = "DBZ";
	QString newFieldName = fldname;
	QString newFieldDesc = "Ground Gates";
	QString newFieldUnits = "binary";
	if(!newField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	float* gates = swpfile.getGateSpacing();
	int numgates = swpfile.getNumGates();
	float max_range = gates[numgates-1];

	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, fldname);
		// Clear the ray
		for (int n=0; n < numgates; n++) {
			if (data[n] != -32768.) data[n] = 0;
		}
		float elev = swpfile.getElevation(i)*0.017453292;
		float tan_elev = tan(elev);
		float alt = swpfile.getRadarAlt(i)*1000;
		float ground_intersect = (-(alt)/sin(elev))*
			(1.+alt/(2.*earth_radius*tan_elev*tan_elev));

		if(ground_intersect > max_range || ground_intersect <= 0 )
			continue;

		float footprint = ground_intersect*eff_beamwidth*0.017453292/tan_elev;
		footprint = fabs((double)footprint);

		// Flag all the gates that may be compromised
		float range1 = ground_intersect - (footprint*.5);
		//float range2 = ground_intersect + (footprint*.5);
		float range2 = ground_intersect;

		// Flag the gates in between r1 & r2
		int g1 = -999.;
		for (int n=0; n < numgates; n++) {
			if((gates[n] <= range1) and (gates[n+1] >= range1)) {
				g1 = n; // Found the first ground gate
				break;
			}
		}
		// Add a 6 gate fudge factor
		for (int g=g1-6; g< numgates; g++) {
			//if ((gates[g] < range2) or ((g-g1) < 11)) {
			if (gates[g] < range2) {
				if (data[g] != -32768.) data[g] = 1.;
			}
		}
	}

}


/****************************************************************************************
** compareForAftRef : This subroutine dumps reflectivity values at the same point from
** both the fore and aft radars to a file for intercomparison.
****************************************************************************************/
void AirborneRadarQC::compareForeAftRef()
{
	float minLon = 999;
	float maxLon = 0;
	float minLat = 999;
	float maxLat = 0;
	double gridsp = 0.005;

	QFile refoutFile;
	QTextStream refout(&refoutFile);
	float radarLat = swpfile.getRadarLat();
	float radarLon = swpfile.getRadarLon();
	float radarAlt = swpfile.getRadarAlt();
	QString radarName = swpfile.getRadarname();
	double Pi = acos(-1);
	QString refoutName = radarName + "_reflectivity.txt";
	refoutFile.setFileName(refoutName);
	refoutFile.open(QIODevice::Append | QIODevice::WriteOnly);

	for (int i=0; i < 1; i++) {
		//for (int i=0; i < swpfile.getNumRays(); i++) {
		float az = swpfile.getAzimuth(i);
		float el = swpfile.getElevation(i);
		float* refdata = swpfile.getReflectivity(i);
		//QDateTime rayTime = swpfile.getRayTime(i);

		float* gatesp = swpfile.getGateSpacing();
		for (int n=0; n < swpfile.getNumGates(); n++) {
			float dz = refdata[n];
			float range = gatesp[n];
			//if ((range <= 0) or (range > 4000)) continue;

			double relX = -range*sin(az*Pi/180.)*cos(el*Pi/180.);
			double relY = -range*cos(az*Pi/180.)*cos(el*Pi/180.);
			double relZ = range*sin(el*Pi/180.);
			double latrad = radarLat * Pi/180.0;
			double fac_lat = 111.13209 - 0.56605 * cos(2.0 * latrad)
			+ 0.00012 * cos(4.0 * latrad) - 0.000002 * cos(6.0 * latrad);
			double fac_lon = 111.41513 * cos(latrad)
			- 0.09455 * cos(3.0 * latrad) + 0.00012 * cos(5.0 * latrad);
			double gateLon = radarLon - (relX/1000)/fac_lon;
			double gateLat = radarLat - (relY/1000)/fac_lat;
			double gateAlt = (relZ + radarAlt)*1000;
			//printf("%f\n", gateAlt);
			//if ((abs(gateAlt) < 20000) and (dz != -32768)) {
			refout << radarName << "\t";
			// Bin to nearest 0.005 degrees and km altitude
			gateLon = double(int(gateLon/gridsp))*gridsp;
			gateLat = double(int(gateLat/gridsp))*gridsp;
			gateAlt = double(int(gateAlt/1000.));
			refout << gateLon << "\t" << gateLat << "\t"
			<< gateAlt << "\t" << dz << "\n";
			if (gateLon < minLon) minLon = gateLon;
			if (gateLat < minLat) minLat = gateLat;
			if (gateLon > maxLon) maxLon = gateLon;
			if (gateLat > maxLat) maxLat = gateLat;
			//}
		}
	}
	refoutFile.close();

	// Now load it and compare local gates
	int lonDim = (maxLon-minLon)/gridsp + 1;
	int latDim = (maxLat-minLat)/gridsp + 1;
	float** foreref = new float*[lonDim];
	float** aftref = new float*[lonDim];
	float** forecount = new float*[lonDim];
	float** aftcount = new float*[lonDim];
	for (int i = 0; i<lonDim; i++) {
		foreref[i] = new float[latDim];
		aftref[i] = new float[latDim];
		forecount[i] = new float[latDim];
		aftcount[i] = new float[latDim];
	}
	for (int i = 0; i<lonDim; i++) {
        for (int j = 0; j<latDim; j++) {
			foreref[i][j] = 0;
			aftref[i][j] = 0;
			forecount[i][j] = 0;
			aftcount[i][j] = 0;
		}
	}
	refoutFile.setFileName("TF-ELDR_reflectivity.txt");
	refoutFile.open(QIODevice::ReadOnly);
	while (!refout.atEnd()) {
		QString line = refout.readLine();
		QStringList lineparts = line.split("\t");
		int lon = (lineparts[1].toFloat()-minLon)/gridsp;
		int lat = (lineparts[2].toFloat()-minLat)/gridsp;
		lon = lat = 0;
		foreref[lon][lat] += lineparts[4].toFloat();
		forecount[lon][lat] ++;
	}
	refoutFile.close();
	refoutFile.setFileName("TA-ELDR_reflectivity.txt");
	refoutFile.open(QIODevice::ReadOnly);
	while (!refout.atEnd()) {
        QString line = refout.readLine();
        QStringList lineparts = line.split("\t");
        int lon = (lineparts[1].toFloat()-minLon)/gridsp;
        int lat = (lineparts[2].toFloat()-minLat)/gridsp;
		lon = lat = 0;
        aftref[lon][lat] += lineparts[4].toFloat();
		aftcount[lon][lat] ++;
	}
	refoutFile.close();
	refoutFile.setFileName("ELDR_refcompare.txt");
	refoutFile.open(QIODevice::WriteOnly);
	for (int i = 0; i<lonDim; i++) {
		for (int j = 0; j<latDim; j++) {
			if ((forecount[i][j] != 0) and (aftcount[i][j] != 0)) {
				float refdiff = foreref[i][j]/forecount[i][j] - aftref[i][j]/aftcount[i][j];
				//printf("%d %d : %f %f %f\n",i,j,foreref[i][j], aftref[i][j], refdiff);
				refout << i << "\t" << j << "\t" << refdiff << "\n";
			}
		}
	}
	refoutFile.close();
	for (int i = 0; i<lonDim; i++) {
		delete[] foreref[i];
		delete[] aftref[i];
		delete[] forecount[i];
		delete[] aftcount[i];
	}
	delete[] foreref;
	delete[] aftref;
	delete[] forecount;
	delete[] aftcount;
}

void AirborneRadarQC::dumpFLwind()
{
	float minLon = 999;
	float maxLon = 0;
	float minLat = 999;
	float maxLat = 0;
	double gridsp = 0.005;

	QFile veloutFile;
	QTextStream velout(&veloutFile);
	float radarLat = swpfile.getRadarLat();
	float radarLon = swpfile.getRadarLon();
	float radarAlt = swpfile.getRadarAlt();
	QString radarName = swpfile.getRadarname();
	double Pi = acos(-1);
	QString veloutName = radarName + "_velocity.txt";
	veloutFile.setFileName(veloutName);
	veloutFile.open(QIODevice::Append | QIODevice::WriteOnly);
	QString velFieldName = "VG";
	for (int i=0; i < swpfile.getNumRays(); i++) {
		float az = swpfile.getAzimuth(i);
		float el = swpfile.getElevation(i);
		float* veldata = swpfile.getRayData(i, velFieldName);
		//QDateTime rayTime = swpfile.getRayTime(i);
		float flu = swpfile.getFLwind_u(i);
		float flv = swpfile.getFLwind_v(i);
		float head = swpfile.getHeading(i);
		float flvr = flu*sin(az*Pi/180.)*cos(el*Pi/180.) + flv*cos(az*Pi/180.)*cos(el*Pi/180.);
		float* gatesp = swpfile.getGateSpacing();
		for (int n=0; n < swpfile.getNumGates(); n++) {
			float vg = veldata[n];
			if (vg == -32768) continue;
			float range = gatesp[n];
			if ((range <= 0) or (range > 2000)) continue;

			double relX = -range*sin(az*Pi/180.)*cos(el*Pi/180.);
			double relY = -range*cos(az*Pi/180.)*cos(el*Pi/180.);
			double relZ = range*sin(el*Pi/180.);
			if (abs(relZ) > 50) continue;
			double latrad = radarLat * Pi/180.0;
			double fac_lat = 111.13209 - 0.56605 * cos(2.0 * latrad)
			+ 0.00012 * cos(4.0 * latrad) - 0.000002 * cos(6.0 * latrad);
			double fac_lon = 111.41513 * cos(latrad)
			- 0.09455 * cos(3.0 * latrad) + 0.00012 * cos(5.0 * latrad);
			double gateLon = radarLon - (relX/1000)/fac_lon;
			double gateLat = radarLat - (relY/1000)/fac_lat;
			double gateAlt = (relZ + radarAlt)*1000;
			//printf("%f\n", gateAlt);
			//if ((abs(gateAlt) < 20000) and (dz != -32768)) {
			velout << radarName << "\t";
			// Bin to nearest 0.005 degrees and km altitude
			gateLon = double(int(gateLon/gridsp))*gridsp;
			gateLat = double(int(gateLat/gridsp))*gridsp;
			gateAlt = double(int(gateAlt/1000.));
			float vrdiff = vg - flvr;
			velout << head << "\t" << gateLon << "\t" << gateLat << "\t"
			<< relZ << "\t" << vg << "\t" << radarLon << "\t" << radarLat
			<< "\t"  << radarAlt << "\t" << flvr << "\t" << vrdiff << "\t"
			<< range << "\t" << az << "\n";
			if (gateLon < minLon) minLon = gateLon;
			if (gateLat < minLat) minLat = gateLat;
			if (gateLon > maxLon) maxLon = gateLon;
			if (gateLat > maxLat) maxLat = gateLat;
			//}
		}
	}
	veloutFile.close();
}

/* This will load the flight level wind from the insitu data and compare with the
 near aircraft Doppler velocities */
void AirborneRadarQC::compareFLwind()
{

}

void AirborneRadarQC::writeToCSV()
{

	QFile veloutFile;
	QTextStream velout(&veloutFile);
	float radarLat = swpfile.getRadarLat();
	float radarLon = swpfile.getRadarLon();
	float radarAlt = swpfile.getRadarAlt();
	QString radarName = swpfile.getRadarname();
	double Pi = acos(-1);
	QString veloutName = radarName + "_data.txt";
	veloutFile.setFileName(veloutName);
	veloutFile.open(QIODevice::Append | QIODevice::WriteOnly);
	for (int i=0; i < swpfile.getNumRays(); i++) {
		float az = swpfile.getAzimuth(i);
		float el = swpfile.getElevation(i);
		float* vg = swpfile.getRayData(i, "VG");
		float* vb = swpfile.getRayData(i, "VB");
		float* vstd = swpfile.getRayData(i,"VSTD");
		float* vlp = swpfile.getRayData(i,"VLP");
		float* vmp = swpfile.getRayData(i,"VMP");
		float* vgr = swpfile.getRayData(i,"VGR");
		float* zz = swpfile.getRayData(i,"ZZ");
		float* gg = swpfile.getRayData(i,"GG");
		float* gatesp = swpfile.getGateSpacing();

		for (int n=0; n < swpfile.getNumGates(); n++) {
			float vtest = vb[n];
			if (vtest == -32768) continue;
			float range = gatesp[n];
			velout << radarName << ",";
			double relX = -range*sin(az*Pi/180.)*cos(el*Pi/180.);
			double relY = -range*cos(az*Pi/180.)*cos(el*Pi/180.);
			double relZ = range*sin(el*Pi/180.);
			double latrad = radarLat * Pi/180.0;
			double fac_lat = 111.13209 - 0.56605 * cos(2.0 * latrad)
			+ 0.00012 * cos(4.0 * latrad) - 0.000002 * cos(6.0 * latrad);
			double fac_lon = 111.41513 * cos(latrad)
			- 0.09455 * cos(3.0 * latrad) + 0.00012 * cos(5.0 * latrad);
			double gateLon = radarLon - (relX/1000)/fac_lon;
			double gateLat = radarLat - (relY/1000)/fac_lat;
			double gateAlt = (relZ + radarAlt)*1000;
			/* Bin to nearest 0.005 degrees and km altitude
			double gridsp = 0.005;
			gateLon = double(int(gateLon/gridsp))*gridsp;
			gateLat = double(int(gateLat/gridsp))*gridsp;
			gateAlt = double(int(gateAlt/1000.)); */
			velout << gateLon << "," << gateLat << "," << gateAlt << ",";
			velout << vg << "," << vb << "," << vstd << "," << vlp << ",";
			velout << vmp << "," << vgr << "," << zz << "," << gg << ",";
		}
	}
	veloutFile.close();
}

/* Remove the aircraft velocity from the Doppler velocity */
void AirborneRadarQC::removeAircraftMotion(const QString& vrFieldName, const QString& vgFieldName)
{
	// Aircraft Doppler velocity calculated following Lee et al. (1994)
	QString newFieldDesc = "Ground relative velocity";
	QString newFieldUnits = "m/s";
	if(!copyField(vrFieldName, vgFieldName)) {
		printf("Error creating new field!!!\n");
		return;
	}

	double nyquist = swpfile.getNyquistVelocity();
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, vgFieldName);
		double aircraft_vr = swpfile.getAircraftVelocity(i);
		for (int n=0; n < swpfile.getNumGates(); n++) {
			if (data[n] == -32768) continue;
			double vr = data[n] + aircraft_vr;
			if (fabs(vr) > nyquist) {
				// Fold data back into Nyquist range
				while (vr > nyquist) {
					vr -= nyquist*2;
				}
				while (vr < -nyquist) {
					vr += nyquist*2;
				}
			}
			data[n] = vr;
		}
	}
}

/* Apply a new set of navigation corrections */
void AirborneRadarQC::setNavigationCorrections(const QString& cfacFileName, const QString& radarName)
{
	// Check to see if this is the correct radar
	if (swpfile.getRadarname() != radarName) return;

	// Initialize the cfacData
	float cfacData[16];
	for (int i = 0; i < 16; i++) cfacData[i] = 0.0;
	int count = 0;

	// Load the cfac file
	QFile cfacFile;
	QTextStream cfac(&cfacFile);
	cfacFile.setFileName(cfacFileName);
	cfacFile.open(QIODevice::ReadOnly);
	while (!cfac.atEnd()) {
		QString line = cfac.readLine();
		QStringList lineparts = line.split("=");
		if (lineparts.size() < 2) {
			printf("Error reading cfac file\n");
			return;
		}
		cfacData[count] = lineparts[1].toFloat();
		count++;
	}
	cfacFile.close();

	// Assign the cfac block
	cfac_info* cfptr = swpfile.getCfacBlock();
	cfptr->c_azimuth = cfacData[0];
	cfptr->c_elevation = cfacData[1];
	cfptr->c_range_delay = cfacData[2];
	cfptr->c_rad_lon = cfacData[3];
	cfptr->c_rad_lat = cfacData[4];
	cfptr->c_alt_msl = cfacData[5];
	cfptr->c_alt_agl = cfacData[6];
	cfptr->c_ew_grspeed = cfacData[7];
	cfptr->c_ns_grspeed = cfacData[8];
	cfptr->c_vert_vel = cfacData[9];
	cfptr->c_head = cfacData[10];
	cfptr->c_roll = cfacData[11];
	cfptr->c_pitch = cfacData[12];
	cfptr->c_drift = cfacData[13];
	cfptr->c_rotang = cfacData[14];
	cfptr->c_tiltang = cfacData[15];

	// Apply the cfacs
	swpfile.recalculateAirborneAngles();

}

/****************************************************************************************
** calcWeatherProb : This subroutine calculates the probability of weather based on the
** interest maps calculated earlier.  There are multiple ways in which this can be done
** so this code will likely change.  The three I initially see attempting are:
**  1. multiplicative - (map1 * map2 *....* mapn)
**  2. weights and maps - (weight1 * map1 +...+weightn * mapn) / (weight1 +...+ weightn)
**  3. combination of 1 & 2 - map1 * (weight2 * map2 +...+weightn * mapn) / (weight2 +...+ weightn)
****************************************************************************************/
void AirborneRadarQC::calcWeatherProb(const QString& mdbzt_name, const QString& mdbzs_name, const QString& mdbzl_name,
									  const QString& mvgs_name, const QString& mncp_name) {

	// Copy to a new field first
	QString oldFieldName = "DBZ";
	QString newFieldName = "PWX";
	QString newFieldDesc = "Probability of Weather";
	QString newFieldUnits = "";
	if(!newField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	// Go through all of the rays and gates and calculate the probability
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, "PWX");
		float* mdbzt = swpfile.getRayData(i, mdbzt_name);
		float* mdbzs = swpfile.getRayData(i, mdbzs_name);
		float* mdbzl = swpfile.getRayData(i, mdbzl_name);
		float* mvgs = swpfile.getRayData(i, mvgs_name);
		float* mncp = swpfile.getRayData(i, mncp_name);

		for (int n=1; n < (swpfile.getNumGates()-1); n++) {

			// Method 1
			//data[n] = mdbzt[n] * mdbzs[n] * mdbzl[n] * mvgs[n] * mncp[n];

			// Method 2 (these weights may vary wildly)
			data[n] = ((5.0*mncp[n]) + (3.0*mdbzt[n]) + (2.0*mdbzs[n]) + (1.0*mdbzl[n]) + (2.0*mvgs[n])) / (5.0+3.0+2.0+1.0+2.0);
		}
	}
}

/****************************************************************************************
 ** wxProbability2 : This subroutine calculates the probability of weather based on the
 ** interest maps calculated earlier. This is the second in a series of tests.
 ****************************************************************************************/
void AirborneRadarQC::wxProbability2() {

	// Copy to a new field first
	QString oldFieldName = "VT";
	QString newFieldName = "PWX";
	QString newFieldDesc = "Probability of Weather";
	QString newFieldUnits = "";
	if(!newField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	// Copy to a new field
	oldFieldName = "VT";
	newFieldName = "PSK";
	newFieldDesc = "Probability of Speckle";
	if(!newField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}

	// Go through all of the rays and gates and calculate the probability
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, "PWX");
		float* ncpmap = swpfile.getRayData(i, "NCM");
		float* swz = swpfile.getRayData(i, "SWZ");
		float* gg = swpfile.getRayData(i, "GG");
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			if (data[n] != -32768) {
				double notgg = 1.0 - gg[n];
				double wxprob = ((1.0*ncpmap[n]) + (1.0*swz[n]) + (1.0*notgg))/3;
				data[n] = wxprob;
				if (fabs(wxprob) > 1.) {
					printf("Problem in prob! %f, %f, %f, %f\n",
					wxprob,ncpmap[n],swz[n],notgg);
				}
			}
		}
		// Calculate the probability it is a speckle
		float* threshprob = new float[swpfile.getNumGates()];
		float* speckletemp = new float[swpfile.getNumGates()];
		float* speckleprob = swpfile.getRayData(i, "PSK");
		for (int n=0; n < (swpfile.getNumGates()); n++) {
			if (speckleprob[n] != -32768)
				speckleprob[n] = 0;
		}
		for (int p=10; p < 101; p+=10) {
			double threshold = double(p)/100;
			if (threshold > 0.99) threshold = 0.99;
			for (int n=0; n < (swpfile.getNumGates()); n++) {
				threshprob[n] = data[n];
				if (threshprob[n] != -32768) {
					speckletemp[n] = 0.;
					if (threshprob[n] < threshold)
						threshprob[n] = -32768.;
				}
			}
			for (int speckle = 11; speckle > 1; speckle--) {
				int n = 0;
				while (n < swpfile.getNumGates()) {
					// Found a good gate, go forward to look for bad flag
					if (threshprob[n] != -32768.) {
						int m = 0;
						int s = n;
						while (threshprob[n] != -32768.) {
							n++; m++;
						}
						// Found a bad flag, check m
						if (m > speckle) continue;
						// Found a speckle
						while(s <= n) {
							if ((speckletemp[s] != -32768) and (s < n)) {
								float prob = float(11 - speckle)/10;
								if (prob == 0) prob = 0.01;
								speckletemp[s] = prob;
								//printf("%d speckle at %d = %f (%f)\n",speckle, s, speckletemp[s], data[s]);
							}
							s++;
						}
					} else { n++; }
				}
			}
			for (int n=1; n < (swpfile.getNumGates()-1); n++) {
				if (speckletemp[n] != -32768) {
					speckleprob[n] += speckletemp[n];
				}
			}
		}

		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			if (data[n] != -32768) {
				speckleprob[n] = speckleprob[n]/10;
				//printf("Speckle at %d = %f (%f)\n",n, speckleprob[n], data[n]);
				float notspeckle = 1-speckleprob[n];
				data[n] = 0.25*(data[n]*3 + notspeckle);
			}
		}
		delete[] threshprob;
		delete[] speckletemp;
	}

}

/****************************************************************************************
 ** BrierSkillScore : This subroutine calculates the Brier Skill Score
 ****************************************************************************************/
void AirborneRadarQC::BrierSkillScore() {

	// Go through all of the rays and gates and get a climatology for this sweep
	double baseprob = 0;
	double basebrier = 0;
	double basecount = 0;
	double probbrier = 0;
	double probcount = 0;
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* prob = swpfile.getRayData(i, "PWX");
		float* base = swpfile.getRayData(i, "VA");
		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			if (prob[n] != -32768) {
				// Possibility of good data
				if (base[n] != -32768) {
					// Baseline weather detection
					baseprob++;
				}
				basecount++;
			}
		}
	}
	// Baseline climatology
	if (basecount > 0)
		baseprob /= basecount;

	// Now calculate the scores
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* prob = swpfile.getRayData(i, "PWX");
		float* truth = swpfile.getRayData(i, "VG");

		for (int n=1; n < (swpfile.getNumGates()-1); n++) {
			if (prob[n] != -32768) {
				// Possibility of good data
				if (fabs(prob[n]) > 1.) {
					printf("Problem in prob! %f\n",prob[n]);
				}
				if (truth[n] != -32768) {
					// Weather
					probbrier += ((prob[n] - 1) * (prob[n] - 1));
					basebrier += ((baseprob - 1) * (baseprob - 1));
				} else {
					probbrier += prob[n] * prob[n];
					basebrier += baseprob * baseprob;
				}
				probcount++;
			}
		}
	}
	QFile verifyFile;
	QTextStream vout(&verifyFile);
	QString fileName = "bss.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::Append | QIODevice::WriteOnly);
	if (probcount > 0) {
		probbrier /= probcount;
		basebrier /= probcount;
		double bss = 1.0 - probbrier / basebrier;
		vout << probcount << "\t" << probbrier << "\t"
			<< basebrier << "\t" << bss << "\n";
		printf("BSS: %f\n",bss);
	} else {
		printf("No data for probabilities :(\n");
	}
	verifyFile.close();
}

/****************************************************************************************
 ** ReliabilityDiagram : This subroutine calculates the reliability of the probabilities
 ****************************************************************************************/
void AirborneRadarQC::ReliabilityDiagram() {

	// Calculate the hits and false alarms for different thresholds
	QFile verifyFile;
	QTextStream vout(&verifyFile);
	QString fileName = "reliability.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::Append | QIODevice::WriteOnly);
	/* vout << "threshold \t totalcount \t weather \t";
	vout << "nonweather \t truepos \t trueneg \t falsepos \t";
	vout << "falseneg \t hitrate \t falsealarmrate\n"; */

	for (int p=5; p < 101; p+=5) {
		double lowthresh = double(p-5)/100;
		double highthresh = double(p)/100;
		if (highthresh > 0.99) highthresh = 0.99;
		double avgthresh = (lowthresh + highthresh)/2;
		int weather = 0;
		int totalcount = 0;
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* prob = swpfile.getRayData(i, "PWX");
			float* truth = swpfile.getRayData(i, "VG");
			for (int n=1; n < (swpfile.getNumGates()-1); n++) {
				if (prob[n] != -32768) {
					// Possibility of good data
					if ((prob[n] > lowthresh) and (prob[n] <= highthresh)) {
						if (truth[n] != -32768) {
						// Weather
							weather++;
						}
						totalcount++;
					}
				}
			}
		}
		double truewxprob = 0;
		if (totalcount > 0)
			truewxprob = double(weather)/double(totalcount);
		vout << avgthresh << "\t" << totalcount << "\t" << weather << "\t"
		<< truewxprob << "\n";
		printf("Threshold: %f, True Wx Probability: %f\n", avgthresh, truewxprob);
	}

	verifyFile.close();
}

/****************************************************************************************
 ** RelativeOperatingCharacteristic : This subroutine calculates the ROC values
 ****************************************************************************************/
void AirborneRadarQC::RelativeOperatingCharacteristic() {

	// Calculate the hits and false alarms for different thresholds
	QFile verifyFile;
	QTextStream vout(&verifyFile);
	QString fileName = "roc.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::Append | QIODevice::WriteOnly);
	/* vout << "threshold \t totalcount \t weather \t";
	vout << "nonweather \t truepos \t trueneg \t falsepos \t";
	vout << "falseneg \t hitrate \t falsealarmrate\n"; */

	for (int p=5; p < 101; p+=5) {
		int truepos = 0;
		int falsepos = 0;
		int trueneg = 0;
		int falseneg = 0;
		int totalcount = 0;
		double threshold = double(p)/100;
		if (threshold > 0.99) threshold = 0.99;
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* prob = swpfile.getRayData(i, "PWX");
			float* truth = swpfile.getRayData(i, "VG");

			for (int n=1; n < (swpfile.getNumGates()-1); n++) {
				if (prob[n] != -32768) {
					// Possibility of good data
					if (truth[n] != -32768) {
						// Weather
						if (prob[n] < threshold) {
							falseneg++;
						} else {
							truepos++;
						}
					} else {
						// Not weather
						if (prob[n] < threshold) {
							trueneg++;
						} else {
							falsepos++;
						}
					}
					totalcount++;
				}
			}
		}

		double weather = (truepos + falseneg);
		double nonweather = (falsepos + trueneg);
		double hitrate = double(truepos)/weather;
		double falsealarmrate = double(falsepos)/nonweather;
		vout << threshold << "\t" << totalcount << "\t" << weather << "\t"
		<< nonweather << "\t" << truepos << "\t"
	    << trueneg << "\t" << falsepos << "\t"
		<< falseneg << "\t" << hitrate << "\t"
		<< falsealarmrate << "\n";
		printf("Threshold: %f, Hit rate: %f, False Alarm rate: %f\n",threshold, hitrate,falsealarmrate);
	}

	verifyFile.close();
}

void AirborneRadarQC::verify()
{
	QFile verifyFile;
	QTextStream vin(&verifyFile);
	QString fileName = "bss.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	vin.setPadChar('\t');
	double probcount, probbrier, basebrier, bss;
	double sumcount = 0;
	double sumbss = 0;
	while (!vin.atEnd()) {
		vin >> probcount >> probbrier >> basebrier >> bss;
		sumcount += probcount;
		sumbss += probcount*bss;
	}
	verifyFile.close();
	if (sumcount > 0)
		bss = sumbss/sumcount;

	fileName = "roc.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	double threshold, totalcount, weather,
	nonweather,truepos,trueneg, falsepos,
	falseneg,hitrate,falsealarmrate;
	double roccount[20], rochitrate[20], rocfar[20];
	for (int i=0; i<20; i++) {
		roccount[i]=0;
		rochitrate[i]=0;
		rocfar[i]=0;
	}
	int tindex = 0;
	while (!vin.atEnd()) {
		vin >> threshold >> totalcount >> weather
			>> nonweather >> truepos >> trueneg
			>> falsepos >> falseneg >> hitrate
			>> falsealarmrate;
		roccount[tindex] += totalcount;
		rochitrate[tindex] += totalcount*hitrate;
		rocfar[tindex] += totalcount*falsealarmrate;
		tindex++;
		if (tindex == 20) tindex = 0;
	}

	verifyFile.close();

	fileName = "reliability.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	double numprob[20], numwx[20];
	for (int i=0; i<20; i++) {
		numprob[i] = 0;
		numwx[i] = 0;
	}
	tindex = 0;
	double avgthresh, truewxprob;
	while (!vin.atEnd()) {
		vin >> avgthresh >> totalcount >> weather >> truewxprob;
		numprob[tindex] += totalcount;
		numwx[tindex] += weather;
		tindex++;
		if (tindex == 20) tindex = 0;
	}
	verifyFile.close();

	fileName = "verification.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::WriteOnly);
	vin << "Threshold\tWx Identified\tNon-Wx Retained\tEstimated Wx Prob.\tTrue Wx Prob.\tBSS\tAOC\n";

	double aoc = 0.;
	for (int i=0; i<20; i++) {
		if (roccount[i] > 0) {
			hitrate = rochitrate[i]/roccount[i];
			falsealarmrate = rocfar[i]/roccount[i];
			threshold = (i+1)*0.05;
			if (threshold >= 1.0) threshold = 0.99;
			if (i > 0) aoc += 0.5*(hitrate - (rochitrate[i-1]/roccount[i-1]))*
				(falsealarmrate - (rocfar[i-1]/roccount[i-1]));
		}
		if (numprob[i] > 0) {
			truewxprob = numwx[i]/numprob[i];
			avgthresh= (i+1)*0.05;
			if (avgthresh >= 1.0) avgthresh = 0.99;
			avgthresh -= 0.025;
		}
		vin << threshold << hitrate << falsealarmrate << avgthresh << truewxprob << bss << aoc;
		//printf("%f, %f, %f, %f, %f, %f, %f\n",threshold, hitrate, falsealarmrate, avgthresh, truewxprob, bss, aoc);
	}
}

/****************************************************************************************
 ** RelativeOperatingCharacteristic : This subroutine calculates the ROC values
 ****************************************************************************************/
void AirborneRadarQC::soloiiScriptROC() {

	// Calculate the hits and false alarms for different thresholds
	QFile verifyFile;
	QTextStream vout(&verifyFile);
	/* vout << "threshold \t totalcount \t weather \t";
	 vout << "nonweather \t truepos \t trueneg \t falsepos \t";
	 vout << "falseneg \t hitrate \t falsealarmrate\n"; */

	for (int p=80; p < 101; p+=10) {
		QString fileName = "roc.txt";
		int threshold = p;
		if (threshold > 99) threshold = 99;
		QString level;
		level.setNum(threshold);
		//fileName.prepend(level);
		verifyFile.setFileName(fileName);
		verifyFile.open(QIODevice::Append | QIODevice::WriteOnly);
		level.prepend("V");
		int truepos = 0;
		int falsepos = 0;
		int trueneg = 0;
		int falseneg = 0;
		int totalcount = 0;

		double baseprob = 0;
		double levelprob = 0;

		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* prob = swpfile.getRayData(i, level);
			float* truth = swpfile.getRayData(i, "VG");
			float* base = swpfile.getRayData(i, "VT");
			float* data = swpfile.getRayData(i, "ZZ");
			for (int n=1; n < (swpfile.getNumGates()-1); n++) {
				// Possibility of wx
				if (data[n] != -32768) {
					// Baseline weather detection
					if (base[n] != -32768) {
						baseprob++;
						if (truth[n] != -32768) {
							// Weather
							if (prob[n] == -32768) {
								falseneg++;
							} else {
								truepos++;
								levelprob++;
							}
						} else {
							// Not weather
							if (prob[n] == -32768) {
								trueneg++;
							} else {
								falsepos++;
								levelprob++;
							}
						}
					}
					totalcount++;
				}
			}
		}

		// Baseline climatology
		if (totalcount > 0) {
			baseprob /= totalcount;
			levelprob /= totalcount;
		}

		double weather = (truepos + falseneg);
		double nonweather = (falsepos + trueneg);
		double hitrate = double(truepos)/weather;
		double falsealarmrate = double(falsepos)/nonweather;
		vout << threshold << "\t" << totalcount << "\t" << weather << "\t"
		<< nonweather << "\t" << truepos << "\t"
	    << trueneg << "\t" << falsepos << "\t"
		<< falseneg << "\t" << hitrate << "\t"
		<< falsealarmrate << "\t" << baseprob << "\t" << levelprob << "\n";
		printf("Threshold: %d, Base data retained: %f, Data retained: %f, Hit rate: %f, False Alarm rate: %f\n",
			   threshold, baseprob, levelprob, hitrate,falsealarmrate);
		verifyFile.close();
	}

}


void AirborneRadarQC::soloiiScriptVerification()
{
	QFile verifyFile;
	QTextStream vin(&verifyFile);
	/* QString fileName = "bss.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	vin.setPadChar('\t');
	double probcount, probbrier, basebrier, bss;
	double sumcount = 0;
	double sumbss = 0;
	while (!vin.atEnd()) {
		vin >> probcount >> probbrier >> basebrier >> bss;
		sumcount += probcount;
		sumbss += probcount*bss;
	}
	verifyFile.close();
	if (sumcount > 0)
		bss = sumbss/sumcount; */

	QString fileName = "roc.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	double threshold, totalcount, weather,
	nonweather,truepos,trueneg, falsepos,
	falseneg,hitrate,falsealarmrate, baseprob, levelprob;
	double roctotal[3], rocwx[3], rocnonwx[3], roctrueneg[3];
	double rochits[3], rocfalsepos[3], rocmisses[3];
	for (int i=0; i<3; i++) {
		roctotal[i]=0;
		rocwx[i]=0;
		rocnonwx[i]=0;
		rochits[i]=0;
		rocfalsepos[i]=0;
		roctrueneg[i]=0;
		rocmisses[i]=0;
	}
	int tindex = 0;
	while (!vin.atEnd()) {
		vin >> threshold >> totalcount >> weather
		>> nonweather >> truepos >> trueneg
		>> falsepos >> falseneg >> hitrate
		>> falsealarmrate >> baseprob >> levelprob;
		roctotal[tindex] += totalcount;
		rocwx[tindex] += weather;
		rocnonwx[tindex] += nonweather;
		rochits[tindex] += truepos;
		rocfalsepos[tindex] += falsepos;
		roctrueneg[tindex] += trueneg;
		rocmisses[tindex] += falseneg;
		tindex++;
		if (tindex == 3) tindex = 0;
	}

	verifyFile.close();

	/* fileName = "reliability.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::ReadOnly);
	double numprob[20], numwx[20];
	for (int i=0; i<20; i++) {
		numprob[i] = 0;
		numwx[i] = 0;
	}
	tindex = 0;
	double avgthresh, truewxprob;
	while (!vin.atEnd()) {
		vin >> avgthresh >> totalcount >> weather >> truewxprob;
		numprob[tindex] += totalcount;
		numwx[tindex] += weather;
		tindex++;
		if (tindex == 20) tindex = 0;
	}
	verifyFile.close(); */

	fileName = "verification.txt";
	verifyFile.setFileName(fileName);
	verifyFile.open(QIODevice::WriteOnly);
	//vin << "Threshold\tWx Identified\tNon-Wx Retained\tEstimated Wx Prob.\tTrue Wx Prob.\tBSS\tAOC\n";
	vin << "Threshold\tBase Prob\tPOD\tFAR\tAOC\tThreat Score\tEquitable Threat Score\tHK\n";

	double aoc = 0.;
	double ts = 0.;
	double ets = 0.;
	double hk = 0.;
	for (int i=0; i<3; i++) {
		if (rocwx[i] > 0) {
			hitrate = rochits[i]/rocwx[i];
			falsealarmrate = rocfalsepos[i]/rocnonwx[i];
			baseprob = rocwx[i]/roctotal[i];
			threshold = (i+8)*.1;
			if (threshold >= 1.0) threshold = 0.99;
			if (i > 0) aoc += 0.5*(hitrate - (rochits[i-1]/rocwx[i-1]))*
				(falsealarmrate - (rocfalsepos[i-1]/rocnonwx[i-1]));
			ts = rochits[i] / (rochits[i] + rocfalsepos[i] + rocmisses[i]);
			double chance = (rochits[i] + rocfalsepos[i]) * (rochits[i] + rocmisses[i]) / (rocwx[i] + rocnonwx[i]);
			ets = (rochits[i] - chance) / (rochits[i] + rocfalsepos[i] + rocmisses[i] - chance);
			hk = (rochits[i] / (rochits[i] + rocmisses[i])) - (rocfalsepos[i] / (rocfalsepos[i] + roctrueneg[i]));
		}
		/* if (numprob[i] > 0) {
			truewxprob = numwx[i]/numprob[i];
			avgthresh= (i+1)*0.05;
			if (avgthresh >= 1.0) avgthresh = 0.99;
			avgthresh -= 0.025;
		} */
		//vin << threshold << hitrate << falsealarmrate << avgthresh << truewxprob << bss << aoc;
		vin << threshold << "\t" << baseprob << "\t" << hitrate << "\t" << falsealarmrate
			<< "\t" << aoc << "\t" << ts << "\t" << ets << "\t" << hk << endl;
		//printf("%f, %f, %f, %f, %f, %f, %f\n",threshold, hitrate, falsealarmrate, avgthresh, truewxprob, bss, aoc);
	}
}

/****************************************************************************************
 ** groundProbability : This subroutine calculates the probability that a given gate is ground
 using just the beamwidth
 ****************************************************************************************/
void AirborneRadarQC::probGroundGates(const QString& oriFieldName, const QString& newFieldName, const float& eff_beamwidth,
                                      const QString& demFileName)
{
	bool demFlag = false;
	DEM asterDEM;
	//GeographicLib::TransverseMercatorExact tm = GeographicLib::TransverseMercatorExact::UTM;
	const TransverseMercator& tm = TransverseMercator::UTM(); //syntax for GeographicLib 1.39

	if (!demFileName.isEmpty()) {
	        if(!asterDEM.readDem(demFileName.toLatin1().data())) {
	            printf("Error reading DEM file! Using flat ground instead\n");
	        } else {
	            demFlag = true;
	        }
	}
	//asterDEM.dumpAscii(1);
	float earth_radius=6366805.6;
	QString newFieldDesc = "Ground Gates";
	QString newFieldUnits = "binary";
	if(!newField(oriFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	float* gates = swpfile.getGateSpacing();
	int numgates = swpfile.getNumGates();
	float max_range = gates[numgates-1];
	for (int g=0; g< numgates; g++) {
		float left_distance = max_range;
		float right_distance = max_range;
		int left_index = -999;
		int right_index = -999;
		float ground_intersect = 0;
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			// Clear the gate
			float* data = swpfile.getRayData(i, newFieldName);
			if (data[g] != -32768.) data[g] = 0; // <- probability of ground gate is 0 for NaN
			// Find the beam axis intersection with the grounds
	        	float az = swpfile.getAzimuth(i)*0.017453292;
			float elev = (swpfile.getElevation(i))*0.017453292;// <- elevation from horizon
			if (elev > 0) { continue; }
			float tan_elev = tan(elev);
			float radarAlt = swpfile.getRadarAlt(i)*1000;
			if (gates[g] < radarAlt) { continue; }
			ground_intersect = (-(radarAlt)/sin(elev))*(1.+radarAlt/(2.*earth_radius*tan_elev*tan_elev));
			if(ground_intersect >= max_range*2.5 || ground_intersect <= 0 ) {continue; }
			ground_intersect = fabs(ground_intersect-gates[g]);
			if ((ground_intersect < left_distance) and (az > 3.14159)) {
				left_distance = ground_intersect;
				left_index = i;
			}
			if ((ground_intersect < right_distance) and (az <= 3.14159)){
				right_distance = ground_intersect;
				right_index = i;
			}
		}
		if ((left_index < 0) or (right_index < 0)) { continue; }
		for (int i=0; i < swpfile.getNumRays(); i++)  {
			float* data = swpfile.getRayData(i, newFieldName);
        		float az = swpfile.getAzimuth(i)*0.017453292;
			float elev = (swpfile.getElevation(i))*0.017453292;
			float tan_elev = tan(elev);
			float radarAlt = swpfile.getRadarAlt(i)*1000;
			float azground, elevground;
			if (az > 3.14159) {
				azground = swpfile.getAzimuth(left_index)*0.017453292;
				elevground = (swpfile.getElevation(left_index))*0.017453292;
			} else {
				azground = swpfile.getAzimuth(right_index)*0.017453292;
				elevground = (swpfile.getElevation(right_index))*0.017453292;
			}
			float azoffset = az - azground;
			float eloffset = elev - elevground;
			ground_intersect = (-(radarAlt)/sin(elev))*(1.+radarAlt/(2.*earth_radius*tan_elev*tan_elev));
			if(ground_intersect >= max_range*2.5 || ground_intersect <= 0 ) {
				continue;
			}

			// Calculate prob based on Flat-panel beam shape

	         	if (demFlag) {
				double range = gates[g];
				double relX = range*sin(az)*cos(elev);
				double relY = range*cos(az)*cos(elev);
				float radarLat = swpfile.getRadarLat(i);
				float radarLon = swpfile.getRadarLon(i);

				// printf("radarLon = %f \n", radarLon);
				// printf("radarLat = %f \n", radarLat);

				double radarX, radarY;
				tm.Forward(radarLon, radarLat, radarLon, radarX, radarY); // forward projection


				double absLat, absLon;
				tm.Reverse(radarLon, radarX + relX, radarY + relY, absLat, absLon); // reverse projection

				//std::cout << absLat << "\t" << absLon << "\t" << h << "\n";
				double h;
				h = asterDEM.getElevation(absLat, absLon);  // <--segmentation fault///////////////////

				std::exit;

				//if (g == 0) { std::cout << absLat << "\t" << absLon << "\t" << h << "\n"; }
				double agl = radarAlt - h;
				ground_intersect = (-(agl)/sin(elev))*(1.+agl/(2.*earth_radius*tan_elev*tan_elev));
	         	}
	         	float grange = ground_intersect-gates[g];
			if (grange <= 0) {
				if (data[g] != -32768.) data[g]	= 1.; // <- probability of a ground gate is 1 for NaN
			} else {
				// Alternate exponential formula
				//float gprob = exp(-grange/(ground_intersect*0.33));
				float beamaxis = sqrt(azoffset*azoffset + eloffset*eloffset);
				float beamwidth = eff_beamwidth*0.017453292;
				float gprob = 0.0;
				if (beamaxis > 0) {
					gprob = exp(-0.69314718055995*beamaxis/beamwidth);
				    //gprob = fabs(sin(27*sin(beamaxis))/(27*sin(beamaxis)));
			    	} else {
					gprob = 1.0;
				}
				if (gprob > 1.0) gprob = 1.0;
				if (data[g] != -32768.) data[g]	= gprob;
				//printf("Ground (%f) %f / %f\n",elev,grange,footprint);
			}
		}
	}
}

/****************************************************************************************
 ** groundProbability : This subroutine calculates the probability that a given gate is ground
 using just the beamwidth to a 2-D array
 ****************************************************************************************/
void AirborneRadarQC::probGroundGates(float** field, const float& eff_beamwidth,
                                      const QString& demFileName)
{
  bool demFlag = false;
  DEM asterDEM;
  //GeographicLib::TransverseMercatorExact tm = GeographicLib::TransverseMercatorExact::UTM;
	const TransverseMercator& tm = TransverseMercator::UTM();

	if (!demFileName.isEmpty()) {
        if(!asterDEM.readDem(demFileName.toLatin1().data())) {
            printf("Error reading DEM file! Using flat ground instead\n");
        } else {
            demFlag = true;
        }
    }
    //asterDEM.dumpAscii(1);
	float earth_radius=6366805.6;
	float* gates = swpfile.getGateSpacing();
	int numgates = swpfile.getNumGates();
	int numrays = swpfile.getNumRays();
	float max_range = gates[numgates-1];
	for (int g=0; g< numgates; g++) {
		float left_distance = max_range;
		float right_distance = max_range;
		int left_index = -999;
		int right_index = -999;
		float ground_intersect = 0;
		for (int i=0; i < numrays; i++)  {
			// Clear the gate
			if (field[i][g] != -32768.) field[i][g] = 0;

			// Find the beam axis intersection with the ground
	        float az = swpfile.getAzimuth(i)*0.017453292;
			float elev = (swpfile.getElevation(i))*0.017453292;
			if (elev > 0) { continue; }
			float tan_elev = tan(elev);
			float radarAlt = swpfile.getRadarAlt(i)*1000;
			if (gates[g] < radarAlt) { continue; }
			ground_intersect = (-(radarAlt)/sin(elev))*(1.+radarAlt/(2.*earth_radius*tan_elev*tan_elev));
			if(ground_intersect >= max_range*2.5 || ground_intersect <= 0 ) {
				continue;
			}
			ground_intersect = fabs(ground_intersect-gates[g]);
			if ((ground_intersect < left_distance) and (az > 3.14159)) {
				left_distance = ground_intersect;
				left_index = i;
			}
			if ((ground_intersect < right_distance) and (az <= 3.14159)){
				right_distance = ground_intersect;
				right_index = i;
			}
		}
		if ((left_index < 0) or (right_index < 0)) { continue; }
		for (int i=0; i < numrays; i++)  {
	        float az = swpfile.getAzimuth(i)*0.017453292;
			float elev = (swpfile.getElevation(i))*0.017453292;
			float tan_elev = tan(elev);
			float radarAlt = swpfile.getRadarAlt(i)*1000;
			float azground, elevground;
			if (az > 3.14159) {
				azground = swpfile.getAzimuth(left_index)*0.017453292;
				elevground = (swpfile.getElevation(left_index))*0.017453292;
			} else {
				azground = swpfile.getAzimuth(right_index)*0.017453292;
				elevground = (swpfile.getElevation(right_index))*0.017453292;
			}
			float azoffset = az - azground;
			float eloffset = elev - elevground;
			ground_intersect = (-(radarAlt)/sin(elev))*(1.+radarAlt/(2.*earth_radius*tan_elev*tan_elev));
			if(ground_intersect >= max_range*2.5 || ground_intersect <= 0 ) {
				continue;
			}

			// Calculate prob based on Flat-panel beam shape
	        double absLat, absLon, h;
            if (demFlag) {
                double range = gates[g];
                double relX = range*sin(az)*cos(elev);
                double relY = range*cos(az)*cos(elev);
		        float radarLat = swpfile.getRadarLat(i);
				float radarLon = swpfile.getRadarLon(i);
		        double radarX, radarY;
				tm.Forward(radarLon, radarLat, radarLon, radarX, radarY);
                tm.Reverse(radarLon, radarX + relX, radarY + relY, absLat, absLon);
                h = asterDEM.getElevation(absLat, absLon);
                //if (g == 0) { std::cout << absLat << "\t" << absLon << "\t" << h << "\n"; }
                double agl = radarAlt - h;
                ground_intersect = (-(agl)/sin(elev))*(1.+agl/(2.*earth_radius*tan_elev*tan_elev));
            }
            float grange = ground_intersect-gates[g];
			if (grange <= 0) {
				if (field[i][g] != -32768.) field[i][g]	= 1.;
			} else {
				// Alternate exponential formula
				//float gprob = exp(-grange/(ground_intersect*0.33));
				float beamaxis = sqrt(azoffset*azoffset + eloffset*eloffset);
				float beamwidth = eff_beamwidth*0.017453292;
				float gprob = 0.0;
				if (beamaxis > 0) {
					gprob = exp(-0.69314718055995*beamaxis/beamwidth);
				    //gprob = fabs(sin(27*sin(beamaxis))/(27*sin(beamaxis)));
			    } else {
					gprob = 1.0;
				}
				if (gprob > 1.0) gprob = 1.0;
				if (field[i][g] != -32768.) field[i][g]	= gprob;
				//printf("Ground (%f) %f / %f\n",elev,grange,footprint);
			}

		}
	}

}

/****************************************************************************************
** histogram : Make text file with histogram for all sweeps
****************************************************************************************/
void AirborneRadarQC::histogram(const QString& fldname, double min, double max, double interval,
						int thisfile) {

QFile verifyFile;
QTextStream vout(&verifyFile);
//QString fileName = fldname + "_histogram.txt";
QString fileName = outPath.absolutePath() + "/" + fldname + "_histogram.txt";
verifyFile.setFileName(fileName);
verifyFile.open(QIODevice::Append | QIODevice::WriteOnly);

if ((max == -999999) and (min = -999999)) {
	for (int i=0; i < swpfile.getNumRays(); i++)  {
		float* data = swpfile.getRayData(i, fldname);
		for (int n=0; n < swpfile.getNumGates(); n++) {
            		if (data[n] != -32768.) {
                		if (data[n] > max) max = data[n];
                		if (data[n] < min) min = data[n];
            }
        }
    }
    interval = (max - min)/100.;
}
int bins = int((max - min)/interval + 1);
float good[bins], bad[bins];
for (int i=0; i<bins; i++) {
     good[i] = 0;
     bad[i] = 0;
 }
for (int i=0; i < swpfile.getNumRays(); i++)  {
	float* data = swpfile.getRayData(i, fldname);
	float* test = swpfile.getRayData(i, "VV");
	for (int n=0; n < swpfile.getNumGates(); n++) {
		if (data[n] != -32768.) {
			int index = int((data[n] - min)/interval);
			if (index < 0) { index = 0; }
			if (index > (bins-1)) { index = bins-1; }
			if (test[n] == -32768.) {
				bad[index]++;
			} else {
				good[index]++;
			}
		}
	}
}

// Write text file with histogram. Columns are histogram bins and
// rows are sweeps
if (thisfile==0){
	for (int i=0; i<bins; i++) {
		vout << interval*i+min << "\t" ;
		if (i==bins-1) {vout<< "\n";}
	}
	for (int i=0; i<bins; i++) {
		vout << good[i]  << "\t" ;
		if (i==bins-1) {vout<< "\n";}
	}
}else{
	for (int i=0; i<bins; i++) {
		vout <<good[i]  << "\t" ;
		if (i==bins-1) {vout<< "\n";}
	}
}

verifyFile.close();
}


/****************************************************************************************
** exportVad : Export text file with a azimuth and velocity for each sweep file.
				Rows are ranges and columns azimuths
****************************************************************************************/
void AirborneRadarQC::exportVad(const QString& fldname,const int& swpIndex) {

QFile verifyFile;
QTextStream vout(&verifyFile);
QString fileName = outPath.absolutePath() + "/" + getswpfileName(swpIndex) + "_" + fldname + ".vad";
verifyFile.setFileName(fileName);
verifyFile.open(QIODevice::WriteOnly);

//float Nyq=swpfile.getNyquistVelocity();
int rays = swpfile.getNumRays();
int gates = swpfile.getNumGates();

// gets azimuths
float* azim = new float[rays];
for (int i=0; i < rays; i++)  {
	azim[i] = swpfile.getAzimuth(i);
}

// 2D matrix
float** data = new float*[rays];
for (int i=0; i < rays; i++)  {
	data[i] = new float[gates];
}
this->swpField2array(fldname, data);

// Write text file with VAD by adding new data (i.e. rows)
//at the end of the file
/* first line with header */
for (int i=0; i<rays; i++) {
	vout << azim[i] << "\t" ;
}
vout<< "\n";
/* rest of the lines */
for (int i=0; i<rays; i++) {
	for (int n=0; n<gates; n++) {
		vout << data[i][n]  << "\t" ;
	}
	vout<< "\n";
}

/* delete array */
for (int i=0; i < rays; i++)  {
	delete[] data[i];
}
delete[] data;


/* close file*/
verifyFile.close();
}

/****************************************************************************************
** getRayIndex : This function returns the correct ray index given a value.  Most of the
** time it will simply return the value it was sent, but it is written so that it will
** return the correct index when a negative number or a number greater than the number of
** rays is given.
****************************************************************************************/
int AirborneRadarQC::getRayIndex(int ri, int nrays)  {
	if (ri < 0)
		return (nrays + ri);
	else if (ri >= nrays)
		return (ri - nrays);

	return ri;
}

/****************************************************************************************
** These interest map functions return a value between 0 and 1, which represents the
** likelihood of that value is representing weather.
** Some are based on maps derived by Kessinger et al. (2003)
** Graphical versions of the maps can be found in /Users/cwolff/Documents/NCAR/eldora/interestmaps.xls
****************************************************************************************/
float AirborneRadarQC::mapGradient(const float& value) {
	if (value < 0.0)
		return 1.0;
	else if (value > 10.0)
		return 0.0;
	else
		//return (1.0-(value/10.0));
		return ((10.0 - value)/9.0)*((10.0 - value)/9.0);
}

float AirborneRadarQC::mapStdDev(const float& std) {
	if (std < 0.0)
		return 1.0;
	else if (std > 5.0)
		return 0.0;
	else
		return (1.0-(std/5.0));
}

float AirborneRadarQC::mapNCP(const float& ncp) {
	if (ncp < 0.2)
		return 0.0;
	else if (ncp > 0.4)
		return 1.0;
	else
		return ((ncp-0.2)/0.2);
}

float AirborneRadarQC::mapLaplacian(const float& lap) {
	float abslap = fabs(lap);
	if (abslap < 1.0)
		return 1.0;
	else if (abslap > 8.0)
		return 0.0;
	else
		return (1.0 - (abslap-1.0)/8.0);
}

float AirborneRadarQC::mapMixedPartial(const float& mp) {
	if (fabs(mp) > 1.0)
		return 0.0;
	else
		return 1.0;
}

float AirborneRadarQC::mapSWZ(const float& swz) {
	if (swz < 0.2)
		return 1.0;
	else if (swz > 0.6)
		return 0.0;
	else
		return 1.0-((swz-0.2)/0.2); //Exponential?
}

void AirborneRadarQC::wxProbability(const QString& oriFieldName, const QString& probFieldName, float* weight){
	QString oldFieldName = oriFieldName;
	QString newFieldName = probFieldName;
	QString newFieldDesc = "Probability of Wx";
	QString newFieldUnits = "";
	if(!newField(oldFieldName, newFieldName, newFieldDesc, newFieldUnits)) {
		printf("Error creating new field!!!\n");
		return;
	}
	std::cout << "wxProbability weights: " << weight[0] << "\t" << weight[1] <<  "\t" << weight[2] <<
		 "\t" << weight[3] << "\t" << weight[4] <<  "\t" <<weight[5] <<  "\t" << weight[6] << "\n";

	int rays = swpfile.getNumRays();
	int gates = swpfile.getNumGates();
	// Allocate memory for the interest fields
	float** vg = new float*[rays];
	float** gradient = new float*[rays];
	float** laplacian = new float*[rays];
	float** mixedpartial = new float*[rays];
	float** ncp = new float*[rays];
	float** stddev = new float*[rays];
	float** ground = new float*[rays];
	float** swz = new float*[rays];
	for (int i=0; i < rays; i++)  {
		vg[i] = new float[gates];
		gradient[i] = new float[gates];
		laplacian[i] = new float[gates];
		mixedpartial[i] = new float[gates];
		ncp[i] = new float[gates];
		stddev[i] = new float[gates];
		ground[i] = new float[gates];
		swz[i] = new float[gates];
	}

	this->swpField2array(oriFieldName, vg);

	this->calcGradientMagnitude(vg,gradient,4);
	this->array2swpField(gradient, "VV", "VGR");

	this->calcLaplacian(vg, laplacian);
	this->array2swpField(laplacian, "VV", "VLP");

	this->calcMixedPartial(vg, mixedpartial);
	this->array2swpField(mixedpartial, "VV", "VMP");

	this->swpField2array("NCP", ncp);

	this->calcStdDev(vg,stddev);
	this->array2swpField(stddev, "VV", "VSD");

	float beamwidth = 1.8;
	this->probGroundGates(ground,beamwidth);
	this->array2swpField(ground, "VV", "GG");

	this->calcRatio("SW", "ZZ", "SWZ", true);
	this->swpField2array("SWZ", swz);

	// Map to probability fields
	for (int i=0; i < rays; i++)  {
		float* orifield = swpfile.getRayData(i, oriFieldName);
		for (int n=0; n < gates; n++) {
			if (orifield[n] != -32768.) {
				gradient[i][n] = mapGradient(gradient[i][n]);
				laplacian[i][n] = mapLaplacian(ncp[i][n]);
				mixedpartial[i][n] = mapMixedPartial(ncp[i][n]);
				ncp[i][n] = mapNCP(ncp[i][n]);
				stddev[i][n] = mapStdDev(ncp[i][n]);
				ground[i][n] = 1.0 - ground[i][n];
				swz[i][n] = mapSWZ(swz[i][n]);
			}
		}
	}
	float weightsum = 0;
	for (int i=0; i<7; i++) weightsum += weight[i];

	// Combine interest maps with weights
	for (int i=0; i < rays; i++)  {
		float* wxprob = swpfile.getRayData(i, probFieldName);
		for (int n=0; n < gates; n++) {
			if (wxprob[n] != -32768.) {
				wxprob[n] = (weight[0]*gradient[i][n] + weight[1]*laplacian[i][n]
					+ weight[2]*mixedpartial[i][n] + weight[3]*ncp[i][n]
						+weight[4]*stddev[i][n] + weight[5]*ground[i][n]
							+weight[6]*swz[i][n])/weightsum;
			}
		}
	}

	for (int i=0; i < rays; i++)  {
		delete[] vg[i];
		delete[] gradient[i];
		delete[] laplacian[i];
		delete[] mixedpartial[i];
		delete[] ncp[i];
		delete[] stddev[i];
		delete[] ground[i];
		delete[] swz[i];
	}
	delete[] vg;
	delete[] gradient;
	delete[] laplacian;
	delete[] mixedpartial;
	delete[] ncp;
	delete[] stddev;
	delete[] ground;
	delete[] swz;

}
