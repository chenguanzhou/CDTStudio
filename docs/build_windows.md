Build on Windows
======

## 1. OSGeo4W

> [`OSGeo4W`](http://trac.osgeo.org/osgeo4w/) is a binary distribution of a broad set of open source geospatial software for Win32 environments (Windows XP, Vista, etc). 

To build CDTStudio on Windows, we need to install 3rdParty libraries including QGIS, GDAL, OpenCV and Qt4.8.5 from the advanced mode of OSGeo4W.

PS: Please build CDTStudio and its dependent libraries in 64bit (x64) environment.

## 2. Build STxxL

> The core of [`STxxL`](http://stxxl.sourceforge.net/) is an implementation of the C++ standard template library STL for external memory (out-of-core) computations, i. e., STXXL implements containers and algorithms that can process huge volumes of data that only fit on disks. While the closeness to the STL supports ease of use and compatibility with existing applications, another design priority is high performance. 

Due to limitations of OSGeo4W on Windows, STxxl must be built by VS2010 with [Boost](http://www.boost.org/).
[This](http://stxxl.sourceforge.net/tags/master/install_windows_boost.html) is the stxxl build documentation.

## 3. QWT6.0

The [Qwt](http://qwt.sourceforge.net/) library contains GUI Components and utility classes which are primarily useful for programs with a technical background. The build tutorial is [here](http://qwt.sourceforge.net/qwtinstall.html#qwtinstall-windows).

## 4. Environment variable configuration 
Create new environment variables:

* OSGEO4W_ROOT

* BOOST_LIBRARYDIR

* BOOST_ROOT

* QGIS_ROOT

* QWT_ROOT

* STXXL_ROOT

The value of each variable is the root directory of the library. (e.g., OSGEO4W_ROOT=C:\OSGeo4W64)

Then, create a new environment variables `QT_PLUGIN_PATH`, whose value is the plugins directory of Qt. (e.g., C:\OSGeo4W64\apps\Qt4\plugins)

In addition，add the bin directory of each library to `Path` environment variable. (e.g., C:\OSGeo4W64\bin;C:\OSGeo4W64\apps\qgis\bin;C:\qwt6\bin;C:\boost_1_55_0\lib64-msvc-10.0;)

## 5. Build
Build CDTStudio in Release mode in [Qt Creator](http://www.qt.io/download-open-source/). (Debug mode has not been supported yet)

## 6. Copy the plugins of QGIS
Copy gdalprovider.dll and ogrprovider.dll (in `plugins` dir of QGIS, e.g., C:\OSGeo4W64\apps\qgis\plugins) to `build_path/bin/Plugins` of CDTStudio.

## 7. Run