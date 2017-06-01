//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0184
// ----------------------------------------------------------------------------
// solver_dct3.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include <pcl/View.h>
#include <pcl/Console.h>
#include <pcl/Math.h>
#include <pcl/FFT1D.h>
#include <pcl/Complex.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// Poisson Solver through Constrained Least Squares deconvolution Algoritm implementation
// ----------------------------------------------------------------------------

double Alpha( int i )
{
  double f = 1.0;
  if ( i == 0 )
    f /= Sqrt(2.0);
  return f;
}

template <class P1>
void DCTcolumns( GenericImage<P1>& dct )
{
      int w1 = dct.Width();
      int h1 = dct.Height();

      for (int i = 0; i < w1; i++)
      {

	typename GenericRealFFT<double>::vector v( 0.0 , 2*h1 );
	typename GenericRealFFT<double>::complex_vector c( 0.0 , 2*h1 );
	GenericRealFFT<double> F(2*h1);

	for (int j = 0; j < h1; j++)
	  v[j] = v[2*h1-1-j] = dct.Pixel(i,h1-j-1,0);

	F(*c,*v);


	for (int k = 0; k < h1; k++)
	{
	  dcomplex f( Cos(-double(k)*2*Pi()*(double(h1)-0.5)/(2*double(h1)) ), Sin(-double(k)*2*Pi()*(double(h1)-0.5)/(2*double(h1)) ) );
	  c[k] *= Alpha(k)/f;
	  dct.Pixel(i,k,0) = c[k].Real();
	}

      }

}

template <class P1>
void DCTrows( GenericImage<P1>& img, GenericImage<P1>& dct )
{

      int w = img.Width();
      int h = img.Height();
      int w1 = dct.Width();
      //int h1 = dct.Height();
      int dw = w1-w;

      for (int j = 0; j < h; j++)
      {

	typename GenericRealFFT<double>::vector v( 0.0 , 2*w1 );
	typename GenericRealFFT<double>::complex_vector c( 0.0 , 2*w1 );
	GenericRealFFT<double> F(2*w1);

	for (int i = 0; i < w; i++)
	  v[i+dw] = v[2*w1-1-i-dw] = img.Pixel(w-i-1,j,0);

	F(*c,*v);


	for (int k = 0; k < w1; k++)
	{
	  dcomplex f( Cos(-double(k)*2*Pi()*(double(w1)-0.5)/(2*double(w1)) ), Sin(-double(k)*2*Pi()*(double(w1)-0.5)/(2*double(w1)) ) );
	  c[k] *= Alpha(k)/f;
	  dct.Pixel(k,j,0) = c[k].Real();
	}

      }

}

template <class P1>
void InverseDCTcolumns( GenericImage<P1>& dct )
{
      int w1 = dct.Width();
      int h1 = dct.Height();

      for (int i = 0; i < w1; i++)
      {
	// FFT based iDCT

	typename GenericFFT<double>::complex_vector v( 0.0 , 2*h1 );
	typename GenericFFT<double>::complex_vector c( 0.0 , 2*h1 );
	GenericFFT<double> F( 2*h1 );

	for (int j = 0; j < h1; j++)
	{
	  c[j] = dct.Pixel(i,j,0);
	  dcomplex f( Cos(-double(j)*2*Pi()*(double(h1)-0.5)/(2.0*double(h1)) ), Sin(-double(j)*2*Pi()*(double(h1)-0.5)/(2.0*double(h1)) ) );
	  c[j] /= Alpha(j)/f;
	}
	c[h1] = 0.0;
	int a = 0;
	for (int j = h1+1; j < 2*h1; j++)
	{
	  c[j] = dct.Pixel(i, h1-1-a, 0);
	  dcomplex f( Cos(-double(j)*2*Pi()*(double(h1)-0.5)/(2.0*double(h1)) ), Sin(-double(j)*2*Pi()*(double(h1)-0.5)/(2.0*double(h1)) ) );
	  c[j] /= -Alpha(j)/f;
	  ++a;
	}


	F( *v, *c, PCL_FFT_BACKWARD );

	for (int j = 0; j < h1; j++)
	{
	  dct.Pixel(i,j,0) = v[h1-1-j].Real()/h1;
	}
      }
}

template <class P1>
void InverseDCTrows( GenericImage<P1>& img, GenericImage<P1>& dct )
{

      int w = img.Width();
      int h = img.Height();
      int w1 = dct.Width();
      //int h1 = dct.Height();

      for (int j = 0; j < h; j++)
      {
	// FFT based iDCT

	typename GenericFFT<double>::complex_vector v( 0.0 , 2*w1 );
	typename GenericFFT<double>::complex_vector c( 0.0 , 2*w1 );
	GenericFFT<double> F( 2*w1 );
	for (int i = 0; i < w1; i++)
	{
	  c[i] = dct.Pixel(i,j,0);
	  dcomplex f( Cos(-double(i)*2*Pi()*(double(w1)-0.5)/(2.0*double(w1)) ), Sin(-double(i)*2*Pi()*(double(w1)-0.5)/(2.0*double(w1)) ) );
	  c[i] /= Alpha(i)/f;
	}
	c[w1] = 0.0;
	int a = 0;
	for (int i = w1+1; i < 2*w1; i++)
	{
	  c[i] = dct.Pixel(w1-1-a,j,0);
	  dcomplex f( Cos(-double(i)*2*Pi()*(double(w1)-0.5)/(2.0*double(w1)) ), Sin(-double(i)*2*Pi()*(double(w1)-0.5)/(2.0*double(w1)) ) );
	  c[i] /= -Alpha(i)/f;
	  ++a;
	}


	F( *v, *c, PCL_FFT_BACKWARD );

	for (int i = 0; i < w; i++)
	{
	  img.Pixel(i,j,0) = v[w1-1-i].Real()/w1;
	}
      }
}

template <class P1>
void __SolvePoisson( GenericImage<P1>& img )
{
      int w = img.Width();
      int h = img.Height();
      int w1 = RealFFT::OptimizedLength( RoundI(w) );
      int h1 = RealFFT::OptimizedLength( RoundI(h) );

      GenericImage<P1> temp( w1, h1 );
      temp.Zero();

      DCTrows( img, temp );
      DCTcolumns( temp );

      // Solver
      for (int i = 0; i < w1; i++)
        for (int j = 0; j < h1; j++)
	  if ( i != 0 || j != 0 )
	     temp.Pixel(i,j,0) /= -4+2*Cos(Pi()*double(i)/w1)+2*Cos(Pi()*double(j)/h1);

      //temp.Pixel(0,0,0) = 1.0;


      InverseDCTcolumns( temp );
      InverseDCTrows( img, temp );

}

void SolvePoisson( ImageVariant& L )
{
   switch ( L.BitsPerSample() )
   {
     case 32 : __SolvePoisson( static_cast<pcl::Image&>( *L ) ); break;
     case 64 : __SolvePoisson( static_cast<pcl::DImage&>( *L ) ); break;
   }
}

} // pcl

// ----------------------------------------------------------------------------
// EOF solver_dct3.h - Released 2017-05-02T09:43:01Z
