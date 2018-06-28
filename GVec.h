// *********************************************************************
// **
// ** GVec.h
// ** Template for homogeneous tuples with numeric values
// ** 
// ** Copyright (C) 2014 Carlos Ureña
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

#ifndef GVEC_H
#define GVEC_H

#include <cmath>
#include <cassert>
#include <iostream>

// constantes para acceder a las componentes de las tuplas
const unsigned
   X = 0,
   Y = 1,
   Z = 2,

   R = 0,
   G = 1,
   B = 2 ;

// definir alias de 'unsigned int' cuyo descriptor tiene un solo token
typedef unsigned int uint ;

// *********************************************************************
//
// plantilla de clase: TuplaG
// clase para tuplas genéricas
//
// *********************************************************************

template< class T, unsigned n >
class TuplaG
{
   private:
   T coo[n] ;  // vector de valores escalares

   public:
   // constructor por defecto: no hace nada
   inline TuplaG();

   // constructor usando un array C++
   inline TuplaG( const T * org ) ;

   // acceso de lectura/escritura a un elemento (v[i]=x, x=v[i])
   //T & operator [] (const unsigned i) ;

   // acceso de solo lectura a un elemento ( x=v(i) )
   const T & operator () (const unsigned i) const ;

   // acceso de lectura-escritura a un elemento ( v(i)=x )
   T & operator () (const unsigned i) ;

   // conversion a un puntero de lectura/escritura de tipo T* ( T* p = tupla )
   operator  T * ()  ;

   // conversion a un puntero de solo lectura de tipo: const  T* ( const T* p = tupla )
   operator  const T * ()  const ;

   // suma componente a componente ( v1=v2+v3 )
   TuplaG<T,n> operator + ( const TuplaG & der ) const ;

   // resta componente a componente ( v1=v2-v3 )
   TuplaG<T,n> operator - ( const TuplaG & der ) const ;

   // devuelve tupla negada ( v1 = -v2 )
   TuplaG<T,n> operator - (  ) const ;

   // mult. por escalar por la derecha ( v1=v2*a )
   TuplaG<T,n> operator * ( const T & a ) const ;

   // division por escalar ( v1=v2/a )
   TuplaG<T,n> operator / ( const T & a ) const ;

   // producto escalar (dot)  a = v1.dot(v2)
   T dot( const TuplaG<T,n> & v2 ) const ;

   // operador binario para producto escalar a = v1|v2 ;
   T operator | ( const TuplaG & der ) const ;

   // obtener longitud al cuadrado
   T lengthSq( ) const ;

   // obtener longitud
   T length( ) const ;

   // obtener una copia normalizada
   TuplaG<T,n> normalized() const ;
} ;

//----------------------------------------------------------------------
// operadores y funciones no miembros

// mult. por escalar por la izquierda ( v1=a*v2 )
template< class T, unsigned n >
inline TuplaG<T,n> operator *  ( const T & a, const  TuplaG<T,n> & der ) ;

// escritura de un vector en un ostream
template< class T, unsigned n >
inline std::ostream & operator <<  ( std::ostream & os, const TuplaG<T,n> & der ) ;

// *********************************************************************
//
// Plantilla de clase: TuplaG2
// especialización parcial para tuplas de 2 elementos
// (define constructores específicos)
//
// *********************************************************************

template< class T >
class TuplaG2 : public TuplaG<T,2>
{
   public:

   // constructores: por defecto
   TuplaG2() ;
   TuplaG2( const T & c0, const T & c1 ) ;
   TuplaG2( const TuplaG<T,2> & ini );
   void operator = ( const TuplaG<T,2> & der ) ;
} ;



// *********************************************************************
//
// Plantilla de clase: TuplaG3
// especialización parcial para tuplas de 3 elementos
// (define constructores específicos)
//
// *********************************************************************

template< class T >
class TuplaG3 : public TuplaG<T,3>
{
   public:

   // constructores: por defecto
   TuplaG3() ;
   TuplaG3( const T & c0, const T & c1, const T & c2 ) ;
   TuplaG3( const TuplaG<T,3> & ini );
   void operator = ( const TuplaG<T,3> & der ) ;
   void operator = ( const TuplaG<T,4> & der ) ; // asignar ignorando ultimo

   // producto vectorial (cross)  a = v1.cross(v2)
   TuplaG3<T> cross( const TuplaG3<T> & v2 ) const ;
} ;


// *********************************************************************
//
// Plantilla de clase: TuplaG4
// especialización parcial para tuplas de 4 elementos
// (define constructores específicos)
//
// *********************************************************************

template< class T >
class TuplaG4 : public TuplaG<T,4>
{
   public:

   // constructores: por defecto
   TuplaG4() ;
   TuplaG4( const T & c0, const T & c1, const T & c2, const T & c3 ) ;
   TuplaG4( const TuplaG<T,4> & ini );
   void operator = ( const TuplaG<T,4> & der ) ;
} ;



// *********************************************************************
// tipos concretos:

typedef TuplaG2<float>  Tupla2f ;
typedef TuplaG2<double> Tupla2d ;
typedef TuplaG2<uint>   Tupla2u ;
typedef TuplaG2<int>    Tupla2i ;

typedef TuplaG3<float>  Tupla3f ;
typedef TuplaG3<double> Tupla3d ;
typedef TuplaG3<uint>   Tupla3u ;
typedef TuplaG3<int>    Tupla3i ;

typedef TuplaG4<float>  Tupla4f ;
typedef TuplaG4<double> Tupla4d ;
typedef TuplaG4<int>    Tupla4i ;
typedef TuplaG4<uint>   Tupla4u ;


// *********************************************************************
//
// Implementation of methods and operators
//
// *********************************************************************


template< class T, unsigned n> inline
TuplaG<T,n>::TuplaG()
{

}

// constructor usando un array C++
template< class T, unsigned n> inline
TuplaG<T,n>::TuplaG( const T * org )
{
   for( unsigned i = 0 ; i < n ; i++ )
      (*this)[i] = org[i] ;
}

//----------------------------------------------------------------------

//template< class T, unsigned n >
//T & TuplaG<T,n>::operator [] (const unsigned i)
//{
   //assert( i < n ) ;
   //return coo[i] ;
//}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
const T & TuplaG<T,n>::operator () (const unsigned i) const
{
   assert( i < n ) ;
   return coo[i] ;
}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
T & TuplaG<T,n>::operator () (const unsigned i)
{
   assert( i < n ) ;
   return coo[i] ;
}

//----------------------------------------------------------------------
// conversion a un puntero de lectura/escritura de tipo: T* ( T* p = tupla )

template< class T, unsigned n > inline
TuplaG<T,n>::operator  T * ()
{
   return coo ;
}

//----------------------------------------------------------------------
// conversion a un puntero de solo lectura de tipo T* ( const T* p = tupla )

template< class T, unsigned n > inline
TuplaG<T,n>::operator  const T * () const
{
   return coo ;
}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
TuplaG<T,n> TuplaG<T,n>::operator + ( const TuplaG<T,n> & der ) const
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = (*this)(i)+der(i) ;
   return res ;
}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
TuplaG<T,n> TuplaG<T,n>::operator - ( const TuplaG<T,n> & der ) const
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = (*this)(i)-der(i) ;
   return res ;
}

//----------------------------------------------------------------------

// devuelve tupla negada ( v1 = -v2 )
template< class T, unsigned n > inline
TuplaG<T,n> TuplaG<T,n>::operator - (  ) const
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = -(*this)(i) ;
   return res ;

}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
TuplaG<T,n> TuplaG<T,n>::operator * ( const T & a ) const
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = (*this)(i)*a ;
   return res ;
}

//----------------------------------------------------------------------

template< class T, unsigned n >  inline
TuplaG<T,n> TuplaG<T,n>::operator / ( const T & a ) const
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = (*this)(i)/a ;
   return res ;
}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
TuplaG<T,n> operator * ( const T & a, const TuplaG<T,n> & der )
{
   TuplaG<T,n> res ;
   for( unsigned i = 0 ; i < n ; i++ )
      res[i] = a*der(i) ;
   return res ;
}

//----------------------------------------------------------------------

template< class T, unsigned n > inline
std::ostream & operator <<  ( std::ostream & os, const TuplaG<T,n> & der )
{
   os << "(" ;
   for( unsigned i = 0 ; i < n ; i++ )
   {  os << der(i) ;
      if ( i+1 < n )
         os << "," ;
   }
   os << ")" ;
   return os ;
}

//----------------------------------------------------------------------

// producto escalar (dot)  a = v1.dot(v2)
template< class T, unsigned n > inline
T TuplaG<T,n>::dot( const TuplaG<T,n> & v2 ) const
{
   double res = 0.0 ;
   for( unsigned int i = 0 ; i < n ; i++ )
      res += double((*this)(i)) * double(v2(i)) ;
   return T(res) ;
}

//----------------------------------------------------------------------

// obtener longitud al cuadrado
template< class T, unsigned n > inline
T TuplaG<T,n>::lengthSq( ) const
{
   return T( this->dot( *this ) ) ;
}
//----------------------------------------------------------------------

// obtener longitud
template< class T, unsigned n > inline
T TuplaG<T,n>::length( ) const
{
   return T( std::sqrt( lengthSq() ) ) ;
}
//----------------------------------------------------------------------
// operador binario para producto escalar

template< class T, unsigned n > inline
T TuplaG<T,n>::operator | ( const TuplaG & der ) const
{
   return this->dot( der ) ;
}

// ---------------------------------------------------------------------

template< class T, unsigned n > inline
TuplaG<T,n> TuplaG<T,n>::normalized() const
{
   T lenSq = T(0.0) ;
   for( unsigned i = 0 ; i < n ; i++ )
      lenSq += (*this)(i) * (*this)(i) ;

   if ( ! ( lenSq > 0.0 ) )
   {  using namespace std ;
      cout << "lenSq == " << lenSq << endl << flush ;
   }

   assert( lenSq > 0.0 ) ;

   return (*this)*(T(1.0/sqrt(double(lenSq)))) ;

}

// *********************************************************************
//
// plantilla de clase:
//
//   TuplaG2<T>
//
// *********************************************************************


template< class T > inline
TuplaG2<T>::TuplaG2(  )
{

}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG2<T>::TuplaG2( const TuplaG<T,2> & ini )
{
   (*this)[0] = ini(0) ;
   (*this)[1] = ini(1) ;
}

// ---------------------------------------------------------------------

template< class T > inline
void TuplaG2<T>::operator = ( const TuplaG<T,2> & der )
{
   (*this)[0] = der(0) ;
   (*this)[1] = der(1) ;
}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG2<T>::TuplaG2( const T & c0, const T & c1 )
{
   (*this)[0] = c0 ;
   (*this)[1] = c1 ;
}

// *********************************************************************
//
// plantilla de clase:
// TuplaG3<T>
//
// *********************************************************************


template< class T > inline
TuplaG3<T>::TuplaG3(  )
{

}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG3<T>::TuplaG3( const TuplaG<T,3> & ini )
{
   (*this)[0] = ini(0) ;
   (*this)[1] = ini(1) ;
   (*this)[2] = ini(2) ;
}

// ---------------------------------------------------------------------

template< class T > inline
void TuplaG3<T>::operator = ( const TuplaG<T,3> & der )
{
   (*this)[0] = der(0) ;
   (*this)[1] = der(1) ;
   (*this)[2] = der(2) ;
}

// ---------------------------------------------------------------------

template< class T > inline
void TuplaG3<T>::operator = ( const TuplaG<T,4> & der )
{
   (*this)[0] = der(0) ;
   (*this)[1] = der(1) ;
   (*this)[2] = der(2) ;
}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG3<T>::TuplaG3( const T & c0, const T & c1, const T & c2 )
{
   (*this)[0] = c0 ;
   (*this)[1] = c1 ;
   (*this)[2] = c2 ;
}

// ---------------------------------------------------------------------


template< class T > inline
TuplaG3<T> TuplaG3<T>::cross( const TuplaG3<T> & v2 ) const
{
   // cuidado: no hay acceso a 'coo' tal cual, mirar:
   // http://stackoverflow.com/questions/7281072/accessing-public-members-of-base-class-fails

   return TuplaG3<T>(  (*this)(1)*v2(2) -  (*this)(2)*v2(1),
                       (*this)(2)*v2(0) -  (*this)(0)*v2(2),
                       (*this)(0)*v2(1) -  (*this)(1)*v2(0)
                     );
}

// *********************************************************************
//
// plantilla de clase:
//
//   TuplaG4<T>
//
// *********************************************************************


template< class T > inline
TuplaG4<T>::TuplaG4(  )
{

}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG4<T>::TuplaG4( const TuplaG<T,4> & ini )
{
   (*this)[0] = ini(0) ;
   (*this)[1] = ini(1) ;
   (*this)[2] = ini(2) ;
   (*this)[3] = ini(3) ;
}

// ---------------------------------------------------------------------

template< class T > inline
void TuplaG4<T>::operator = ( const TuplaG<T,4> & der )
{
   (*this)[0] = der(0) ;
   (*this)[1] = der(1) ;
   (*this)[2] = der(2) ;
   (*this)[3] = der(3) ;
}

// ---------------------------------------------------------------------

template< class T > inline
TuplaG4<T>::TuplaG4( const T& c0, const T& c1, const T& c2, const T& c3 )
{
   (*this)[0] = c0 ;
   (*this)[1] = c1 ;
   (*this)[2] = c2 ;
   (*this)[3] = c3 ;
}



// *********************************************************************
#endif
