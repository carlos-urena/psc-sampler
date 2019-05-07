// *********************************************************************
// **
// ** Projected Spherical Cap Sampling
// ** Interactive map viewer
// **
// ** Copyright (C) 2018 Carlos Ureña and Iliyan Georgiev
// **
// ** Licensed under the Apache License, Version 2.0 (the "License");
// ** you may not use this file except in compliance with the License.
// ** You may obtain a copy of the License at
// **
// **    http://www.apache.org/licenses/LICENSE-2.0
// **
// ** Unless required by applicable law or agreed to in writing, software
// ** distributed under the License is distributed on an "AS IS" BASIS,
// ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// ** See the License for the specific language governing permissions and
// ** limitations under the License.

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cassert>
#include <vector>

#include <PSCMaps.h> // maps implementation

#define GL_SILENCE_DEPRECATION // needed on latest macOS OS versions

#include <GLFW/glfw3.h>  // GLFW
#include <AntTweakBar.h> // Anttweak bar GUI

using namespace PSCM ; // projected spherical cap map namespace (see 'PSCMaps.h')
using namespace std ;

#include <GVec.h>


// --------------------------------------------------------------------------
// type definitions

typedef double   // use either double or float here
   scalar ;
typedef TuplaG3<scalar>
   Vec3s ;       // type for vectors
typedef Tupla2f
   Vec2f ;       // type for 2d float vectors (mainly for OpenGL drawing)
// --------------------------------------------------------------------------
// compile-time constants

constexpr int
   initial_win_width_xl  = 1024 , // initial window width
   initial_win_height_xl = 1024 , // initial window height
   left_margin           = 250 ;  // left margin (room for the tweak bar)
constexpr float
   vp_width_wc       = 2.1 ,        // viewport width in world coordinates
   ref_line_val_min  = 0.0,         // line used to debug inversion: min. value
   ref_line_val_max  = 1.0 ,        // line used to debug inversion: max. value
   ref_line_val_step = 0.002,       // line used to debug inversion: increment
   alpha_min         = 0.0,         // minimum value for alpha in the tweak bar
   alpha_max         = M_PI*0.499f, // maximum value for alpha in the tweak bar (almost PI/2)
   alpha_step        = 0.001,       // increment for alpha in the tweak bar
   beta_min          = -M_PI*0.5f,  // minimum value for beta in the tweak bar
   beta_max          =  M_PI*0.5f,  // maximum value for beta in the tweak bar
   beta_step         = 0.001 ;      // increment for alpha in the tweak bar

// --------------------------------------------------------------------------
// variables

bool
   mode_radial   = false ,     // false --> use parallel map, true -> use radial map
   paramsChanged = true ;      // parameters updated after last scene redraw (forces sampler reinitialization before draw)
int
   win_width_xl     = -1,      // current window width (in pixels)
   win_height_xl    = -1,      // current window height (in pixels)
   mouse_pos_factor =  1 ;     // conv. factor for adjusting retina displays window size with GLFW
float
   alpha            = 0.4,     // current value for alpha parameter
   beta             = 0.4,     // current value for beta parameter
   ref_line_val     = 0.5;     // argument used to test Newton inversion
PSCMaps<scalar>
   sampler ;                   // sampler in use
GLFWwindow *
   glfw_window      = nullptr; // GLFW window:

// --------------------------------------------------------------------------
// draw a sequence of vertexes stored in a vector, using any given OpenGL mode

inline void DrawVertexes( const GLint gl_mode, const std::vector<Vec2f> & vertexes )
{
   glVertexPointer( 2, GL_FLOAT, 0, vertexes.data() );
   glEnableClientState( GL_VERTEX_ARRAY );
   glDrawArrays( gl_mode, 0, vertexes.size() );
   glDisableClientState( GL_VERTEX_ARRAY );
}
// --------------------------------------------------------------------------
// draw a sequence of vertexes, each one is computed by using a function of
// a float value which goes from 0 to 1 in 'n' steps
// (that is: draw a parametric curve)

inline void DrawCurve( const GLint gl_mode, const int n,
                       const function<Vec2f(float)> & C )
{
   assert( 1 < n );
   vector<Vec2f> vertexes ;
   for( int i = 0 ; i < n ; i++ )
   {
      const float t = float(i)/float(n-1) ;
      vertexes.push_back( C(t) );
   }
   DrawVertexes( gl_mode, vertexes );
}

// --------------------------------------------------------------------------

void DrawIsoCurves(  )
{
   constexpr int  n        = 16,
                  m        = n*128 ;
   constexpr bool use_dots = false ;

   glLineWidth( 1.2 );
   glPointSize( 1.3 );
   glColor3f( 0.0, 0.5, 0.5 );

   const GLenum mode = use_dots ? GL_POINTS : GL_LINE_STRIP ;

   // lines with constant 't' (straight horizontal/radial lines)
   for( int i = 0 ; i <=n  ; i++ )
   {
      const scalar t = scalar(i)/scalar(n) ;
      DrawCurve( mode, n+1, [&]( float s )
      {
         scalar x,y ;
         sampler.eval_map( s, t, x, y );
         return Vec2f { float(x), float(y) };
      });
   }

   // lines with constant 's' (curves)
   for( int i = 0 ; i <=n  ; i++ )
   {
      const scalar s = scalar(i)/scalar(n) ;
      DrawCurve( mode, m+1, [&]( float t )
      {
         scalar x,y ;
         sampler.eval_map( s, t, x, y );
         return Vec2f { float(x), float(y) };
      });
   }
}
// --------------------------------------------------------------------------

void DrawRadialSegments_Ellipse( const int n )
{
   vector<Vec2f> vertexes ;
   for( int i= 0 ; i <= n ; i++ )
   {
      const scalar theta = M_PI*scalar(i)/scalar(n) ;
      vertexes.push_back( { float(sampler.get_xe()),  0.0f } );
      vertexes.push_back( { float(sampler.get_xe() + sampler.get_ax()*std::cos(theta) ),
                            float(sampler.get_ay()*std::sin(theta) ) } );
   }
   DrawVertexes( GL_LINES, vertexes );
}
// --------------------------------------------------------------------------
// draw circle outline

void DrawUnitCircle()
{
   //static bool first = true ;
   constexpr int n = 256 ;

   glLineWidth( 2.3 );
   glColor3f( 0.4, 0.0, 0.0 );

   DrawCurve( GL_LINE_LOOP, n, [&]( float t )
   {  return Vec2f { cos( 2.0f*float(M_PI)*t ), sin( 2.0f*float(M_PI)*t ) };
   });
}
// --------------------------------------------------------------------------
// draw the ellipse outline

void DrawEllipse( float xe, float hy, float ax, float ay )
{
   constexpr int n = 256 ;

   glLineWidth( 2.3 );
   glColor3f( 0.7, 0.0, 0.0 );

   // draw pts in the ellipse outline
   DrawCurve( GL_LINE_LOOP, n, [&]( float t )
   {  const float angr = 2.0f*M_PI*t ;
      return  Vec2f { xe+ax*cos(angr), hy+ay*sin(angr) } ;
   } );

   // vertical line through ellipse center
   glLineWidth( 1.0 );
   DrawVertexes( GL_LINES, { { xe, -0.2}, {xe, +0.2} } );
}
// --------------------------------------------------------------------------
// draw dots at the two ellipse-circle tangency points at (xl,yl) and (xl,-yl)

void DrawTangencyPoints( float xl, float yl )
{
   glPointSize( 7.0 );
   glColor3f( 0.0,0.0,0.0);
   DrawVertexes( GL_POINTS, { {xl,yl}, {xl,-yl} } );
}
// --------------------------------------------------------------------------
// draw 2D X and Y axes

void DrawAxes()
{
   const float m = 0.2 ;
   glColor3f( 0.5, 0.5, 0.5 );
   DrawVertexes( GL_LINES, { { -m, 0.0 }, { 1.0+m, 0.0 },
                             { 0.0, -m }, { 0.0, 1.0+m } } );
}
// --------------------------------------------------------------------------
// draw a box with function Ap or Ar, just to see its shape

void DrawIntegralCurve( const int x0m, const int y0m, const int wxm, const int wym )
{

   const int margin = int( 0.1f*float(wxm) ) ;
   const int x0 = x0m + margin/2,
             y0 = y0m + margin/2,
             wx = wxm/2 -margin/2,
             wy = wym/2  -margin ;

   // setup viewport and transformation matrix
   glViewport( x0, y0, wx, wy );

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho( -0.1, +1.1, -0.1, +1.1, -1.0, +1.0 );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // draw filled box
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   glColor3f( 0.9, 0.9, 0.9 );
   DrawVertexes( GL_POLYGON,   {{-0.1, -0.1}, {-0.1, 1.1}, {1.1, 1.1}, {1.1, -0.1} });

   // draw box outline
   glColor3f( 0.0, 0.0, 1.0 );
   DrawVertexes( GL_LINE_LOOP, {{-0.1, -0.1}, {-0.1, 1.1}, {1.1, 1.1}, {1.1, -0.1} });

   // draw box outline around function curve
   glColor3f( 0.0, 0.0, 0.0 );
   DrawVertexes( GL_LINES, { {-0.1, 0.0}, {1.1, 0.0}, {0.0, -0.1}, {0.0, 1.1} } );

   assert( sampler.is_initialized() );
   if ( sampler.is_invisible() )
      return ;

   constexpr int n = 1024 ;

   // [0,1]^2 rectangle
   glLineWidth( 1.0 );
   glColor3f( 1.0, 0.0, 0.0 );
   DrawVertexes( GL_LINE_LOOP, {{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}} );

   const scalar max_area = sampler.get_area()*scalar(0.5) ;

   DrawCurve( GL_LINE_STRIP, n+1, [&]( float t )
   {
      const scalar I = sampler.is_using_radial()
         ? sampler.eval_Ar( t*scalar(M_PI)     ) / max_area
         : sampler.eval_Ap( t*sampler.get_ay() ) / max_area ;
      return Vec2f { t, (float)I } ;
   } );

   // for the ellipse+lune and lune cases, draw vertical red line at yl or phi_l
   if ( ! sampler.is_fully_visible() )
   {
      const float x = ( sampler.is_using_radial() )
                        ? sampler.get_phi_l()/M_PI
                        : sampler.get_yl()/sampler.get_ay() ;
      glColor3f( 1.0, 0.0, 0.0 );
      DrawVertexes( GL_LINES, { {x,0.0}, {x,1.0} } );
   }

   // for the lune only case, draw a parabola...
   if ( ! sampler.is_fully_visible() && sampler.is_center_below_hor() )
   {
      glColor3f( 0.0, 0.5, 0.0 );
      const float limx = ( sampler.is_using_radial() )
                        ? sampler.get_phi_l()/M_PI
                        : sampler.get_yl()/sampler.get_ay() ;
      DrawCurve( GL_LINES, n, [&]( float t )
      {  return Vec2f { t*limx, (1.0f-(1.0f-t)*(1.0f-t)) };
      });
   }

   // draw vertical blue line at ref_line_val
   glColor3f( 0.0, 0.0, 1.0 );
   DrawVertexes( GL_LINES, { {ref_line_val, 0.0}, {ref_line_val, 1.0} } );

   // trace newton inversion at ref_line_val angle
   if ( sampler.is_using_radial() )
   {
      const scalar
         theta_ref     = ref_line_val*scalar(M_PI) ,
         Ar_theta_ref  = sampler.eval_Ar( theta_ref ) ;

      cout << "Test for RADIAL inversion at theta == " << theta_ref <<  " (blue radius)" << endl ;

      Vars<scalar>::trace_newton_inversion = true ;
      const scalar theta_inv = sampler.eval_Ar_inverse( Ar_theta_ref );
      Vars<scalar>::trace_newton_inversion = false ;

      cout << "-----" << endl
           << "theta ref  == " << theta_ref << endl
           << "theta inv  == " << theta_inv << endl
           << "diff       == " << std::abs( theta_ref-theta_inv ) << endl ;
   }
   else
   {
      const scalar
         y_ref     = ref_line_val*sampler.get_ay() ,
         Ap_y_ref  = sampler.eval_Ap( y_ref ) ;

      cout << "Test for PARALLEL inversion at y == " << y_ref <<  ", Ap(y_ref) == " << Ap_y_ref << " (blue line)" << endl ;

      Vars<scalar>::trace_newton_inversion = true ;
      const scalar y_inv = sampler.eval_Ap_inverse( Ap_y_ref );
      Vars<scalar>::trace_newton_inversion = false ;

      cout << "-----" << endl
           << "y ref     == " << y_ref << endl
           << "y inv     == " << y_inv << endl
           << "diff      == " << std::abs( y_ref-y_inv ) << endl ;
   }
}

// --------------------------------------------------------------------------
// draw everything

void DrawScene( )
{

   if ( paramsChanged )
   {
      // initialization (based only on alpha and beta)
      sampler.initialize( alpha, beta, mode_radial );
      paramsChanged = false ; // force redraw
   }

   assert( 0 < win_width_xl );
   assert( 0 < win_height_xl );

   // Clear frame buffer
   glClearColor( 1.0, 1.0, 1.0, 1.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // set up simple 2D drawing, no Z-buffer, no textures, no local illum. model
   glDisable( GL_CULL_FACE );
   glDisable( GL_LIGHTING );
   glDisable( GL_TEXTURE );
   glDisable( GL_DEPTH_TEST );

   // draw anti-aliased lines and polygons
   glEnable( GL_LINE_SMOOTH );
   glEnable( GL_POINT_SMOOTH );
   glEnable( GL_POLYGON_SMOOTH );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   // Set OpenGL viewport and camera
   const int vp_width_xl  = std::max( 10, win_width_xl-left_margin ),
             vp_height_xl = win_height_xl ;

   glViewport( left_margin, 0, vp_width_xl, vp_height_xl );

   // set projection and modelview matrix
   const float ratioyx = float(vp_height_xl)/float(vp_width_xl),
               w2      = 0.5*vp_width_wc ,
               rx      = std::max( 1.0f, 1.0f/ratioyx ),
               ry      = std::max( 1.0f, ratioyx     );

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho( -rx*w2, +rx*w2, -ry*w2, +ry*w2, -1.0, +1.0 );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   DrawAxes();
   DrawUnitCircle();

   if ( sampler.is_invisible() )
   {
      cout << "sphere invisible (not drawing nothing)" << endl ;
      return ;
   }

   /// draw all the iso curves
   DrawIsoCurves(  );

   Vars<scalar>::print_settings();
   sampler.debug();

   // draw the ellipse
   DrawEllipse( sampler.get_xe(), 0.0, sampler.get_ax(), sampler.get_ay() );

   // if appropiate, draw tangency points
   if ( sampler.is_partially_visible() )
      DrawTangencyPoints( sampler.get_xl(), sampler.get_yl() );


   // draw radial line at angle ref_line_val*M_PI
   // or the horizontal line at height ref_line_val*ay

   glLineWidth( 1.3 );
   glColor3f( 0.0, 0.0, 1.0 );
   if ( sampler.is_using_radial() )
   {
      const scalar
         cox = 1.5*std::cos( ref_line_val*scalar(M_PI) ),
         coy = 1.5*std::sin( ref_line_val*scalar(M_PI) );
      DrawVertexes( GL_LINES, { { float( sampler.get_xe() ),     0.0f },
                                { float( sampler.get_xe()+cox ), float(coy) } } );
   }
   else
   {
      const scalar yref = ref_line_val*sampler.get_ay() ;
      DrawVertexes( GL_LINES, { {-1.0f, float(yref) }, {1.0f, float(yref) } } );
   }

   // draw the curve integral (changes viewport)
   DrawIntegralCurve( left_margin, 0, vp_width_xl, vp_width_xl );

}
// --------------------------------------------------------------------------
// Function called from main loop to render screen

void Display( )
{
   static int display_count = 0 ;
   display_count++ ;

   glfwMakeContextCurrent( glfw_window );
   DrawScene();
   TwDraw();
   glfwSwapBuffers( glfw_window );
}

// --------------------------------------------------------------------------
// Callback function called by GLFW when window size changes

void Reshape_CB( GLFWwindow* window, int new_width, int new_height )
{
   paramsChanged = true ;

   int fbx, fby ;
   glfwGetFramebufferSize( glfw_window, &fbx, &fby );

   win_width_xl  = fbx ;
   win_height_xl = fby ;

   TwWindowSize( win_width_xl, win_height_xl );
}

// --------------------------------------------------------------------------

// Function called at exit
void Terminate(void)
{
   TwTerminate();
}

// --------------------------------------------------------------------------

void MouseButton_CB( GLFWwindow* window, int button, int action, int mods )
{
   if ( TwEventMouseButtonGLFW( button, action ) )
      paramsChanged = true ;
}
// --------------------------------------------------------------------------

void MousePosition_CB( GLFWwindow* window, double xpos, double ypos )
{
   if ( TwEventMousePosGLFW( xpos*mouse_pos_factor, ypos*mouse_pos_factor ) )
      paramsChanged = true ;
}

// --------------------------------------------------------------------------

void Keyboard_CB( GLFWwindow* window, int key, int scancode, int action, int mods )
{
   if ( TwEventKeyGLFW( key, action ) )  // if ATB handled it, do nothing more and refresh
   {
      paramsChanged = true ;
      return ;
   }
   if ( action != GLFW_PRESS )
      return ;

   if ( key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE )  // q,Q or ESC
      exit( 0 );
   else if ( key ==  GLFW_KEY_T )
      sampler.run_test_integrals(  ) ;
   else if ( key ==  GLFW_KEY_D )
      sampler.debug();
   else if ( key == GLFW_KEY_M )
   {
      mode_radial = ! mode_radial ;
      cout << "mode changed to: " << (mode_radial ? "radial" : "horizontal") << endl ;
      paramsChanged = true ;
   }
}
// --------------------------------------------------------------------------

void Special_CB( int key, int x, int y )
{
   if ( TwEventSpecialGLUT( key, x, y ) )
      paramsChanged = true ;
}

// --------------------------------------------------------------------------

void CreateATBWidgets()
{
   // initialize AntTweakBar
   TwInit( TW_OPENGL, nullptr );

   // Create a tweak bar
   auto * bar = TwNewBar("Params");

   // Add Message to the help bar, change default tweak bar size and color
   TwDefine(" GLOBAL help='This tool allows to test both area-preserving maps' ");
   TwDefine(" Params size='200 200' color='10 10 60' text=light alpha=210 fontsize=3");

   // parameters alpha and beta
   TwAddVarRW( bar, "alpha", TW_TYPE_FLOAT, &alpha, "help='spherical cap aperture'" );
   TwSetParam( bar, "alpha", "min",  TW_PARAM_FLOAT, 1, &alpha_min );
   TwSetParam( bar, "alpha", "max",  TW_PARAM_FLOAT, 1, &alpha_max );
   TwSetParam( bar, "alpha", "step", TW_PARAM_FLOAT, 1, &alpha_step );

   TwAddVarRW( bar, "beta", TW_TYPE_FLOAT, &beta, "help='to_center angle with local X axis'" );
   TwSetParam( bar, "beta", "min",  TW_PARAM_FLOAT, 1, &beta_min );
   TwSetParam( bar, "beta", "max",  TW_PARAM_FLOAT, 1, &beta_max );
   TwSetParam( bar, "beta", "step", TW_PARAM_FLOAT, 1, &beta_step );

   // value used to test Newton inversion
   TwAddVarRW( bar, "ref_line_val", TW_TYPE_FLOAT, &ref_line_val, "help='reference line'" );
   TwSetParam( bar, "ref_line_val", "min",  TW_PARAM_FLOAT, 1, &ref_line_val_min );
   TwSetParam( bar, "ref_line_val", "max",  TW_PARAM_FLOAT, 1, &ref_line_val_max );
   TwSetParam( bar, "ref_line_val", "step", TW_PARAM_FLOAT, 1, &ref_line_val_step );


   // initialy, inform ATB about the window size
   TwWindowSize( win_width_xl, win_height_xl );
}
// --------------------------------------------------------------------------

void InitializeGLFW(  )
{
   // Initialize the library
   if ( ! glfwInit() )
   {
      cout << "Error: unable to initialize GLFW" << endl ;
      exit(1) ;
   }

   // Create a windowed mode window and its OpenGL context
   glfw_window = glfwCreateWindow( initial_win_width_xl, initial_win_height_xl,
                    "Projected Disk Sampling (horizontal map)", nullptr, nullptr );

   // Make the window's context current
   glfwMakeContextCurrent( glfw_window );

   //win_width_xl  = initial_win_width_xl ;
   //win_height_xl = initial_win_height_xl ;

   int fbx, fby, wx, wy ;
   glfwGetWindowSize( glfw_window, &wx, &wy );
   glfwGetFramebufferSize( glfw_window, &fbx, &fby );

   win_width_xl  = fbx ;
   win_height_xl = fby ;
   mouse_pos_factor = fbx/wx ;  // on macos retina displays, this seems to be '2'

   //cout << "MOUSE POS FACTOR == " << mouse_pos_factor << endl ;

   if ( ! glfw_window )
   {
      cout << "Error: unable to create GLFW window!" << endl ;
      glfwTerminate();
      exit(1);
   }

   // define the functions to call when polling events
   glfwSetWindowSizeCallback ( glfw_window, Reshape_CB );
   glfwSetKeyCallback        ( glfw_window, Keyboard_CB );
   glfwSetMouseButtonCallback( glfw_window, MouseButton_CB );
   glfwSetCursorPosCallback  ( glfw_window, MousePosition_CB );
}

// --------------------------------------------------------------------------
// Main function

int main( int argc, char *argv[] )
{
   cout << "Map viewer (press 'M' to switch from horizontal to radial map mode and back)" << endl ;

   InitializeGLFW();
   CreateATBWidgets();

   while ( ! glfwWindowShouldClose( glfw_window ) )
   {
      if ( paramsChanged )
         Display();       // draw scene and widgets
      glfwWaitEvents();   // wait and then process events

   }
   glfwTerminate();

   return 0;
}
