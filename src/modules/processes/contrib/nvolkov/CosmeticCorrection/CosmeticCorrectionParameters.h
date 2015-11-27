//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0139
// ----------------------------------------------------------------------------
// CosmeticCorrectionParameters.h - Released 2015/11/26 16:00:13 UTC
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

#ifndef __CosmeticCorrectionParameters_h
#define __CosmeticCorrectionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CCTargetFrames : public MetaTable
{
public:
   CCTargetFrames( MetaProcess* );
   virtual IsoString Id() const;
};

class CCTargetFrameEnabled : public MetaBoolean
{
public:
   CCTargetFrameEnabled( MetaTable* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCTargetFramePath : public MetaString
{
public:
   CCTargetFramePath( MetaTable* );
   virtual IsoString Id() const;
};


class CCOutputDir : public MetaString
{
public:
   CCOutputDir( MetaProcess* );
   virtual IsoString Id() const;
};

class CCOutputExtension : public MetaString
{
public:
   CCOutputExtension( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class CCPrefix : public MetaString
{
public:
   CCPrefix( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class CCPostfix : public MetaString
{
public:
   CCPostfix( MetaProcess* );
   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

class CCOverwrite : public MetaBoolean
{
public:
   CCOverwrite( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};
extern CCOverwrite*         TheOverwrite;

class CCCFA : public MetaBoolean
{
public:
   CCCFA( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};
class CCAmount : public MetaFloat
{
public:
   CCAmount( MetaProcess* );
   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

// Via Master Dark -----------------------------
class CCUseMasterDark : public MetaBoolean
{
public:
   CCUseMasterDark( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCMasterDarkPath : public MetaString
{
public:
   CCMasterDarkPath( MetaProcess* );
   virtual IsoString Id() const;
};

class CCHotDarkCheck : public MetaBoolean
{
public:
   CCHotDarkCheck( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCHotLevel : public MetaFloat
{
public:
   CCHotLevel( MetaProcess* );
   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

class CCColdDarkCheck : public MetaBoolean
{
public:
   CCColdDarkCheck( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCColdLevel : public MetaFloat
{
public:
   CCColdLevel( MetaProcess* );
   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

// Via Auto Detect -----------------------------
class CCUseAutoDetect : public MetaBoolean
{
public:
   CCUseAutoDetect( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCHotAutoValue : public MetaFloat
{
public:
   CCHotAutoValue( MetaProcess* );
   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

class CCHotAutoCheck : public MetaBoolean
{
public:
   CCHotAutoCheck( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

class CCColdAutoValue : public MetaFloat
{
public:
   CCColdAutoValue( MetaProcess* );
   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

class CCColdAutoCheck : public MetaBoolean
{
public:
   CCColdAutoCheck( MetaProcess* );
   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

// Via Defect List -----------------------------
class CCUseDefectList : public MetaBoolean
{
public:
   CCUseDefectList( MetaProcess* );
   virtual IsoString Id() const;
};

class CCDefects : public MetaTable
{
public:
   CCDefects( MetaProcess* );
   virtual IsoString Id() const;
};

class CCDefectEnabled : public MetaBoolean
{
public:
   CCDefectEnabled( MetaTable* );
   virtual IsoString Id() const;
};

class CCDefectIsRow : public MetaBoolean
{
public:
   CCDefectIsRow( MetaTable* );
   virtual IsoString Id() const;
};

class CCDefectAddress : public MetaUInt16
{
public:
   CCDefectAddress( MetaTable* );
   virtual IsoString Id() const;
};

class CCDefectIsRange : public MetaBoolean
{
public:
   CCDefectIsRange( MetaTable* );
   virtual IsoString Id() const;
};

class CCDefectBegin : public MetaUInt16
{
public:
   CCDefectBegin( MetaTable* );
   virtual IsoString Id() const;
};

class CCDefectEnd : public MetaUInt16
{
public:
   CCDefectEnd( MetaTable* );
   virtual IsoString Id() const;
};


// ----------------------------------------------------------------------------

extern CCTargetFrames*        TheTargetFrames;
extern CCTargetFrameEnabled*  TheTargetFrameEnabled;
extern CCTargetFramePath*     TheTargetFramePath;

extern CCOutputDir*           TheOutputDir;
extern CCOutputExtension*     TheOutputExtension;
extern CCPrefix*              ThePrefix;
extern CCPostfix*             ThePostfix;

extern CCCFA*                 TheCFA;
extern CCAmount*              TheAmount;

extern CCUseMasterDark*       TheUseMasterDark;
extern CCMasterDarkPath*      TheMasterPath;
extern CCHotDarkCheck*        TheHotDarkCheck;
extern CCHotLevel*            TheHotLevel;
extern CCColdDarkCheck*       TheColdDarkCheck;
extern CCColdLevel*           TheColdLevel;

extern CCUseAutoDetect*       TheUseAutoDetect;
extern CCHotAutoCheck*        TheHotAutoCheck;
extern CCHotAutoValue*        TheHotAutoValue;
extern CCColdAutoCheck*       TheColdAutoCheck;
extern CCColdAutoValue*       TheColdAutoValue;

extern CCUseDefectList*       TheUseDefectList;
extern CCDefects*             TheDefects;
extern CCDefectEnabled*       TheDefectEnabled;
extern CCDefectIsRow*         TheDefectIsRow;
extern CCDefectAddress*       TheDefectAddress;
extern CCDefectIsRange*       TheDefectIsRange;
extern CCDefectBegin*         TheDefectBegin;
extern CCDefectEnd*           TheDefectEnd;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CosmeticCorrectionParameters_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionParameters.h - Released 2015/11/26 16:00:13 UTC
