PCL - PixInsight Class Library
------------------------------

[PixInsight](http://pixinsight.com/) is an advanced image processing software platform designed specifically for astrophotography and other technical imaging fields. PixInsight is a modular, open-architecture system where the entire processing and file handling capabilities are implemented as external installable modules.

The PixInsight core application provides the infrastructure on top of which external modules can implement processes, image file formats, and their associated user interfaces. The [PixInsight Class Library](http://pixinsight.com/developer/pcl/) (PCL) is a C++ development framework to build PixInsight modules.

PixInsight modules are special shared libraries (.so files on FreeBSD and Linux; .dylib under Mac OS X; .dll files on Windows) that communicate with the PixInsight core application through a high-level API provided by PCL. Along with a core communication API, PCL includes a comprehensive set of image processing algorithms, ranging from geometrical transformations to multiscale analysis algorithms, most of them available as multithreaded parallel implementations.

PCL is highly portable code. As of writing this document, it is available on the same platforms supported by the PixInsight core application: 64-bit FreeBSD, Linux, Mac OS X, and Windows. PixInsight modules written around PCL are directly portable to all supported platforms *without changing a single line of source code*. This is possible because PCL is a high-level framework. PCL isolates your module from platform-specific implementation details: all platform-dependent complexities are handled behind the scenes by the PixInsight core application and internal PCL routines.

Starting from version 2.0, PCL is released as an open-source library released under the [PixInsight Class Library License](http://pixinsight.com/license/PCL_PJSR_1.0.html) (PCLL). In essence, PCLL is a liberal BSD-like license that allows you to develop open-source and closed-source, free and commercial PixInsight modules without restrictions. As long as you observe all PCLL terms, you can modify PCL and use your modified version with or without releasing your source code.

##### Copyright (C) 2003-2014 Pleiades Astrophoto

Pleiades Astrophoto, S.L.
Apartado Postal 204
46185 Pobla de Vallbona (Valencia)
Spain
