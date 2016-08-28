.. CDTStudio documentation master file, created by
   sphinx-quickstart on Mon Jul 25 15:00:45 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to CDTStudio's documentation!
=====================================

**CDTStudio** is an open-source and cross-platform desktop software in object-based image analysis (OBIA or GeoOBIA). The first version of it is written by Chen Guanzhou (`English <https://github.com/chenguanzhou>`_, `中文 <http://www.chenguanzhou.com/about>`_).

**CDTStudio** is based on Qt4 framework, in which most algorithms are in the form of plugins. **CDTStudio** can be run in Windows & Ubuntu (Other linux distributions and MacOS have not been tested).

It contains the following GeoOBIA framework:

- Remote sensing image segmentation
- Object-based image analysis (OBIA) & Object-based image classification (OBIC)
- Pixel-based change detection (PBCD) & Object-based change detection (OBCD)
- Semi-automatic feature extraction by Snakes

.. toctree::
   :maxdepth: 1
   :caption: User Documentation
   
   features
   tutorials
   
.. toctree::
   :maxdepth: 1
   :caption: Developer Documentation
   
   3rdparty
   build_windows
   write_custom_plugins
   
.. toctree::
   :maxdepth: 1
   :caption: About Documentation
   
   screenshots
   license
   about

