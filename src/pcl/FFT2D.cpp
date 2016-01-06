//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/FFT2D.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include <pcl/FFT2D.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename To, typename Ti>
class PCL_FFT2DEngineBase
{
public:

   typedef Ti             input_type;
   typedef To             output_type;
   typedef ReferenceArray<Thread> thread_list;

   PCL_FFT2DEngineBase( int rows, int cols, To* output, const Ti* input, int dir, StatusMonitor* monitor, bool parallel, int maxProcessors ) :
   m_rows( rows ), m_cols( cols ), m_output( output ), m_input( input ), m_dir( dir ),
   m_monitor( monitor ), m_parallel( parallel ), m_maxProcessors( maxProcessors )
   {
      if ( m_monitor != 0 )
         if ( m_monitor->IsInitializationEnabled() )
            m_monitor->Initialize( (m_dir == PCL_FFT_FORWARD) ? "FFT" : "Inverse FFT", m_rows + m_cols );
   }

   virtual ~PCL_FFT2DEngineBase()
   {
   }

protected:

         int            m_rows;
         int            m_cols;
         To*            m_output;
   const Ti*            m_input;
         int            m_dir;
         StatusMonitor* m_monitor;
         bool           m_parallel : 1;
         unsigned       m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   void RunThreads( thread_list& threads, int count )
   {
      for ( typename thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
         i->Start( ThreadPriority::DefaultMax, Distance( threads.Begin(), i ) );
      for ( typename thread_list::iterator i = threads.Begin(); i != threads.End(); ++i )
         i->Wait();

      threads.Destroy();

      if ( m_monitor != 0 )
         *m_monitor += count;
   }
};

// ----------------------------------------------------------------------------

/*
 * 2-D transforms of complex data
 */
template <typename T>
class PCL_FFT2DEngine : public PCL_FFT2DEngineBase<Complex<T>,Complex<T> >
{
public:

   typedef Complex<T>                           complex;
   typedef PCL_FFT2DEngineBase<complex,complex> base;
   typedef typename base::thread_list           thread_list;

   PCL_FFT2DEngine( int rows, int cols, complex* output, const complex* input, int dir, StatusMonitor* monitor, bool parallel, int maxProcessors ) :
   base( rows, cols, output, input, dir, monitor, parallel, maxProcessors )
   {
      size_type N = size_type( this->m_rows )*size_type( this->m_cols );
      const void* i0 = this->m_input;
      const void* i1 = this->m_input + N;
      const void* o0 = this->m_output;
      const void* o1 = this->m_output + N;
      m_overlapped = o1 > i0 && o0 < i1;

      for ( int direction = 0; direction < 2; ++direction ) // FFT of m_rows, m_cols
      {
         int numberOfItems = (direction == 0) ? this->m_rows : this->m_cols;
         int numberOfThreads = this->m_parallel ? Min( int( this->m_maxProcessors ), pcl::Thread::NumberOfThreads( numberOfItems, 1 ) ) : 1;
         int itemsPerThread = numberOfItems/numberOfThreads;

         thread_list threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         {
            int a = i*itemsPerThread;
            int b = (j < numberOfThreads) ? j*itemsPerThread : numberOfItems;
            threads.Add( (direction == 0) ? static_cast<Thread*>( new RowThread( *this, a, b ) ) :
                                            static_cast<Thread*>( new ColThread( *this, a, b ) ) );
         }

         this->RunThreads( threads, numberOfItems );
      }
   }

   virtual ~PCL_FFT2DEngine()
   {
   }

private:

   bool m_overlapped : 1;

   friend class RowThread;

   class RowThread : public Thread, public FFT1DBase
   {
   public:

      RowThread( PCL_FFT2DEngine& e, int r0, int r1 ) : m_engine( e ), m_firstRow( r0 ), m_endRow( r1 )
      {
      }

      virtual void Run()
      {
         void* h = (m_engine.m_dir == PCL_FFT_FORWARD) ?
               this->Create( m_engine.m_cols, (complex*)0 ) : this->CreateInv( m_engine.m_cols, (complex*)0 );

         if ( m_engine.m_overlapped )
         {
            GenericVector<complex> irow( m_engine.m_cols );

            for ( int i = m_firstRow; i < m_endRow; ++i )
            {
               int d = i*m_engine.m_cols;
               memcpy( *irow, m_engine.m_input + d, m_engine.m_cols*sizeof( complex ) );
               this->Transform( h, m_engine.m_output + d, *irow );
            }
         }
         else
         {
            for ( int i = m_firstRow; i < m_endRow; ++i )
            {
               int d = i*m_engine.m_cols;
               this->Transform( h, m_engine.m_output + d, m_engine.m_input + d );
            }
         }

         this->Destroy( h );
      }

   private:

      PCL_FFT2DEngine& m_engine;
      int              m_firstRow;
      int              m_endRow;
   };

   friend class ColThread;

   class ColThread : public Thread, public FFT1DBase
   {
   public:

      ColThread( PCL_FFT2DEngine& e, int c0, int c1 ) : m_engine( e ), m_firstCol( c0 ), m_endCol( c1 )
      {
      }

      virtual void Run()
      {
         void* h = (m_engine.m_dir == PCL_FFT_FORWARD) ?
               this->Create( m_engine.m_rows, (complex*)0 ) : this->CreateInv( m_engine.m_rows, (complex*)0 );

         GenericVector<complex> icol( m_engine.m_rows );
         GenericVector<complex> ocol( m_engine.m_rows );

         for ( int j = m_firstCol; j < m_endCol; ++j )
         {
            for ( int i = 0, k = j; i < m_engine.m_rows; ++i, k += m_engine.m_cols )
               icol[i] = m_engine.m_output[k];

            this->Transform( h, *ocol, *icol );

            for ( int i = 0, k = j; i < m_engine.m_rows; ++i, k += m_engine.m_cols )
               m_engine.m_output[k] = ocol[i];
         }

         this->Destroy( h );
      }

   private:

      PCL_FFT2DEngine& m_engine;
      int              m_firstCol;
      int              m_endCol;
   };
};

// ----------------------------------------------------------------------------

template <typename To, typename Ti>
class PCL_FFT2DRealEngineBase : public PCL_FFT2DEngineBase<To,Ti>
{
public:

   typedef PCL_FFT2DEngineBase<To,Ti> base;
   typedef typename base::thread_list thread_list;

   PCL_FFT2DRealEngineBase( int _rows, int _cols, To* _output, const Ti* _input, int dir, StatusMonitor* _monitor, bool parallel, int maxProcessors ) :
   base( _rows, _cols, _output, _input, dir, _monitor, parallel, maxProcessors ),
   m_transformCols( _cols/2 + 1 )
   {
   }

   virtual ~PCL_FFT2DRealEngineBase()
   {
   }

protected:

   int m_transformCols;
};

// ----------------------------------------------------------------------------

/*
 * 2-D forward transforms of real data
 */
template <typename T>
class PCL_FFT2DRealEngine : public PCL_FFT2DRealEngineBase<Complex<T>,T>
{
public:

   typedef T                                       scalar;
   typedef Complex<T>                              complex;
   typedef PCL_FFT2DRealEngineBase<complex,scalar> base;
   typedef typename base::thread_list              thread_list;

   PCL_FFT2DRealEngine( int rows, int cols, complex* output, const scalar* input, StatusMonitor* monitor, bool parallel, int maxProcessors ) :
   base( rows, cols, output, input, PCL_FFT_FORWARD, monitor, parallel, maxProcessors )
   {
      for ( int direction = 0; direction < 2; ++direction ) // FFT of m_rows, m_cols
      {
         int numberOfItems = (direction == 0) ? this->m_rows : this->m_transformCols;
         int numberOfThreads = this->m_parallel ? Min( int( this->m_maxProcessors ), pcl::Thread::NumberOfThreads( numberOfItems, 1 ) ) : 1;
         int itemsPerThread = numberOfItems/numberOfThreads;

         thread_list threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         {
            int a = i*itemsPerThread;
            int b = (j < numberOfThreads) ? j*itemsPerThread : numberOfItems;
            threads.Add( (direction == 0) ? static_cast<Thread*>( new RowThread( *this, a, b ) ) :
                                            static_cast<Thread*>( new ColThread( *this, a, b ) ) );
         }

         this->RunThreads( threads, numberOfItems );
      }
   }

   virtual ~PCL_FFT2DRealEngine()
   {
   }

private:

   friend class RowThread;

   class RowThread : public Thread, public FFT1DBase
   {
   public:

      RowThread( PCL_FFT2DRealEngine& e, int r0, int r1 ) : m_engine( e ), m_firstRow( r0 ), m_endRow( r1 )
      {
      }

      virtual void Run()
      {
         void* h = this->Create( m_engine.m_cols, (scalar*)0 );
         for ( int i = m_firstRow; i < m_endRow; ++i )
            this->Transform( h, m_engine.m_output + i*m_engine.m_transformCols, m_engine.m_input + i*m_engine.m_cols );
         this->Destroy( h );
      }

   private:

      PCL_FFT2DRealEngine& m_engine;
      int                  m_firstRow;
      int                  m_endRow;
   };

   friend class ColThread;

   class ColThread : public Thread, public FFT1DBase
   {
   public:

      ColThread( PCL_FFT2DRealEngine& e, int c0, int c1 ) : m_engine( e ), m_firstCol( c0 ), m_endCol( c1 )
      {
      }

      virtual void Run()
      {
         void* h = this->Create( m_engine.m_rows, (complex*)0 );

         GenericVector<complex> icol( m_engine.m_rows );
         GenericVector<complex> ocol( m_engine.m_rows );

         for ( int j = m_firstCol; j < m_endCol; ++j )
         {
            for ( int i = 0, k = j; i < m_engine.m_rows; ++i, k += m_engine.m_transformCols )
               icol[i] = m_engine.m_output[k];

            this->Transform( h, *ocol, *icol );

            for ( int i = 0, k = j; i < m_engine.m_rows; ++i, k += m_engine.m_transformCols )
               m_engine.m_output[k] = ocol[i];
         }

         this->Destroy( h );
      }

   private:

      PCL_FFT2DRealEngine& m_engine;
      int                  m_firstCol;
      int                  m_endCol;
   };
};

// ----------------------------------------------------------------------------

/*
 * 2-D backward transforms of real data
 */
template <typename T>
class PCL_FFT2DRealInverseEngine : public PCL_FFT2DRealEngineBase<T,Complex<T> >
{
public:

   typedef T                                       scalar;
   typedef Complex<T>                              complex;
   typedef GenericMatrix<complex>                  transform;
   typedef PCL_FFT2DRealEngineBase<scalar,complex> base;
   typedef typename base::thread_list              thread_list;

   PCL_FFT2DRealInverseEngine( int rows, int cols, scalar* output, const complex* input, StatusMonitor* monitor, bool parallel, int maxProcessors ) :
   base( rows, cols, output, input, PCL_FFT_BACKWARD, monitor, parallel, maxProcessors ),
   m_colTransform( this->m_rows, this->m_transformCols )
   {
      for ( int direction = 0; direction < 2; ++direction ) // FFT of m_cols, m_rows
      {
         int numberOfItems = (direction == 0) ? this->m_transformCols : this->m_rows;
         int numberOfThreads = this->m_parallel ? Min( int( this->m_maxProcessors ), Thread::NumberOfThreads( numberOfItems, 1 ) ) : 1;
         int itemsPerThread = numberOfItems/numberOfThreads;

         thread_list threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         {
            int a = i*itemsPerThread;
            int b = (j < numberOfThreads) ? j*itemsPerThread : numberOfItems;
            threads.Add( (direction == 0) ? static_cast<Thread*>( new ColThread( *this, a, b ) ) :
                                            static_cast<Thread*>( new RowThread( *this, a, b ) ) );
         }

         this->RunThreads( threads, numberOfItems );
      }
   }

   virtual ~PCL_FFT2DRealInverseEngine()
   {
   }

private:

   transform m_colTransform;

   friend class ColThread;

   class ColThread : public Thread, public FFT1DBase
   {
   public:

      ColThread( PCL_FFT2DRealInverseEngine& e, int c0, int c1 ) : m_engine( e ), m_firstCol( c0 ), m_endCol( c1 )
      {
      }

      virtual void Run()
      {
         void* h = this->CreateInv( m_engine.m_rows, (complex*)0 );

         GenericVector<complex> icol( m_engine.m_rows );
         GenericVector<complex> ocol( m_engine.m_rows );

         for ( int j = m_firstCol; j < m_endCol; ++j )
         {
            for ( int i = 0, k = j; i < m_engine.m_rows; ++i, k += m_engine.m_transformCols )
               icol[i] = m_engine.m_input[k];

            this->Transform( h, *ocol, *icol );

            for ( int i = 0; i < m_engine.m_rows; ++i )
               m_engine.m_colTransform[i][j] = ocol[i];
         }

         this->Destroy( h );
      }

   private:

      PCL_FFT2DRealInverseEngine& m_engine;
      int                         m_firstCol;
      int                         m_endCol;
   };

   friend class RowThread;

   class RowThread : public Thread, public FFT1DBase
   {
   public:

      RowThread( PCL_FFT2DRealInverseEngine& e, int r0, int r1 ) : m_engine( e ), m_firstRow( r0 ), m_endRow( r1 )
      {
      }

      virtual void Run()
      {
         void* h = this->CreateInv( m_engine.m_cols, (scalar*)0 );
         for ( int i = m_firstRow; i < m_endRow; ++i )
            this->Transform( h, m_engine.m_output + i*m_engine.m_cols, m_engine.m_colTransform[i] );
         this->Destroy( h );
      }

   private:

      PCL_FFT2DRealInverseEngine& m_engine;
      int                         m_firstRow;
      int                         m_endRow;
   };
};

// ----------------------------------------------------------------------------

void FFT2DBase::Transform( int rows, int cols, fcomplex* y, const fcomplex* x, int dir, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DEngine<float>( rows, cols, y, x, dir, monitor, parallel, maxProcessors );
}

void FFT2DBase::Transform( int rows, int cols, dcomplex* y, const dcomplex* x, int dir, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DEngine<double>( rows, cols, y, x, dir, monitor, parallel, maxProcessors );
}

void FFT2DBase::Transform( int rows, int cols, fcomplex* y, const float* x, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DRealEngine<float>( rows, cols, y, x, monitor, parallel, maxProcessors );
}

void FFT2DBase::Transform( int rows, int cols, dcomplex* y, const double* x, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DRealEngine<double>( rows, cols, y, x, monitor, parallel, maxProcessors );
}

void FFT2DBase::Transform( int rows, int cols, float* y, const fcomplex* x, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DRealInverseEngine<float>( rows, cols, y, x, monitor, parallel, maxProcessors );
}

void FFT2DBase::Transform( int rows, int cols, double* y, const dcomplex* x, StatusMonitor* monitor, bool parallel, int maxProcessors )
{
   PCL_FFT2DRealInverseEngine<double>( rows, cols, y, x, monitor, parallel, maxProcessors );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFT2D.cpp - Released 2015/12/17 18:52:18 UTC
