PixInsight INDI Client
======================
PixInsight INDI Client is an INDI controller client for the image processing platform [PixInsight](http://pixinsight.com/).
Using the [INDI technology](http://www.indilib.org/), it allows to control astronomical devices like telescope mounts, 
CCD cameras, filter wheels, DSLRs from PixInsight.    

![INDI Device Controller](images/INDIDeviceController.png)

Installation
------------
The client can be installed as a regular PixInsight module. To build the module from sources use the PixInsight Makefile Generator script with the 
following settings (for all OS except windows). For windows builds you have to link against the pthread library additionally.

![Makefile Generator](images/MakefileGeneartor_Module.png)

