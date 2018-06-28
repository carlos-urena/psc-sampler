# PSC Maps

This code includes code for two maps and a tool to visualize those maps. It can be easily integrated in any renderer just by including the corresponding header library.

## Prerequisites

The code for light-source sampling (file `PSCMap.h`) has no prerequisites, as it can be compiled as is. However file `MapViewer.cpp`  (which implements a tool which visualizes the maps) requires **AntTweakBar** library, which can be obtained here: [http://anttweakbar.sourceforge.net/doc/](http://anttweakbar.sourceforge.net/doc/)

## Maps viewer tool build and usage

You can build the maps viewer tool just by typing  `make`. The corresponding `makefile` file has been tested on macOS and Linux (not yet on MS Windows), by using both GNU g++ and LLVM clang++ compilers.
Probably you'll need to set the `AntTweakBar` source folder in the `makefile`.

When running, the map viewer tool visualizes iso-curves for both maps, and the partial area curve. You can adjust the values of `alpha` and `beta` by using the sliders. You can switch between radial and parallel maps by pressing M key. You can also run numerical integration tests by pressing T key.

## Using the maps code in a renderer

If you want to use the maps in your renderer, you just need to include `PSCMaps.h`, as this is a header only, single file, templatized library (see example usage below). The map evaluates the sample position in the disc (see paper), thus, in order to obtain a sample direction, this position must be converted to world coordinates.

Here is an example code using a map instance. This computation produces `sample_dir_wc` (sample direction in world coordinates) and `area` (area of the projected spherical cap). The sample direction should be used to evaluate visibility to the light source, while the area weights the resulting radiance.

```C++
#include "PSCMaps.H"

....
using namespace PSCM ;

// object which stores map evaluation status
PSCMaps<float> pscm ;

// get the orthonormal reference frame aligned with the shading point normal
// and the spherical cap

vec3  vz  = ......., // Z axis (unit-length normal vector at shading point)
      vx = ....... , // X axis (unit-length, perp. to 'vz', in the plane of 'n' and the vector to cap center)
      vy = ....... ; // Y axis (unit length, perpendicular to 'vx' and 'vz')

// compute angle alpha and beta
const float alpha = ....... , // spherical cap aperture angle
            beta  = ....... , // spherical center's angle with local X axis

// initialize the map evaluator
// (once per each (spherical cap,shading point) pair)
pscm.initialize( alpha, beta, true );

// check if the spherical cap is visible over the horizon
if ( pscm.is_invisible() )
   return ;  

// get the area
const float area = pscm.get_area();

// evaluate the map (once per sample vector)
float s  = ....... , // s- coordinate of sample point (in [0,1])
      t  = ....... , // t-coordinate of sample point (in [0,1])
      x ,            // x- coordinate of resulting vector (in the local frame)
      y ;            // y- coordinate of resulting vector (in the local frame)
pscm.eval_map( s, t, x, y );

// convert from coordinates in the shading point reference frame to world coordinates
const vec3 sample_dir_wc = x*vx + y*vy + sqrt(1.0-x*x-y*y)*vz ;

```
