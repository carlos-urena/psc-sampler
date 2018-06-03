# PSC Maps

This repository is (currently) just a place-holder for code implementing ideas included in an upcoming paper about direct light source sampling. The code includes code for two maps and a tool to visualize those maps. It can be easily integrated in any renderer just by including the corresponding header library.

## Prerequisites

The code for light-source sampling (files `PSCMap.h` and `PSCMap_impl.h`) has no prerequisites, as it can be compiled as is. However file `MapViewer.cpp`  (which implements a tool which visualizes the maps) requires **AntTweakBar** library, which can be obtained here: [http://anttweakbar.sourceforge.net/doc/](http://anttweakbar.sourceforge.net/doc/)

## Maps viewer tool build

You can build the maps viewer tool just by typing  `make`. The corresponding `makefile` file has been tested on macOS and Linux (not yet on MS Windows), by using both GNU g++ and LLVM clang++ compilers. If you want to use the maps in your renderer, you can just include `PSCMaps.h`, as this is a header only, templatized library (see example usage below)

##  Example maps usage

... to do....
