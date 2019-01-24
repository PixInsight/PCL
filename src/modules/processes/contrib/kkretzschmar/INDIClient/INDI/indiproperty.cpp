//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// indiproperty.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

/*******************************************************************************
  Copyright(c) 2011 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#include "indiproperty.h"

#include <cstdlib>

INDI::Property::Property()
{
    pPtr        = nullptr;
    pRegistered = false;
    pDynamic    = false;
    pType       = INDI_UNKNOWN;
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
            {
                INumberVectorProperty *p = (INumberVectorProperty *)pPtr;
                free(p->np);
                delete p;
                break;
            }

            case INDI_TEXT:
            {
                ITextVectorProperty *p = (ITextVectorProperty *)pPtr;
                for (int i = 0; i < p->ntp; ++i)
                {
                    free(p->tp[i].text);
                }
                free(p->tp);
                delete p;
                break;
            }

            case INDI_SWITCH:
            {
                ISwitchVectorProperty *p = (ISwitchVectorProperty *)pPtr;
                free(p->sp);
                delete p;
                break;
            }

            case INDI_LIGHT:
            {
                ILightVectorProperty *p = (ILightVectorProperty *)pPtr;
                free(p->lp);
                delete p;
                break;
            }

            case INDI_BLOB:
            {
                IBLOBVectorProperty *p = (IBLOBVectorProperty *)pPtr;
                for (int i = 0; i < p->nbp; ++i)
                {
                    free(p->bp[i].blob);
                }
                free(p->bp);
                delete p;
                break;
            }

            case INDI_UNKNOWN:
                break;
        }
    }
}

void INDI::Property::setProperty(void *p)
{
    pRegistered = true;
    pPtr        = p;
}

void INDI::Property::setType(INDI_PROPERTY_TYPE t)
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

const char *INDI::Property::getName() const
{
    if (pPtr == nullptr)
        return nullptr;

    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->name;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->name;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->name;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->name;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->name;
            break;

        case INDI_UNKNOWN:
            break;
    }

    return nullptr;
}

const char *INDI::Property::getLabel() const
{
    if (pPtr == nullptr)
        return nullptr;

    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->label;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->label;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->label;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->label;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->label;
            break;

        case INDI_UNKNOWN:
            break;
    }
    return nullptr;
}

const char *INDI::Property::getGroupName() const
{
    if (pPtr == nullptr)
        return nullptr;

    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->group;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->group;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->group;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->group;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->group;
            break;

        case INDI_UNKNOWN:
            break;
    }

    return nullptr;
}

const char *INDI::Property::getDeviceName() const
{
    if (pPtr == nullptr)
        return nullptr;

    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->device;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->device;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->device;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->device;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->device;
            break;

        case INDI_UNKNOWN:
            break;
    }

    return nullptr;
}

const char *INDI::Property::getTimestamp() const
{
    if (pPtr == nullptr)
        return nullptr;

    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->timestamp;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->timestamp;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->timestamp;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->timestamp;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->timestamp;
            break;

        case INDI_UNKNOWN:
            break;
    }

    return nullptr;
}

IPState INDI::Property::getState() const
{
    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->s;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->s;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->s;
            break;

        case INDI_LIGHT:
            return ((ILightVectorProperty *)pPtr)->s;
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->s;
            break;

        default:
            break;
    }

    return IPS_IDLE;
}

IPerm INDI::Property::getPermission() const
{
    switch (pType)
    {
        case INDI_NUMBER:
            return ((INumberVectorProperty *)pPtr)->p;
            break;

        case INDI_TEXT:
            return ((ITextVectorProperty *)pPtr)->p;
            break;

        case INDI_SWITCH:
            return ((ISwitchVectorProperty *)pPtr)->p;
            break;

        case INDI_LIGHT:
            break;

        case INDI_BLOB:
            return ((IBLOBVectorProperty *)pPtr)->p;
            break;

        default:
            break;
    }

    return IP_RO;
}

INumberVectorProperty *INDI::Property::getNumber()
{
    if (pType == INDI_NUMBER)
        return ((INumberVectorProperty *)pPtr);

    return nullptr;
}

ITextVectorProperty *INDI::Property::getText()
{
    if (pType == INDI_TEXT)
        return ((ITextVectorProperty *)pPtr);

    return nullptr;
}

ISwitchVectorProperty *INDI::Property::getSwitch()
{
    if (pType == INDI_SWITCH)
        return ((ISwitchVectorProperty *)pPtr);

    return nullptr;
}

ILightVectorProperty *INDI::Property::getLight()
{
    if (pType == INDI_LIGHT)
        return ((ILightVectorProperty *)pPtr);

    return nullptr;
}

IBLOBVectorProperty *INDI::Property::getBLOB()
{
    if (pType == INDI_BLOB)
        return ((IBLOBVectorProperty *)pPtr);

    return nullptr;
}

// ----------------------------------------------------------------------------
// EOF indiproperty.cpp - Released 2019-01-21T12:06:42Z
