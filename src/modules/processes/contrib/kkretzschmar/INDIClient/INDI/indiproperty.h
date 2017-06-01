//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// indiproperty.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef INDI_INDIPROPERTY_H
#define INDI_INDIPROPERTY_H

#include "indibase.h"



namespace INDI
{

class BaseDevice;

class Property
{
public:
    Property();
    ~Property();

    void setProperty(void *);
    void setType(INDI_TYPE t);
    void setRegistered(bool r);
    void setDynamic(bool d);
    void setBaseDevice(BaseDevice *idp);

    void *getProperty() { return pPtr; }
    INDI_TYPE getType() { return pType; }
    bool getRegistered() { return pRegistered; }
    bool isDynamic() { return pDynamic; }
    BaseDevice *getBaseDevice() { return dp; }

    // Convenience Functions
    const char *getName();
    const char *getLabel();
    const char *getGroupName();
    const char *getDeviceName();
    IPState getState();
    IPerm getPermission();

    INumberVectorProperty *getNumber();
    ITextVectorProperty   *getText();
    ISwitchVectorProperty *getSwitch();
    ILightVectorProperty  *getLight();
    IBLOBVectorProperty   *getBLOB();

private:
    void *pPtr;
    BaseDevice *dp;
    INDI_TYPE pType;
    bool pRegistered;
    bool pDynamic;
};

} // namespace INDI

#endif // INDI_INDIPROPERTY_H

// ----------------------------------------------------------------------------
// EOF indiproperty.h - Released 2017-05-02T09:43:01Z
