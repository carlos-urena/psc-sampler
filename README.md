# PSC Maps

This repository is (currently) just a place-holder for code implementing ideas included in an upcoming paper about direct light source sampling. The code includes code for two maps and a tool to visualize those maps. It can be easily integrated in any renderer just by including the corresponding header library.

## Prerequisites

The code for light-source sampling (files <tt>PSCMap.h</tt> and <tt>PSCMap_impl.h</tt>) has no prerequisites, as it can be compiled as is. However file <tt>MapViewer.cpp</tt>  (which implements a tool which visualizes the maps) requires <tt>anttweakbar</tt> library, which can be obtained here: [http://anttweakbar.sourceforge.net/doc/]

## Maps viewer tool build

You can build the maps viewer tool just by typing <tt>make</tt>. The corresponding <tt>makefile</tt> file has been tested on macOS and Linux (not yet on MS Windows), by using both GNU g++ and LLVM clang++ compilers. If you want to use the maps in your renderer, you can just include <tt>PSCMaps.h</tt>, as this is a header only, templatized library (see example usage below)

##  Example maps usage

... to do....
