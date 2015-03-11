%module AirborneRadarQC
 %{
 /* Includes the header in the wrapper code */
 #include "AirborneRadarQC.h"
 // #include "DEM.h"
 // #include "Dorade.h"
 %}
 
 /* Parse the header file to generate wrappers */
 %include "AirborneRadarQC.h"
 // %include "DEM.h"
 // %include "Dorade.h"