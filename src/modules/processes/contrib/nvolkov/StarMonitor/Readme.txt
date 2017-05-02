The module required OpenGL freeglut.
Download it from http://freeglut.sourceforge.net

Extract from freeglut sources "include/GL" to "PCl/include/GL" 

build cminpack-pxi from PCL sources.

Linux:
sudo apt-get install freeglut3-dev

in 
MakeFile Script 
Libraries: cminpack-pxi glut GLU GL



windows:
MakeFile Script
Processor: CMINPACK_NO_DLL
Libraries: cminpack-pxi,
freeglut
