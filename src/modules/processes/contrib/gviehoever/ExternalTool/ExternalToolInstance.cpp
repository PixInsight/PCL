// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ExternalTool Process Module Version 01.00.02.0144
// ****************************************************************************
// ExternalTool.cpp - Released 2014/11/14 17:19:24 UTC
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

#include "ExternalToolInstance.h"
#include "ExternalToolParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/MuteStatus.h>
#include <pcl/View.h>
#include <pcl/File.h>
#include <pcl/FileInfo.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
//#include <pcl/../../src/modules/file-formats/FITS/FITS.h>

#include <pcl/Console.h>
#include <pcl/ExternalProcess.h>
#include <pcl/MetaModule.h>

//#include <pcl/ImageWindow.h>

/// As seen in imagevariant.h, but with no support for complex
#define SOLVE_TEMPLATE_NO_COMPLEX_2(I,F)     \
   if ( (I).IsComplexSample() )              \
      throw("Cannot handle complex valued images"); \
   else if ( (I).IsFloatSample() )           \
      switch ( (I).BitsPerSample() )         \
      {                                      \
      case 32: F( FImage ); break;            \
      case 64: F( DImage ); break;           \
      }                                      \
   else                                      \
      switch ( (I).BitsPerSample() )         \
      {                                      \
      case  8: F( UInt8Image ); break;       \
      case 16: F( UInt16Image ); break;      \
      case 32: F( UInt32Image ); break;      \
      }

namespace pcl
{
    class ExternalToolEngine
    {
    private:
        String m_sSourceFileName;  ///name of source file. Empty if there is none
        String m_sResultFileName;  ///name of result file. Empty if there is none
        ExternalProcess m_process; /// the external process that is running.
        int m_nExitCode;
        String m_sStdOut;
        String m_sStdErr;

    public:

        /// construct, indicating that this is a global apply or help call
        ExternalToolEngine():
                m_nExitCode(0)
        {}

        /// destruct, cleaning up temporary files
        ~ExternalToolEngine()
        {
            if(m_process.IsRunning()||m_process.IsStarting())
            {
                Console console;
                console.WriteLn("Terminating external process that is still running (which should not happen");
                m_process.Terminate();
                if(!m_process.WaitForFinished()) {
                    console.WriteLn("Process did not respond to terminate. Killing it now.");
                    m_process.Kill();
                }
                // it does not make sense to collect the exitCode here
            }
            if (m_sSourceFileName != "") {
                try {
                    File::Delete(m_sSourceFileName);
                } catch (const File::Error &e) {
                    // simply ignore
                }
            }
            if (m_sResultFileName != "") {
                try {
                    File::Delete(m_sResultFileName);
                } catch (const File::Error &e) {
                    // simply ignore
                }
            }
        }

        /// apply the front end part, with writing the optional image in pView_p, updating nExitCode, stdOut and stdErr
        ///
        /// but not reading back any data
        void ApplyWithoutRead(View const * pView_p, ExternalToolInstance &rInstance_p, bool bGlobal_p)
        {
            m_nExitCode=0;
            m_sStdOut="";
            m_sStdErr="";

            try {
                FITSKeywordArray fitsKeywords;
                ICCProfile iccProfile;
                ImageVariant image;
                ImageVariant const * pImage=0;
                Console console;
                if (pView_p!=0){
                    image=pView_p->Image();
                    pImage=&image;
                }
                if (pView_p!=0 && !pView_p->Window().IsNull()) {
                    console.WriteLn("Extracting FITS Keywords and ICC Profile from main Window.");
                    pView_p->Window().GetKeywords(fitsKeywords); //ignoring possible false return value
                    pView_p->Window().GetICCProfile(iccProfile);
                } else if (pView_p !=0) {
                    console.WarningLn("No source for FITS Keywords and ICC Profile, because view has no Window.");
                }
                ApplyWithoutRead(pImage,fitsKeywords,iccProfile,rInstance_p,bGlobal_p);
            } catch (const Exception &e){ //Exception instead of Errror so we also catch ProcessAborted and others
                rInstance_p.m_nExitCode=m_nExitCode;
                rInstance_p.m_sStdOut=m_sStdOut;
                rInstance_p.m_sStdOut=m_sStdErr;
                throw;
            }
            //save results
            rInstance_p.m_nExitCode=m_nExitCode;
            rInstance_p.m_sStdOut=m_sStdOut;
            rInstance_p.m_sStdOut=m_sStdErr;
            if(m_nExitCode!=0)
            {
                throw Error("External Process returned with ExitCode !=0");
            }
        }

        /// assumes that ApplyWithoutRead() has already run. Read result file if it exists
        ///
        /// If requested or targetView==0 create new Window with result. If not requested, read into target view
        void ApplyRead(View *pTargetView_p,ExternalToolInstance &rInstance_p)
        {
            if(rInstance_p.m_bReadImage)
            {
                FITSKeywordArray fitsKeywords;
                ICCProfile iccProfile;
                ImageVariant resultImage;//=pTargetView_p->Image();
                Console console;
                ReadResultFile(resultImage, fitsKeywords, iccProfile);
                // clip or rescale
                if(rInstance_p.m_bRescaleResult) {
                    resultImage.Rescale();
                } else {
                    // clip. Does not change anything if image is within bounds
                    resultImage.Truncate();
                }
                // we have a result image. make sure target view can accommodate it
                View resultView;
                bool bIsNewWindow=false;
                if(pTargetView_p==0 || rInstance_p.m_bNewWindow)
                {
                    // generate new image window
                    ImageWindow w(resultImage.Width(),resultImage.Height(),resultImage.NumberOfChannels(),
                            resultImage.BitsPerSample(),resultImage.IsFloatSample(),
                            resultImage.ColorSpace() == ColorSpace::RGB, true, "ExternalProcess" );
                    if ( !w.IsNull() )
                    {
                        resultView=w.MainView();
                        bIsNewWindow=true;
                    } else {
                        throw Error("Unable to create image window for result");
                    }
                } else {
                    // working with current view
                    resultView = *pTargetView_p;
                    if (resultView.IsMainView() && !resultView.Window().IsNull()) {
                        // if this is main view, adapt format
                        resultView.Window().SetSampleFormat(resultImage.BitsPerSample(), resultImage.IsFloatSample());
                    } else {
                        ImageVariant const updatedImage = resultView.Image();
                        if (updatedImage.BitsPerSample() != resultImage.BitsPerSample() ||
                                updatedImage.IsFloatSample() != resultImage.IsFloatSample()) {
                            console.WarningLn("Not adapting sample type because this is not main view");
                        }
                    }
                }
                //Console().WriteLn("Assign:Width="+String(resultImage.Width())+",Height="+String(resultImage.Height())+
                //        ", isFloatSammple="+String(resultImage.IsFloatSample())+",BitsPerSample="+String(resultImage.BitsPerSample()));
                resultView.Image().AssignImage(resultImage);
                //resultImage >> resultView.Image();
                if(bIsNewWindow){
                    resultView.Window().Show();
                    resultView.Window().ZoomToFit(false);
                }
                // Add FITS keywords and profile
                if(rInstance_p.m_bReadFitsKeys) {
                    if (resultView.IsMainView() && !resultView.Window().IsNull()) {
                        console.WriteLn("Storing FITS Keywords and ICC Profile in image");
                        resultView.Window().SetKeywords(fitsKeywords);
                        resultView.Window().SetICCProfile(iccProfile);
                    } else {
                        console.WarningLn("Ignoring returned FITS Keywords and ICC Profile because result is not going to main view");
                    }
                }
            }
        }

        void ApplyHelp(ExternalToolInstance& rInstance_p )
        {
            const String sCommandLine= GetCommandLine(rInstance_p,true,false);
            try {
                RunExternalProcess(sCommandLine);
            } catch(const Error &e)
            {
                // save results and rethrow
                rInstance_p.m_nExitCode=m_nExitCode;
                rInstance_p.m_sStdOut=m_sStdOut;
                rInstance_p.m_sStdOut=m_sStdErr;
                throw;
            }
            // save results
            rInstance_p.m_nExitCode=m_nExitCode;
            rInstance_p.m_sStdOut=m_sStdOut;
            rInstance_p.m_sStdOut=m_sStdErr;
        }


        /// do everything except for the read part. pImage_p may be 0;
        ///
        /// The image is written with the given keywords
        void ApplyWithoutRead( const ImageVariant* pImage_p,
                FITSKeywordArray const &rFITSKeywordArray_p,
                ICCProfile const &rIccProfile_p,
                const ExternalToolInstance &rInstance_p,
                bool bGlobal_p) {
            if(!bGlobal_p && pImage_p!=0) {
                WriteSourceFile(pImage_p, rFITSKeywordArray_p, rIccProfile_p);
            }
            if(rInstance_p.m_bReadImage) {
                m_sResultFileName = GetTmpFitsFileName();
            }
            const String sCommandLine= GetCommandLine(rInstance_p,false,bGlobal_p);
            RunExternalProcess(sCommandLine);
        }

        /// read result file into rImage_p. Throw error if image cannot be read.
        void ReadResultFile(ImageVariant &rImage_p,
                FITSKeywordArray &rFITSKeywordArray_p,
                ICCProfile &rIccProfile_p)
        {
            if(m_sResultFileName =="")
            {
                throw Error("no result image defined");
            }
            const String &fileName= m_sResultFileName;
            //const String  fileName="/home/georg/GitHub/PCL/src/modules/processes/contrib/gviehoever/ExternalTool/ExampleTools/pixinsight_benchmark_v1.fit";
            //const String fileName="/home/georg/GitHub/PCL/src/modules/processes/contrib/gviehoever/ExternalTool/ExampleTools/out.fits";
            const FileFormat fileFormat(File::ExtractExtension(fileName), true, false);
            FileFormatInstance fileReader(fileFormat);
            ImageDescriptionArray imageDescriptionArray;
            if (!fileReader.Open(imageDescriptionArray,fileName)) {  //hints?, outputHints ) )
                throw Error("Cannot open result file");
            };
            if(imageDescriptionArray.Length()==0) {
                throw Error("Cannot handle empty image files");
            }
#if 0
            Console().WriteLn("imageDescArray.Size()="+String(imageDescriptionArray.Size()));
            Console().WriteLn("imageDescArray.Length()="+String(imageDescriptionArray.Length()));
            for(std::size_t i=0;i<imageDescriptionArray.Length();++i) {
                ImageDescription const &rDesc=imageDescriptionArray[i];
                Console().WriteLn("rDesc: NumberOfPixels="+String(rDesc.info.NumberOfPixels()));
                Console().WriteLn("rDesc: NumberOfSamples="+String(rDesc.info.NumberOfSamples()));
                Console().WriteLn("rDesc: NumberOfChannels="+String(rDesc.info.numberOfChannels));
                Console().WriteLn("rDesc: width="+String(rDesc.info.width));
                Console().WriteLn("rDesc: height="+String(rDesc.info.height));
                Console().WriteLn("rDesc: colorSpace="+String(rDesc.info.colorSpace));
                Console().WriteLn("rDesc: supported="+String(rDesc.info.supported));
                Console().WriteLn("rDesc: bitsPerSample="+String(rDesc.options.bitsPerSample));
                Console().WriteLn("rDesc: id="+String(rDesc.id));

            }
#endif
            if(imageDescriptionArray.Length()>1){
                throw Error("Cannot handle image files with more than 1 component");
            }

            ReadImageFile(rImage_p,fileReader,imageDescriptionArray,0);

            if ( !fileReader.SelectImage( 0 ) ) {
                throw Error("Cannot select image #0");
            }
            if ( !fileReader.Extract(rFITSKeywordArray_p)) {
                throw Error("Cannot extract FITS keywords");
            }
            if ( !fileReader.Extract(rIccProfile_p)) {
                throw Error("Cannot extract ICC profile");
            }
            if ( !fileReader.Close())
            {
                throw Error("Cannot close result file");
            }
        }

    private:
        /// Generate unique FITS filename for temporary file
        static String GetTmpFitsFileName()
        {
            const String sTmpDir=File::SystemTempDirectory();
            const String sUniqueFileName=File::UniqueFileName(sTmpDir, 8, "", ".fits");
            return sUniqueFileName;
        }

        /// compose command line. bHelp_p and bGlobal_p indicate the mode of the execution
        String GetCommandLine(const ExternalToolInstance &rInstance_p,bool bHelp_p, bool bGlobal_p) const
        {
            String sOptions=rInstance_p.m_sOptions;
            if (bHelp_p){
                sOptions=rInstance_p.m_sHelpOptions+" "+sOptions;
                //no filenames, globals,params on help
            } else {
                if (m_sSourceFileName != "") {
                    sOptions = rInstance_p.m_sSourceOptions + "\"" + m_sSourceFileName + "\" " + sOptions;
                }
                if (m_sResultFileName !="") {
                    sOptions = rInstance_p.m_sResultOptions + "\"" + m_sResultFileName + "\" " + sOptions;
                }
                if(bGlobal_p){
                    sOptions=rInstance_p.m_sGlobalOptions+" "+sOptions;
                }
                if(rInstance_p.m_bParam1){
                    sOptions+=" "+String(rInstance_p.m_dParam1);
                }
                if(rInstance_p.m_bParam2){
                    sOptions+=" "+String(rInstance_p.m_dParam2);
                }
                if(rInstance_p.m_bParam3){
                    sOptions+=" "+String(rInstance_p.m_dParam3);
                }
            }
            return rInstance_p.m_sExecutable+" "+sOptions;
        }

        /// write source file if pImage_p points to image
        void WriteSourceFile( const ImageVariant* pImage_p,
                FITSKeywordArray const &rFITSKeywordArray_p,
                ICCProfile const &rIccProfile_p)
        {
            if (pImage_p != 0) {
                // we have to write out the image
                const ImageVariant &rImage =*pImage_p;
                m_sSourceFileName = GetTmpFitsFileName();
                const FileFormat fileFormat(File::ExtractExtension(m_sSourceFileName), false, true);
                FileFormatInstance fileWriter(fileFormat);
                ImageOptions options;
                options.bitsPerSample=rImage.BitsPerSample();
                options.ieeefpSampleFormat=rImage.IsFloatSample();
                options.complexSample=rImage.IsComplexSample();
                options.signedIntegers=false;
                if (!fileWriter.Create(m_sSourceFileName)){
                    throw Error("Cannot create source file");
                };
                fileWriter.SetOptions(options);
                if (!fileWriter.Embed(rFITSKeywordArray_p)) {
                    /// should never happen
                    throw Error("Cannot embed FITS keywords");
                }
                if (!fileWriter.Embed(rIccProfile_p)) {
                    // should never happen
                    throw Error("Cannot embed ICC profile");
                }

                Console().WriteLn("Read Image:Width="+String(rImage.Width())+",Height="+String(rImage.Height())+
                        ", isFloatSammple="+String(rImage.IsFloatSample())+",BitsPerSample="+String(rImage.BitsPerSample())+
                        ", channels="+String(rImage.NumberOfChannels()));
                #define _WRITE_FILE(I) \
                    /*Console().WriteLn(String("Writing type=")+#I);*/ \
                    if (!fileWriter.WriteImage(static_cast<const I &>(**pImage_p))) { \
                        throw Error("Cannot write the image"); \
                    }
                SOLVE_TEMPLATE_NO_COMPLEX_2(*pImage_p,_WRITE_FILE);
                #undef _WRITE_FILE
                fileWriter.Close();

            }
        }

        // Read image from file with given index
        static void ReadImageFile( ImageVariant& v, FileFormatInstance& file, const ImageDescriptionArray& images, std::size_t index )
        {
            if ( !file.SelectImage( index ) ){
                throw Error("Cannot select image");
            }
            v.CreateSharedImage( images[index].options.ieeefpSampleFormat, images[index].options.complexSample, images[index].options.bitsPerSample );
            //v.CreateImage( images[index].options.ieeefpSampleFormat, images[index].options.complexSample, images[index].options.bitsPerSample );
            #define _READFILE(I) \
                /*Console().WriteLn(String("Reading type=")+#I);*/ \
                if(!file.ReadImage(static_cast<I &>(*v))) { \
                    throw("Failed to read image"); \
                }
            SOLVE_TEMPLATE_NO_COMPLEX_2((v),_READFILE);
            #undef _READFILE
            //Console().WriteLn("Read Image:Width="+String(v.Width())+",Height="+String(v.Height())+
            //        ", isFloatSammple="+String(v.IsFloatSample())+",BitsPerSample="+String(v.BitsPerSample())+
            //        ", channels="+String(v.NumberOfChannels()));
        }


        /// if data available on external process, collect in into rsStdout_p and rsStdErr_p. Also write to console;
        void HandleDataAvailable()
        {
            Console console;
            const ByteArray stdOut=m_process.StandardOutput();
            const ByteArray stdErr=m_process.StandardError();
            const String stdOutString=String::UTF8ToUTF16(reinterpret_cast<const char*>( stdOut.Begin() ), 0, stdOut.Length() );
            const String stdErrString=String::UTF8ToUTF16(reinterpret_cast<const char*>( stdErr.Begin() ), 0, stdErr.Length() );
            m_sStdOut+=stdOutString;
            m_sStdErr+=stdErrString;
            console.Write(stdOutString);
            console.Warning(stdErrString);
            Module->ProcessEvents(); // makes sure we can abort and process output appears at console
        }

        void RunExternalProcess(String const &rsCommandLine_p)
        {

            Console console;
            console.WriteLn("Executing with command line "+rsCommandLine_p);
            m_process.Start(rsCommandLine_p);
            if(!m_process.WaitForStarted()) {
                throw Error("Cannot launch external process");
            }
            //console.WriteLn("After WaitForStarted");
            while(m_process.IsRunning()){
                if(m_process.WaitForDataAvailable()) {
                    //console.WriteLn("DataAvail");
                    HandleDataAvailable();
                } else {
                    console.Write("."); // to indicate progress
                    Module->ProcessEvents(); // makes sure we can abort and process output appears at console
                }
            }
            //final bit of I/O
            HandleDataAvailable();
            m_nExitCode=m_process.ExitCode();
            if(m_nExitCode!=0)
            {
                throw Error("external process failed with exit code="+String(m_nExitCode));
            }
        }

    };  // class ExternalToolEngine

// ----------------------------------------------------------------------------

    namespace {
        /// resize string parameter
        void stringResize(String &s_p, size_type sizeOrLength_p)
        {
            s_p.Clear();
            if(sizeOrLength_p >0){
                s_p.Reserve(sizeOrLength_p);
            }
        }
    } // namespace

    ExternalToolInstance::ExternalToolInstance( const MetaProcess* m )
            :ProcessImplementation( m ),
             m_sExecutable(TheExternalToolParamSExecutable->DefaultValue()),
             m_sOptions(TheExternalToolParamSOptions->DefaultValue()),
             m_bReadImage(TheExternalToolParamBReadImage->DefaultValue()),
             m_bReadFitsKeys(TheExternalToolParamBReadFitsKeys->DefaultValue()),
             m_bNewWindow(TheExternalToolParamBNewWindow->DefaultValue()),
             m_bRescaleResult(TheExternalToolParamBRescaleResult->DefaultValue()),
             m_sHelpOptions(TheExternalToolParamSHelpOptions->DefaultValue()),
             m_sGlobalOptions(TheExternalToolParamSGlobalOptions->DefaultValue()),
             m_sSourceOptions(TheExternalToolParamSSourceOptions->DefaultValue()),
             m_sResultOptions(TheExternalToolParamSResultOptions->DefaultValue()),
             m_bParam1(TheExternalToolParamBool1->DefaultValue()),
             m_bParam2(TheExternalToolParamBool2->DefaultValue()),
             m_bParam3(TheExternalToolParamBool3->DefaultValue()),
             m_dParam1(TheExternalToolParamDouble1->DefaultValue()),
             m_dParam2(TheExternalToolParamDouble2->DefaultValue()),
             m_dParam3(TheExternalToolParamDouble3->DefaultValue()),
             m_nExitCode(static_cast<int>(TheExternalToolParamNExitCode->DefaultValue())),
             m_sStdOut(TheExternalToolParamSStdOut->DefaultValue()),
             m_sStdErr(TheExternalToolParamSStdErr->DefaultValue())
    {
    }

    ExternalToolInstance::ExternalToolInstance( const ExternalToolInstance& x ) :
            ProcessImplementation( x )
    {
        Assign( x );
    }

    void ExternalToolInstance::Assign( const ProcessImplementation& p )
    {
        const ExternalToolInstance* x = dynamic_cast<const ExternalToolInstance*>( &p );
        if ( x != 0 )
        {
            m_sExecutable=x->m_sExecutable;
            m_sOptions=x->m_sOptions;
            m_bReadImage=x->m_bReadImage;
            m_bReadFitsKeys=x->m_bReadFitsKeys;
            m_bNewWindow=x->m_bNewWindow;
            m_bRescaleResult=x->m_bRescaleResult;

            m_sHelpOptions=x->m_sHelpOptions;
            m_sGlobalOptions=x->m_sGlobalOptions;
            m_sSourceOptions=x->m_sSourceOptions;
            m_sResultOptions =x->m_sResultOptions;

            m_bParam1=x->m_bParam1;
            m_bParam2=x->m_bParam2;
            m_bParam3=x->m_bParam3;
            m_dParam1=x->m_dParam1;
            m_dParam2=x->m_dParam2;
            m_dParam3=x->m_dParam3;

            m_nExitCode=x->m_nExitCode;
            m_sStdOut=x->m_sStdOut;
            m_sStdErr=x->m_sStdErr;
        }
    }

    bool ExternalToolInstance::CanRunExecutable(String &rsWhyNot_p) const
    {
        FileInfo const f(m_sExecutable);
        //bug always returns IsExecutable("")==True, same on symbolic link
        //Console().WriteLn("Executable is "+m_sExecutable);
        //Console().WriteLn("Exists="+String(f.Exists())+",IsDirectory="+String(f.IsDirectory())+",isExecutable="+String(f.IsExecutable())+",isSymbolicLink="+String(f.IsSymbolicLink()));
        String sPath=m_sExecutable;
        if(f.IsSymbolicLink())
        {
            sPath=f.SymbolicLinkTarget();
        };
        FileInfo const f1(sPath);
        if(m_sExecutable=="" ||!f1.Exists() || f1.IsDirectory() ||!f1.IsExecutable())
        {
            //Console().WriteLn("Cannot Run");
            rsWhyNot_p="File given as executable cannot be run because it is not executable.";
            return false;
        }
        rsWhyNot_p.Clear();
        return true;
    }

    bool ExternalToolInstance::CanExecuteOn( const View& view, String& whyNot ) const
    {
        if(!CanRunExecutable(whyNot))
        {
            return false;
        }
        if(view.Image().IsComplexSample())
        {
            whyNot="Cannot handle images with complex value";
            return false;
        }
        whyNot.Clear();
        return true;
    }

    bool ExternalToolInstance::CanExecuteGlobal( String& whyNot ) const {
        if(!CanRunExecutable(whyNot))
        {
            return false;
        }
        whyNot.Clear();
        return true;
    }



    bool ExternalToolInstance::ExecuteOn( View& view ) {
        String whyNot;
        if(!CanExecuteOn(view,whyNot))
        {
            throw Error(whyNot);
        }
        Console().EnableAbort();
        ImageVariant image;// = view.Image();
        StandardStatus status;
        image.SetStatusCallback(&status);

        //bool const bWriteToView=m_bReadImage&&!m_bNewWindow;
        //Console().WriteLn("bWriteToView="+String(bWriteToView));
        //AutoViewLock lock(view,bWriteToView);
        AutoViewLock lock(view);
        ExternalToolEngine engine;
        engine.ApplyWithoutRead(&view, *this, false);
        engine.ApplyRead(&view,*this);

        return true;
    }

    bool ExternalToolInstance::ExecuteGlobal() {
        String whyNot;
        if(!CanExecuteGlobal(whyNot))
        {
            throw Error(whyNot);
        }
        Console console;
        console.EnableAbort();

        ExternalToolEngine engine;
        engine.ApplyWithoutRead(0,*this,true);
        engine.ApplyRead(0,*this);
        return true;
    }

    bool ExternalToolInstance::ExecuteHelp() {
        String whyNot;
        if (!CanExecuteGlobal(whyNot)) {
            throw Error(whyNot);
        }
        //Console console;
        //console.EnableAbort();
        ExternalToolEngine engine;

        engine.ApplyHelp(*this);
        Console().Show(true);
        return true;
    }

    void* ExternalToolInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
    {
        if ( p == TheExternalToolParamSExecutable )
            return m_sExecutable.c_str();
        if ( p == TheExternalToolParamSOptions )
            return m_sOptions.c_str();
        if ( p == TheExternalToolParamSHelpOptions )
            return m_sHelpOptions.c_str();
        if ( p == TheExternalToolParamSGlobalOptions)
            return m_sGlobalOptions.c_str();
        if ( p == TheExternalToolParamSSourceOptions)
            return m_sSourceOptions.c_str();
        if ( p == TheExternalToolParamSResultOptions)
            return m_sResultOptions.c_str();

        if ( p == TheExternalToolParamBReadImage )
            return &m_bReadImage;
        if ( p == TheExternalToolParamBReadFitsKeys )
            return &m_bReadFitsKeys;
        if ( p == TheExternalToolParamBNewWindow )
            return &m_bNewWindow;
        if ( p == TheExternalToolParamBRescaleResult )
            return &m_bRescaleResult;


        if ( p == TheExternalToolParamBool1 )
            return &m_bParam1;
        if ( p == TheExternalToolParamBool2 )
            return &m_bParam2;
        if ( p == TheExternalToolParamBool3 )
            return &m_bParam3;
        if ( p == TheExternalToolParamDouble1)
            return &m_dParam1;
        if ( p == TheExternalToolParamDouble2)
            return &m_dParam2;
        if ( p == TheExternalToolParamDouble3)
            return &m_dParam3;

        if ( p == TheExternalToolParamNExitCode )
            return &m_nExitCode;
        if ( p == TheExternalToolParamSStdOut )
            return m_sStdOut.c_str();
        if ( p == TheExternalToolParamSStdErr )
            return m_sStdErr.c_str();
        return 0;
    }

    bool ExternalToolInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
    {
        if ( p == TheExternalToolParamSExecutable ) {
            stringResize(m_sExecutable, sizeOrLength);
        }
        else if ( p == TheExternalToolParamSOptions ){
            stringResize(m_sOptions,sizeOrLength);
        }
        else if ( p == TheExternalToolParamSHelpOptions ) {
            stringResize(m_sHelpOptions, sizeOrLength);
        }
        else if ( p == TheExternalToolParamSGlobalOptions) {
            stringResize(m_sGlobalOptions,sizeOrLength);
        }
        else if ( p == TheExternalToolParamSSourceOptions) {
            stringResize(m_sSourceOptions,sizeOrLength);
        }
        else if ( p == TheExternalToolParamSResultOptions) {
            stringResize(m_sResultOptions,sizeOrLength);
        }
        else if ( p == TheExternalToolParamSStdOut ){
            stringResize(m_sStdOut,sizeOrLength);
        }
        else if ( p == TheExternalToolParamSStdErr ){
            stringResize(m_sStdErr,sizeOrLength);
        }
        else {
            return false;
        }

        return true;
    }

    size_type ExternalToolInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
    {
        if ( p == TheExternalToolParamSExecutable ) {
            return m_sExecutable.Length();
        } else if ( p == TheExternalToolParamSOptions ){
            return m_sOptions.Length();
        } else if ( p == TheExternalToolParamSHelpOptions ) {
            return m_sHelpOptions.Length();
        } else if ( p == TheExternalToolParamSSourceOptions ) {
            return m_sSourceOptions.Length();
        } else if ( p == TheExternalToolParamSResultOptions) {
            return m_sResultOptions.Length();
        } else if ( p == TheExternalToolParamSGlobalOptions) {
            return m_sGlobalOptions.Length();
        } else if ( p == TheExternalToolParamSStdOut ){
            return m_sStdOut.Length();
        } else if ( p == TheExternalToolParamSStdErr ){
            return m_sStdErr.Length();
        }
        return 0;
    }

    void ExternalToolInstance::Preview( UInt16Image& image ) const
    {
        MuteStatus status;
        image.SetStatusCallback( &status );
        ExternalToolEngine engine;
        ImageVariant vImage(&image);
        ICCProfile iccProfile;
        FITSKeywordArray fitsKeywords;
        // cannot do any FITS or icc magic for preview
        engine.ApplyWithoutRead(&vImage,fitsKeywords,iccProfile,*this,false);
        ImageVariant resultImage;//=pTargetView_p->Image();
        engine.ReadResultFile(resultImage, fitsKeywords, iccProfile);
        // clip or rescale
        if(m_bRescaleResult) {
            resultImage.Rescale();
        } else {
            // clip. Does not change anything if image is within bounds
            resultImage.Truncate();
        }
        vImage.AssignImage(resultImage);
    }

// ----------------------------------------------------------------------------

} // namespace pcl
