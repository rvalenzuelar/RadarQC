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

Note (03/11/2015):
After importing AirborneRadarQC into python, QString class (via PyQt) seems not
to work. For example:

```python

In [1]: import AirborneRadarQC

In [2]: from PyQt4 import QtCore as Qt

In [3]: in1=Qt.QString('uno')

In [5]: type(in1)
Out[5]: PyQt4.QtCore.QString

In [6]: in2=Qt.QString('dos')

In [7]: ss=Qt.QString('qc')

In [8]: in1
Out[8]: PyQt4.QtCore.QString(u'uno')

In [9]: in2
Out[9]: PyQt4.QtCore.QString(u'dos')

In [10]: ss
Out[10]: PyQt4.QtCore.QString(u'qc')

In [11]: dum=AirborneRadarQC.AirborneRadarQC(in1,in2,ss)
---------------------------------------------------------------------------
NotImplementedError                       Traceback (most recent call last)
<ipython-input-11-4e4db077622e> in <module>()
----> 1 dum=AirborneRadarQC.AirborneRadarQC(in1,in2,ss)

/home/raul/Github/RadarQC/AirborneRadarQC.pyc in __init__(self, *args)
     77     __repr__ = _swig_repr
     78     def __init__(self, *args):
---> 79         this = _AirborneRadarQC.new_AirborneRadarQC(*args)
     80         try: self.this.append(this)
     81         except: self.this = this

NotImplementedError: Wrong number or type of arguments for overloaded function 'new_AirborneRadarQC'.
  Possible C/C++ prototypes are:
    AirborneRadarQC::AirborneRadarQC(QString const &,QString const &,QString const &)
    AirborneRadarQC::AirborneRadarQC()
```

Note (03/17/2015):
- Passing string argument to function seems to be working.
- Passing string back to python returns a symbol (?), not a string. Need to be fixed.
