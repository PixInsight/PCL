// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalTool.h - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the ExternalTool PixInsight module.
//
// Copyright (c) 2014-2015, Georg Viehoever. All Rights Reserved.
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

#ifndef __ExternalToolInstance_h
#define __ExternalToolInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

    class ExternalToolInstance : public ProcessImplementation
    {
    public:

        ExternalToolInstance( const MetaProcess* );
        ExternalToolInstance( const ExternalToolInstance& );

        virtual void Assign( const ProcessImplementation& );

        virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
        virtual bool ExecuteOn( View& );

        virtual bool CanExecuteGlobal( String& whyNot ) const;
        virtual bool ExecuteGlobal();

        /// execute help command.
        virtual bool ExecuteHelp();


        virtual void* LockParameter( const MetaParameter*, size_type tableRow );
        virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
        virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

        /// For RTP
        void Preview( UInt16Image& ) const;

    private:
        /// check if executable in m_sExecutable exists and is runable
        bool CanRunExecutable(String &rsWhyNot_p) const;

    private:

        String m_sExecutable;  /// path to executable
        String m_sOptions;   /// options passed to executable
        pcl_bool m_bReadImage;   /// process creates image that needs to be read
        pcl_bool m_bReadFitsKeys;  /// read FITS keys from read image
        pcl_bool m_bNewWindow;   /// new window created for new image
        pcl_bool m_bRescaleResult; /// rescale result to range [0,1]. Otherwise truncate if necessary
        String m_sHelpOptions; /// options passed to executable if we want a help text
        String m_sGlobalOptions; /// options passed to executable if we want global instead of local execution
        String m_sSourceOptions; /// options passed to executable with source file name
        String m_sResultOptions; /// options passed to executable with result file name

        pcl_bool m_bParam1; /// true if to add m_fParam1 to parameter list
        pcl_bool m_bParam2;
        pcl_bool m_bParam3;
        double m_dParam1; /// value to be added to parameter list if m_bParam1 is true
        double m_dParam2;
        double m_dParam3;

        int m_nExitCode;  /// ExitCode of process
        String m_sStdOut; /// contents of stdOut
        String m_sStdErr; /// contents of stdErr

        friend class ExternalToolEngine;
        friend class ExternalToolProcess;
        friend class ExternalToolInterface;
    };

// ----------------------------------------------------------------------------


} // pcl

#endif   // __ExternalToolInstance_h

// ****************************************************************************
