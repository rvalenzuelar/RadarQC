%module AirborneRadarQC

// %include "typemaps.i"
// %include "std_string.i"
// %include "exception.i"
// %include "constraints.i"
// %include "carrays.i"

%{
/* Includes the header in the wrapper code */
#include "AirborneRadarQC.h"
// #include <QtCore>	
// #include <QtDebug>		
#define SWIG_FILE_WITH_INIT	
%}
 
/* Parse the header file to generate wrappers */
%include "AirborneRadarQC.h"


// // From:
// // http://swig.10945.n7.nabble.com/typemap-for-python-and-c-const-ref-not-working-td4273.html
// %typemap(in) const string& (std::string temp, std::string *s) {
// 	if (PyString_Check($input)) {
// 	 temp = std::string(PyString_AsString($input),
// 	                    PyString_Size($input));
// 	 $1 = &temp;
// 	} else if (SWIG_ConvertPtr($input, (void **) &s,
// 	                        $1_descriptor,0) != -1) {
// 	 $1 = s;
// 	} else {
// 	 SWIG_exception(SWIG_TypeError, "string expected");
// 	}
// } 

// // Convert qstring
// // https://github.com/EiNSTeiN-/idapython/blob/master/swig/typeconv.i
// %typemap(in) qstring*
// {
// 	char *buf;
// 	Py_ssize_t length;
// 	int success = PyString_AsStringAndSize($input, &buf, &length);
// 	if ( success > -1 )
// 	{
// 		$1 = new qstring(buf, length);
// 	}
// }
// %typemap(freearg) qstring*
// {
// 	delete $1;
// }
// %typemap(out) qstring*
// {
// 	$result = PyString_FromStringAndSize($1->c_str(), $1->length());
// }


// // From:
// // http://dtk.inria.fr/dtk/dtk/blobs/b893a110d856924fe87502e522c99a9d65ed79f2/src/dtkCore/dtkCore.i

// %typecheck(SWIG_TYPECHECK_STRING) char * {
//   $1 = PyString_Check($input) ? 1 : 0;
// }

// // %typemap(typecheck)       QString  = char *;
// %typemap(typecheck) const QString& = char *;
// // Python -> C++
// // %typemap(in) QString {
// //     if (PyString_Check($input)) {
// //          $1 = QString(PyString_AsString($input));
// //      } else {
// //          qDebug("QString expected");
// //      }
// // }
// %typemap(in) const QString& {
// 	if (PyString_Check($input)) {
// 		char *t = PyString_AsString($input);
// 		$1 = new QString(t);
// 	} else {
// 		qDebug("QString expected");
// 	}
// }

// // // C++ -> Python
// // %typemap(out) QString {
// //     $result = PyString_FromString($1.toAscii().constData());
// // }
// // %typemap(out) const QString& {
// //     $result = PyString_FromString($1.toAscii().constData());
// // }
