//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard AberrationInspector Process Module Version 01.01.02.0208
// ----------------------------------------------------------------------------
// AberrationInspectorInstance.cpp - Released 2016/02/29 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AberrationInspector PixInsight module.
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

#include "AberrationInspectorInstance.h"
#include "AberrationInspectorParameters.h"
#include <pcl/AutoViewLock.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{
    AberrationInspectorInstance::AberrationInspectorInstance(const MetaProcess* m) :
    ProcessImplementation(m),
	p_mosaicSize(TheMosaicSize->DefaultValue()),
	p_panelSize(ThePanelSize->DefaultValue()),
    p_separation(TheSeparation->DefaultValue()),
    p_brightness(TheBrightness->DefaultValue())
    {
		m_createNewImage = false;
    }

    AberrationInspectorInstance::AberrationInspectorInstance(const AberrationInspectorInstance& x) :
    ProcessImplementation(x)
    {
        Assign(x);
    }

    void AberrationInspectorInstance::Assign(const ProcessImplementation& p)
    {
        const AberrationInspectorInstance* x = dynamic_cast<const AberrationInspectorInstance*> (&p);
        if (x != 0)
        {
			p_mosaicSize = x->p_mosaicSize;
			p_panelSize = x->p_panelSize;
            p_separation = x->p_separation;
            p_brightness = x->p_brightness;
        }
    }

    bool AberrationInspectorInstance::CanExecuteOn(const View& view, pcl::String& whyNot) const
    {
        if (view.Image().IsComplexSample())
        {
            whyNot = "AberrationInspector cannot be executed on complex images.";
            return false;
        }

        whyNot.Clear();
        return true;
    }
	
	bool AberrationInspectorInstance::CanExecuteGlobal(String& whyNot) const
	{
		whyNot.Clear();
		View view = ImageWindow::ActiveWindow().CurrentView();
		if (view.IsNull())
			whyNot = "There is no active image window.";

		else if (view.Image().IsComplexSample())
			whyNot = "AberrationInspector cannot be executed on complex images.";

		return whyNot.IsEmpty();
	}

    bool AberrationInspectorInstance::IsHistoryUpdater(const View& view) const
    {
		return !m_createNewImage;
    }
	
    inline void CopySTF(View& target, const View& source)
    {
        View::stf_list F;
        source.GetScreenTransferFunctions(F);
        target.SetScreenTransferFunctions(F);
        target.EnableScreenTransferFunctions();
    }

	template <class P>
	void AberrationInspectorInstance::Engine(GenericImage<P>& s, const AberrationInspectorInstance& instance, View& view)
	{
		//source image size
		int sWidth = s.Width();
		int sHeight = s.Height();

		int pSize = Min<int>(Min(sWidth, sHeight) / instance.p_mosaicSize, instance.p_panelSize); //corrected panel size accordin image dimention.
		int tSize = (pSize + instance.p_separation) * instance.p_mosaicSize - instance.p_separation; //size of output mosaic in pixels

		GenericImage<P> t(tSize, tSize, s.ColorSpace());

		t.Fill(instance.p_brightness);

		Rect r(0, 0, pSize, pSize);
		int stepTargetXY = pSize + instance.p_separation; //pixels between panels in target image
		float stepSourceX = float(sWidth - pSize) / (instance.p_mosaicSize - 1); //pixels between panels in source image
		float stepSourceY = float(sHeight - pSize) / (instance.p_mosaicSize - 1); //pixels between panels in source image
		for (int x = 0; x != instance.p_mosaicSize; ++x)
		{
			int sX = RoundI(stepSourceX * x); //source point X in pixels
			for (int y = 0; y != instance.p_mosaicSize; ++y)
			{
				int sY = RoundI(stepSourceY * y); //source point Y in pixels
				r.MoveTo(sX, sY); //move source rectangle to panel position 
				Point targetPoint(stepTargetXY * x, stepTargetXY * y); //point in target
				t.Apply(s, pcl::ImageOp::Mov, targetPoint, -1, r); //copy pixels from sourse rectangle to target
			}
		}

		if (m_createNewImage)
		{
			ImageWindow tWindow(tSize, tSize,
				s.NumberOfChannels(), P::BitsPerSample(), P::IsFloatSample(),
				s.NumberOfChannels() > 1 ? true : false, // ? RGB : Mono
				true, view.Id() + "_ai");

			View tView = tWindow.MainView();
			static_cast<GenericImage<P>&>(*tView.Image()) = t;
			CopySTF(tView, view); //transfet STF from sourse to target.
			tWindow.Show();
		}
		else
		{
			s.Assign(t);
		}
	}

	bool AberrationInspectorInstance::ExecuteEngine(View& view)
	{
		try
		{
			view.LockForWrite();

			ImageVariant image;
			image = view.Image();

			if (!image.IsComplexSample())
			{
				if (image.IsFloatSample())
					switch (image.BitsPerSample())
				{
					case 32: Engine(static_cast< Image&>(*image), *this, view); break;
					case 64: Engine(static_cast< DImage&>(*image), *this, view); break;
				}
				else
					switch (image.BitsPerSample())
				{
					case 8: Engine(static_cast< UInt8Image&>(*image), *this, view); break;
					case 16: Engine(static_cast< UInt16Image&>(*image), *this, view); break;
					case 32: Engine(static_cast< UInt32Image&>(*image), *this, view); break;
				}
			}
			view.Unlock();

			return true;
		}

		catch (...)
		{
			view.Unlock(); // Never leave a view locked!
			throw;
		}
	}

	bool AberrationInspectorInstance::ExecuteOn(View& view)
	{
		m_createNewImage = false;
		return ExecuteEngine(view);
	}

	bool AberrationInspectorInstance::ExecuteGlobal()
	{
		m_createNewImage = true;
      View view = ImageWindow::ActiveWindow().CurrentView();
		return ExecuteEngine(view);
	}

    void* AberrationInspectorInstance::LockParameter(const MetaParameter* p, size_type /*tableRow*/)
    {
		if (p == TheMosaicSize) return &p_mosaicSize;
		if (p == ThePanelSize) return &p_panelSize;
        if (p == TheSeparation) return &p_separation;
        if (p == TheBrightness) return &p_brightness;

        return 0;
    }

    // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AberrationInspectorInstance.cpp - Released 2016/02/29 00:00:00 UTC
