/*
 *  AirborneRadarQC.h
 *
 *  Copyright 2011 Michael Bell and Cory Wolff
 *  All rights reserved.
 *
 */

#ifndef AIRBORNEQC_H
#define AIRBORNEQC_H

#include "Dorade.h"
#include <QList>
#include <QDir>
 #include <QFile>
#include "DEM.h"

class AirborneRadarQC
{

public:
	// Conor / Deor
	AirborneRadarQC(const QString& in, const QString& out, 
			const QString& cfacp, const QString& suffix,
			const QString& dtmf, const QString& legtype);
        	AirborneRadarQC();
	virtual ~AirborneRadarQC();

	// I/O
	bool readSwpDir();
	bool load(const int& swpIndex);
	bool loadAuxSwp(const int& swpIndex);
	bool saveQCedSwp(const int& swpIndex);
	bool saveDorade(const QString& doradeFilename);
	int getfileListsize() { return swpfileList.size(); }
	QString getswpfileName(int n) { return swpfileList[n]; }
	QString getInputPath();
	bool setInputPath(const QString& in);
	QString getOutputPath();
	bool setOutputPath(const QString& out);
	void writeToCSV();

	// QC
	bool processSweeps(const QString& typeQC);
	bool newField(const QString& oldFieldName,const QString& newFieldName,
				  const QString& newFieldDesc,const QString& newFieldUnits);
	bool copyField(const QString& oldFieldName,const QString& newFieldName);

	bool copyAuxField(const QString& oldFieldName,const QString& newFieldName,
					  const QString& newFieldDesc,const QString& newFieldUnits);
	void thresholdData(const QString& fldname, const QString& threshfield,
				const QString& direction = "below",const float& threshold = 0);
	void thresholdDataAND(const QString& fldname,
				const QString& threshfield1, const QString& direction1, const float threshold1[],
				const QString& threshfield2, const QString& direction2, const float threshold2[],
				const QString& threshfield3, const QString& direction3, const float threshold3[]);
	void thresholdDataAND(const QString& fldname,
				const QString& threshfield1, const QString& direction1, const float threshold1[],
				const QString& threshfield2, const QString& direction2, const float threshold2[]);	
	void despeckleRadial(const QString& fldname, const int& speckle);
	void despeckleAzimuthal(const QString& fldname, const int& speckle);
	void GaussianSmooth(const QString& oriFieldName, const QString& newFieldName, const int& scale);
	void GaussianSmooth(const QString& oriFieldName, float** field, const int& scale);
	void swpField2array(const QString& oriFieldName, float** field);
	void array2swpField(float** field, const QString& oriFieldName);
	void array2swpField(float** field, const QString& oriFieldName, const QString& newFieldName);
	void copyEdits(const QString& oriFieldName,const QString& newFieldName);

	// REC Fields
	void calcTexture(const QString& oriFieldName, const QString& fldname);
	void calcSpinSteiner(const QString& oriFieldName, const QString& fldname);
	void calcSpinKessinger(const QString& oriFieldName, const QString& fldname);
	void calcStdDev(const QString& oldFieldName, const QString& fldname);
	void calcStdDev(const QString& oldFieldName, float** field);
	void calcStdDev(float** orifield, float** field);

	void calcMeanRef(const QString& fldname);
	void calcSpatialMean(const QString& oriFieldName, const QString& newFieldName, const int& gateWindow, const int& rayWindow);
	void calcTemporalMean(const QString& oriFieldName, const QString& newFieldName);
	void calcGate2GateRefGrad(const QString& fldname);
	void calcAzimuthRefGrad(const QString& fldname);

	void calcRatio(const QString& topFieldName, const QString& bottomFieldName,
				   const QString& newFieldName, const bool& zflag);
	void calcGradientMagnitude(const QString& oriFieldName, const QString& newFieldName, const int& order);
	void calcGradientMagnitude(const QString& oriFieldName, float** field, const int& order);
	void calcGradientMagnitude(float** orifield, float** field, const int& order);

	// Generic derivatives
	// Create new swp field for display
	void calc1stAzimuthalDerivative(const QString& oriFieldName, const QString& newFieldName, const int& order);
	void calc1stRadialDerivative(const QString& oriFieldName, const QString& newFieldName, const int& order);
	void calc2ndAzimuthalDerivative(const QString& oriFieldName, const QString& newFieldName, const int& order);
	void calc2ndRadialDerivative(const QString& oriFieldName, const QString& newFieldName, const int& order);
	void calcLaplacian(const QString& oriFieldName, const QString& newFieldName);
	void calcMixedPartial(const QString& oriFieldName, const QString& newFieldName);

	// Create from swp field to temporary
	void calc1stAzimuthalDerivative(const QString& oriFieldName, float** field, const int& order);
	void calc1stRadialDerivative(const QString& oriFieldName, float** field, const int& order);
	void calc2ndAzimuthalDerivative(const QString& oriFieldName, float** field, const int& order);
	void calc2ndRadialDerivative(const QString& oriFieldName, float** field, const int& order);
	void calcLaplacian(const QString& oriFieldName, float** field);
	void calcMixedPartial(const QString& oriFieldName, float** field);

	// Create from temporary to temporary
	void calc1stAzimuthalDerivative(float** orifield, float** field, const int& order);
	void calc1stRadialDerivative(float** orifield, float** field, const int& order);
	void calc2ndAzimuthalDerivative(float** orifield, float** field, const int& order);
	void calc2ndRadialDerivative(float** orifield, float** field, const int& order);
	void calcLaplacian(float** orifield, float** field);
	void calcMixedPartial(float** orifield, float** field);

	// Flag sensitive areas
	void flagGroundGates(const QString& fldname, const float& eff_beamwidth);

	// Probabilities
	// void probGroundGates(const QString& oriFieldName, const QString& newFieldName, const float& eff_beamwidth,
 //                         const QString& demFileName = 0);
	// void probGroundGates(float** field, const float& eff_beamwidth, const QString& demFileName = 0);

	/* Now it handles DTM at the beginning of processSweeps (RV) */
	void probGroundGates(const QString& oriFieldName, const QString& newFieldName, const float& eff_beamwidth);
	void probGroundGates2_offshore(const QString& oriFieldName, 
				const QString& newFieldName, 
				const float& thres_dbz, const float& thres_elev, 
				const float& thres_bmh, const float& thres_per);	
	void probGroundGates2_onshore(const QString& oriFieldName, 
				const QString& newFieldName, 
				const float& thres_dbz, const float& thres_elev, 
				const float& thres_bmh, const float& thres_per);		
	void probGroundGatesMB(const QString& oriFieldName, const QString& newFieldName, const float& eff_beamwidth);		
	void probGroundGates(float** field, const float& eff_beamwidth);
	void make_ancillary_field(const QString& newFieldName,const QString& ancillaryName); //(RV)
	void calcWeatherProb(const QString& mdbzt_name, const QString& mdbzs_name, const QString& mdbzl_name, const QString& mvgs_name, const QString& mncp_name);
	void wxProbability(const QString& oriFieldName, const QString& probFieldName, float* weight);
	void wxProbability2();
	float mapGradient(const float& value);
	float mapStdDev(const float& std);
	float mapNCP(const float& ncp);
	float mapLaplacian(const float& lap);
	float mapMixedPartial(const float& mp);
	float mapSWZ(const float& swz);

	// Other
	void compareForeAftRef();
	void dumpFLwind();
	void compareFLwind();
	void removeAircraftMotion(const QString& vrFieldName, const QString& vgFieldName);
	void removeAircraftMotion(const QString& vrFieldName, const QString& vgFieldName, const double nyquist);
	// void setNavigationCorrections(const QString& cfacFileName, const QString& radarName);
	void setNavigationCorrections(const QString& filename, const QString& radarName); // (RV)
	void unSetNavigationCorrections(); // (RV)

	// Verification
	void BrierSkillScore();
	void RelativeOperatingCharacteristic();
	void ReliabilityDiagram();
	void verify();
	void soloiiScriptROC();
	void soloiiScriptVerification();
	void histogram(const QString& fldname, double min = 999999.0, double max = -999999.0,
			 double interval = 0.0, int thisfile=0);
	bool exportVad(const QString& fldname,const int& swpIndex);

private:
	QList<QString> swpfileList;
	QDir dataPath;
	QDir outPath;
	QDir cfacPath; //(RV)
	QFile dtmFile; //(RV)
	QString swpSuffix;
	QString legType; //(RV)

	Dorade swpfile;
	Dorade auxSwpfile; // Used to merge or thin sweeps

	int getRayIndex(int ri, int nrays);
	float calcRefTextInterestMap(float texture);
	float calcRefSpinInterestMap(float spin);
	float calcVelStdInterestMap(float std);
	float calcMeanRefInterestMap(float ref);
	float calcNCPInterestMap(float ncp);
	float calcRefLaplacianInterestMap(float lap);

	bool demFlag; //(RV)
	DEM asterDEM; //(RV)
};

#endif
