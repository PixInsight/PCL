//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// indiproperty.cpp - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#include "basedevice.h"
#include "BaseClientImpl.h"
#include "indicom.h"
#include "base64.h"
#include "indiproperty.h"

INDI::Property::Property()
{
    pPtr = NULL;
    pRegistered = false;
    pDynamic = false;
    pType = INDI_UNKNOWN;
}

INDI::Property::~Property()
{
    // Only delete properties if they were created dynamically via the buildSkeleton
    // function. Other drivers are responsible for their own memory allocation.
    if (pDynamic)
    {
        switch (pType)
        {
        case INDI_NUMBER:
         delete ((INumberVectorProperty *) pPtr);
         break;

        case INDI_TEXT:
        delete ((ITextVectorProperty *) pPtr);
        break;

        case INDI_SWITCH:
        delete ((ISwitchVectorProperty *) pPtr);
        break;

        case INDI_LIGHT:
        delete ((ILightVectorProperty *) pPtr);
        break;

        case INDI_BLOB:
        delete ((IBLOBVectorProperty *) pPtr);
        break;

        case INDI_UNKNOWN:
            break;

    }
  }
}

void INDI::Property::setProperty(void *p)
{
    pRegistered = true;
    pPtr = p;

}

void INDI::Property::setType(INDI_TYPE t)
{
    pType = t;
}

void INDI::Property::setRegistered(bool r)
{
   pRegistered = r;
}

void INDI::Property::setDynamic(bool d)
{
   pDynamic = d;
}

void INDI::Property::setBaseDevice(BaseDevice *idp)
{
    dp = idp;
}

const char * INDI::Property::getName()
{
    if (pPtr == NULL)
        return NULL;

    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->name;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->name;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->name;
    break;

    case INDI_LIGHT:
     return ((ILightVectorProperty *) pPtr)->name;
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->name;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return NULL;
}

const char * INDI::Property::getLabel()
{

    if (pPtr == NULL)
        return NULL;

    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->label;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->label;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->label;
    break;

    case INDI_LIGHT:
     return ((ILightVectorProperty *) pPtr)->label;
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->label;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return NULL;

}

const char * INDI::Property::getGroupName()
{
    if (pPtr == NULL)
        return NULL;

    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->group;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->group;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->group;
    break;

    case INDI_LIGHT:
     return ((ILightVectorProperty *) pPtr)->group;
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->group;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return NULL;

}

const char * INDI::Property::getDeviceName()
{
    if (pPtr == NULL)
        return NULL;

    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->device;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->device;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->device;
    break;

    case INDI_LIGHT:
     return ((ILightVectorProperty *) pPtr)->device;
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->device;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return NULL;

}

IPState INDI::Property::getState()
{

    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->s;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->s;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->s;
    break;

    case INDI_LIGHT:
     return ((ILightVectorProperty *) pPtr)->s;
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->s;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return IPS_IDLE;

}

IPerm INDI::Property::getPermission()
{
    switch (pType)
    {
    case INDI_NUMBER:
     return ((INumberVectorProperty *) pPtr)->p;
     break;

    case INDI_TEXT:
     return ((ITextVectorProperty *) pPtr)->p;
    break;

    case INDI_SWITCH:
     return ((ISwitchVectorProperty *) pPtr)->p;
    break;

    case INDI_LIGHT:
    break;

    case INDI_BLOB:
     return ((IBLOBVectorProperty *) pPtr)->p;
    break;

    case INDI_UNKNOWN:
        break;
  }

  return IP_RO;

}

INumberVectorProperty *INDI::Property::getNumber()
{
    if (pType == INDI_NUMBER)
        return ((INumberVectorProperty *) pPtr);

    return NULL;

}

ITextVectorProperty   *INDI::Property::getText()
{
    if (pType == INDI_TEXT)
        return ((ITextVectorProperty *) pPtr);

    return NULL;
}

ISwitchVectorProperty *INDI::Property::getSwitch()
{
    if (pType == INDI_SWITCH)
        return ((ISwitchVectorProperty *) pPtr);

    return NULL;
}

ILightVectorProperty  *INDI::Property::getLight()
{
    if (pType == INDI_LIGHT)
        return ((ILightVectorProperty *) pPtr);

    return NULL;

}

IBLOBVectorProperty   *INDI::Property::getBLOB()
{
    if (pType == INDI_BLOB)
        return ((IBLOBVectorProperty *) pPtr);

    return NULL;

}

// ----------------------------------------------------------------------------
// EOF indiproperty.cpp - Released 2016/04/15 15:37:39 UTC
