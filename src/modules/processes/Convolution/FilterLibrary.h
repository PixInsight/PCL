//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0282
// ----------------------------------------------------------------------------
// FilterLibrary.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __FilterLibrary_h
#define __FilterLibrary_h

#include <pcl/KernelFilter.h>
#include <pcl/SeparableFilter.h>
#include <pcl/Array.h>
#include <pcl/String.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class Filter
{
public:

   Filter() : filter()
   {
   }

   Filter( const Filter& x ) : filter()
   {
      if ( x.IsValid() )
      {
         if ( x.IsSeparable() )
            filter.Set( x.Separable() );
         else
            filter.Set( x.Kernel() );
      }
   }

   Filter( const KernelFilter& k ) : filter( k )
   {
   }

   Filter( const SeparableFilter& s ) : filter( s )
   {
   }

   virtual ~Filter()
   {
   }

   Filter& operator =( const Filter& x )
   {
      if ( &x != this )
         filter = x.filter;
      return *this;
   }

   String Name() const
   {
      if ( !IsValid() )
         return String();
      return IsSeparable() ? Separable().Name() : Kernel().Name();
   }

   bool IsValid() const
   {
      return filter.IsValid();
   }

   bool IsSeparable() const
   {
      return filter.IsSeparable();
   }

   const KernelFilter& Kernel() const
   {
      return filter.AsKernelFilter();
   }

   const SeparableFilter& Separable() const
   {
      return filter.AsSeparableFilter();
   }

   bool IsHighPass() const
   {
      return IsSeparable() ? Separable().IsHighPassFilter() : Kernel().IsHighPassFilter();
   }

   IsoString ToSource() const;

   static Filter FromSource( const IsoString& src );

   template <class S>
   static Filter FromSource( const S& src )
   {
      return FromSource( IsoString( src ) );
   }

   bool operator ==( const Filter& x ) const
   {
      return IsSeparable() == x.IsSeparable() &&
             (IsSeparable() ? Separable() == x.Separable() : Kernel() == x.Kernel());
   }

   bool operator <( const Filter& x ) const
   {
      return Name() < x.Name();
   }

private:

   class FilterVariant
   {
   private:

      void* filter;
      bool  separable : 1;

#define KERNEL( f )     reinterpret_cast<KernelFilter*>( f )
#define SEPARABLE( f )  reinterpret_cast<SeparableFilter*>( f )

      void Destroy()
      {
         if ( filter != 0 )
         {
            if ( separable )
               delete SEPARABLE( filter );
            else
               delete KERNEL( filter );
            filter = 0;
         }
      }

   public:

      FilterVariant() : filter( 0 ), separable( false )
      {
      }

      FilterVariant( const KernelFilter& k ) : filter( 0 ), separable( false )
      {
         filter = new KernelFilter( k );
      }

      FilterVariant( const SeparableFilter& s ) : filter( 0 ), separable( true )
      {
         filter = new SeparableFilter( s );
      }

      ~FilterVariant()
      {
         Destroy();
      }

      void operator =( const FilterVariant& x )
      {
         if ( &x != this )
         {
            Destroy();
            if ( x.IsValid() )
               if ( x.IsSeparable() )
               {
                  filter = new SeparableFilter( *SEPARABLE( x.filter ) );
                  separable = true;
               }
               else
               {
                  filter = new KernelFilter( *KERNEL( x.filter ) );
                  separable = false;
               }
         }
      }

      void Set( const KernelFilter& k )
      {
         Destroy();
         filter = new KernelFilter( k );
         separable = false;
      }

      void Set( const SeparableFilter& s )
      {
         Destroy();
         filter = new SeparableFilter( s );
         separable = true;
      }

      bool IsSeparable() const
      {
         return separable;
      }

      bool IsValid() const
      {
         return filter != 0;
      }

      const KernelFilter& AsKernelFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         if ( IsSeparable() )
            throw Error( "Internal error: not a kernel filter." );
         return *KERNEL( filter );
      }

      const SeparableFilter& AsSeparableFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         if ( !IsSeparable() )
            throw Error( "Internal error: not a separable filter." );
         return *SEPARABLE( filter );
      }

      bool IsHighPassFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         return separable ? SEPARABLE( filter )->IsHighPassFilter() :
                            KERNEL( filter )->IsHighPassFilter();
      }

#undef KERNEL
#undef SEPARABLE
   };

   FilterVariant filter;

   // Special constructor for searching in a filter list.
   Filter( const String& name ) : filter( KernelFilter( name ) )
   {
   }

   friend class FilterLibrary;
};

// ----------------------------------------------------------------------------

class FilterParser
{
public:

   typedef SortedArray<Filter> filter_list;

   FilterParser( const IsoStringList& lines )
   {
      Parse( Tokenize( lines ) );
   }

   const filter_list& Filters() const
   {
      return filters;
   }

private:

   filter_list filters;

   struct Token
   {
      Token( const IsoString& s, int l, int c ) : token( s.UTF8ToUTF16() ), line( l ), column( c )
      {
      }

      String token;
      int    line, column;
   };

   typedef Array<Token> token_list;

   enum { UnknownFilterType, KernelFilterType, SeparableFilterType };

   enum { UnknownState, FilterTypeState, FilterParameterState };

   token_list Tokenize( const IsoStringList& linesUTF8 );
   void Parse( const token_list& tokens );

   static void CaptureParameterValueTokens( token_list::const_iterator&, token_list::const_iterator& );
};

// ----------------------------------------------------------------------------

class FilterLibrary
{
public:

   typedef SortedArray<Filter> filter_list;

   FilterLibrary() : filters(), filePath()
   {
   }

   FilterLibrary( const String& _filePath ) : filters(), filePath()
   {
      Load( _filePath );
   }

   void Load( const String& filePath );
   void LoadDefaultLibrary();
   void Save();
   void SaveAs( const String& filePath );
   void New();

   bool IsNew() const
   {
      return filePath.IsEmpty();
   }

   bool IsWritable() const;

   String FilePath() const
   {
      return filePath;
   }

   size_type Length() const
   {
      return filters.Length();
   }

   bool IsEmpty() const
   {
      return filters.IsEmpty();
   }

   const Filter& operator[]( size_type i ) const
   {
      return filters[i];
   }

   const Filter* FilterByName( const String& filterName ) const
   {
      filter_list::const_iterator i = filters.Search( filterName );
      return (i == filters.End()) ? 0 : i;
   }

   void Add( const Filter& f )
   {
      const Filter* f0 = FilterByName( f.Name() );
      if ( f0 == 0 )
         filters.Add( f );
      else
      {
         *const_cast<Filter*>( f0 ) = f;
         filters.Sort();
      }
   }

   bool Remove( const Filter& f )
   {
      const Filter* f0 = FilterByName( f.Name() );
      if ( f0 == 0 )
         return false;
      filters.Remove( f );
      return true;
   }

   static String DefaultLibraryPath();

private:

   filter_list filters;
   String      filePath;
};

// ----------------------------------------------------------------------------

extern FilterLibrary TheFilterLibrary;

// ----------------------------------------------------------------------------

} // pcl

#endif // __FilterLibrary_h

// ----------------------------------------------------------------------------
// EOF FilterLibrary.h - Released 2019-01-21T12:06:41Z
