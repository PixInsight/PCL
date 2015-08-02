PCL - PixInsight Class Library
------------------------------

[PixInsight](http://pixinsight.com/) is an advanced image processing software platform designed specifically for astrophotography and other technical imaging fields. PixInsight is a modular, open-architecture system where the entire processing and file handling capabilities are implemented as external installable modules.

The PixInsight core application provides the infrastructure on top of which external modules can implement processes, image file formats, and their associated user interfaces. The [PixInsight Class Library](http://pixinsight.com/developer/pcl/) (PCL) is a C++ development framework to build PixInsight modules.

PixInsight modules are special shared libraries (.so files on FreeBSD and Linux; .dylib under Mac OS X; .dll files on Windows) that communicate with the PixInsight core application through a high-level API provided by PCL. Along with a core communication API, PCL includes a comprehensive set of image processing algorithms, ranging from geometrical transformations to multiscale analysis algorithms, most of them available as multithreaded parallel implementations.

PCL is highly portable code. As of writing this document, it is available on the same platforms supported by the PixInsight core application: 64-bit FreeBSD, Linux, Mac OS X, and Windows. PixInsight modules written around PCL are directly portable to all supported platforms *without changing a single line of source code*. This is possible because PCL is a high-level framework. PCL isolates your module from platform-specific implementation details: all platform-dependent complexities are handled behind the scenes by the PixInsight core application and internal PCL routines.

Starting from version 2.0, which was published in December of 2012, PCL is an open-source library released under the [PixInsight Class Library License](http://pixinsight.com/license/PCL_PJSR_1.0.html) (PCLL). In essence, PCLL is a liberal BSD-like license that allows you to develop open-source and closed-source, free and commercial PixInsight modules without restrictions. As long as you observe all PCLL terms, you can modify PCL and use your modified version with or without releasing your source code.

A good starting point for developing PixInsight modules are the open sourced modules in src/modules directory. You may also want to take a look at the PCL Reference Documentation mentioned below. PixInsight and PCL are supported by a vibrant community in the [PixInsight Forum](http://forum.pixinsight.com) that also contains a developers section. Join us there!

## PCL Reference Documentation

The official reference documentation for PCL has been generated automatically from PCL source code files with the [Doxygen](http://www.stack.nl/~dimitri/doxygen/) documentation tool.

[PCL Reference Documentation - HTML Format](http://pixinsight.com/developer/pcl/doc/html/index.html)

Currently the PCL documentation is rather complete, although there are still some classes and functions lacking detailed descriptions. If you find errors or inaccuracies in our documentation, we'd like to hear you to get them fixed. If you want to collaborate with us to enhance the existing PCL documentation, you are also welcome.


## PCL Repository

This PCL repository includes all standard PCL headers and source code, along with all third-party libraries required to build PCL on the user's machine. The repository also contains the complete source code of all standard PixInsight modules that have been released as open-source products, most of them under PCLL license.

Here is a list of subdirectories, along with descriptions of the files therein. We assume that the PCL distribution has been installed on a <PCL> directory on your local filesystem; for example:

<dl><dd>
   UNIX and Linux : [PCL] = $HOME/PCL<br/>
   Windows        : [PCL] = C:\PCL
</dd></dl>

**[PCL]/include/pcl**

<dl><dd>
   Standard PCL C++ header files. These files are necessary for development of PixInsight modules.
</dd></dl>

**[PCL]/src/3rdparty**

<dl><dd>
   Some third-party libraries, mainly file format support libraries, used by the PixInsight modules included in this distribution. The CFITSIO and JasPer libraries include custom modifications, which are conveniently identified in the source code (search for the 'CUSTOM CODE' string).
</dd></dl>

**[PCL]/src/modules/file-formats**

<dl><dd>
   Open-source PixInsight file format modules with complete source code.
</dd></dl>

**[PCL]/src/modules/processes**

<dl><dd>
   Open-source PixInsight process modules, with complete source code. These source codes are ready for compilation and form an excellent body of development reference material.
</dd><dd>
   On module subdirectories under the [PCL]/src/modules branch, we provide makefiles and project files that can be used to build and test the supplied modules; these files have been automatically generated by the standard Makefile Generator script in PixInsight. They can be regenerated with the same script to adapt the build process to the user requirements.
</dd><dd>
   For each [module_name] module, makefiles and projects are available on the following subdirectories:
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/freebsd/g++**<br/>
**[PCL]/src/modules/processes/[module_name]/freebsd/g++**

<dl><dd>
   Makefiles for FreeBSD 10 with the system clang C++ compiler
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/linux/g++**<br/>
**[PCL]/src/modules/processes/[module_name]/linux/g++**

<dl><dd>
   Makefiles for Linux with GCC ]= 4.7 (4.8.2 or higher recommended)
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/macosx/g++**<br/>
**[PCL]/src/modules/processes/[module_name]/macosx/g++**

<dl><dd>
   Makefiles for Mac OS X ]= 10.8 with clang C++ compiler version ]= 5.0
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/windows/vc10**<br/>
**[PCL]/src/modules/processes/[module_name]/windows/vc10**

<dl><dd>
   Project files (.vcxproj) for Microsoft Visual C++ 2010
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/windows/vc11**<br/>
**[PCL]/src/modules/processes/[module_name]/windows/vc11**

<dl><dd>
   Project files (.vcxproj) for Microsoft Visual C++ 2012
</dd></dl>

**[PCL]/src/modules/file-formats/[module_name]/windows/vc12**<br/>
**[PCL]/src/modules/processes/[module_name]/windows/vc12**

<dl><dd>
   Project files (.vcxproj) for Microsoft Visual C++ 2013
</dd></dl>

**[PCL]/src/pcl**

<dl><dd>
   The complete source code of PixInsight Class Library (PCL). Along with the PCL headers distributed on the [PCL]/include/pcl directory, you can use these source files with the provided makefiles and project files to rebuild PCL on your system.
</dd></dl>

**[PCL]/devTools**

<dl><dd>
   Some small helpers for development on Linux. See the README.md in this directory
</dd></dl>



## Supported Compilers

The current versions of PCL have been successfully compiled and tested with:

   * FreeBSD 10: System clang compiler
   * Linux: GCC C++ compiler 4.7.4 or higher
   * Mac OS X 10.8/10.9: Clang C++ compiler - Xcode 5.0.0 or later
   * Mac OS X 10.6/10.7: Clang C++ compiler - Xcode 3.2 - 4.6.x
   * Windows XP/Vista/7/8.x: Microsoft Visual C++ 2010, 2012 and 2013

The GCC and clang C++ compilers provide higher conformance to ISO C++ standards and are high quality development tools. On FreeBSD and Mac OS X we rely on the Clang/LLVM compiler included with the latest versions of Xcode and FreeBSD.

On Windows, we provide project files for the Microsoft Visual Studio integrated development environment (.vcproj and .vcxproj files). We support Visual C++ 2010, 2012 and 2013. An excellent option on Windows is Visual C++ Express
Edition, which is free and includes a full featured development environment with an optimizing compiler. As far as we know, Express editions of Visual C++ can be used to develop commercial products.


## Environment Variables

In order to use the PCL build system to compile and link PCL-based code, a number of environment variables must be defined on all platforms.

With the following environment variables correctly defined, you can generate makefiles and project files for your modules on all platforms automatically with the Makefile Generator script, which we have included as a standard tool on all PixInsight Core distributions.

**PCLDIR**

<dl><dd>
   PCL root directory. The value of this variable must be the full directory path where the PCL distribution has been installed on your computer. On FreeBSD, Linux and Mac OS X, this variable should normally be (assuming that you have installed PCL on a 'PCL' subdirectory of your home directory):
</dd></dl>

<dl><dd><dl><dd>
   $HOME/PCL
</dd></dl></dd></dl>

<dl><dd>
   and on Windows, something like:
</dd></dl>

<dl><dd><dl><dd>
   C:\Users\[your-user-name]\PCL
</dd></dl></dd></dl>

<dl><dd>
   or maybe something more practical such as:
</dd></dl>

<dl><dd><dl><dd>
   C:\PCL
</dd></dl></dd></dl>

**PCLBINDIR32** (* Obsolete *)

<dl><dd>
   Binary files subdirectory for 32-bit executables and shared objects. This variable has been deprecated and is no longer necessary. PCL development on 32-bit platforms is no longer supported.
</dd></dl>

**PCLBINDIR64**

<dl><dd>
   Binary files subdirectory for 64-bit executables and shared objects. Normally this variable should be equal to $PCLDIR/bin.
</dd></dl>

**PCLBINDIR**

<dl><dd>
   PCL binaries directory. This variable should be equal to PCLBINDIR64.
</dd></dl>

**PCLLIBDIR32** (* Obsolete *)

<dl><dd>
   32-bit libraries subdirectory. This variable has been deprecated and is no longer necessary. PCL development on 32-bit platforms is no longer supported.
</dd></dl>

**PCLLIBDIR64**

<dl><dd>
   64-bit libraries subdirectory. This variable should normally be equal to $PCLDIR/lib/[platform]/x64 on each platform.
</dd></dl>

**PCLLIBDIR**

<dl><dd>
   PCL library files directory. This variable should be equal to PCLLIBDIR64.
</dd></dl>

**PCLINCDIR**

<dl><dd>
   PCL include files directory. Should be equal to $PCLDIR/include.
</dd></dl>

**PCLSRCDIR**

<dl><dd>
   PCL source files directory. Should be equal to $PCLDIR/src.
</dd></dl>



******
###### Copyright (C) 2003-2015 Pleiades Astrophoto
