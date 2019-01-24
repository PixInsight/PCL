//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0347
// ----------------------------------------------------------------------------
// GREYCstorationInstance.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#ifndef __GREYCstorationInstance_h
#define __GREYCstorationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------

class GREYCstorationInstance : public ProcessImplementation
{
public:

   GREYCstorationInstance( const MetaProcess* );
   GREYCstorationInstance( const GREYCstorationInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual UndoFlags UndoMode( const View& ) const;
   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );
   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );

private:

   float    amplitude;
   int32    numberOfIterations;
   float    sharpness;
   float    anisotropy;
   float    alpha;
   float    sigma;
   pcl_bool fastApproximation;
   float    precision;
   float    spatialStepSize;
   float    angularStepSize;
   pcl_enum interpolation;
   pcl_bool coupledChannels;

   friend class GREYCstorationProcess;
   friend class GREYCstorationInterface;
   friend class GREYCstorationEngine;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __GREYCstorationInstance_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationInstance.h - Released 2019-01-21T12:06:41Z
