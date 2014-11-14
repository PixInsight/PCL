// ****************************************************************************
// PixInsight Class Library - PCL 02.00.09.0651
// Standard PhotometricSuperflat Process Module Version 01.00.02.0116
// ****************************************************************************
// PhotometricSuperflatInstance.cpp - Released 2013/12/13 00:03:11 UTC
// ****************************************************************************
// This file is part of the standard PhotometricSuperflat PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "PhotometricSuperflatInstance.h"
#include "PhotometricSuperflatParameters.h"

#include "PolynomialSurface.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

    // ----------------------------------------------------------------------------

    PhotometricSuperflatInstance::PhotometricSuperflatInstance( const MetaProcess* m ) :
    ProcessImplementation( m ),
    fitDegree( int32( ThePhotometricSuperflatFitDegreeParameter->DefaultValue() ) )
    {
    }

    PhotometricSuperflatInstance::PhotometricSuperflatInstance( const PhotometricSuperflatInstance& x ) :
    ProcessImplementation( x )
    {
        Assign( x );
    }

    void PhotometricSuperflatInstance::Assign( const ProcessImplementation& p )
    {
        const PhotometricSuperflatInstance* x = dynamic_cast<const PhotometricSuperflatInstance*>( &p );
        if ( x != 0 )
        {
            starDatabasePath = x->starDatabasePath;
            fitDegree = x->fitDegree;
        }
    }

    bool PhotometricSuperflatInstance::CanExecuteOn( const View& view, String& whyNot ) const
    {
        if ( view.Image().IsComplexSample() )
        {
            whyNot = "PhotometricSuperflat cannot be executed on complex images.";
            return false;
        }

        whyNot.Clear();
        return true;
    }


    // ------------------------------------ ENGINE -----------------------------------

    // Here is where the magic is supposed to happen....

    class PhotometricSuperflatEngine
    {
    public:

        template <class P>
        static void Apply( GenericImage<P>& image, const PhotometricSuperflatInstance& instance )
        {

            PolynomialSurface* S;

            if ( !File::Exists( instance.starDatabasePath ) )
                throw Error( "No such file: " + instance.starDatabasePath );
            S = new PolynomialSurface( instance.starDatabasePath, image.Width(), image.Height() );
            //S->PrintCatalog();
            S->PrintCatalogSummary();
            S->PlotXYKeyedToRelativeFlux(false);
            String eqn = S->ComputeBestFitModel(instance.fitDegree);
            S->PlotXYKeyedToRelativeFlux(true);
            S->ShowBestFitModelImage();
            delete(S);
        };




    private:

        ImageWindow CreateImageWindow( const IsoString& id, int sensorWidth, int sensorHeight, int bitsPerSample, bool floatSample )
        {
            ImageWindow window( sensorWidth, sensorHeight, 1, bitsPerSample, floatSample, false, true, id );
            if ( window.IsNull() )
                throw Error( "Unable to create image window: " + id );
            return window;
        }


    };


    // ------------------------------------ INSTANCE -----------------------------------


    bool PhotometricSuperflatInstance::ExecuteOn( View& view )
    {
        AutoViewLock lock( view );

        ImageVariant image = view.Image();
        if ( image.IsComplexSample() )
            return false;

        StandardStatus status;
        image.SetStatusCallback( &status );

        Console().EnableAbort();

        if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
        {
            case 32: PhotometricSuperflatEngine::Apply( static_cast<Image&>( *image ), *this ); break;
            case 64: PhotometricSuperflatEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
        }
        else
            switch ( image.BitsPerSample() )
        {
            case  8: PhotometricSuperflatEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
            case 16: PhotometricSuperflatEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
            case 32: PhotometricSuperflatEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
        }

        return true;
    }

    void* PhotometricSuperflatInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
    {
        if ( p == ThePhotometricSuperflatStarDatabasePathParameter )
            return starDatabasePath.c_str();
        if ( p == ThePhotometricSuperflatFitDegreeParameter )
            return &fitDegree;
        return 0;
    }

    bool PhotometricSuperflatInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
    {
        if ( p == ThePhotometricSuperflatStarDatabasePathParameter )
        {
            starDatabasePath.Clear();
            if ( sizeOrLength > 0 )
                starDatabasePath.Reserve( sizeOrLength );
            return true;
        }
        else
            return false;

        return true;
    }

    size_type PhotometricSuperflatInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
    {
        if ( p == ThePhotometricSuperflatStarDatabasePathParameter )
            return starDatabasePath.Length();
        return 0;
    }

    // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF PhotometricSuperflatInstance.cpp - Released 2013/12/13 00:03:11 UTC
