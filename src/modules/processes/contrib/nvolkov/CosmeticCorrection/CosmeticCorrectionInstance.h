//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0139
// ----------------------------------------------------------------------------
// CosmeticCorrectionInstance.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#ifndef __CosmeticCorrectionInstance_h
#define __CosmeticCorrectionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Convolution.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/MorphologicalTransformation.h>

#include "CosmeticCorrectionParameters.h"

//#define debug 1

namespace pcl
{


// ----------------------------------------------------------------------------

class CCThread;
struct CCThreadData;

typedef IndirectArray<CCThread> thread_list;

class CosmeticCorrectionInstance : public ProcessImplementation
{
public:
    CosmeticCorrectionInstance( const MetaProcess* );
    CosmeticCorrectionInstance( const CosmeticCorrectionInstance& );

    virtual void Assign( const ProcessImplementation& );
    virtual bool CanExecuteOn( const View&, String& whyNot ) const;
    //virtual bool ExecuteOn( View& );
    virtual bool IsHistoryUpdater( const View& v ) const;
    virtual bool CanExecuteGlobal( String& whyNot ) const;
    virtual bool ExecuteGlobal();
    virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
    virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
    virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:
    struct ImageItem
    {
        pcl_bool enabled; // if disabled, skip (ignore) this image
        String   path;    // absolute file path
        ImageItem( const String& p = String() ) : enabled( true ), path( p )
        {
        }

        ImageItem( const ImageItem& x ) : enabled( x.enabled ), path( x.path )
        {
        }
        bool IsValid() const
        {
            return !enabled || !path.IsEmpty();
        }
    };

    struct DefectItem
    {
        pcl_bool enabled;       // if disabled, skip (ignore) this defect
        pcl_bool isRow;         // true == Row, false == Col
        uint16 address;         // address of selected col or row
        pcl_bool isRange;       // true == range of selected pixels defined, false == All pixels selected as bad
        uint16 begin;           // first bad pixel in col/row
        uint16 end;             // last bad pixel in col/row

        DefectItem( const bool a_enabled = false, const bool a_isRow = false, const uint16 a_address = 0, const bool a_isRange = false, const uint16 a_begin = 0, const uint16 a_end = 0 ):
        enabled( a_enabled ), isRow( a_isRow ), address( a_address ), isRange( a_isRange ), begin( a_begin ), end( a_end )
        {
        }

        DefectItem( const DefectItem& x ) : enabled( x.enabled ), isRow( x.isRow ), address( x.address ), isRange( x.isRange ), begin( x.begin ), end( x.end )
        {
        }
    };

#define MapImg UInt8Image
#define DarkImg UInt16Image

    typedef Array<ImageItem> image_list;
    typedef Array<DefectItem> defect_list;

    Rect    m_geometry;
    MapImg* m_mapDarkHot;
    MapImg* m_mapDarkCold;

    // instance ---------------------------------------------------------------
    image_list  p_targetFrames;
    String      p_outputDir;
    String      p_outputExtension;
    pcl_bool    p_overwrite;
    String      p_prefix;
    String      p_postfix;

    pcl_bool    p_cfa;
    float       p_amount;

    pcl_bool    p_useMasterDark;
    String      p_masterDark;
    pcl_bool    p_hotDarkCheck;
    float       p_hotDarkLevel;
    pcl_bool    p_coldDarkCheck;
    float       p_coldDarkLevel;

    pcl_bool    p_useAutoDetect;
    pcl_bool    p_hotAutoCheck;
    float       p_hotAutoValue;
    pcl_bool    p_coldAutoCheck;
    float       p_coldAutoValue;

    pcl_bool    p_useDefectList;
    defect_list p_defects;

    // -------------------------------------------------------------------------

    bool   CanExecute( String& whyNot ) const;
    inline MorphologicalTransformation* BkgMT() const;
    inline MorphologicalTransformation* MedMT() const;
    inline MorphologicalTransformation* AvrMT() const;
    inline DarkImg GetDark( const String& );
    void   PrepareMasterDarkMaps();

    inline thread_list LoadTargetFrame( const String& , const CCThreadData& );
    inline String OutputFilePath( const String& , const size_t );
    inline void SaveImage( const CCThread* );

    friend class CCThread;
    friend class CosmeticCorrectionInterface;
};

// ----------------------------------------------------------------------------
} // pcl

#endif   // __CosmeticCorrectionInstance_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInstance.h - Released 2015/11/26 16:00:13 UTC
