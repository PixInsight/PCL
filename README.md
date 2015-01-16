PixInsight INDI Device Controller
=================================

![Build Status Image](https://travis-ci.org/kkretzschmar/PixInsightINDIclient.svg?branch=master)

PixInsight INDI Device Controller is an INDI controller client for the image processing platform [PixInsight](http://pixinsight.com/).
Using the [INDI technology](http://www.indilib.org/), it allows to control astronomical devices like telescope mounts, 
CCD cameras, filter wheels, DSLRs from PixInsight.    

![INDI Device Controller](images/INDIDeviceController.png)

Installation
------------
The client can be installed as a regular PixInsight module. To build the module from sources use the PixInsight Makefile Generator script with the 
following settings (for all Linux and MacOS): 

![Makefile Generator](images/MakefileGeneartor_Module.png)

And for windows builds:

![Makefile Generator](images/MakefileGeneartor_Module_Windows.png)